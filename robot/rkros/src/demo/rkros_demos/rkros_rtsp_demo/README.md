# rkros_rtsp_demo

ROS2 package for Rockchip platform demonstrating VI-VPS-VENC-RTSP pipeline with optional encoded stream publishing.

## Overview

This package demonstrates a full video processing pipeline on Rockchip platforms:
1. Video input capture (VI)
2. Video processing (VPSS)
3. Video encoding (VENC)
4. RTSP streaming
5. Optional ROS2 topic publishing of encoded streams

The package contains two main executables:
- `video_rtsp`: Main demo node that captures camera input, encodes it, streams via RTSP, and optionally publishes encoded data to a ROS2 topic
- `h264_recorder`: Subscriber node that records encoded stream from a ROS2 topic to disk

## Features

- Support for multiple video codecs (H.264, H.265, MJPEG)
- Configurable video resolution
- RTSP streaming capability
- Optional publishing of encoded video data to ROS2 topics
- H.264 stream recorder

## Nodes

### video_rtsp

Main demonstration node implementing the VI-VPSS-VENC pipeline.

#### Published Topics

- `/camera/encoded` (sensor_msgs/msg/Image, optional) - Encoded video stream (only published when enabled)

#### Parameters

- `width` (int, default: 1920) - Input video width
- `height` (int, default: 1080) - Input video height
- `disp_width` (int, default: 1280) - Display/output width
- `disp_height` (int, default: 720) - Display/output height
- `channel_id` (int, default: 0) - Camera channel ID
- `encode_type` (string, default: "h264") - Encoding type ("h264", "h265", or "mjpeg")
- `frame_count` (int, default: -1) - Number of frames to process (-1 for infinite)
- `enable_publish_encoder` (bool, default: true) - Enable/disable publishing of encoded stream to ROS2 topic

#### RTSP Streaming

The node streams encoded video via RTSP on port 554 at path `/live/0`.

### h264_recorder

Subscriber node that records H.264 streams from a ROS2 topic to disk.

#### Subscribed Topics

- Configurable topic (default: `/camera/encoded`) - Encoded video stream

#### Parameters

- `output_directory` (string, default: "/tmp") - Directory to save recorded files
- `filename_prefix` (string, default: "record") - Prefix for recorded filenames
- `subscription_topic` (string, default: "/camera/encoded") - Topic to subscribe to for encoded stream

## Usage

### Running the main demo

```bash
ros2 run rkros_rtsp_demo video_rtsp
```

With custom parameters:
```bash
ros2 run rkros_rtsp_demo video_rtsp --ros-args -p width:=1280 -p height:=720 -p encode_type:="h265"
```

To disable ROS2 topic publishing of encoded stream:
```bash
ros2 run rkros_rtsp_demo video_rtsp --ros-args -p enable_publish_encoder:=false
```

### Recording encoded streams

```bash
ros2 run rkros_rtsp_demo h264_recorder
```

With custom parameters:
```bash
ros2 run rkros_rtsp_demo h264_recorder --ros-args -p output_directory:="/tmp" -p filename_prefix:="record_001"
```

### Using launch files

```bash
ros2 launch rkros_rtsp_demo video_rtsp.launch.py
```

## Dependencies

- Rockchip ROS2 Rockit package (`rkros_rockit`)
- OpenCV
- sensor_msgs
- rclcpp

## Building

Standard ROS2 colcon build process:

```bash
colcon build --packages-select rkros_rtsp_demo
```

## Notes

- The `enable_publish_encoder` parameter allows you to control whether the node publishes encoded video data to the ROS2 topic. When disabled, no publisher is created and no encoded data is published, saving bandwidth and computational resources.
- Make sure your Rockchip platform has the necessary hardware blocks (VI, VPSS, VENC) enabled and configured.
- RTSP streaming is always active regardless of the `enable_publish_encoder` setting.