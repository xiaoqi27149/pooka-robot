# this is required
#set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../..)

# set toolchain
set(TOOLCHAIN_PATH "${SDK_PATH}/prebuilts/gcc/linux-x86/aarch64/gcc-arm-10.3-2017.07-x86_64_aarch64-linux-gnu")
set(TOOLCHAIN_NAME "aarch64-rockchip1031-linux-gnu")

# set cross compiler
set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_NAME}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_NAME}-g++)

# this is optional
#set(CMAKE_SYSROOT /path/to/target/sysroot)
#set(CMAKE_FIND_ROOT_PATH /path/to/target/sysroot)

# search programs in host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# search libraries in specify directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# search headers in specify directories
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
