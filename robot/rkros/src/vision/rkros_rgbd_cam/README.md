# rkros_rgbd_cam Driver

    ROS2 driver for rgbd camera, that publishes RGB / depth images and pointsclouds to ROS2 topics.
    Currently supported model: rmsl321.
    Default mode : sync node, currently in this mode, maintain consistent frame rates for color and depth streams.

## Features
- Publishes RGB images to `/rmsl321_cam/color/image_rgb`
- Publishes depth images to `/rmsl321_cam/depth/image_depth`
- Publishes pointsclouds to `/rmsl321_cam/depth/points`, optional RGB-D mode to `/rmsl321_cam/depth_registered/points_aligned`
- Configurable resolution and frame rate
- Supports UVC camera devices

## Topics
```
/rmsl321_cam/color/camera_info
/rmsl321_cam/color/image_rgb
/rmsl321_cam/depth/camera_info
/rmsl321_cam/depth/depth_pseudo
/rmsl321_cam/depth/image_depth
/rmsl321_cam/depth/points
/rmsl321_cam/depth_registered/depth_aligned
/rmsl321_cam/depth_registered/points_aligned
```

## Build

```bash
# install udev depends
sudo apt update
sudo apt install libudev-dev

# cd ros workspace
cd ~/rkros_ws
source install/setup.bash
colcon build --packages-select rkros_rgbd_cam
source install/setup.bash
```

## Run

### 1. using exec file
Using launch file:
```bash
ros2 launch rkros_rgbd_cam rk_rmsl321_640x540.launch.py
ros2 launch rkros_rgbd_cam rk_rmsl321_640x540.launch.py enable_pointcloud:=false enable_depth_aligned:=true enable_pointcloud_rgbd:=true
```
or
```bash
ros2 run rkros_rgbd_cam rk_rmsl321_node
```
### 2. using component startup

```
ros2 launch rkros_rgbd_cam container_rmsl321_640x540.launch.py
or
ros2 launch rkros_rgbd_cam rk_rmsl321_640x540.launch.py enable_pointcloud:=true unused_component:=false
```

### 3. More configuration options
to list all args of node
```
ros2 launch rkros_rgbd_cam rk_rmsl321_640x540.launch.py --show-args
```

### 4. notes
- if enable_depth_aligned, make sure: enable_depth:=true
- if enable_pointcloud_aligned, make sure: enable_depth:=true enable_depth_aligned:=true
- to enable RGBD pointscloud, make sure enable_pointcloud_rgbd:=true enable_depth_:=true enable_depth_aligned:=true

## Launch Parameters
The following are the available launch parameters:
- rgb_width: set color stream width
- rgb_height: set color stream height
- cam_name: set the current camera name, it will be reflected in the ROS data topic
- unused_component: for launch.py file, default true, using exec startup.

- enable_color: it is enabled by default and is not recommended to be disabled
- enable_depth: it is enabled by default and is not recommended to be disabled
- enable_depth_pseudo: deep pseudo-color display, used for previewing effects, turn on when needed
- enable_depth_aligned: enable depth alignment to RGB
- enable_pointcloud: enable point cloud (unaligned)
- enable_pointcloud_aligned: enable point cloud (aligned to rgb)
- enable_pointcloud_rgbd: to enable RGBD point cloud, you need to make sure enable_depth_aligned:=true

To achieve optimal performance, please activate the corresponding theme data according to actual needs

## Dependencies

- OpenCV
- ROS2 Humble
- sensor_msgs
- cv_bridge
- image_transport


## File Structure

```
rkros_rgbd_cam/
├── CMakeLists.txt
├── include
│   ├── rmsl32x_calib.h
│   ├── rmsl32x_node.h
│   ├── rmsl32x_param.h
│   ├── rmsl32x_usb.h
│   ├── rmsl_core_version.h
│   ├── rmsl_image_processor.h
│   ├── rmsl_logger.h
│   └── rmsl_uvc.h
├── launch
│   ├── container_rmsl321_performance_640x540.launch.py
│   ├── rmsl321_performance_1280x1080.launch.py
│   ├── rmsl321_performance_640x540.launch.py
│   └── rmsl321_quality_1280x1080.launch.py
├── lib
│   └── librmsl_core.so
├── LICENSE
├── package.xml
├── README.md
└── src
    ├── list_device_node.cpp
    ├── rmsl32x_node.cpp
    ├── rmsl32x_param.cpp
    └── switch_model.cpp
```

## Notes

- The camera device should be accessible via UVC protocol
- Make sure the user has permission to access video devices (`/dev/video*`)
- The depth images are published in 16-bit unsigned format (CV_16UC1)
- RGB images are published in RGB8 format
