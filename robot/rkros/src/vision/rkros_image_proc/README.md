## rkros_image_proc

基于 Rockchip RGA（硬件加速）实现的一组图像处理工具函数，封装了对 `rkros_dmabuf::Image` 的操作，提供常用图像操作：resize、copy、rotate、crop、flip、cvtColor 以及更通用的 process 接口。

本库旨在在 Rockchip 平台上（例如 RK3588）利用 RGA 加速常见图像处理操作，同时与 `rkros_dmabuf` 的零拷贝内存模型配合使用。

## 关键特性

- 使用 RGA 硬件加速的图像复合与变换。
- 支持常见像素格式（RGB/ARGB/BGR/YUV 系列）与多种插值方法。
- 使用 `rkros_dmabuf::Image` 与 `DmaBuf` 管理共享内存，支持直接导入 fd 到 RGA handle。
- 简单的同步模型（autoSync）与面向性能的可选参数（im_opt_t、usage）。

## 依赖与环境

- ament_cmake / colcon（ROS2 构建系统）
- rclcpp
- rkros_dmabuf（DmaBuf 分配/同步/映射）
- rkros_zcopy_msgs
- rkros_common（像素格式常量与工具）
- librga（通过 pkg-config 查找）

建议在支持 RGA 的 Rockchip 平台上使用（例如 RK3588），并确保内核/驱动支持 dma-buf 与 RGA。

## 构建与安装

本包使用 `ament`/`colcon` 构建（`CMakeLists.txt` 已包含必要配置）。示例构建流程：

```bash
# 在工作空间根目录（包含 src/）执行
colcon build --packages-select rkros_image_proc
```

如果系统上的 `librga` 非标准路径，可设置 `PKG_CONFIG_PATH`：

```bash
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
```

## 设计概览

- 主要头文件：`include/rkros_image_proc/image_proc.hpp`
- 主要实现：`src/image_proc.cpp`
- 同步模型：函数可选 `autoSync`（默认 true）在内部调用 `dmabuf->syncStart/End` 或 `syncPartial*`，用户也可选择手动管理以提升性能

## 示例（参考 `test/test_image_proc.cpp`）

下面示例展示如何在项目中使用`rkros_image_proc`软件包

### `package.xml`

修改`package.xml`，添加依赖

```
<depend>rkros_image_proc</depend>
```

### `CMakeLists.txt`

修改`CMakeLists.txt`，使用ament自动加入`rkros_image_proc`头文件和库

```
find_package(rkros_image_proc REQUIRED)

...

ament_target_dependencies(${PROJECT_NAME}
  ...
  rkros_image_proc
)
```

### CPP代码修改

下面示例展示如何用 `DmaBuf` 分配缓冲、填充数据并调用 `copy`/`resize`/`rotate`：

```cpp
#include "rkros_image_proc/image_proc.hpp"

// 伪代码示例，省略错误检查
int width = 1920, height = 1080;
uint32_t format = RKCLR_BGR_888;
rkros_dmabuf::Image src_img;
int stride = rkros_image_proc::getStrideBytes(width, format);
src_img.create(width, height, format, stride);
// 填充 src_img.dmabuf（map/write/sync）

rkros_dmabuf::Image dst_img;
dst_img.create(width, height, format, stride);

// 直接复制
bool ok = rkros_image_proc::copy(src_img, dst_img);

// 缩放
rkros_dmabuf::Image out_resize;
out_resize.create(width/2, height/2, format, stride);
rkros_image_proc::resize(src_img, out_resize, rkros_image_proc::INTERP_LINEAR);
```

### 运行内置测试

构建后，可运行测试程序（路径与构建和安装方式相关）。`test_image_proc` 的用法参考：

```bash
# test_image_proc [input_file] [width] [height]
./install/lib/rkros_image_proc/test_image_proc /userdata/test/test_1920x1080.bgr24 1920 1080
```

测试程序会把结果写入 `/userdata/test`（可在源码中修改 `out_path`）。

## 性能与使用建议

- 对于高吞吐场景，建议关闭 `autoSync`，并在外部统一管理 `dmabuf->syncStart/End` 来减少同步开销。
- 合理调整 `stride` 与分配策略以减少内存浪费与拷贝。
- 对大批量帧处理，考虑复用 `DmaBuf` 并避免频繁 import/export rga handle。
- 插值模式（INTERP_LINEAR/INTERP_CUBIC 等）在速度与质量间权衡。

## 常见问题与故障排查

- 报错 `unsupport image format`：确认传入的 `Image.format` 为 `rkros_common` 中定义的格式常量，且能映射到 RGA 支持的格式。
- importbuffer/importbuffer_fd 失败：检查传入 fd 是否有效，驱动/权限是否允许导入。
- rga 处理失败：查看控制台输出的 `imStrError` 信息，检查 srect/drect 的尺寸与对齐是否正确。
- 输出图像异常（颜色/形状）：确认 `stride`、`format`、`width`/`height` 是否正确匹配。
