# rkros_idc（ROS 2 包）

## 简介

`rkros_idc` 是基于 GPU 的图像几何校正加速算法，实现图像重映射功能，可用于视频防抖、畸变校正、全景拼接等视觉应用。

详细文档:
```
<SDK>/robot/common_algorithm\idc\rkalg_idc_lut_demo\doc
└── Rockchip_IDC_Developer_Guide_CN.md
```

---

## 快速构建

在工作区根目录（包含 `src/`）下执行：

```bash
cd ~/rkros_ws
colcon build --packages-select rkros_idc
```

构建后记得 source 安装空间：

```bash
source install/setup.bash
```

---

## 在下游包中使用（CMake 示例）

在另一个 ament_cmake 包的 `CMakeLists.txt` 中，只需像下面这样声明依赖：

```cmake
find_package(rkros_idc REQUIRED)

add_executable(my_app src/my_app.cpp)
ament_target_dependencies(my_app rkros_idc)

install(TARGETS my_app DESTINATION lib/${PROJECT_NAME})
```

在源代码中直接包含头文件：

```cpp
#include "rkros_idc/rkalg_idc_lut_api.h"
#include "rkros_idc/rkalg_idc_dma_buffer.h"
```

在 `package.xml` 中加入依赖：

```xml
<depend>rkros_idc</depend>
```

---

## 常见问题与故障排查

- Q：构建后下游找不到头文件或链接失败？
- A：请确保已对整个工作区执行 `colcon build`，并执行 `source install/setup.bash`。
