# rkros_audio ROS2 Package

This ROS2 package is used for capturing and playing audio data through the ALSA library. It provides an audio publisher node, an audio subscriber node, and an audio processing node integrated with Rockchip audio library to enable real-time audio transmission and processing.

## Features

- Audio processing with Rockchip library
- Support for 8-channel audio input (processed to single-channel output)
- Real-time audio processing at 16kHz sample rate
- Publishes processed single-channel audio to the `rkaudio` topic
- Publishes ASR-optimized single-channel audio to `rkaudio_asr` topic
- Support for Direction of Arrival (DOA) estimation
- Real-time publishing of DOA values to the `rk_doa` topic
- Audio playback nodes for both `rkaudio` and `rkaudio_asr` topics
- Audio resampling support: automatically converts audio data of different sample rates to 16kHz for playback
- Optional WAV file recording (in.wav, out.wav, and doa.wav) with configurable parameter (default: false)
- ASR audio data publishing to the `rkaudio_asr` topic for automatic speech recognition
- Audio version information logging
- Voice Activity Detection (VAD) support with real-time status logging
- Wakeup word detection with real-time status logging

## Dependencies

- ROS2 Humble Hawksbill
- ALSA development libraries
- FFmpeg development libraries (for audio resampling)
- Rockchip audio library files (included in the package)

## Install Dependencies

```bash
sudo apt update
sudo apt install -y libasound2-dev libsamplerate0-dev libavutil-dev libswresample-dev
```

## Build

```bash
cd rkros_rk_audio
colcon build --packages-select rkros_audio
source install/setup.bash
```

## Usage

### Basic Audio Recording and Playback

#### 1. Run the basic audio publisher node (single-channel recording)

```bash
ros2 run rkros_audio audio_publisher
```

#### 2. Play back audio using subscriber node

```bash
ros2 run rkros_audio audio_subscriber
```

#### 3. Use launch file to run basic audio nodes (publisher + subscriber)

```bash
ros2 launch rkros_audio rk_audio.launch.xml
```

### Advanced Audio Processing with Rockchip Library

#### 1. Run the Rockchip audio library integrated processor node

The `audio_rk_processor` node captures 8-channel audio, processes it with the Rockchip library, and publishes processed single-channel audio to `/rkaudio` and DOA data to `/rk_doa`.

**Additional parameter for WAV file recording:**
- `save_wav_files`: Enable/disable recording of audio to WAV files (default: false)
  - When enabled, creates: `in.wav` (8-channel input), `out.wav` (processed single-channel output), and `doa.wav` (DOA data as audio)
  - Usage: `ros2 run rkros_audio audio_rk_processor --ros-args -p save_wav_files:=true`

**Method A: Using launch file (Recommended)**
```bash
# From project root directory
cd rkros_rk_audio  # Replace with your project directory
source install/setup.bash
ros2 launch rkros_audio rk_audio_rk_process.launch.xml
```

**Method B: Direct run**
```bash
# From project root directory
cd rkros_rk_audio  # Replace with your project directory
source install/setup.bash

# Run the node (library files will be loaded automatically from install directory)
ros2 run rkros_audio audio_rk_processor
```

#### 2. Alternative launch files

```bash
# Launch file for Rockchip audio processing with environment setup
ros2 launch rkros_audio rk_audio_process_with_env.launch.xml

# Launch file for Rockchip audio processing only
ros2 launch rkros_audio rk_audio_process.launch.xml
```

### Playing Processed Audio

Play audio regardless of source (basic recording or Rockchip processing):

```bash
# Use this node to play audio from rkaudio topic
ros2 run rkros_audio audio_subscriber
```

Play ASR-optimized audio from the `rkaudio_asr` topic:

```bash
# Use this node to play ASR-optimized audio (using remap functionality)
ros2 run rkros_audio audio_subscriber --ros-args --remap rkaudio:=rkaudio_asr
```

## Published Topics

- `rkaudio`: Audio data topic
  - Basic publisher: single channel, 16kHz, 16-bit
  - Rockchip processor: single channel (algorithm processed from 8-channel input), 16kHz, 16-bit
- `rkaudio_asr`: ASR (Automatic Speech Recognition) audio data topic, published by `audio_rk_processor` only
  - Contains audio data specifically processed for speech recognition, same format as `rkaudio` topic
- `rk_doa`: Direction of Arrival estimation (degrees), published by `audio_rk_processor` only

## Message Types

- `RkAudio.msg`: Custom audio message type (defined in rkros_audio_msgs package)
  - `sample_rate`: Sample rate
  - `channels`: Number of channels
  - `bits_per_sample`: Bits per sample
  - `audio_data`: Audio data byte array

The audio subscriber node now supports audio data with different sample rates and automatically resamples them to 16kHz for playback.

## Configuration

Audio parameters are set by default to:
- Sample rate: 16000 Hz
- Channels: 1 (mono) for basic publisher, 8 for Rockchip processor
- Bit depth: 16-bit

Node parameters:
- `save_wav_files`: Boolean parameter to enable/disable WAV file recording (default: false)

Rockchip audio processing is configured via the JSON configuration file (`rk_audio_hci.json`) which includes parameters for:
- Microphone array configuration (6 microphones by default)
- AEC, AIEC, AINR, DOA, and beamforming levels
- Sample rate and frame length

### Audio 3A Processing License
- The audio processing functionality requires a license file: `/data/key_audio3a.lic`
- This license is required for advanced audio processing features including DOA (Direction of Arrival), AEC (Acoustic Echo Cancellation), AIEC (Acoustic Impulse/Echo Cancellation), and other audio enhancement features
- Make sure to place the `key_audio3a.lic` file in the `/data/` directory to enable full audio processing capabilities

## Troubleshooting

If you encounter audio permission issues, try:

```bash
sudo usermod -a -G audio $USER
```

Then log out and log back in or restart the system.

If you have issues with the Rockchip library, ensure the library files are properly installed in the package share directory:
```
install/rkros_audio/share/rkros_audio/rk_lib/
```
