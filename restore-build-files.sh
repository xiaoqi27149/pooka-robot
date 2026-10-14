#!/usr/bin/env bash
#
# restore-build-files.sh
#
# 把「本地完整 SDK」里编译必需、但被 .gitignore 排除（>100MB）的文件，
# 恢复到当前克隆出来的精简 SDK 中。
#
# 默认恢复：2 个 >100MB 的 ros2 .deb（构建 ROS rootfs 需要）。
# 加 --with-docker：额外恢复 debian-for-ros-arm64-bullseye.tar（docker 镜像，
#   编译 rkros 需要，需先 docker load）。
#
# 用法（在克隆出来的 SDK 根目录执行）：
#   ./restore-build-files.sh <本地完整SDK路径> [目标SDK路径] [--with-docker]
#
# 示例：
#   cd pooka-robot
#   ./restore-build-files.sh ~/workspace/test/ok3588-robot-source              # 只补 2 个 .deb
#   ./restore-build-files.sh ~/workspace/test/ok3588-robot-source --with-docker # 同时补 docker 镜像 tar
#

set -euo pipefail

SRC=""
DST="$(pwd)"
WITH_DOCKER=0

# 解析参数
while [ $# -gt 0 ]; do
    case "$1" in
        --with-docker)
            WITH_DOCKER=1
            ;;
        *)
            if [ -z "$SRC" ]; then
                SRC="$1"
            else
                DST="$1"
            fi
            ;;
    esac
    shift
done

if [ -z "$SRC" ]; then
    echo "用法: $0 <本地完整SDK路径> [目标SDK路径] [--with-docker]" >&2
    exit 1
fi

if [ ! -d "$SRC" ]; then
    echo "错误: 源 SDK 目录不存在: ${SRC}" >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# 编译必需、被 .gitignore 排除的 2 个 >100MB 文件（默认恢复）。
#
# 其余被忽略项（构建输出、调试工具、Android 专用库等）编译用不到，无需恢复。
# ---------------------------------------------------------------------------
RESTORE_FILES=(
    "robot/prebuilts/ros2/humble/ros2-humble-bin-1.0.0-1-arm64.deb"
    "robot/prebuilts/ros2/jazzy/ros2-jazzy-bin-1.0.0-1-arm64.deb"
)

# --with-docker 时额外恢复 docker 镜像文件（编译 rkros 需要）
if [ "$WITH_DOCKER" = "1" ]; then
    RESTORE_FILES+=("debian-for-ros-arm64-bullseye.tar")
fi

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

if [ "$WITH_DOCKER" = "1" ]; then
    echo ""
    echo "提示: 编译 rkros 前请先加载 docker 镜像："
    echo "  docker load < ${DST}/debian-for-ros-arm64-bullseye.tar"
    echo "  docker image ls | grep debian-for-ros   # 应能看到 debian-for-ros-arm64:bullseye"
fi

echo "完成。"
