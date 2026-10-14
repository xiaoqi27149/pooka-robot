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

#include "rkros_dmabuf/dmabuf.hpp"
#include <cstring>
#include <iostream>

int main() {
    std::size_t size = 1024 * 1024;
    auto buf = rkros_dmabuf::DmaBuf::allocate(size);
    if (buf == nullptr) {
        std::cerr << "allocate dmabuf failed, size=" << size << std::endl;
        return -1;
    }

    std::cout << "dmabuf allocated, fd=" << buf->fd() << ", size=" << buf->size()
              << std::endl;

    void *addr = buf->map();
    if (addr == nullptr) {
        std::cerr << "map dmabuf failed" << std::endl;
        return -1;
    }
    std::cout << "dmabuf mapped, addr=" << buf->addr() << std::endl;

    if (buf->syncStart() == false) {
        std::cerr << "dmabuf sync start failed" << std::endl;
        return -1;
    }
    std::memset(addr, 0x5A, buf->size());
    if (buf->syncEnd() == false) {
        std::cerr << "dmabuf sync end failed" << std::endl;
        return -1;
    }

    const char *test_str = "Hello, DmaBuf!";
    if (buf->copyFrom((void *)test_str, std::strlen(test_str) + 1) == false) {
        std::cerr << "setData dmabuf failed" << std::endl;
        return -1;
    }

    if (buf->unmap() == false) {
        std::cerr << "unmap dmabuf failed" << std::endl;
        return -1;
    }

    std::cout << "dmabuf test okay" << std::endl;

    return 0;
}