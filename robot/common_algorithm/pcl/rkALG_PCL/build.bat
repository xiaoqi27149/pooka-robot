cmake ..  ^
-G "MinGW Makefiles" ^
-DCMAKE_C_FLAGS="${CMAKE_C_FLAGS} -fPIC"  ^
-DCMAKE_C_FLAGS_RELEASE="${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG"  ^
-DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -fPIC"  ^
-DCMAKE_CXX_FLAGS_RELEASE="${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG"  ^
-DCMAKE_BUILD_TYPE=Release  ^
-DCMAKE_TOOLCHAIN_FILE="D:/toolchain/gcc-linaro-6.3.1-2017.05-i686-mingw32_aarch64-linux-gnu/toolChain.cmake"  ^
-DEIGEN_DIR="D:/3rd_party/eigen-3.4.0/"
