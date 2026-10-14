# rkros_led ROS2 Package

## Introduction

The `rkros_led` package is a ROS2 package for controlling LEDs based on DOA (Direction of Arrival) data. The package subscribes to the `rk_doa` topic and lights up the corresponding LED groups according to angle data in the range of 0-360 degrees.

## Features

- Subscribes to the `rk_doa` topic (std_msgs/Float32 type, range 0-360 degrees)
- Divides the 360-degree space into configurable number of regions, with support for:
  - 8 regions (default): each corresponding to a group of 3 LEDs (total 24 LEDs), each sector covers 45° with ±22.5° detection range
  - 12 regions: each corresponding to a group of 3 LEDs (total 36 LEDs), each sector covers 30° with ±15° detection range
- Lights up the LEDs in the corresponding region based on the DOA value
- For enhanced visualization, adjacent sector LEDs are also lit when DOA is close to sector boundaries, this feature can be controlled with the macro definition `ENABLE_DOA_BORDER_EFFECT` and is disabled by default
- Compatible with `rkros_audio` package: receives real-time DOA data from audio processing
- Provides a test node that publishes simulated DOA data
- Proper wrap-around handling for circular cases (e.g., angles near 0° and 360°)
- Automatic cleanup: turns off all LEDs when the node exits

## Installation and Build

```bash
# In your ROS2 workspace
cd ~/ros2_ws/src
# Copy or link the rkros_led package to the src directory
cd ~/ros2_ws
colcon build --packages-select rkros_led
source install/setup.bash
```

## Running

### Start the LED Controller

```bash
# Default: 8 LED groups (for 8mic board)
ros2 run rkros_led led_controller

# With 12 LED groups (for 6mic board)
ros2 run rkros_led led_controller --ros-args -p led_group_num:=12
```

### Start the DOA Test Publisher

```bash
ros2 run rkros_led doa_publisher
```

### Manually Publish DOA Values for Testing

```bash
# Publish a DOA value (e.g., 90 degrees)
ros2 topic pub /rk_doa std_msgs/Float32 "{data: 90.0}"
```

## Technical Details

- Supports configurable number of sectors:
  - 8 sectors (default): each corresponds to a group of 3 LEDs (total 24 LEDs), each sector covers 45° with ±22.5° detection range
  - 12 sectors: each corresponds to a group of 3 LEDs (total 36 LEDs), each sector covers 30° with ±15° detection range
- Each sector is centered at multiples of the angle step (45° for 8 sectors, 30° for 12 sectors)
  - For 8 sectors: 0°, 45°, 90°, ..., 315°
  - For 12 sectors: 0°, 30°, 60°, ..., 330°
- Hardware mapping:
  - For 8 sectors: logical sectors 0-7 map directly to physical LED groups 0-7 (1, 2, 3, 4, 5, 6, 7, 8)
  - For 12 sectors: logical sectors 0-11 map to physical LED groups in order 2, 3, 4, ..., 11, 0, 1 (3, 4, 5, ..., 12, 1, 2)
- When a DOA value falls within a sector, the corresponding LED group is lit with red color (R=255, G=0, B=0)
- For enhanced visualization, if the DOA value is close to a sector boundary, the adjacent sector LED group is also lit with lower intensity, this feature can be controlled with the macro definition `ENABLE_DOA_BORDER_EFFECT` and is disabled by default
- Proper handling of circular nature of angles: angles that cross 0°/360° boundary are correctly handled
- Uses direct access to system LED interfaces: `/sys/class/leds/led[1-N]/brightness` where N is the total number of LEDs (24 or 36)
- ROS2 Parameter: `led_group_num` (default: 8) - controls the number of LED groups to use

## Nodes

### led_controller

- **Subscribes to**: `rk_doa` (std_msgs/Float32)
- **Function**: Controls LEDs based on DOA data, with enhanced visualization for boundary cases

### doa_publisher

- **Publishes**: `rk_doa` (std_msgs/Float32)
- **Function**: Randomly publishes DOA values for testing (every 2 seconds)

## Hardware Interface

This package directly accesses system LED interfaces: `/sys/class/leds/led[1-36]/brightness`

If you encounter brightness permission issues when running the package, please execute the following command to set the appropriate permissions:

```bash
# From the rkros_led package directory
sudo ./scripts/change_led_permissions.sh
```

## Dependencies

- ROS2 Humble Hawksbill
- rclcpp
- std_msgs
- builtin_interfaces
