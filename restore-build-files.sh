#!/usr/bin/env bash
#
# restore-build-files.sh
#
# 把「本地完整 SDK」里编译必需、但被 .gitignore 排除（>100MB）的 2 个 ros2 .deb，
# 恢复到当前克隆出来的精简 SDK 中。
#
# 为什么需要：这 2 个 ros2 预编译包各超 100MB，GitHub 单文件硬限制无法上传；
# 但构建 ROS rootfs 时（mk-debian.sh 会从 robot/prebuilts/ros2 拷包）需要它们。
#
# 用法（在克隆出来的 SDK 根目录执行）：
#   ./restore-build-files.sh <本地完整SDK路径> [目标SDK路径]
#
# 示例：
#   cd pooka-robot
#   ./restore-build-files.sh ~/workspace/test/ok3588-robot-source
#

set -euo pipefail

SRC="${1:?用法: $0 <本地完整SDK路径> [目标SDK路径]}"
DST="${2:-$(pwd)}"

if [ ! -d "${SRC}" ]; then
    echo "错误: 源 SDK 目录不存在: ${SRC}" >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# 编译必需、被 .gitignore 排除的 2 个 >100MB 文件。
#
# 其余被忽略项（构建输出、调试工具、Android 专用库等）编译用不到，无需恢复。
# ---------------------------------------------------------------------------
RESTORE_FILES=(
    "robot/prebuilts/ros2/humble/ros2-humble-bin-1.0.0-1-arm64.deb"
    "robot/prebuilts/ros2/jazzy/ros2-jazzy-bin-1.0.0-1-arm64.deb"
)

for f in "${RESTORE_FILES[@]}"; do
    src="${SRC}/${f}"
    dst="${DST}/${f}"

    if [ ! -f "${src}" ]; then
        echo "!! 跳过（源不存在）: ${src}" >&2
        continue
    fi

    echo ">> 恢复 ${f} ..."
    mkdir -p "$(dirname "${dst}")"
    cp -a "${src}" "${dst}"
done

echo "完成。若需构建 ROS rootfs，缺失的 2 个 ros2 .deb 已补齐，可运行 ./build.sh 编译。"
