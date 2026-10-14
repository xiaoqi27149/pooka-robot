# rkros_audio ROS2 功能包

这个 ROS2 功能包通过 ALSA 库用于捕获和播放音频数据。它提供了一个音频发布节点、一个音频订阅节点和一个与 Rockchip 音频库集成的音频处理节点，以实现实时音频传输和处理。

## 功能

- 使用 Rockchip 库进行音频处理
- 支持 8 声道音频输入（处理为单声道输出）
- 16kHz 采样率实时音频处理
- 将处理后的单声道音频发布到 `rkaudio` 主题
- 将 ASR 优化的单声道音频发布到 `rkaudio_asr` 主题
- 支持到达方向 (DOA) 估计
- 实时发布 DOA 值到 `rk_doa` 主题
- 用于 `rkaudio` 和 `rkaudio_asr` 主题的音频播放节点
- 音频重采样支持：自动将不同采样率的音频数据转换为 16kHz 进行播放
- 可选的 WAV 文件录制功能（in.wav、out.wav 和 doa.wav），可通过参数配置（默认：false）
- ASR 音频数据发布到 `rkaudio_asr` 主题，用于自动语音识别
- 音频版本信息日志记录
- 语音活动检测 (VAD) 支持，实时状态日志记录
- 唤醒词检测，实时状态日志记录

## 依赖项

- ROS2 Humble Hawksbill
- ALSA 开发库
- FFmpeg 开发库（用于音频重采样）
- Rockchip 音频库文件（包含在包中）

## 安装依赖

```bash
sudo apt update
sudo apt install -y libasound2-dev libsamplerate0-dev libavutil-dev libswresample-dev
```

## 构建

```bash
cd rkros_rk_audio
colcon build --packages-select rkros_audio
source install/setup.bash
```

## 使用方法

### 基础音频录制和播放

#### 1. 运行基础音频发布节点（单声道录制）

```bash
ros2 run rkros_audio audio_publisher
```

#### 2. 使用订阅节点播放音频

```bash
ros2 run rkros_audio audio_subscriber
```

#### 3. 使用启动文件运行基础音频节点（发布 + 订阅）

```bash
ros2 launch rkros_audio rk_audio.launch.xml
```

### 使用 Rockchip 库进行高级音频处理

#### 1. 运行与 Rockchip 音频库集成的处理器节点

`audio_rk_processor` 节点捕获 8 声道音频，使用 Rockchip 库处理，并将处理后的单声道音频发布到 `/rkaudio`，将 DOA 数据发布到 `/rk_doa`。

**WAV 文件录制的附加参数：**
- `save_wav_files`：启用/禁用将音频录制到 WAV 文件（默认：false）
  - 启用时，创建：`in.wav`（8 声道输入）、`out.wav`（处理后的单声道输出）和 `doa.wav`（DOA 数据作为音频）
  - 使用方法：`ros2 run rkros_audio audio_rk_processor --ros-args -p save_wav_files:=true`

**方法 A：使用启动文件（推荐）**
```bash
# 从项目根目录
cd rkros_rk_audio  # 替换为您的项目目录
source install/setup.bash
ros2 launch rkros_audio rk_audio_rk_process.launch.xml
```

**方法 B：直接运行**
```bash
# 从项目根目录
cd rkros_rk_audio  # 替换为您的项目目录
source install/setup.bash

# 运行节点（库文件将从安装目录自动加载）
ros2 run rkros_audio audio_rk_processor
```

#### 2. 其他启动文件

```bash
# 启动文件，包含 Rockchip 音频处理和环境设置
ros2 launch rkros_audio rk_audio_process_with_env.launch.xml

# 仅启动 Rockchip 音频处理
ros2 launch rkros_audio rk_audio_process.launch.xml
```

### 播放处理后的音频

播放任何来源的音频（基础录制或 Rockchip 处理）：

```bash
# 使用此节点播放来自 rkaudio 主题的音频
ros2 run rkros_audio audio_subscriber
```

播放来自 `rkaudio_asr` 主题的 ASR 优化音频：

```bash
# 使用此节点播放 ASR 优化音频（使用重映射功能）
ros2 run rkros_audio audio_subscriber --ros-args --remap rkaudio:=rkaudio_asr
```
## 发布的主题

- `rkaudio`: 音频数据主题
  - 基础发布器：单声道，16kHz，16 位
  - Rockchip 处理器：单声道（从 8 声道输入算法处理），16kHz，16 位
- `rkaudio_asr`: ASR (自动语音识别) 音频数据主题，仅由 `audio_rk_processor` 发布
  - 包含专门用于语音识别处理的音频数据，格式与 `rkaudio` 主题相同
- `rk_doa`: 到达方向估计（度），仅由 `audio_rk_processor` 发布

## 消息类型

- `RkAudio.msg`: 自定义音频消息类型（在 rkros_audio_msgs 包中定义）
  - `sample_rate`: 采样率
  - `channels`: 通道数
  - `bits_per_sample`: 每个样本的位数
  - `audio_data`: 音频数据字节数组

音频订阅节点现在支持不同采样率的音频数据，并自动将它们重采样到 16kHz 进行播放。

## 配置

音频参数默认设置为：
- 采样率：16000 Hz
- 通道数：基础发布器为 1（单声道），Rockchip 处理器为 8
- 位深度：16 位

节点参数：
- `save_wav_files`：用于启用/禁用 WAV 文件录制的布尔参数（默认：false）

Rockchip 音频处理通过 JSON 配置文件（`rk_audio_hci.json`）进行配置，包括以下参数：
- 麦克风阵列配置（默认为 6 个麦克风）
- AEC、AIEC、AINR、DOA 和波束形成等级
- 采样率和帧长度

### 音频 3A 处理许可证
- 音频处理功能需要许可证文件：`/data/key_audio3a.lic`
- 此许可证是高级音频处理功能所必需的，包括 DOA (到达方向)、AEC (声学回声消除)、AIEC (声学脉冲/回声消除) 和其他音频增强功能
- 请确保将 `key_audio3a.lic` 文件放置在 `/data/` 目录中以启用完整的音频处理功能

## 故障排除

如果您遇到音频权限问题，请尝试：

```bash
sudo usermod -a -G audio $USER
```

然后注销并重新登录或重启系统。

如果遇到 Rockchip 库的问题，请确保库文件正确安装在包共享目录中：
```
install/rkros_audio/share/rkros_audio/rk_lib/
```
