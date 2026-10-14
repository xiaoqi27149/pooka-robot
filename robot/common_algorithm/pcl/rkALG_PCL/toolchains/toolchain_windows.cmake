# cross.toolchain.cmake
# this is required
SET(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(target_arch aarch64-linux-gnu)
set(CMAKE_LIBRARY_ARCHITECTURE ${target_arch} CACHE STRING "" FORCE)
#set(LINUX 1)
SET(TOOLCHAIN_DIR "D:/toolchain/gcc-linaro-6.3.1-2017.05-i686-mingw32_aarch64-linux-gnu")
# specify the cross compiler
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-g++.exe)
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-gcc.exe)
set(CMAKE_STRIP ${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-strip.exe)

# specify the cpu architecture for rk3588
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=cortex-a76.cortex-a55")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=cortex-a76.cortex-a55")

# cache flags
set(CMAKE_C_FLAGS "" CACHE STRING "c flags" FORCE)
set(CMAKE_CXX_FLAGS "-fPIC -Wno-unused-result -Wno-write-strings" CACHE STRING "C++ Flags" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "C++ Release Flags" FORCE)
# Configure cmake to look for libraries, include directories and
# packages inside the target root prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

SET(CMAKE_FIND_ROOT_PATH  ${TOOLCHAIN_DIR}
     ${TOOLCHAIN_DIR}/aarch64-linux-gnu/bin
     ${TOOLCHAIN_DIR}/aarch64-linux-gnu/include
     ${TOOLCHAIN_DIR}/aarch64-linux-gnu/lib
     ${TOOLCHAIN_DIR}/aarch64-linux-gnu/lib64
     ${TOOLCHAIN_DIR}/aarch64-linux-gnu/libc
     ${TOOLCHAIN_DIR}/lib/gcc/aarch64-linux-gnu/6.3.1
     ${TOOLCHAIN_DIR}/lib/gcc/aarch64-linux-gnu/6.3.1/include
     ${TOOLCHAIN_DIR}/lib64
     ${TOOLCHAIN_DIR}/libexec
     )

set(CMAKE_INCLUDE_PATH
     ${TOOLCHAIN_DIR}/aarch64-linux-gnu/include
     ${TOOLCHAIN_DIR}/lib/gcc/aarch64-linux-gnu/6.3.1
     ${TOOLCHAIN_DIR}/lib/gcc/aarch64-linux-gnu/6.3.1/include
)

set(CMAKE_LIBRARY_PATH
     ${TOOLCHAIN_DIR}/aarch64-linux-gnu/lib
     ${TOOLCHAIN_DIR}/aarch64-linux-gnu/lib64
     ${TOOLCHAIN_DIR}/aarch64-linux-gnu/libc
     ${TOOLCHAIN_DIR}/lib64
     ${TOOLCHAIN_DIR}/libexec
     )

set(CMAKE_PROGRAM_PATH       ${TOOLCHAIN_DIR}/aarch64-linux-gnu/bin)

# needed to avoid doing some more strict compiler checks that
# are failing when cross-compiling
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)