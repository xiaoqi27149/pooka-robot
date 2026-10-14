# pooka-robot

飞凌 Forlinx OK3588-C（Rockchip RK3588）机器人 SDK，集成 RKROS（ROS2 humble）。

## 仓库说明

本仓库已包含完整 SDK 源码（kernel / u-boot / rkbin / prebuilts / debian / device / buildroot / external / rtos / robot 等）。
为满足 GitHub「单文件 100MB」限制，以下 2 个 >100MB 的 ros2 预编译包被 `.gitignore` 排除、未上传：

| 路径 | 大小 | 说明 |
|---|---|---|
| `robot/prebuilts/ros2/humble/ros2-humble-bin-1.0.0-1-arm64.deb` | 142MB | ROS2 humble 预编译包 |
| `robot/prebuilts/ros2/jazzy/ros2-jazzy-bin-1.0.0-1-arm64.deb` | 145MB | ROS2 jazzy 预编译包 |

> 此外还有若干构建产物、调试工具、Android 专用库被忽略（见 `.gitignore`），编译都用不到。

clone 后 `./build.sh` 可直接运行（`robot/scripts` 已入库），但**构建 ROS rootfs 前需补齐上述 2 个 .deb**。

## 快速开始

### 0. 前置

- 本地有一份**完整的飞凌 SDK**（含上述 2 个 .deb），路径记为 `<FULL_SDK>`

### 1. 克隆

```bash
git clone git@github.com:xiaoqi27149/pooka-robot.git
cd pooka-robot
```

### 2. 补齐 2 个 ros2 .deb

```bash
./restore-build-files.sh <FULL_SDK>
```

### 3. 编译

```bash
./build.sh
```

> 若不需要 ROS rootfs（只编 kernel / u-boot / 基础 rootfs），可跳过第 2 步直接 `./build.sh`。
> 首次编译 debian rootfs 需联网下载 Debian 软件包（走清华镜像）；遇到 `Couldn't download packages` 类报错多为瞬时网络抖动，直接重跑 `./build.sh` 即可。

## 提交历史结构

- 9 个 `init:` 分块提交 —— 飞凌原生 SDK 基线（按目录拆成 <2GB 提交，以绕过 GitHub 2GB/pack 限制）
- 4 个功能提交 —— AMOLED 屏、双目 SC132GS 相机、RKROS 集成
- `robot/` 目录提交 —— `build.sh` 入口 + RKROS 源码 + model_zoo

## 许可

本 SDK 包含 GPL 源码（kernel / u-boot）及 Rockchip / 第三方专有二进制（rkbin、libmali、rknpu2、robot 等），
版权声明见 `Copyright_Statement.md`，使用请遵守相应许可。
