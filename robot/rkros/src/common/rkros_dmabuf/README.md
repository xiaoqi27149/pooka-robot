# RKROS DMA-BUF (rkros_dmabuf)

简体中文说明文档

## 概述

`rkros_dmabuf` 是一个轻量的 C++ 库，封装了 Linux dma-buf 的分配、映射、缓存同步和数据拷贝等常用操作，并提供了与 ROS2 消息类型互转的 TypeAdapter（将自定义 `rkros_dmabuf::Image` 类型与 `rkros_zcopy_msgs::msg::Img1080p` 互转）。

该库的目标是在 Rockchip 平台上便捷地使用 dma-buf 进行零拷贝或低拷贝的数据传递，适用于视频帧或大块二进制缓冲区的管理。

## 目录结构

- `include/rkros_dmabuf/`
  - `dmabuf.hpp`        : DmaBuf 类的声明
  - `dmabuf_image.hpp`  : Image 结构体与 rclcpp::TypeAdapter 的声明
- `src/`
  - `dmabuf.cpp`        : DmaBuf 的实现（alloc/map/unmap/sync/copy）
  - `dmabuf_image.cpp`  : TypeAdapter 的实现（自定义类型与 ROS 消息互转）
- `test/`
  - `test_rkros_dmabuf.cpp` : 简单的分配/映射/写入/同步测试示例

## 功能简介

### DmaBuf 类（位于 `rkros_dmabuf::DmaBuf`）

- `allocate(size, heapName = "system", heapFlags = 0)`
  分配一个新的 dma-buf，返回 `std::shared_ptr<DmaBuf>`。若分配失败返回 `nullptr`。
- `make(fd, size, addr = nullptr)`
  封装已有的 dma-buf（由外部进程或设备提供的 fd）。
- `map()` / `unmap()`
  将 dma-buf mmap 到当前进程地址空间并返回地址；`unmap` 用于解除映射。
- `copyFrom(data, size)` / `copyTo(data, size)`
  把数据从用户内存拷贝到 dmabuf，或从 dmabuf 拷贝到用户内存。
- `syncStart()` / `syncEnd()` / `syncPartialStart()` / `syncPartialEnd()`
  对整个或部分区域执行 DMA 缓存同步（调用 `DMA_BUF_IOCTL_SYNC` / `DMA_BUF_IOCTL_SYNC_PARTIAL`）。
- 支持设置 RGA handle（`setRgaHandle`）及对应释放回调（注意：handle 由 RGA 负责释放，DmaBuf 仅保存引用并在析构时调用回调）。

### Image 与 TypeAdapter

- `rkros_dmabuf::Image` 包含帧信息（frame_no、timestamp、width、height、stride、format）和一个 `std::shared_ptr<DmaBuf>`。
- 通过 `rclcpp::TypeAdapter<rkros_dmabuf::Image, rkros_zcopy_msgs::msg::Img1080p>` 实现与 ROS 消息 `rkros_zcopy_msgs::msg::Img1080p` 的互转。互转过程中会发生数据拷贝：
  - `convert_to_ros_message`：把 dmabuf 的内容拷贝到 ROS 消息的 `data` 字段（dmabuf -> message.data）
  - `convert_to_custom`：为接收的 ROS 消息分配一个新的 dmabuf 并把 message.data 拷贝进去（message.data -> dmabuf）

## 构建（在 ROS2 ament 工程中）

该包使用 ament_cmake 构建，CMakeLists 已设置为导出 include 和 库。一般的构建流程：

1. 在 ROS2 工作区根目录（例如 `rkros_ws`）下运行：

```bash
# 构建整个工作区（假设已正确安装 ROS2 环境）
colcon build --packages-select rkros_dmabuf
```

2. 或者在单独包目录使用 cmake（不常见，推荐使用 colcon/ament）：

```bash
mkdir build && cd build
cmake ..
make -j
```

## 运行单元测试

你可以直接在源码目录运行测试程序（需要 root 或有访问 `/dev/dma_heap/*` 的权限）：

```bash
# 在构建（或安装）后运行示例测试程序
./install/lib/rkros_dmabuf/rkros_dmabuf_test
```

## 示例（在代码中使用）

下面是使用 `DmaBuf` 的常见示例（参照 `test/test_rkros_dmabuf.cpp`）：

### 分配并写入数据

```cpp
auto buf = rkros_dmabuf::DmaBuf::allocate(1024*1024);
if (!buf) { /* 处理错误 */ }
void *addr = buf->map();
memset(addr, 0x5A, buf->size());
```

### 封装外部 fd（例如通过设备或其他进程获得）

```cpp
int external_fd = /* 从其它接口获得 */;
auto wrapped = rkros_dmabuf::DmaBuf::make(external_fd, size, nullptr);
```

## 在 ROS2 节点中使用 TypeAdapter（注意：仍然存在拷贝）

把 `rkros_dmabuf::Image` 作为自定义类型在 Publisher/Subscription 中使用前，请确保包含头文件并在 CMakeLists 中正确链接 `rkros_dmabuf` 与 `rkros_zcopy_msgs`。

示例：发布 `rkros_dmabuf::Image`（伪代码）

```cpp
#include "rkros_dmabuf/dmabuf_image.hpp"
// ...
auto pub = node->create_publisher<rkros_dmabuf::Image>("/image", 10);
rkros_dmabuf::Image img;
// 分配并填充 dmabuf
img.dmabuf = rkros_dmabuf::DmaBuf::allocate(img.stride * img.height);
img.dmabuf->copyFrom(src_data, img.stride * img.height);
pub->publish(img);
```

## 注意事项与限制

- 权限：访问 `/dev/dma_heap/*` 设备节点通常需要 root 权限或相应的 udev 权限设置。没有权限会导致 allocate/open 失败。
- 进程间共享：dmabuf 的内存映射地址不能直接跨进程复用，本库在 TypeAdapter 转换时会进行数据拷贝，因此不能做到完全零拷贝的 ROS 消息传输（受限于 rclcpp/ROS2 消息语义）。
- RGA handle：DmaBuf 提供保存 RGA handle 的能力，但不负责释放 handle，Dmabuf在析构时会调用用户提供的释放回调（若设置）。
- 性能提示：TypeAdapter 的 convert_to_custom/convert_to_ros_message 会将数据拷贝到/从 ROS 消息，这在高帧率或大分辨率时会增加 CPU 与内存带宽负担。建议只在必要时使用 TypeAdapter，或在系统层面优化消息大小与频率。
