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

int32_t testTransformCloudPoint(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " <point number>" << std::endl;
        return -1;
    }
    int numPoints = atoi(argv[1]);
    std::cout << "point number: " << numPoints << std::endl;
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

    Eigen::Matrix4f transform_eigen;
    transform_eigen << 1, 0, 0, 0.5,
                       0, 1, 0, -0.5,
                       0, 0, -1, 0,
                       0, 0, 0, 1;
    ALG_PCL_POINTS_BUFFER_S srcPointBuffer;
    ALG_PCL_POINTS_BUFFER_S dstPointBuffer;
    srcPointBuffer.s32PointsNum = numPoints;
    dstPointBuffer.s32PointsNum = numPoints;
    size_t pointsBufferSize = srcPointBuffer.s32PointsNum * sizeof(ALG_PCL_POINT_S);

    srcPointBuffer.virAddr = aligned_alloc(16, pointsBufferSize);
    dstPointBuffer.virAddr = aligned_alloc(16, pointsBufferSize);
    for (int i = 0; i < srcPointBuffer.s32PointsNum; ++i) {
        float* p = (float*)srcPointBuffer.virAddr + i * 4;
        p[0] = 1024.f * rand() / (RAND_MAX + 1.0f);
        p[1] = 1024.f * rand() / (RAND_MAX + 1.0f);
        p[2] = 1024.f * rand() / (RAND_MAX + 1.0f);
    }
    ALG_PCL_BACKEND_E backend = ALG_CPU_BACKEND; /* cpu/gpu */
    float* pTransform = transform_eigen.data();
    ret |= rkALG_pclTransformPointCloud(&stEngine, &srcPointBuffer, &dstPointBuffer, pTransform, backend);

    free(srcPointBuffer.virAddr);
    free(dstPointBuffer.virAddr);
    ret |= rkALG_pclDestroy(&stEngine);
    return ret;
}

int testCopyPointCloud(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " <point number>" << std::endl;
        return -1;
    }
    int numPoints = atoi(argv[1]);
    std::cout << "point number: " << numPoints << std::endl;
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
    ALG_PCL_POINTS_BUFFER_S dstPointBuffer;
    srcPointBuffer.s32PointsNum = numPoints;
    dstPointBuffer.s32PointsNum = numPoints;
    size_t pointsBufferSize = srcPointBuffer.s32PointsNum * sizeof(ALG_PCL_POINT_S);

    srcPointBuffer.virAddr = aligned_alloc(16, pointsBufferSize);
    dstPointBuffer.virAddr = aligned_alloc(16, pointsBufferSize);

    for (int i = 0; i < srcPointBuffer.s32PointsNum; ++i) {
        float* p = (float*)srcPointBuffer.virAddr + i * 4;
        p[0] = i;
        p[1] = i;
        p[2] = i;
        p[3] = 1.0f;
    }
    ALG_PCL_BACKEND_E eBackEnd = ALG_CPU_BACKEND;

    ret = rkALG_pclCopyPointCloud(&stEngine, &srcPointBuffer, &dstPointBuffer, eBackEnd); /* without idx */

    std::vector<int> index;
    for (int i = 0; i < numPoints; i += 3) { /* generate index */
        index.push_back(i);
    }
    int32_t* pIndex = index.data();
    int validPointsNum = index.size();
    ret = rkALG_pclCopyPointCloudWithIndex(&stEngine, &srcPointBuffer, &dstPointBuffer, pIndex, validPointsNum, eBackEnd); /* with idx */

    free(srcPointBuffer.virAddr);
    free(dstPointBuffer.virAddr);
    rkALG_pclDestroy(&stEngine);

    return 0;
}


int testGetMinMax3D(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " <point number>" << std::endl;
        return -1;
    }
    int numPoints = atoi(argv[1]);
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
    std::uniform_real_distribution<float> dist(-10000.0f, 10000.0f);

    for (int i = 0; i < numPoints; ++i) {
        float* p = (float*)srcPointBuffer.virAddr + i * 4;
        p[0] = dist(rng);
        p[1] = dist(rng);
        p[2] = dist(rng);
        p[3] = 1.0f;
    }

    ALG_PCL_POINT_S stMinPoint, stMaxPoint;
    rkALG_pclGetMinMax3D(&stEngine, &srcPointBuffer, &stMinPoint, &stMaxPoint);
    printf("min: (%f, %f, %f)\n", stMinPoint.x, stMinPoint.y, stMinPoint.z);
    printf("max: (%f, %f, %f)\n", stMaxPoint.x, stMaxPoint.y, stMaxPoint.z);

    std::vector<int> index;
    for (int i = 0; i < numPoints / 3; ++i) {
        index.push_back(i * 2);
    }
    rkALG_pclGetMinMax3DWithIndex(&stEngine, &srcPointBuffer, index.data(), index.size(), &stMinPoint, &stMaxPoint);
    printf("min: (%f, %f, %f)\n", stMinPoint.x, stMinPoint.y, stMinPoint.z);
    printf("max: (%f, %f, %f)\n", stMaxPoint.x, stMaxPoint.y, stMaxPoint.z);

    free(srcPointBuffer.virAddr);
    rkALG_pclDestroy(&stEngine);
    return 0;
}

int main(int argc, char** argv) {
    // testTransformCloudPoint(argc, argv);
    // testCopyPointCloud(argc, argv);
    testGetMinMax3D(argc, argv);
    return 0;
}