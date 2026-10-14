#include <iostream>

#include "rkros_rockit/rk_mpi_sys.h"

int main() {
    if (RK_MPI_SYS_Init() != RK_SUCCESS) {
        std::cout << "rockit init failed!" << std::endl;
        return -1;
    }

    RK_MPI_SYS_Exit();

    std::cout << "Done." << std::endl;

    return 0;
}
