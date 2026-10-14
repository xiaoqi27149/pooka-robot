# rkros_audio_msgs

这是一个 ROS 2 消息包，提供用于音频数据处理的自定义消息定义。

## 概述

`rkros_audio_msgs` 包包含用于 Rockchip 音频处理系统的自定义消息类型。这个包的主要目的是将音频消息定义独立出来，以便多个包可以共享使用，而不必依赖完整的音频处理功能。

## 消息类型

### RkAudio.msg

定义了音频数据及其元数据的结构：

```
uint32 sample_rate
uint32 channels
uint32 bits_per_sample
uint8[] audio_data
```

- `sample_rate`: 音频采样率 (Hz)
- `channels`: 音频通道数
- `bits_per_sample`: 每个采样点的位数
- `audio_data`: 实际的音频数据字节数组

## 依赖

- `std_msgs`: ROS 2 标准消息类型

## 使用方法

在其他 ROS 2 包中使用此消息类型的示例：

```cpp
#include "rkros_audio_msgs/msg/rk_audio.hpp"

// 创建消息实例
rkros_audio_msgs::msg::RkAudio audio_msg;
audio_msg.sample_rate = 44100;
audio_msg.channels = 2;
audio_msg.bits_per_sample = 16;

// 填充音频数据
audio_msg.audio_data = {0x01, 0x02, 0x03, ...};
```

## 构建

要构建此包，首先确保已安装 ROS 2 和相关依赖，然后执行：

```bash
colcon build --packages-select rkros_audio_msgs
```

## 许可证

Apache-2.0