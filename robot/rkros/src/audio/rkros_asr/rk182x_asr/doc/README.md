# ASR ROS2 Package

这是一个基于Rockchip芯片的ASR（自动语音识别）ROS2包。该包订阅来自`rkaudio_asr`的音频数据，进行语音识别处理，并将识别结果发布到`rk_asr_result`话题。

## 功能特性

- 订阅`rkaudio_asr`话题获取音频数据
- 使用Rockchip的RockX2 SDK进行ASR处理
- 将ASR识别结果发布到`rk_asr_result`话题
- 支持中英文语音识别
- 支持语音命令控制机器人移动
- 支持流式语音识别处理
- 自动检测语音结束并发送结束标记

## 依赖项

- ROS2 Humble
- rkros_audio_msgs包（提供音频消息定义）
- rkros_asr_msgs包（提供ASR结果消息定义）
- Rockchip RockX2 SDK

## 编译

**注意**：此包默认被COLCON_IGNORE文件忽略，如需编译需先删除该文件。

```bash
# 删除忽略文件以启用编译
rm COLCON_IGNORE

cd ~/zzc_175
source /opt/ros/humble/setup.bash
colcon build --packages-select rkros_asr
source install/setup.bash

# 编译完成后，如需恢复忽略状态，可重新创建忽略文件
# touch COLCON_IGNORE
```

## 运行

```bash
# 运行ASR节点
ros2 run rkros_asr rk_asr_node

# 运行ASR结果订阅者节点（用于打印ASR输出）
ros2 run rkros_asr rk_asr_result_subscriber
```

## 参数

- `language`: 语言设置（0=英文，1=中文，默认为1）
- `data_path`: 模型数据路径（默认为/data/rkaudio/asr_182x，需要手动拷贝相关数据）
- `auth_config`: 认证配置文件路径（默认为package share目录下的config/rockx_auth_config.json）
- `model_name`: 模型名称（默认为ASR）

## 话题

- 订阅: `rkaudio_asr` (rkros_audio_msgs/msg/RkAudio) - 音频数据
- 发布: `rk_asr_result` (rkros_asr_msgs/msg/RkAsrResult) - ASR识别结果
- 发布: `cmd_vel` (geometry_msgs/msg/Twist) - 机器人运动控制

## 消息类型

### RkAsrResult

```text
string result          # 识别结果
string new_result      # 新的识别结果
int32 state           # 识别状态
builtin_interfaces/Time timestamp  # 时间戳
```

## 注意事项

1. 确保Rockchip硬件平台已正确安装并配置了RockX2 SDK
2. 需要先运行音频采集节点以提供`rkaudio_asr`话题数据
3. 确保有足够的计算资源进行实时ASR处理
4. 语音结束后约2秒会自动发送结束标记，以完成识别会话
5. 需要将模型数据手动拷贝到`/data/rkaudio/asr_182x`目录
6. 确保认证配置文件在正确位置
7. 默认情况下，该包被COLCON_IGNORE文件忽略，如需编译请先删除此文件