# rkros_led ROS2 功能包

## 简介

`rkros_led` 功能包是一个基于 DOA (到达方向) 数据控制 LED 的 ROS2 功能包。该功能包订阅 `rk_doa` 主题，并根据 0-360 度范围内的角度数据显示点亮相应的 LED 组。

## 功能

- 订阅 `rk_doa` 主题 (std_msgs/Float32 类型, 范围 0-360 度)
- 将 360 度空间划分为可配置数量的区域，支持：
  - 8 个区域 (默认)：每个区域对应一组 3 个 LED (总共 24 个 LED)，每个扇区覆盖 45°，±22.5° 检测范围
  - 12 个区域：每个区域对应一组 3 个 LED (总共 36 个 LED)，每个扇区覆盖 30°，±15° 检测范围
- 根据 DOA 值点亮相应区域内的 LED
- 为了增强可视化效果，当 DOA 接近扇区边界时，相邻扇区的 LED 也会点亮 (可通过宏定义 `ENABLE_DOA_BORDER_EFFECT` 控制，默认关闭)
- 与 `rkros_audio` 功能包兼容：接收来自音频处理的实时 DOA 数据
- 提供一个测试节点，发布模拟的 DOA 数据
- 对圆形情况 (例如接近 0° 和 360° 的角度) 进行适当的环绕处理
- 自动清理：节点退出时自动关闭所有 LED

## 安装和构建

```bash
# 在您的 ROS2 工作空间中
cd ~/ros2_ws/src
# 将 rkros_led 功能包复制或链接到 src 目录
cd ~/ros2_ws
colcon build --packages-select rkros_led
source install/setup.bash
```

## 运行

### 启动 LED 控制器

```bash
# 默认：8 组 LED (用于 8mic 板)
ros2 run rkros_led led_controller

# 使用 12 组 LED (用于 6mic 板)
ros2 run rkros_led led_controller --ros-args -p led_group_num:=12
```

### 启动 DOA 测试发布器

```bash
ros2 run rkros_led doa_publisher
```

### 手动发布 DOA 值进行测试

```bash
# 发布一个 DOA 值 (例如 90 度)
ros2 topic pub /rk_doa std_msgs/Float32 "{data: 90.0}"
```

## 技术细节

- 支持可配置数量的扇区：
  - 8 个扇区 (默认)：每个扇区对应一组 3 个 LED (总共 24 个 LED)，每个扇区覆盖 45°，±22.5° 检测范围
  - 12 个扇区：每个扇区对应一组 3 个 LED (总共 36 个 LED)，每个扇区覆盖 30°，±15° 检测范围
- 每个扇区以角度步长的倍数为中心 (8 扇区为 45°，12 扇区为 30°)
  - 8 扇区：0°, 45°, 90°, ..., 315°
  - 12 扇区：0°, 30°, 60°, ..., 330°
- 硬件映射：
  - 8 扇区：逻辑扇区 0-7 直接映射到物理 LED 组 0-7 (1, 2, 3, 4, 5, 6, 7, 8)
  - 12 扇区：逻辑扇区 0-11 映射到物理 LED 组，顺序为 2, 3, 4, ..., 11, 0, 1 (3, 4, 5, ..., 12, 1, 2)
- 当 DOA 值落在扇区内时，相应的 LED 组以红色点亮 (R=255, G=0, B=0)
- 为了增强可视化效果，如果 DOA 值接近扇区边界，相邻扇区的 LED 组也会以较低亮度点亮，此功能可通过宏定义 `ENABLE_DOA_BORDER_EFFECT` 控制，默认关闭
- 对角度的圆形特性进行适当处理：跨越 0°/360° 边界的角度假设有正确处理
- 直接访问系统 LED 接口：`/sys/class/leds/led[1-N]/brightness`，其中 N 是 LED 总数 (24 或 36)
- ROS2 参数：`led_group_num` (默认: 8) - 控制要使用的 LED 组数量

## 节点

### led_controller

- **订阅**: `rk_doa` (std_msgs/Float32)
- **功能**: 根据 DOA 数据控制 LED，包括边界情况的增强可视化

### doa_publisher

- **发布**: `rk_doa` (std_msgs/Float32)
- **功能**: 随机发布 DOA 值用于测试 (每 2 秒一次)

## 硬件接口

该功能包直接访问系统 LED 接口：`/sys/class/leds/led[1-36]/brightness`

如果您在运行功能包时遇到亮度权限问题，请执行以下命令设置适当权限：

```bash
# 在 rkros_led 功能包目录中
sudo ./scripts/change_led_permissions.sh
```

## 依赖项

- ROS2 Humble Hawksbill
- rclcpp
- std_msgs
- builtin_interfaces
