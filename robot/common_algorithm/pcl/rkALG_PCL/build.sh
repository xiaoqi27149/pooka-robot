#! /bin/bash

SCRIPT_DIR=$(cd $(dirname ${BASH_SOURCE[0]}); pwd)
SOURCE_PATH=${SCRIPT_DIR}

TARGET_NAME="linux"

TOOLCHAIN_PATH=${SOURCE_PATH}/toolchains/toolchain_${TARGET_NAME}.cmake
BUILD_DIR=build/build_${TARGET_NAME}
DEBIAN_PATH=${SOURCE_PATH}/../../../../debian/

rm -rf ${BUILD_DIR}
mkdir -p ${BUILD_DIR}
pushd ${BUILD_DIR}

cmake ../.. \
	-DCMAKE_C_FLAGS="${CMAKE_C_FLAGS} -fPIC" \
	-DCMAKE_C_FLAGS_RELEASE="${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG" \
	-DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -fPIC" \
	-DCMAKE_CXX_FLAGS_RELEASE="${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG" \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_PATH}" \
	-DEIGEN_DIR="${DEBIAN_PATH}binary/usr/include/eigen3/" \
	-DCMAKE_INSTALL_PREFIX=install

make -j32
make install

popd
