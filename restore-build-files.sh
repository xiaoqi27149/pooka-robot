#!/usr/bin/env bash
#
# restore-build-files.sh
#
# 把「本地完整 SDK」里编译必需、但被 .gitignore 排除（未推上 GitHub）的文件，
# 恢复到当前克隆出来的精简 SDK 中。
#
# 为什么需要这个脚本：
#   推 GitHub 时，robot/ 因体积大（1.3GB）被 .gitignore 排除；
#   但顶层 build.sh 是指向 robot/scripts/build.sh 的符号链接，
#   不恢复 robot/ 就无法用 ./build.sh 编译。
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
mkdir -p "${DST}"

# ---------------------------------------------------------------------------
# 编译必需、被 .gitignore 排除的目录（当前只有 robot/）。
#
# 不需要恢复的（被忽略但编译用不到）：
#   output/ rockdev/                 —— 编译输出目录，构建时自动生成
#   debian-for-ros-arm64-bullseye.tar —— 预构建 ROS rootfs 镜像，构建脚本未引用
#   prebuilts/.../aarch64-none-elf-gdb —— 调试器，非编译所需
#   tools/linux/PinDebug/*.tar       —— 调试工具，非编译所需
#   external/camera_engine_rkaiq/IspFec/src/gen_mesh/android/*.a —— Android 专用库
#   debian/packages/arm64/humble/ros2-humble-bin-*.deb —— 会从 robot/prebuilts 自动拷出
# ---------------------------------------------------------------------------
RESTORE_ITEMS=(
    "robot"
)

copy_item() {
    local src_path="${SRC}/${1}"
    local dst_path="${DST}/${1}"

    if [ ! -e "${src_path}" ]; then
        echo "!! 跳过（源不存在）: ${src_path}" >&2
        return 0
    fi

    echo ">> 恢复 ${1}/  ..."
    if command -v rsync >/dev/null 2>&1; then
        rsync -a --info=progress2 "${src_path}/" "${dst_path}/"
    else
        cp -a "${src_path}" "${dst_path}"
    fi
}

for item in "${RESTORE_ITEMS[@]}"; do
    copy_item "${item}"
done

# ---------------------------------------------------------------------------
# 校验 build.sh 符号链接是否恢复成功
# ---------------------------------------------------------------------------
if [ -L "${DST}/build.sh" ] && [ -f "${DST}/robot/scripts/build.sh" ]; then
    echo "✓ build.sh -> robot/scripts/build.sh 已就绪"
else
    echo "!! 警告: build.sh 链接目标仍缺失，请检查 robot/ 是否恢复完整" >&2
fi

echo "完成。可在 ${DST} 下运行 ./build.sh 编译。"
