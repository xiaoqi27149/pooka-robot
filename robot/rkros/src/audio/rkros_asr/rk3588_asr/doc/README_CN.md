# rkros_asr ROS2 功能包

## 介绍

`rkros_asr` 功能包是一个 ROS2 功能包，使用 Rockchip 的 RockX2 SDK 提供自动语音识别（ASR）功能。它订阅 `rkaudio_asr` 主题的音频数据，使用 Rockchip 的 ASR 引擎进行处理，并将识别结果发布到 `rk_asr_result` 主题。

## 功能

- 对 ROS2 主题中的音频流进行实时 ASR 处理
- 支持英语和中文
- 流模式处理以实现实时识别
- 与 RockX2 SDK 和 RockASR 库集成
- 用于 ASR 结果的自定义消息类型（包含状态信息）
- 基于参数的配置（语言、数据路径和模型选择）
- 支持 16kHz 音频采样率，16 位采样
- **新增：语音命令控制机器人移动** - 识别语音命令并发布 Twist 消息来控制机器人移动

## 依赖项

此功能包需要：
- RK35XX_RockASR_SDK_V1.0.0_20250621
- RK35XX_ROCKX2_SDK_V1.0.0_20250621
- rkros_audio_msgs ROS2 功能包（用于音频消息定义）
- rkros_asr_msgs ROS2 功能包（用于 ASR 结果消息定义）

RockX2 SDK 应位于功能包目录中，名为 `RK3588_ROCKX2_SDK_V1.0.0_20250621`

RK35XX_RockASR_SDK_V1.0.0_20250621下载链接（主要包含模型数据）：
[ROCKASR SDK下载](https://console.box.lenovo.com/l/T55yZw) (访问码: rockasr)

使用下载的包（主要为模型数据）：
1. 下载 `ROCKASR/RK3588/RK3588_RockASR_SDK_V1.0.0_20250621.zip`
2. 解压归档文件
3. 将 `data/rk3588` 下的所有文件（模型数据）推送到目标设备的 `/data/rkaudio/asr` 目录

## 构建

```bash
cd ros2_rkros_audio  # 您的 ROS2 工作空间根目录
colcon build --packages-select rkros_asr
source install/setup.bash
```

## 节点

### rk_asr_node
- **订阅**: `rkaudio_asr` (rkros_audio_msgs/msg/RkAudio)
- **发布**: `rk_asr_result` (rkros_asr_msgs/msg/RkAsrResult), `cmd_vel` (geometry_msgs/msg/Twist)
- **功能**: 对传入的音频数据执行 ASR 处理，并发布基于语音命令的机器人移动控制

**参数:**
- `language`: 输入语言 (0=英语, 1=中文, 默认: 1)
- `data_path`: ASR 模型数据路径 (默认: 功能包数据目录)
- `auth_config`: 授权配置文件路径 (默认: 功能包配置目录)
- `model_name`: 要使用的 ASR 模型 (默认: "ASR_SS_ZH_EN")

### rk_asr_result_subscriber
- **订阅**: `rk_asr_result` (rkros_asr_msgs/msg/RkAsrResult)
- **功能**: 将 ASR 识别结果显示到控制台

## 使用方法

### 带音频输入的 ASR 运行

1. 首先，启动音频发布器（来自 rkros_audio 功能包）：
```bash
ros2 run rkros_audio audio_publisher
# 或对于 Rockchip 处理：
ros2 run rkros_audio audio_rk_processor
```

2. 启动 ASR 节点：
```bash
ros2 run rkros_asr rk_asr_node
```

3. 可选，运行结果订阅器以查看 ASR 结果：
```bash
ros2 run rkros_asr rk_asr_result_subscriber
```

### 使用自定义参数

```bash
# 使用英语支持运行
ros2 run rkros_asr rk_asr_node --ros-args -p language:=0

# 使用自定义数据路径运行
ros2 run rkros_asr rk_asr_node --ros-args -p data_path:="/custom/path/to/data"
```

## 发布的主题

- `rk_asr_result`: ASR 识别结果，包含以下字段：
  - `result`: 累计识别文本
  - `new_result`: 在此块中识别的新文本
  - `state`: 识别状态 (2: 进行中, 3: 句子结束)
  - `timestamp`: 识别事件的 ROS 时间戳

- `cmd_vel`: 用于基于语音命令的机器人移动控制的 Twist 消息，包含以下字段：
  - `linear`: 线性速度分量 (x, y, z)，单位为 m/s
  - `angular`: 角速度分量 (x, y, z)，单位为 rad/s
  - 支持的命令：前进、后退、左转、右转、停止

## 消息类型

### RkAsrResult.msg
```
string result
string new_result
int32 state
builtin_interfaces/Time timestamp
```

## 音频要求

- 采样率：16kHz
- 通道数：单声道（1 通道）
- 采样格式：16 位有符号整数（int16）
- 输入：通过使用 rkros_audio 功能包的 `rkaudio_asr` 主题接收的音频数据

## 技术细节

- 使用 RockX2 SDK 和 RockASR 模块进行语音识别
- 以 20ms 块处理音频以实现实时性能
- 流模式处理（ROCKASR_MODE_STREAM）以进行连续识别
- 自动音频缓冲区管理和分块
- 与 ROS2 参数系统的集成
- 自动清理和资源管理

## 授权

该功能包使用 Rockchip 的许可证系统。在功能包中包含了一个授权配置文件（`rockx_auth_config.json`）。当前配置使用位于 `rockx_auth_config.json` 中指定的 `/data/key_asr.lic` 许可证文件。

对于生产使用，您可能需要使用 Rockchip 授权工具生成适当的许可证文件。许可证生成方法可在文档中查看：`src/rkros_asr/doc/Rockchip_Developer_Guide_RockASR_SDK_CN.md`

`rockx_auth_config.json` 中的默认许可证配置：
```json
{ 
    "trial": 1,
    "rkauth_modules_config": [
        {
        "module": "asr",
        "license_path": "/data/key_asr.lic"
        }
    ]
}
```

使用自定义授权文件：
```bash
# 将您的自定义许可证文件放在配置目录中
# 然后使用自定义 auth_config 参数运行
ros2 run rkros_asr rk_asr_node --ros-args -p auth_config:="/path/to/custom/auth.json"
```

## 集成

此功能包设计用于与以下组件一起工作：
- `rkros_audio` 功能包用于音频输入
- `rkros_audio_msgs` 功能包用于音频消息定义
- `rkros_asr_msgs` 功能包用于 ASR 结果消息定义
- 标准 ROS2 音频消息格式
- 需要 ASR 功能的其他 ROS2 功能包
- `geometry_msgs` 功能包，用于通过语音命令控制机器人移动
- 订阅 `cmd_vel` 进行移动的机器人平台
