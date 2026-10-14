#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cmath>
#include <Eigen/Dense>
#include "rkALG_pclApp.h"
#include "rkALG_drmBuffer.h"
#include "utils.h"
#include <random>

#include <cmath>
#include <random>

#define DATA_DIR "/data/pcl_tmp/"

int testKDTreeSearch() {
    const int numPoints = 300000;
    printf("point number: %d\n", numPoints);
    ALG_PCL_ENGINE stEngine;
    ALG_PCL_INIT_S stInitParams;
    stInitParams.s32MaxPointNum = numPoints;
    stInitParams.s32IsZeroCopy = 0;
    stInitParams.stDepthInfo.s32Flag = 0;

    int ret = rkALG_pclInit(&stEngine, &stInitParams);
    if (ret != 0) {
        std::cout << "init failed" << std::endl;
        return -1;
    }
    ALG_PCL_POINTS_BUFFER_S srcPointBuffer;
    srcPointBuffer.s32PointsNum = numPoints;
    size_t pointsBufferSize = srcPointBuffer.s32PointsNum * sizeof(ALG_PCL_POINT_S);
    srcPointBuffer.virAddr = aligned_alloc(16, pointsBufferSize);

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(0.0f, 1024.0f);

    for (int i = 0; i < numPoints; ++i) {
        float* p = (float*)srcPointBuffer.virAddr + i * 4;
        p[0] = dist(rng);
        p[1] = dist(rng);
        p[2] = dist(rng);
        p[3] = 1.0f;
    }

    /* set search point */
    ALG_PCL_POINT_S searchPoint;
    searchPoint.x = dist(rng);
    searchPoint.y = dist(rng);
    searchPoint.z = dist(rng);

    /* Nearest K Search */
    int K = 10;
    int* indices_KNN = new int[K];
    float* dists_KNN = new float[K];
    int numKNNeighbors = rkALG_pclKdTreeNearestKSearch(&stEngine, &srcPointBuffer, &searchPoint, K, indices_KNN, dists_KNN);
    float* pSrcPoints = (float*)srcPointBuffer.virAddr;
    for (int i = 0; i < numKNNeighbors; ++i) {
        printf("%d[%d]: (%6f, %6f, %6f) (squared distance: %6f)\n", i, indices_KNN[i], pSrcPoints[indices_KNN[i] * 4], pSrcPoints[indices_KNN[i] * 4 + 1], pSrcPoints[indices_KNN[i] * 4 + 2], dists_KNN[i]);
    }

    /* Radius Search */
    float radius = 50.0;
    uint32_t max_nn = 0;
    int* indices_RSearch = new int[numPoints];
    float* dists_RSearch = new float[numPoints];
    int numRadiusNeighbors = rkALG_pclKdTreeRadiusSearch(&stEngine, &srcPointBuffer, &searchPoint, radius, indices_RSearch, dists_RSearch, max_nn);
    printf("radius search: %d neighbors\n", numRadiusNeighbors);
    for (int i = 0; i < numRadiusNeighbors; ++i) {
        printf("%d[%d]: (%6f, %6f, %6f) (squared distance: %6f)\n", i, indices_RSearch[i], pSrcPoints[indices_RSearch[i] * 4], pSrcPoints[indices_RSearch[i] * 4 + 1], pSrcPoints[indices_RSearch[i] * 4 + 2], dists_RSearch[i]);
    }

    delete [] indices_KNN;
    delete [] dists_KNN;
    delete [] indices_RSearch;
    delete [] dists_RSearch;
    free(srcPointBuffer.virAddr);
    rkALG_pclDestroy(&stEngine);

    return 0;
}


int main() {
    testKDTreeSearch();
    return 0;
}
