# rkros_rockit（ROS 2 包）

## 简介

`rkros_rockit` 是基于 ROS 2 的图像编解码加速库，集成了 Rockchip 硬件加速（如 VPU），提升编解码的处理效率。适用于需要高性能图像编解码处理的机器人与嵌入式场景。

详细文档:

```
<SDK>/external/rockit/mpi/doc/
├── Rockchip_Developer_Guide_MPI_CN.pdf
└── Rockchip_FAQ_MPI_CN.pdf
```

---

## 快速构建

在工作区根目录（包含 `src/`）下执行：

```bash
cd ~/rkros_ws
colcon build --packages-select rkros_rockit
```

构建后记得 source 安装空间：

```bash
source install/setup.bash
```

---

## 在下游包中使用（CMake 示例）

在 `package.xml` 中加入依赖：

```xml
<depend>rkros_rockit</depend>
```

在 `CMakeLists.txt` 中，使用ament导入头文件和库：

```cmake
find_package(rkros_rockit REQUIRED)

add_executable(my_app src/my_app.cpp)
ament_target_dependencies(my_app rkros_rockit)

install(TARGETS my_app DESTINATION lib/${PROJECT_NAME})
```

在源代码中，根据你要使用的功能，包含相应的rockit头文件：

```cpp
#include "rkros_rockit/rk_mpi_sys.h"
```

接下来就可以调用rockit提供的api，注意要包含相应的rockit头文件

---

## 常见问题与故障排查

- Q：构建后下游找不到头文件或链接失败？
- A：请确保已对整个工作区执行 `colcon build`，并执行 `source install/setup.bash`。
