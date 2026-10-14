
# rkros_vcam

一个虚拟相机 ROS2 包。它在独立线程中按指定帧率生成图片（白色背景带一个不断移动的黑色小块），并可按配置将图像发布为不同的 topic、不同的格式（rgb / yuv / jpeg）和不同的消息类型（ROS 自带的 `sensor_msgs::msg::Image` 或自定义的 `rkros_dmabuf::ImageMsg`）。

## 功能要点
- 支持按 topic 配置不同格式（rgb、yuv、jpeg）和消息类型（sensor / dmabuf）。
- 在独立线程中生成图像（生成帧率 `gen_fps` 可配置），发布由 `publish_fps` 控制。
- 生成器直接输出目标格式（避免发布时再转换），并且只有在有订阅者时才会真实发布，从而节省资源。

## 参数说明
（可通过 `ros2 param set` 设置）

- `publish_fps` (double): 发布频率（默认 30.0）。
- `gen_fps` (double): 生成器帧率（默认 30.0）。
- `width` (int): 图像宽度（默认 640）。
- `height` (int): 图像高度（默认 480）。
- `topics` (list[string]): 每项为字符串，格式为 `"<topic>:<format>:<msg_type>"`。
  - `<format>` 支持 `rgb`、`yuv`、`jpeg`。
  - `<msg_type>` 支持 `sensor`（`sensor_msgs::msg::Image`）或 `dmabuf`（`rkros_dmabuf::ImageMsg`）。

## 构建
在工作区根目录下执行：
```bash
cd /home/linaro/rkros_ws
source /opt/ros/humble/setup.bash
colcon build --packages-select rkros_vcam
source install/setup.bash
```

## 运行
推荐使用提供的 launch 文件：
```bash
ros2 launch rkros_vcam vcam_launch.py
```
该 launch 会启动一个 `ComposableNodeContainer` 并加载 `rkros_vcam::VCamNode`，参数已经配置在`vcam_launch.py`文件中。

手动运行（可用于调试）：
```bash
# 启动 component container
ros2 run rclcpp_components component_container

# 在另一个终端加载组件
ros2 component load /ComponentManager rkros_vcam rkros_vcam::VCamNode
```

订阅/查看图像：
```bash
ros2 topic list
# 使用 rqt_image_view 或其它工具订阅对应 topic
```

## 调试提示
- 找不到 launch 文件：确认已 `colcon build` 并 `source install/setup.bash`。
- 组件加载失败（plugin 未注册）：确认包已正确构建并安装（`colcon build`），且 `rclcpp_components` 可用。
- dmabuf 相关：`rkros_dmabuf::DmaBuf::allocate` 依赖平台支持，若分配失败会在日志中报错并跳过发布。
