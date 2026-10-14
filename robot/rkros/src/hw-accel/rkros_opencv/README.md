
# Rockchip ROS OpenCV 加速库 (rkros_opencv)

## 简介
`rkros_opencv` 是基于 ROS 2 的图像处理加速库，集成了 Rockchip 硬件加速与 OpenCV，自动选择最优实现，提升图像处理效率。适用于需要高性能图像处理的机器人与嵌入式场景。

## 主要特性
- 使用cv::UMat，支持GPU硬件加速
- 支持导入外部dmabuf，避免了使用外部dmabuf时候需要进行内存拷贝
- 支持导出dmabuf给其它硬件使用，避免出现内存拷贝
- 自动缓存opencl的内存映射(cl_mem)，免除再次映射的耗时

## 依赖
- ROS 2 (建议 Humble 及以上)
- OpenCV
- rkros_dmabuf

## 安装与编译
1. 确保已安装上述依赖包。
2. 在 RK-ROS 工作空间下编译：

	```bash
	cd ~/rkros_ws
	colcon build --packages-select rkros_opencv
	source install/setup.bash
	```

## 使用示例

请参考：`rkros_examples\rkros_opencv_example`

### 1. C++ 代码调用

下面是一个简单的 C++ 示例：
```cpp
#include <opencv2/opencv.hpp>
#include <rkros_opencv/rkcv.hpp>

rkcv::DmabufPtr test(rkcv::DmabufPtr dmabuf) {
    // 导入dmabuf到UMat
    cv::UMat src = rkcv::createUMat(rows, cols, type, dmabuf);
    // 创建一个支持dmabuf的UMat
    cv::UMat dst = rkcv::createUMat();

    // 使用GPU加速
    cv::flip(src, dst, 1);

    //　导出dmabuf
    return rkcv::getMat(dst);
}
```

### 2. 在你的 package.xml 中添加依赖
```xml
<depend>rkros_opencv</depend>
<depend>opencv</depend>
<depend>rkros_dmabuf</depend>
<depend>rkros_common</depend>
```

### 3. 在你的 CMakeLists.txt 中添加依赖和链接
```cmake
find_package(rkros_opencv REQUIRED)
find_package(OpenCV REQUIRED)
find_package(rkros_dmabuf REQUIRED)
find_package(rkros_common REQUIRED)

add_executable(your_node src/your_node.cpp)
ament_target_dependencies(your_node
  rkros_opencv
  OpenCV
  rkros_dmabuf
  rkros_common
)
```

## 性能测试

测试说明：
- 测试平台： RK3588 + Linux6.1 + Debian11
- 测试程序： 使用 `test/bench_cv.cpp` 进行测试
- 测试环境： CPU定最高频，程序绑定在CPU6
- 测试源文件： 1920x1080 BGR24图像

### 使用 opencv + CPU

使用opencv算法，并关闭opencl加速，跑出来的性能数据如下：

执行命令：

```
ros2 run rkros_opencv bench_cv warmup=1 runs=1000 allocator=std opencl=0
```

|          Algorithm  |  min(us)  |  max(us)  | mean(us)  | median(us)  |  Load(%)  | 120FPS(%)  |
|:--------------------:|----------:|----------:|----------:|----------:|----------:|----------:|
|         Resize x0.5 |      2452 |      2479 |      2464 |      2464 |   100.507 |    29.718 |
|              copyTo |       463 |       478 |       470 |       470 |   100.000 |     5.640 |
|       crop(512x512) |        89 |        98 |        94 |        95 |   105.263 |     1.187 |
|  flip(horizontally) |      2618 |      2648 |      2630 |      2629 |   100.000 |    31.560 |
|rotate(90° clockwise) |      5794 |      5873 |      5836 |      5838 |   109.730 |    76.846 |
|cvtColor(bgr -> rgb) |       555 |       566 |       559 |       558 |   103.341 |     6.932 |

说明：
- 算法的耗时主要看平均值(mean)这一栏
- `Load(%)`是指CPU的使用率，rk3588总共有8个CPU核，当一个cpu核跑满的话，其CPU使用率100%
- `120FPS(%)`是指当每秒处理120帧图像的时候，该算法所占用的CPU负载
- 而测试程序是绑定在一个cpu核的，从上面数据可以看出来算法基本上吃掉了整个CPU

### 使用 opencv + opencl

使用opencv算法，开启opencl加速，跑出来的性能数据如下：

执行命令：

```
ros2 run rkros_opencv bench_cv warmup=1 runs=1000 allocator=std opencl=1
```

|          Algorithm  |  min(us)  |  max(us)  | mean(us)  | median(us)  |  Load(%)  | 120FPS(%)  |
|:--------------------:|----------:|----------:|----------:|----------:|----------:|----------:|
|         Resize x0.5 |       400 |       435 |       427 |       429 |    39.420 |     2.020 |
|              copyTo |       792 |       795 |       793 |       793 |   101.106 |     9.621 |
|       crop(512x512) |       398 |       405 |       401 |       401 |   102.181 |     4.917 |
|  flip(horizontally) |       550 |       560 |       555 |       555 |    25.112 |     1.672 |
|rotate(90° clockwise) |      1770 |      1812 |      1804 |      1806 |     9.477 |     2.052 |
|cvtColor(bgr -> rgb) |       592 |       597 |       594 |       595 |    18.526 |     1.321 |

说明：
- copyTo和crop这两个拷贝算法，在opencl加速下，性能会下降
- 其它算法在opencl加速下，性能会有明显提升，cpu负载也会下降。

### 使用 opencv + opencl + dmabuf

使用opencv算法和支持dmabuf的内存，开启opencl加速，跑出来的性能数据如下：

执行命令：

```
ros2 run rkros_opencv bench_cv warmup=1 runs=1000 allocator=dmabuf opencl=1
```

|          Algorithm  |  min(us)  |  max(us)  | mean(us)  | median(us)  |  Load(%)  | 120FPS(%)  |
|:--------------------:|----------:|----------:|----------:|----------:|----------:|----------:|
|         Resize x0.5 |       407 |       414 |       410 |       410 |    24.316 |     1.196 |
|              copyTo |       817 |       822 |       819 |       819 |   103.659 |    10.188 |
|       crop(512x512) |       399 |       406 |       402 |       402 |    99.688 |     4.809 |
|  flip(horizontally) |       517 |       522 |       519 |       520 |    15.385 |     0.958 |
|rotate(90° clockwise) |      1708 |      1717 |      1713 |      1713 |    10.480 |     2.154 |
|cvtColor(bgr -> rgb) |       558 |       563 |       560 |       560 |     8.969 |     0.603 |

说明：
- 在使用opencl加速的情况下，使用外部导入的dmabuf内存和使用内部分配的内存，性能基本上一致。

## 常见问题与故障排查

- Q: 什么时候使用GPU加速？
- A: 针对具体场景以及具体的cv算子，你可以测量它们的运行耗时，来决定是否使用GPU加速。
- Q: 什么时候使用支持dmabuf的cv::UMat？
- A: 当你需要导入／导出dmabuf，并使用gpu处理的时候，可以使用支持dmabuf的cv::UMat。