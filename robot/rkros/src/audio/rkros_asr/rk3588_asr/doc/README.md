# rkros_asr ROS2 Package

## Introduction

The `rkros_asr` package is a ROS2 package that provides automatic speech recognition (ASR) functionality using Rockchip's RockX2 SDK. It subscribes to audio data from the `rkaudio_asr` topic, processes it using Rockchip's ASR engine, and publishes recognition results to the `rk_asr_result` topic.

## Features

- Real-time ASR processing of audio streams from ROS2 topics
- Support for both English and Chinese languages
- Stream mode processing for real-time recognition
- Integration with RockX2 SDK and RockASR libraries
- Custom message types for ASR results with state information
- Parameter-based configuration for language, data paths, and model selection
- 16kHz audio sample rate support with 16-bit samples
- **New: Voice command control for robot movement** - Recognizes voice commands and publishes Twist messages to control robot movement

## Dependencies

This package requires:
- RK35XX_RockASR_SDK_V1.0.0_20250621
- RK35XX_ROCKX2_SDK_V1.0.0_20250621
- rkros_audio_msgs ROS2 package (for audio message definitions)
- rkros_asr_msgs ROS2 package (for ASR result message definitions)

The RockX2 SDK should be located in the package directory as `RK3588_ROCKX2_SDK_V1.0.0_20250621`

Download link for RK35XX_RockASR_SDK_V1.0.0_20250621 (mainly contains model data):
[ROCKASR SDK Download](https://console.box.lenovo.com/l/T55yZw) (Access code: rockasr)

To use the downloaded package (mainly model data):
1. Download `ROCKASR/RK3588/RK3588_RockASR_SDK_V1.0.0_20250621.zip`
2. Extract the archive
3. Push all files from `data/rk3588` (model data) to `/data/rkaudio/asr` on your target device

## Build

```bash
cd ros2_rkros_audio  # Your ROS2 workspace root
colcon build --packages-select rkros_asr
source install/setup.bash
```

## Nodes

### rk_asr_node
- **Subscribes to**: `rkaudio_asr` (rkros_audio_msgs/msg/RkAudio)
- **Publishes to**: `rk_asr_result` (rkros_asr_msgs/msg/RkAsrResult), `cmd_vel` (geometry_msgs/msg/Twist)
- **Function**: Performs ASR processing on incoming audio data and publishes voice command control for robot movement

**Parameters:**
- `language`: Input language (0=English, 1=Chinese, default: 1)
- `data_path`: Path to ASR model data (default: package data directory)
- `auth_config`: Path to authorization config file (default: package config directory)
- `model_name`: ASR model to use (default: "ASR_SS_ZH_EN")

### rk_asr_result_subscriber
- **Subscribes to**: `rk_asr_result` (rkros_asr_msgs/msg/RkAsrResult)
- **Function**: Displays ASR recognition results to console

## Usage

### Running ASR with Audio Input

1. First, start the audio publisher (from rkros_audio package):
```bash
ros2 run rkros_audio audio_publisher
# or for Rockchip processing:
ros2 run rkros_audio audio_rk_processor
```

2. Start the ASR node:
```bash
ros2 run rkros_asr rk_asr_node
```

3. Optionally, run the result subscriber to view ASR results:
```bash
ros2 run rkros_asr rk_asr_result_subscriber
```

### Using Custom Parameters

```bash
# Run with English language support
ros2 run rkros_asr rk_asr_node --ros-args -p language:=0

# Run with custom data path
ros2 run rkros_asr rk_asr_node --ros-args -p data_path:="/custom/path/to/data"
```

## Published Topics

- `rk_asr_result`: ASR recognition results with the following fields:
  - `result`: Cumulative recognized text
  - `new_result`: New text recognized in this chunk
  - `state`: Recognition state (2: ongoing, 3: end of sentence)
  - `timestamp`: ROS timestamp of the recognition event

- `cmd_vel`: Twist messages for robot movement control based on voice commands with the following fields:
  - `linear`: Linear velocity components (x, y, z) in m/s
  - `angular`: Angular velocity components (x, y, z) in rad/s
  - Supports commands: forward, backward, turn left, turn right, stop

## Message Types

### RkAsrResult.msg
```
string result
string new_result
int32 state
builtin_interfaces/Time timestamp
```

## Audio Requirements

- Sample rate: 16kHz
- Channels: Mono (1 channel)
- Sample format: 16-bit signed integers (int16)
- Input: Audio data received via `rkaudio_asr` topic using rkros_audio package

## Technical Details

- Uses RockX2 SDK with RockASR module for speech recognition
- Processes audio in 20ms chunks for real-time performance
- Stream mode processing (ROCKASR_MODE_STREAM) for continuous recognition
- Automatic audio buffer management and chunking
- Integration with ROS2 parameter system for configuration
- Automatic cleanup and resource management

## Authorization

The package uses Rockchip's license system. An authorization configuration file (`rockx_auth_config.json`) is included in the package. The current configuration uses a license file located at `/data/key_asr.lic` as specified in the `rockx_auth_config.json`.

For production use, you may need to generate proper license files using the Rockchip authorization tools. The license generation method can be found in the documentation: `src/rkros_asr/doc/Rockchip_Developer_Guide_RockASR_SDK_CN.md`

The default license configuration in `rockx_auth_config.json`:
```json
{ 
    "trial": 1,
    "rkauth_modules_config": [
        {
        "module": "asr",
        "license_path": "/data/key_asr.lic"
        }
    ]
}
```

To use a custom authorization file:
```bash
# Place your custom license file in the config directory
# Then run with custom auth_config parameter
ros2 run rkros_asr rk_asr_node --ros-args -p auth_config:="/path/to/custom/auth.json"
```

## Integration

This package is designed to work with:
- `rkros_audio` package for audio input
- `rkros_audio_msgs` package for audio message definitions
- `rkros_asr_msgs` package for ASR result message definitions
- Standard ROS2 audio message formats
- Other ROS2 packages that need ASR capabilities
- `geometry_msgs` package for robot movement control via voice commands
- Robot platforms that subscribe to `cmd_vel` for movement
