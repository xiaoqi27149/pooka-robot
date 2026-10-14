# rkros_pcl（ROS 2 包）

## 简介

`rkros_pcl` 是基于 ROS 2 的三维点云处理加速库，集成了 Rockchip 硬件加速（如 GPU），提升三维点云处理效率。适用于需要高性能三维点云处理的机器人与嵌入式场景。

详细文档:
```
<SDK>/robot/common_algorithm/pcl/rkALG_PCL/doc/
└── Rockchip-Developer-Guide-PCL-CN.md
```

---

## 快速构建

在工作区根目录（包含 `src/`）下执行：

```bash
cd ~/rkros_ws
colcon build --packages-select rkros_pcl
```

构建后记得 source 安装空间：

```bash
source install/setup.bash
```

---

## 在下游包中使用（CMake 示例）

在另一个 ament_cmake 包的 `CMakeLists.txt` 中，只需像下面这样声明依赖：

```cmake
find_package(rkros_pcl REQUIRED)

add_executable(my_app src/my_app.cpp)
ament_target_dependencies(my_app rkros_pcl)

install(TARGETS my_app DESTINATION lib/${PROJECT_NAME})
```

在源代码中直接包含头文件：

```cpp
#include "rkros_pcl/rkALG_pclApp.h"
```

在 `package.xml` 中加入依赖：

```xml
<depend>rkros_pcl</depend>
```

---

## 常见问题与故障排查

- Q：构建后下游找不到头文件或链接失败？
- A：请确保已对整个工作区执行 `colcon build`，并执行 `source install/setup.bash`。
