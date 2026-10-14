#include <iostream>

#include "rkros_opencv/rkcv.hpp"

static int WIDTH = 1280;
static int HEIGHT = 720;
static int TYPE = CV_8UC3;

// 使用cv::Mat，走cpu路径
static void cpu_process(rkcv::DmabufPtr &src_dmabuf, rkcv::DmabufPtr &dst_dmabuf) {
    if (src_dmabuf == nullptr || dst_dmabuf == nullptr) {
        printf(">>> dmabuf is null\n");
        return;
    }
    // 如果此前有硬件处理过数据，需要做下sync
    src_dmabuf->syncStart(DMA_BUF_SYNC_READ);
    dst_dmabuf->syncStart(DMA_BUF_SYNC_WRITE);

    cv::Mat src(HEIGHT, WIDTH, TYPE, src_dmabuf->map());
    printf("src dmabuf: fd=%d, size=%zu\n", src_dmabuf->fd(), src_dmabuf->size());

    cv::Mat dst(HEIGHT / 2, WIDTH / 2, TYPE, dst_dmabuf->map());
    printf("dst dmabuf: fd=%d, size=%zu\n", dst_dmabuf->fd(), dst_dmabuf->size());

    // 对src图像进行处理，走cpu路径
    cv::resize(src, dst, {}, 0.5, 0.5);
    // 调用cv算子进行其它处理

    // 如果接下来要把buf交给硬件处理，需要做下sync
    src_dmabuf->syncEnd(DMA_BUF_SYNC_READ);
    dst_dmabuf->syncEnd(DMA_BUF_SYNC_WRITE);
}

// 使用cv::UMat，走gpu路径
static void gpu_process(rkcv::DmabufPtr &src_dmabuf, rkcv::DmabufPtr &dst_dmabuf) {
    cv::UMat src, dst;

    if (src_dmabuf) {
        // 使用外部传递进来的dmabuf
        src = rkcv::createUMat(HEIGHT, WIDTH, TYPE, src_dmabuf);
    } else {
        // 创建一个支持dmabuf的UMat
        src = rkcv::createUMat(HEIGHT, WIDTH, TYPE);
        src_dmabuf = rkcv::getDmabuf(src);
    }
    printf("src dmabuf: fd=%d, size=%zu\n", src_dmabuf->fd(), src_dmabuf->size());

    if (dst_dmabuf) {
        // 使用外部传递进来的dmabuf
        dst = rkcv::createUMat(HEIGHT / 2, WIDTH / 2, TYPE, dst_dmabuf);
    } else {
        // 创建一个支持dmabuf的UMat
        dst = rkcv::createUMat(HEIGHT, WIDTH, TYPE);
        dst_dmabuf = rkcv::getDmabuf(dst);
    }
    printf("dst dmabuf: fd=%d, size=%zu\n", dst_dmabuf->fd(), dst_dmabuf->size());

    // 对src图像进行处理，优先走gpu路径
    cv::resize(src, dst, {}, 0.5, 0.5);
    // 调用cv算子进行其它处理

    // 把处理完成后的dmabuf放到dst_dmabuf中
    dst_dmabuf = rkcv::getDmabuf(dst);

    // 等待GPU任务结束
    cv::ocl::finish();
}

int main() {
    // src dmabuf可能是从Camera采集进来的数据
    rkcv::DmabufPtr src_dmabuf = nullptr;

    // 处理后的数据放在dst dmabuf中，后续可交给vpu做编码，或者送rknn做推理
    rkcv::DmabufPtr dst_dmabuf = nullptr;

    gpu_process(src_dmabuf, dst_dmabuf);

    cpu_process(src_dmabuf, dst_dmabuf);

    if (src_dmabuf)
        printf("src dmabuf: fd=%d, size=%zu\n", src_dmabuf->fd(), src_dmabuf->size());

    if (dst_dmabuf)
        printf("dst dmabuf: fd=%d, size=%zu\n", dst_dmabuf->fd(), dst_dmabuf->size());

    std::cout << "<<< Done." << std::endl;

    return 0;
}
