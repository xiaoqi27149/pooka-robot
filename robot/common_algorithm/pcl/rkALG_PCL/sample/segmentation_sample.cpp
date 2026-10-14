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

int testEuclideanClusterExtraction() {
    int maxPointsNum = 300000;
    ALG_PCL_ENGINE stEngine;
    ALG_PCL_INIT_S stInitParams;
    stInitParams.s32MaxPointNum = maxPointsNum;
    stInitParams.s32IsZeroCopy = 0;
    stInitParams.stDepthInfo.s32Flag = 0;

    int ret = rkALG_pclInit(&stEngine, &stInitParams);
    if (ret != 0) {
        std::cout << "init failed" << std::endl;
        return -1;
    }
    ALG_PCL_POINTS_BUFFER_S srcPointBuffer;
    ALG_PCL_INDICES_BUFFER_S indicesBuffer;
    srcPointBuffer.s32PointsNum = maxPointsNum;

    size_t pointsBufferSize = srcPointBuffer.s32PointsNum * sizeof(ALG_PCL_POINT_S);
    srcPointBuffer.virAddr = malloc(pointsBufferSize);
    indicesBuffer.virAddr = malloc(srcPointBuffer.s32PointsNum * sizeof(int));

    size_t bufferSize = 0;
    readBinaryFile2(DATA_DIR"frame_0.bin", srcPointBuffer.virAddr, bufferSize);
    int pointsNum = bufferSize / sizeof(ALG_PCL_POINT_S);
    srcPointBuffer.s32PointsNum = pointsNum;
    indicesBuffer.s32PointsNum = pointsNum;
    std::cout << "src point cloud size: " << pointsNum << std::endl;

    ALG_PCL_EUCLIDEAN_CLUSTER_S stEuclideanClusterParams;
    stEuclideanClusterParams.f32ClusterTolerance = 5.0;
    stEuclideanClusterParams.s32MinClusterSize = 10;
    stEuclideanClusterParams.s32MaxClusterSize = 20000;

    ALG_PCL_CLUSTER_RESULT_S stClusterResult;
    stClusterResult.pstIndicesBuffer = &indicesBuffer;

    ret = rkALG_pclEuclideanClusterExtraction(&stEngine, &stEuclideanClusterParams, &srcPointBuffer, &stClusterResult);

    std::cout << "cluster number: " << stClusterResult.s32ClustersNum << std::endl;
    for (int i = 0; i < stClusterResult.s32ClustersNum; ++i) {
        std::cout << "cluster [" << i << "].size: " << stClusterResult.s32ClustersPointsNum[i] << std::endl;
        if (stClusterResult.s32ClustersPointsNum[i] < 20) {
            int* pIndices = (int*)stClusterResult.pstIndicesBuffer->virAddr + stClusterResult.s32ClustersOffset[i];
            for (int j = 0; j < stClusterResult.s32ClustersPointsNum[i]; ++j) {
                std::cout << pIndices[j] << " ";
            }
            std::cout << std::endl;
        }
    }
    free(srcPointBuffer.virAddr);
    rkALG_pclDestroy(&stEngine);
    return 0;
}

int testRegionGrowing() {
    int maxPointsNum = 10000;
    ALG_PCL_ENGINE stEngine;
    ALG_PCL_INIT_S stInitParams;
    stInitParams.s32MaxPointNum = maxPointsNum;
    stInitParams.s32IsZeroCopy = 0;
    stInitParams.stDepthInfo.s32Flag = 0;

    int ret = rkALG_pclInit(&stEngine, &stInitParams);
    if (ret != 0) {
        std::cout << "init failed" << std::endl;
        return -1;
    }
    ALG_PCL_POINTS_BUFFER_S srcPointBuffer;
    srcPointBuffer.virAddr = aligned_alloc(16, maxPointsNum * sizeof(ALG_PCL_POINT_S));
    size_t bufferSize = 0;
    readBinaryFile2(DATA_DIR"frame_0.bin", srcPointBuffer.virAddr, bufferSize);
    int pointsNum = bufferSize / sizeof(ALG_PCL_POINT_S);
    srcPointBuffer.s32PointsNum = pointsNum;

    /* estimate normal first */
    ALG_PCL_NORMAL_ESTIMATION_S stNormalEstimationParam;
    memset(&stNormalEstimationParam, 0, sizeof(stNormalEstimationParam));
    stNormalEstimationParam.eNeighborMode = PCL_NEIGHBOR_MODE_KNN;
    stNormalEstimationParam.s32K = 30;

    ALG_PCL_NORMALS_BUFFER_S dstNormalsBuffer;
    dstNormalsBuffer.s32PointsNum = pointsNum;
    size_t normalsBufferSize = pointsNum * sizeof(ALG_PCL_NORMAL_S);
    dstNormalsBuffer.virAddr = aligned_alloc(16, normalsBufferSize);
    rkALG_pclNormalEstimation(&stEngine, &stNormalEstimationParam, &srcPointBuffer, &dstNormalsBuffer);

    /* then region grow */
    ALG_PCL_REGION_GROWING_S stRegionGrowingParams;
    stRegionGrowingParams.u32MinPointsPerCluster = 10;
    stRegionGrowingParams.u32MaxPointsPerCluster = 100000;
    stRegionGrowingParams.u8SmoothModeFlag = 1;
    stRegionGrowingParams.u8CurvatureFlag = 1;
    stRegionGrowingParams.u8ResidualFlag = 0;
    stRegionGrowingParams.f32RadianThetaThreshold = 5.f / 180.0 * 3.141592665f;
    stRegionGrowingParams.f32CurvatureThrehold = 0.9;
    stRegionGrowingParams.f32ResidualThreshold = 0.05;
    stRegionGrowingParams.u32NeighborNumber = 30;
    stRegionGrowingParams.pNormalsBuffer = &dstNormalsBuffer;

    ALG_PCL_INDICES_BUFFER_S dstIndicesBuffer;
    dstIndicesBuffer.s32PointsNum = pointsNum;
    dstIndicesBuffer.virAddr = aligned_alloc(16, pointsNum * sizeof(int));
    ALG_PCL_CLUSTER_RESULT_S stClusterResult;
    memset(&stClusterResult, 0, sizeof(stClusterResult));
    stClusterResult.pstIndicesBuffer = &dstIndicesBuffer;
    rkALG_pclRegionGrowing(&stEngine, &srcPointBuffer, &stRegionGrowingParams, &stClusterResult);
    std::cout << "cluster number: " << stClusterResult.s32ClustersNum << std::endl;
    for (int i = 0; i < stClusterResult.s32ClustersNum; ++i) {
        std::cout << "cluster [" << i << "].size: " << stClusterResult.s32ClustersPointsNum[i] << std::endl;
        if (stClusterResult.s32ClustersPointsNum[i] < 20) {
            int* pIndices = (int*)stClusterResult.pstIndicesBuffer->virAddr + stClusterResult.s32ClustersOffset[i];
            for (int j = 0; j < stClusterResult.s32ClustersPointsNum[i]; ++j) {
                std::cout << pIndices[j] << " ";
            }
            std::cout << std::endl;
        }
    }

    free(srcPointBuffer.virAddr);
    free(dstNormalsBuffer.virAddr);
    free(dstIndicesBuffer.virAddr);
    rkALG_pclDestroy(&stEngine);
    return 0;
}

int main(int argc, char** argv) {
    // testRegionGrowing();
    testEuclideanClusterExtraction();

    return 0;
}
