
# Rockchip ROS mipi sensor 取流 (rkros_mipi_cam)

# Using
1. 正确设置  "dev_id": 0, "pipe_id": 0, "channel_id": 1 等参数

2. 功能控制参数设置：
-publish_rgb： 控制发布rgb或者原始NV12数据
-enable_rkros_zcopy: 启动零拷贝消息传递（否则启动普通标准消息）

3. 依赖
    rclcpp
    OpenCV
    sensor_msgs
    rkros_rockit
    rkros_zcopy_msgs
    rclcpp_components

4. 编译
```
cd <ros_workspace>, such as: cd /root/rkros_ws
colcon build --packages-select rkros_mipi_cam
source install/setup.bash
```

5. 运行

-1.直接运行为可执行文件
```
colcon build --packages-select rkros_mipi_cam

ros2 run rkros_mipi_cam rkros_mipi_cam_node
or
ros2 launch  rkros_mipi_cam rkros_mipi_cam_1280x960.launch.py
```

-2.使用 component_container 将节点作为组件启动(用于支持 intra-process)
```
ros2 run rclcpp_components component_container --ros-args -p use_intra_process_comms:=true

ros2 component load /ComponentManager rkros_mipi_cam RockitCameraNode

```