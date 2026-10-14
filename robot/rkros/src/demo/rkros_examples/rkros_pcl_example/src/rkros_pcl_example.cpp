#include <iostream>

#include "rkros_pcl/rkALG_pclApp.h"

int main() {
    char buf[128] = {0};
    int32_t ret = rkALG_pclVersion(buf);
    if (ret == 0) {
        std::cout << "rkALG_pcl version: " << buf << std::endl;
        return 0;
    } else {
        std::cerr << "rkALG_pclVersion failed with code " << ret << std::endl;
        return 1;
    }
}
