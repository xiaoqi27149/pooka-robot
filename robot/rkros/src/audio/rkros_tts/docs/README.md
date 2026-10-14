# rkros_tts

Rockchip TTS ROS 2 package for converting text to speech using Rockchip TTS SDK.

**Note:** This package currently uses Rockchip TTS SDK version `Rockchip_RKTTS_SDK_Release_V1.1.0beta.zip`.

## Overview

This package provides a ROS 2 node that subscribes to text messages and converts them to audio using Rockchip's TTS SDK. The audio output is published to a topic that can be consumed by audio playback nodes.

## Features

- Subscribe to text input via ROS 2 topics
- Convert text to speech using Rockchip TTS SDK
- Publish audio data in RkAudio format (using rkros_audio_msgs package)
- Compatible with rkros_audio package for audio playback

## Prerequisites

- ROS 2 Rolling
- Rockchip TTS SDK dependencies
- rkros_audio package
- TTS model data (see Installation section for details)

## Installation

1. Clone the repository:
   ```bash
   cd ~/ros2_ws/src
   git clone <repository-url>
   ```

2. Download the required TTS model data from:
   https://console.box.lenovo.com/l/mu2EEe
   Extract code: rktts

   After downloading, extract the Rockchip_RKTTS_SDK_Release_V1.1.0beta.zip file,
   and copy all model files from the Rockchip_RKTTS_SDK_Release_V1.1.0beta/models/rk3588
   directory to /data/rkaudio/tts directory:
   ```bash
   # Example:
   # After extracting the downloaded zip file
   sudo mkdir -p /data/rkaudio/tts
   sudo cp /path/to/Rockchip_RKTTS_SDK_Release_V1.1.0beta/models/rk3588/* /data/rkaudio/tts/
   ```

3. Install dependencies:
   ```bash
   cd ~/ros2_ws
   rosdep install --from-paths src --ignore-src -r -y
   ```

4. Build the package:
   ```bash
   colcon build --packages-select rkros_tts
   ```

## Configuration

Before running the TTS node, you need to place the license file in the correct location:

```bash
sudo cp /path/to/your/key_tts.lic /data/
```

## Usage

1. Source the workspace:
   ```bash
   source install/setup.bash
   ```

2. Run the TTS node:
   ```bash
   ros2 run rkros_tts rktts_node
   ```

3. In another terminal, run the audio subscriber (from rkros_audio package):
   ```bash
   ros2 run rkros_audio audio_subscriber
   ```

4. Publish text messages to trigger TTS:
   ```bash
   ros2 topic pub /rktts_text std_msgs/String "data: 'Hello, this is a test of the Rockchip TTS system.'"
   ```

## Topics

- **Subscribed Topics:**
  - `/rktts_text` (std_msgs/String): Input text for TTS conversion

- **Published Topics:**
  - `/rkaudio` (rkros_audio_msgs/msg/RkAudio): Output audio data for playback

## Architecture

The package consists of:
- TTS Node: Converts text to audio using Rockchip TTS SDK
- Audio Publisher: Publishes audio data in RkAudio format
- Integration with rkros_audio package for audio playback

## Troubleshooting

- If you encounter library loading errors, ensure the environment is properly sourced
- Verify that `/data/key_tts.lic` exists and is accessible
- Check that audio_subscriber from rkros_audio package is running for audio playback

## License

Apache License 2.0
