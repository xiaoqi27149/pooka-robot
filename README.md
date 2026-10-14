# pooka-robot

飞凌 Forlinx OK3588-C（Rockchip RK3588）机器人 SDK，集成 RKROS（ROS2 humble）。

## 仓库说明

本仓库是**精简版** SDK：为满足 GitHub「单文件 100MB」「单次 push 2GB」限制，
以下内容被 `.gitignore` 排除、未上传：

| 路径 | 大小 | 编译必需 | 说明 |
|---|---|---|---|
| `robot/` | 1.3GB | ✅ 是 | `build.sh` 入口脚本 + rkros 源码 + ros2 预编译包 |
| `debian-for-ros-arm64-bullseye.tar` | 4.4GB | ❌ 否 | 预构建 rootfs 镜像，构建脚本未引用 |
| `prebuilts/.../aarch64-none-elf-gdb` | 180MB | ❌ 否 | 调试器，非编译所需 |
| `tools/linux/PinDebug/*.tar` | 171MB | ❌ 否 | 调试工具，非编译所需 |
| `external/.../gen_mesh/android/*.a` | 109+101MB | ❌ 否 | Android 专用库，Linux 编译用 `linux/` 下小库 |
| `debian/packages/arm64/humble/ros2-humble-bin-*.deb` | 148MB | ❌ 否 | 构建时从 `robot/prebuilts` 自动拷出 |

直接 clone 后 `./build.sh` 是断链（指向缺失的 `robot/scripts/build.sh`），
编译前需先恢复 `robot/`。

## 快速开始

### 0. 前置

- 本地有一份**完整的飞凌 SDK**（含 `robot/` 目录），路径记为 `<FULL_SDK>`
- 已配置 GitHub SSH（公开仓库也可用 HTTPS）

### 1. 克隆

```bash
git clone git@github.com:xiaoqi27149/pooka-robot.git
cd pooka-robot
```

### 2. 恢复编译必需文件

```bash
./restore-build-files.sh <FULL_SDK>
```

`restore-build-files.sh` 会把完整 SDK 里的 `robot/` 拷进当前仓库，使 `build.sh` 断链恢复。

### 3. 编译

```bash
./build.sh
```

> 首次编译 debian rootfs 阶段需联网下载 Debian 软件包（走清华镜像）。
> 若遇到 `Couldn't download packages` 类报错，多为瞬时网络抖动，直接重跑 `./build.sh` 即可。

## 提交历史结构

- 9 个 `init:` 分块提交 —— 飞凌原生 SDK 基线（按目录拆成 <2GB 的提交，以绕过 GitHub 2GB/pack 限制）
- 4 个功能提交 —— AMOLED 屏、双目 SC132GS 相机、RKROS 集成

## 许可

本 SDK 包含 GPL 源码（kernel/u-boot）及 Rockchip/第三方专有二进制（rkbin、libmali、rknpu2 等），
版权声明见 `Copyright_Statement.md`，使用请遵守相应许可。
