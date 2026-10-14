# rkros_tts

基于瑞芯微TTS SDK的ROS 2文本转语音包。

**注意：** 本包当前使用瑞芯微TTS SDK版本 `Rockchip_RKTTS_SDK_Release_V1.1.0beta.zip`。

## 概述

该包提供了一个ROS 2节点，订阅文本消息并使用瑞芯微TTS SDK将其转换为语音。音频输出发布到一个话题，可以被音频播放节点消费。

## 功能特性

- 通过ROS 2话题订阅文本输入
- 使用瑞芯微TTS SDK将文本转换为语音
- 以RkAudio格式发布音频数据（使用rkros_audio_msgs包）
- 兼容rkros_audio包进行音频播放

## 先决条件

- ROS 2 Rolling
- 瑞芯微TTS SDK依赖
- rkros_audio包
- TTS模型数据（详见安装部分）

## 安装

1. 克隆仓库：
   ```bash
   cd ~/ros2_ws/src
   git clone <repository-url>
   ```

2. 下载必需的TTS模型数据，下载链接为：
   https://console.box.lenovo.com/l/mu2EEe
   提取码: rktts

   下载后解压Rockchip_RKTTS_SDK_Release_V1.1.0beta.zip文件，
   并将Rockchip_RKTTS_SDK_Release_V1.1.0beta/models/rk3588目录下的所有模型文件
   复制到/data/rkaudio/tts目录：
   ```bash
   # 示例：
   # 解压下载的zip文件后
   sudo mkdir -p /data/rkaudio/tts
   sudo cp /path/to/Rockchip_RKTTS_SDK_Release_V1.1.0beta/models/rk3588/* /data/rkaudio/tts/
   ```

3. 安装依赖：
   ```bash
   cd ~/ros2_ws
   rosdep install --from-paths src --ignore-src -r -y
   ```

4. 构建包：
   ```bash
   colcon build --packages-select rkros_tts
   ```

## 配置

在运行TTS节点之前，您需要将许可证文件放置在正确的位置：

```bash
sudo cp /path/to/your/key_tts.lic /data/
```

## 使用方法

1. 源码工作空间：
   ```bash
   source install/setup.bash
   ```

2. 运行TTS节点：
   ```bash
   ros2 run rkros_tts rktts_node
   ```

3. 在另一个终端中，运行音频订阅者（来自rkros_audio包）：
   ```bash
   ros2 run rkros_audio audio_subscriber
   ```

4. 发布文本消息以触发TTS：
   ```bash
   ros2 topic pub /rktts_text std_msgs/String "data: '你好，这是瑞芯微TTS系统的测试。'"
   ```

## 话题

- **订阅话题：**
  - `/rktts_text` (std_msgs/String): TTS转换的输入文本

- **发布话题：**
  - `/rkaudio` (rkros_audio_msgs/msg/RkAudio): 用于播放的输出音频数据

## 架构

该包包含：
- TTS节点：使用瑞芯微TTS SDK将文本转换为音频
- 音频发布者：以RkAudio格式发布音频数据
- 与rkros_audio包集成进行音频播放

## 故障排除

- 如果遇到库加载错误，请确保环境已正确源码
- 验证`/data/key_tts.lic`是否存在且可访问
- 检查rkros_audio包中的audio_subscriber是否正在运行以进行音频播放

## 许可证

Apache License 2.0
