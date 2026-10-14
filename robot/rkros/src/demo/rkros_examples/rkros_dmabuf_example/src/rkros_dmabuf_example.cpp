#include <iostream>

#include "rkros_dmabuf/dmabuf.hpp"

int main() {
    std::size_t len = 4 * 1024;
    auto buf = rkros_dmabuf::DmaBuf::allocate(len);
    if (buf == nullptr) {
        std::cerr << "allocate dmabuf failed, size=" << len << std::endl;
        return -1;
    }

    if (buf->map() == nullptr) {
        std::cerr << "map dmabuf failed" << std::endl;
        return -1;
    }

    std::printf("buf: addr=%p, size=%zu\n", buf->addr(), buf->size());

    return 0;
}
