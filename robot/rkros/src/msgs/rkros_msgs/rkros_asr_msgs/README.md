# rkros_asr_msgs

## 简介

`rkros_asr_msgs` 包是一个 ROS2 消息包，为 Rockchip 自动语音识别（ASR）功能提供消息定义。此包仅包含 Rockchip ASR 系统中其他包使用的消息定义。

## 消息类型

### RkAsrResult.msg

此消息包含 ASR（自动语音识别）处理的结果：

```text
string result          # 非流式识别结果（完整句子）
string new_result      # 流式识别结果（当前片段）
int32 state           # 识别状态（2：进行中，3：句子结束）
builtin_interfaces/Time timestamp  # 识别事件的 ROS 时间戳
```

## 使用方法

此包被以下包作为依赖项使用：
- `rkros_asr` - 主要 ASR 处理包
- 需要发布或订阅 ASR 结果的任何其他包

### 在其他包中使用消息

在您的 ROS2 包中使用此消息定义：

**C++ 示例：**
```cpp
#include "rkros_asr_msgs/msg/rk_asr_result.hpp"

// 创建消息实例
auto asr_result_msg = rkros_asr_msgs::msg::RkAsrResult();
asr_result_msg.result = "最终识别结果";      // 非流式识别结果（完整句子）
asr_result_msg.new_result = "当前片段";     // 流式识别结果（当前片段）
asr_result_msg.state = 3;  // 识别结束

// 发布消息
auto publisher = this->create_publisher<rkros_asr_msgs::msg::RkAsrResult>("rk_asr_result", 10);
publisher->publish(asr_result_msg);
```
## 依赖项

- `builtin_interfaces` - 用于 Time 消息类型

## 构建

```bash
cd /path/to/your/ros2/workspace
colcon build --packages-select rkros_asr_msgs
source install/setup.bash
```

## 集成

此包提供标准化消息定义，以确保 ASR 相关 ROS2 节点之间的一致通信。通过拥有独立的消息包，它可以实现：

- 在不同 ASR 实现之间的可重用性
- 在不同硬件平台（RK3588、RK182X 等）上的一致消息格式
- 消息定义的独立版本控制
- 代码库中更清晰的关注点分离
