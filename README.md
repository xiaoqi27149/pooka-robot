# pooka-robot

飞凌 Forlinx OK3588-C（Rockchip RK3588）机器人 SDK，集成 RKROS（ROS2 humble）。

## 仓库说明

本仓库已包含完整 SDK 源码（kernel / u-boot / rkbin / prebuilts / debian / device / buildroot / external / rtos / robot 等）。
为满足 GitHub「单文件 100MB」限制，以下文件被 `.gitignore` 排除、未上传：

| 路径 | 大小 | 用途 | 编译必需 |
|---|---|---|---|
| `robot/prebuilts/ros2/humble/ros2-humble-bin-1.0.0-1-arm64.deb` | 142MB | ROS2 humble 预编译包 | 编 ROS rootfs 需要 |
| `robot/prebuilts/ros2/jazzy/ros2-jazzy-bin-1.0.0-1-arm64.deb` | 145MB | ROS2 jazzy 预编译包 | 编 ROS rootfs 需要 |
| `debian-for-ros-arm64-bullseye.tar` | 4.4GB | docker 镜像（debian-for-ros-arm64:bullseye） | 编 rkros 需要 |

> 其余被忽略项（构建输出、调试工具、Android 专用库等）编译用不到，见 `.gitignore`。

## 快速开始

### 0. 前置

- 本地有一份**完整的飞凌 SDK**，路径记为 `<FULL_SDK>`

### 1. 克隆

```bash
git clone git@github.com:xiaoqi27149/pooka-robot.git
cd pooka-robot
```

### 2. 补齐被排除的文件

```bash
./restore-build-files.sh <FULL_SDK>                  # 只补 2 个 ros2 .deb
./restore-build-files.sh <FULL_SDK> --with-docker    # 同时补 docker 镜像 tar（编 rkros 用）
```

### 3. 编译

```bash
./build.sh
```

## 编译 RKROS 前必读

`./build.sh` 编译 rkros 时需要一个名为 `debian-for-ros-arm64:bullseye` 的 docker 镜像。
**如果镜像不存在，rkros 编译会被静默跳过**——脚本（`check-docker.sh`）只打印一行
`Error: not found docker image` 后 `return 1`，但不带 `set -e`、也不检查返回值，
于是构建继续往下走，最终产出的 rootfs **不包含 RKROS 包**，而构建看起来仍然「成功」。

因此编 rkros 前，请先加载镜像：

```bash
docker load < debian-for-ros-arm64-bullseye.tar
docker image ls | grep debian-for-ros   # 应能看到 debian-for-ros-arm64:bullseye
```

如何确认 rkros 真的编进去了：看 `robot/rkros/output/.last_build` 是否存在——
存在即表示 rkros 打包完成；只有 `.timestamp` 而没有 `.last_build`，说明被静默跳过了。

> 只编 kernel / u-boot / 基础 rootfs（不需要 ROS）的话，无需 docker 镜像，直接 `./build.sh` 即可。

## 提交历史结构

- 9 个 `init:` 分块提交 —— 飞凌原生 SDK 基线（按目录拆成 <2GB 提交，以绕过 GitHub 2GB/pack 限制）
- 4 个功能提交 —— AMOLED 屏、双目 SC132GS 相机、RKROS 集成
- `robot/` 目录提交 —— `build.sh` 入口 + RKROS 源码 + model_zoo

## 许可

本 SDK 包含 GPL 源码（kernel / u-boot）及 Rockchip / 第三方专有二进制（rkbin、libmali、rknpu2、robot 等），
版权声明见 `Copyright_Statement.md`，使用请遵守相应许可。
