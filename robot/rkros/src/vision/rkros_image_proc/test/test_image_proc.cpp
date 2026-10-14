
/*
 * Copyright (C) 2025 Rockchip Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rkros_image_proc/image_proc.hpp"
#include "sensor_msgs/image_encodings.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

static const char *test_file = "test_1920x1080.bgr24";
static int WIDTH = 1920;
static int HEIGHT = 1080;
static uint32_t ENCODING = RKCLR_BGR_888; // 0xRRGGBB
static const char *out_path = "./";
static bool autoSync = false; // 禁止自动同步cache

static int readFromFile(void *data, const char *filename, int width, int height,
                        int stride, uint32_t format) {
    if (rkclr_is_yuv_p_sp(format)) {
        std::cout << "unsupport yuv p/sp format " << format << std::endl;
        return -1;
    }

    float bytesPerPixel = rkclr_get_pixel_bytes(format);
    std::size_t rowBytesFile = width * bytesPerPixel;
    std::size_t rowBytesMem = stride;
    std::size_t totalBytesMem = rowBytesMem * height;

    FILE *fp = std::fopen(filename, "rb");
    if (!fp) {
        std::cerr << "cannot open file: " << filename << std::endl;
        return -1;
    }

    // 可一次性读取图像
    if (rowBytesMem == rowBytesFile) {
        if (std::fread(data, 1, totalBytesMem, fp) != totalBytesMem) {
            std::fclose(fp);
            throw std::runtime_error("fread failed or EOF");
        }
        std::fclose(fp);
        return 0;
    }

    // 需要按行读取
    uint8_t *rowPtr = (uint8_t *)data;
    for (int y = 0; y < height; y++, rowPtr += rowBytesMem) {
        if (std::fread(rowPtr, 1, rowBytesFile, fp) != rowBytesFile) {
            std::fclose(fp);
            throw std::runtime_error("unexpected EOF or read error");
        }
    }

    std::fclose(fp);

    return 0;
}

static int writeToFile(const char *filename,
                       std::shared_ptr<rkros_dmabuf::DmaBuf> dmabuf) {
    std::cout << "write to " << filename << std::endl;

    FILE *fp = std::fopen(filename, "wb");
    if (!fp) {
        std::cerr << "cannot open file: " << filename << std::endl;
        return -1;
    }

    dmabuf->syncStart();
    fwrite(dmabuf->map(), dmabuf->size(), 1, fp);
    dmabuf->syncEnd();

    fclose(fp);

    return 0;
}

static void makeImage(void *data, int width, int height, int stride, uint32_t format) {
    float bytesPerPixel = rkclr_get_pixel_bytes(format);
    std::size_t rowBytesData = width * bytesPerPixel;
    std::size_t rowBytesMem = stride;
    std::size_t totalBytesMem = rowBytesMem * height;
    uint8_t value = 0x5A; // 填充的值

    memset(data, 0, totalBytesMem);

    if (rowBytesMem == rowBytesData) {
        // 无padding
        memset(data, value, totalBytesMem);
    } else {
        // 有padding
        uint8_t *rowPtr = (uint8_t *)data;
        for (int y = 0; y < height; y++, rowPtr += rowBytesMem) {
            memset(rowPtr, value, rowBytesData);
        }
    }
}

static int test_dmabuf_copy(rkros_dmabuf::Image &src_img) {
    std::cout << ">> test dmabuf copy <<" << std::endl;

    // 准备destination image
    rkros_dmabuf::Image dst_img;
    if (!dst_img.create(src_img.width, src_img.height, src_img.format,
                        src_img.stride)) {
        return -1;
    }

    if (!rkros_image_proc::copy(src_img, dst_img, autoSync)) {
        std::cerr << "dmabuf copy failed" << std::endl;
        return -1;
    }

    std::cout << "dmabuf copy success" << std::endl;
    char out_file[64];
    snprintf(out_file, sizeof(out_file), "%s/out_dmabuf_copy_%dx%d.bgr24", out_path,
             dst_img.width, dst_img.height);

    writeToFile(out_file, dst_img.dmabuf);

    return 0;
}

static int test_dmabuf_rotate(rkros_dmabuf::Image &src_img, int angle) {
    std::cout << ">> test dmabuf rotate <<" << std::endl;

    int width, height, stride;
    if (angle == rkros_image_proc::ROTATION_90 ||
        angle == rkros_image_proc::ROTATION_270) {
        width = src_img.height;
        height = src_img.width;
    } else {
        width = src_img.width;
        height = src_img.height;
    }
    stride = rkros_image_proc::getStrideBytes(width, src_img.format);

    rkros_dmabuf::Image dst_img;
    if (!dst_img.create(width, height, src_img.format, stride)) {
        return -1;
    }

    if (!rkros_image_proc::rotate(src_img, dst_img, angle, autoSync)) {
        std::cerr << "dmabuf rotate failed" << std::endl;
        return -1;
    }

    std::cout << "dmabuf rotate success" << std::endl;
    char out_file[64];
    snprintf(out_file, sizeof(out_file), "%s/out_dmabuf_rotate_%d_%dx%d.bgr24",
             out_path, angle, dst_img.width, dst_img.height);

    writeToFile(out_file, dst_img.dmabuf);

    return 0;
}

static int test_dmabuf_resize(rkros_dmabuf::Image &src_img, int width, int height) {
    std::cout << ">> test dmabuf resize <<" << std::endl;

    rkros_dmabuf::Image dst_img;
    int stride = rkros_image_proc::getStrideBytes(width, src_img.format);
    if (!dst_img.create(width, height, src_img.format, stride)) {
        return -1;
    }

    if (!rkros_image_proc::resize(src_img, dst_img, rkros_image_proc::INTERP_LINEAR,
                                  autoSync)) {
        std::cerr << "dmabuf resize failed" << std::endl;
        return -1;
    }

    std::cout << "dmabuf resize success" << std::endl;
    char out_file[64];
    snprintf(out_file, sizeof(out_file), "%s/out_dmabuf_resize_%dx%d.bgr24", out_path,
             width, height);
    writeToFile(out_file, dst_img.dmabuf);

    return 0;
}

static int test_dmabuf_crop(rkros_dmabuf::Image &src_img) {
    std::cout << ">> test dmabuf crop <<" << std::endl;

    // 准备destination image
    rkros_dmabuf::Image dst_img;
    int stride = rkros_image_proc::getStrideBytes(src_img.roi.w, src_img.format);
    if (!dst_img.create(src_img.roi.w, src_img.roi.h, src_img.format, stride)) {
        return -1;
    }

    if (!rkros_image_proc::crop(src_img, dst_img, autoSync)) {
        std::cerr << "dmabuf crop failed" << std::endl;
        return -1;
    }

    std::cout << "dmabuf crop success" << std::endl;
    char out_file[64];
    snprintf(out_file, sizeof(out_file), "%s/out_dmabuf_crop_%dx%d.bgr24", out_path,
             dst_img.width, dst_img.height);

    writeToFile(out_file, dst_img.dmabuf);

    return 0;
}

static int test_dmabuf_flip(rkros_dmabuf::Image &src_img, int flip_code) {
    std::cout << ">> test dmabuf flip <<" << std::endl;

    // 准备destination image
    rkros_dmabuf::Image dst_img;
    if (!dst_img.create(src_img.width, src_img.height, src_img.format,
                        src_img.stride)) {
        return -1;
    }

    if (!rkros_image_proc::flip(src_img, dst_img, flip_code, autoSync)) {
        std::cerr << "dmabuf flip failed" << std::endl;
        return -1;
    }

    std::cout << "dmabuf flip success" << std::endl;
    char out_file[64];
    snprintf(out_file, sizeof(out_file), "%s/out_dmabuf_flip_%dx%d.bgr24", out_path,
             dst_img.width, dst_img.height);

    writeToFile(out_file, dst_img.dmabuf);

    return 0;
}

static int test_dmabuf_convertColor(rkros_dmabuf::Image &src_img, uint32_t format) {
    std::cout << ">> test dmabuf convert color <<" << std::endl;

    // 准备destination image
    rkros_dmabuf::Image dst_img;
    int stride = rkros_image_proc::getStrideBytes(src_img.width, format);
    if (!dst_img.create(src_img.width, src_img.height, format, stride)) {
        return -1;
    }

    if (!rkros_image_proc::cvtColor(src_img, dst_img, autoSync)) {
        std::cerr << "dmabuf convert color failed" << std::endl;
        return -1;
    }

    std::cout << "dmabuf convert color success" << std::endl;
    char out_file[64];
    snprintf(out_file, sizeof(out_file), "%s/out_dmabuf_cvtColor_%dx%d.nv12", out_path,
             dst_img.width, dst_img.height);

    writeToFile(out_file, dst_img.dmabuf);

    return 0;
}

static int test_dmabuf() {
    std::cout << "test dmabuf" << std::endl;

    rkros_dmabuf::Image src_img;
    int stride = rkros_image_proc::getStrideBytes(WIDTH, ENCODING);
    if (!src_img.create(WIDTH, HEIGHT, ENCODING, stride)) {
        return -1;
    }
    auto src_buf = src_img.dmabuf;

    src_buf->syncStart();
    void *data = src_buf->map();
    if (readFromFile(data, test_file, src_img.width, src_img.height, stride, ENCODING) <
        0) {
        makeImage(data, src_img.width, src_img.height, stride, ENCODING);
    }
    src_buf->syncEnd();

    if (test_dmabuf_copy(src_img) != 0) {
        return -1;
    }

    if (test_dmabuf_rotate(src_img, rkros_image_proc::ROTATION_90) != 0) {
        return -1;
    }

    if (test_dmabuf_resize(src_img, 640, 360) != 0) {
        return -1;
    }

    src_img.roi = {0, 0, 80, 80};
    if (test_dmabuf_crop(src_img) != 0) {
        return -1;
    }
    src_img.roi = {0, 0, 0, 0};

    if (test_dmabuf_flip(src_img, rkros_image_proc::FLIP_H_V) != 0) {
        return -1;
    }

    if (test_dmabuf_convertColor(src_img, RKCLR_NV12) != 0) {
        return -1;
    }

    return 0;
}

int main(int argc, const char **argv) {
    if (argc >= 4) {
        test_file = argv[1];
        WIDTH = atoi(argv[2]);
        HEIGHT = atoi(argv[3]);
    }

    std::cout << "input file: " << test_file << std::endl;
    std::cout << "size: " << WIDTH << "x" << HEIGHT << std::endl;

    test_dmabuf();

    return 0;
}
