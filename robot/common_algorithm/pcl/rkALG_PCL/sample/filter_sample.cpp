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

#ifdef WIN32
#include <time.h>
#else
#include <sys/time.h>
//#define USE_ZERO_COPY
#endif

#define PRINT_COUT
int64_t sumTime1 = 0;
int64_t sumTime2 = 0;
int64_t sumTime3 = 0;
int64_t sumTime4 = 0;
int64_t sumTime = 0;
int32_t rkPclVoxelGrid(ALG_PCL_ENGINE* pstEngine, ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer)
{
#ifdef _WIN32
    clock_t startTime, endTime, timeAll;
    startTime = clock();
#else
    struct timeval startTime, endTime;// 秒+微妙
    time_t timeAll, timeAll_us;
    gettimeofday(&startTime, NULL);
#endif
    ALG_PCL_VOXEL_GRID_S VoxelGrid;
    VoxelGrid.f32LeafSizeX = 0.03;
    VoxelGrid.f32LeafSizeY = 0.03;
    VoxelGrid.f32LeafSizeZ = 0.03;
    rkALG_pclVoxelGrid(pstEngine, &VoxelGrid, pSrcPointBuffer, pDstPointBuffer);
#ifdef PRINT_COUT
    std::cout << "VoxelGrid count size: " << pDstPointBuffer->s32PointsNum << std::endl;
#endif
#ifdef _WIN32
    endTime = clock();
    timeAll = endTime - startTime;
    sumTime += timeAll;
    sumTime1 += timeAll;
#else
    gettimeofday(&endTime, NULL);
    timeAll_us = 1000000 * (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec);
    sumTime += timeAll_us;
    sumTime1 += timeAll_us;
#endif

    //#ifdef PRINT_COUT
    //    ALG_PCL_POINT_S* points = (ALG_PCL_POINT_S*)pDstPointBuffer->virAddr;
    //    for (int32_t idx = 0; idx < pDstPointBuffer->s32PointsNum; idx++)
    //    {
    //        std::cout << "x:" << points[idx].x << " y:" << points[idx].y << " z:" << points[idx].z <<std::endl;
    //    }
    //#endif
    return 0;
}

int32_t rkPclUniformSampling(ALG_PCL_ENGINE* pstEngine, ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer)
{
#ifdef _WIN32
    clock_t startTime, endTime, timeAll;
    startTime = clock();
#else
    struct timeval startTime, endTime;// 秒+微妙
    time_t timeAll, timeAll_us;
    gettimeofday(&startTime, NULL);
#endif
    ALG_PCL_UNIFORM_SAMPLING_S UniformSampling;
    UniformSampling.f32RadiusSearch = 0.03;
    rkALG_pclUniformSampling(pstEngine, &UniformSampling, pSrcPointBuffer, pDstPointBuffer);
#ifdef PRINT_COUT
    std::cout << "UniformSampling count size: " << pDstPointBuffer->s32PointsNum << std::endl;
#endif
#ifdef _WIN32
    endTime = clock();
    timeAll = endTime - startTime;
    sumTime += timeAll;
    sumTime1 += timeAll;
#else
    gettimeofday(&endTime, NULL);
    timeAll_us = 1000000 * (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec);
    sumTime += timeAll_us;
    sumTime1 += timeAll_us;
#endif

//#ifdef PRINT_COUT
//    ALG_PCL_POINT_S* points = (ALG_PCL_POINT_S*)pDstPointBuffer->virAddr;
//    for (int32_t idx = 0; idx < pDstPointBuffer->s32PointsNum; idx++)
//    {
//        std::cout << "x:" << points[idx].x << " y:" << points[idx].y << " z:" << points[idx].z <<std::endl;
//    }
//#endif
    return 0;
}


int32_t rkPclFilter(ALG_PCL_ENGINE* pstEngine, ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer)
{
#ifdef _WIN32
    clock_t startTime, endTime, timeAll;
    startTime = clock();
#else
    struct timeval startTime, endTime;// 秒+微妙
    time_t timeAll, timeAll_us;
    gettimeofday(&startTime, NULL);
#endif
    //passThrough
    ALG_PCL_PASS_THROUGH_S stPassThroughParams;
    stPassThroughParams.limitsNegative = 0;
    stPassThroughParams.minX = 0.01;
    stPassThroughParams.maxX = 1.0;
    stPassThroughParams.minZ = -0.14;
    stPassThroughParams.maxZ = 4.5;
    stPassThroughParams.passThroughMode = int32_t(PCL_PASS_THROUGH_MODE_XZ);
    rkALG_pclPassThrough(pstEngine, &stPassThroughParams, pSrcPointBuffer, pDstPointBuffer);
#ifdef PRINT_COUT
    std::cout << "PassThrough count size: " << pDstPointBuffer->s32PointsNum << std::endl;
#endif
#ifdef _WIN32
    endTime = clock();
    timeAll = endTime - startTime;
    sumTime += timeAll;
    sumTime1 += timeAll;
#else
    gettimeofday(&endTime, NULL);
    timeAll_us = 1000000 * (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec);
    sumTime += timeAll_us;
    sumTime1 += timeAll_us;
#endif

#ifdef _WIN32
    startTime = clock();
#else
    gettimeofday(&startTime, NULL);
#endif
    //radiusOutlierRemoval
    ALG_PCL_RADIUS_OUTLIER_REMOVAL_S stRadiusOutlierRemovalParams;
    stRadiusOutlierRemovalParams.s32NeighborsInRadius = 5;
    stRadiusOutlierRemovalParams.f32RadiusSearch = 0.05;
    rkALG_pclRadiusOutlierRemoval(pstEngine, &stRadiusOutlierRemovalParams, pDstPointBuffer, pSrcPointBuffer);
#ifdef PRINT_COUT
    std::cout << "RadiusOutlierRemoval count size: " << pSrcPointBuffer->s32PointsNum << std::endl;
#endif
#ifdef _WIN32
    endTime = clock();
    timeAll = endTime - startTime;
    sumTime += timeAll;
    sumTime2 += timeAll;
#else
    gettimeofday(&endTime, NULL);
    timeAll_us = 1000000 * (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec);
    sumTime += timeAll_us;
    sumTime2 += timeAll_us;
#endif

#ifdef _WIN32
    startTime = clock();
#else
    gettimeofday(&startTime, NULL);
#endif
    //statisticalOutlierRemoval
    ALG_PCL_STATISTICAL_OUTLIER_REMOVAL_S stStatisticalOutlierRemovalParams;
    stStatisticalOutlierRemovalParams.s32MeanK = 20;
    stStatisticalOutlierRemovalParams.f32StddevMulThresh = 1.0;
    rkALG_pclStatisticalOutlierRemoval(pstEngine, &stStatisticalOutlierRemovalParams, pSrcPointBuffer, pDstPointBuffer);
#ifdef PRINT_COUT
    std::cout << "StatisticalOutlierRemoval count size: " << pDstPointBuffer->s32PointsNum << std::endl;
#endif
#ifdef _WIN32
    endTime = clock();
    timeAll = endTime - startTime;
    sumTime += timeAll;
    sumTime3 += timeAll;
#else
    gettimeofday(&endTime, NULL);
    timeAll_us = 1000000 * (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec);
    sumTime += timeAll_us;
    sumTime3 += timeAll_us;
#endif

#ifdef _WIN32
    startTime = clock();
#else
    gettimeofday(&startTime, NULL);
#endif

    //euclideanClusterExtraction
    ALG_PCL_EUCLIDEAN_CLUSTER_S stEuclideanClusterExtractionParams;
    stEuclideanClusterExtractionParams.f32ClusterTolerance = 0.2;
    stEuclideanClusterExtractionParams.s32MinClusterSize = 10;
    stEuclideanClusterExtractionParams.s32MaxClusterSize = 20000;
    ALG_PCL_CLUSTER_RESULT_S stClusterResult;
    stClusterResult.pstIndicesBuffer = pSrcPointBuffer;
    rkALG_pclEuclideanClusterExtraction(pstEngine, &stEuclideanClusterExtractionParams, pDstPointBuffer, &stClusterResult);
#ifdef _WIN32
    endTime = clock();
    timeAll = endTime - startTime;
    sumTime += timeAll;
    sumTime4 += timeAll;
#else
    gettimeofday(&endTime, NULL);
    timeAll_us = 1000000 * (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec);
    sumTime += timeAll_us;
    sumTime4 += timeAll_us;
#endif

#ifdef PRINT_COUT
    int32_t* pointsIdx = (int32_t*)stClusterResult.pstIndicesBuffer->virAddr;
    for (int32_t idx = 0; idx < stClusterResult.s32ClustersNum; idx++)
    {
        int32_t idxOffset = stClusterResult.s32ClustersOffset[idx];
        int32_t clusterPointsNum = stClusterResult.s32ClustersPointsNum[idx];
        std::cout << "Cluster id: " << idx << "   Count size: "<< clusterPointsNum <<std::endl;
    }
#endif
    return 0;
}

int32_t testDepth2Points()
{
    int32_t width = 500;
    int32_t height = 400;
    int32_t depthSize = width * height;
    int32_t depthBufferSize = depthSize * sizeof(float);
    ALG_PCL_ENGINE pstEngine;
    ALG_PCL_INIT_S stInitParams;
    stInitParams.s32MaxPointNum = 15000;
    stInitParams.s32IsZeroCopy = 0;
    stInitParams.stDepthInfo.s32Flag = 1;
    //set depth info
    {
        stInitParams.stDepthInfo.f32DepthRatio = 0.001;
        stInitParams.stDepthInfo.s32Height = height;
        stInitParams.stDepthInfo.s32Width = width;
        stInitParams.stDepthInfo.s32StepW = 4;
        stInitParams.stDepthInfo.s32StepH = 4;
        stInitParams.stDepthInfo.stCameraMatrix.f32CenterX = 250;
        stInitParams.stDepthInfo.stCameraMatrix.f32CenterY = 200;
        stInitParams.stDepthInfo.stCameraMatrix.f32FocalX = 300;
        stInitParams.stDepthInfo.stCameraMatrix.f32FocalY = 300;
    }

#ifdef USE_ZERO_COPY
    ALG_DMA_BUFFER_S srcDepthDmaBuf;
    ALG_DMA_BUFFER_S dstPointsDmaBuf;
    stInitParams.s32IsZeroCopy = 1;
#endif // USE_ZERO_COPY
    rkALG_pclInit(&pstEngine, &stInitParams);
    //malloc
    ALG_DEPTH_IMG_S stSrcDepthBuffer;
    stSrcDepthBuffer.height = height;
    stSrcDepthBuffer.width = width;
    ALG_PCL_POINTS_BUFFER_S stDstPointBuffer;
    uint32_t pointsBufferSize = stInitParams.s32MaxPointNum * sizeof(ALG_PCL_POINT_S);
#ifdef USE_ZERO_COPY
    size_t bpp_depth_merged = 8 * sizeof(float);
    size_t bpp_point_merged = 8 * sizeof(ALG_PCL_POINT_S);
    rkALG_allocDrmBuffer(depthSize, 1, bpp_depth_merged, &srcDepthDmaBuf);
    stSrcDepthBuffer.fd = srcDepthDmaBuf.dmabuf_fd;
    stSrcDepthBuffer.virAddr = srcDepthDmaBuf.virAddr;
    rkALG_allocDrmBuffer(stInitParams.s32MaxPointNum, 1, bpp_point_merged, &dstPointsDmaBuf);
    stDstPointBuffer.fd = dstPointsDmaBuf.dmabuf_fd;
    stDstPointBuffer.virAddr = dstPointsDmaBuf.virAddr;
#else
    stSrcDepthBuffer.virAddr = malloc(depthBufferSize);
    stDstPointBuffer.virAddr = malloc(pointsBufferSize);
#endif // USE_ZERO_COPY
    float* tmpDepth = (float*)stSrcDepthBuffer.virAddr;
    for (int32_t h = 0; h < height; h++)
    {
        for (int32_t w = 0; w < width; w++)
        {
            tmpDepth[h * width + w] = w * h;
        }
    }
    int32_t iterNum = 10;
    for (int iter = 0; iter < iterNum; iter++)
    {
        rkALG_pclDepth2Points(&pstEngine, &stSrcDepthBuffer, &stDstPointBuffer);
    }
    std::vector<ALG_PCL_POINT_S> srcPoints;
    for (int m = 0; m < height; m = m + stInitParams.stDepthInfo.s32StepH)
    {
        for (int n = 0; n < width; n = n + stInitParams.stDepthInfo.s32StepW)
        {
            float d = tmpDepth[m * width + n];
            //if (d == 0.0)
            //{
            //    continue;
            //}
            // 注入点云
            //  pcl::PointXYZRGB point;
            ALG_PCL_POINT_S point;
            point.z = double(d) * stInitParams.stDepthInfo.f32DepthRatio;
            point.x = ((n - stInitParams.stDepthInfo.stCameraMatrix.f32CenterX) * point.z / stInitParams.stDepthInfo.stCameraMatrix.f32FocalX);
            point.y = ((m - stInitParams.stDepthInfo.stCameraMatrix.f32CenterY) * point.z / stInitParams.stDepthInfo.stCameraMatrix.f32FocalY);
            srcPoints.push_back(point);
        }
    }
    ALG_PCL_POINT_S* pTmpPoints = (ALG_PCL_POINT_S*)stDstPointBuffer.virAddr;
    printf("Points size: %d\n", stDstPointBuffer.s32PointsNum);
    float meanError = 0;
    for (int32_t i = 0; i < stDstPointBuffer.s32PointsNum; i++)
    {
        meanError += (srcPoints[i].x - pTmpPoints[i].x + srcPoints[i].y - pTmpPoints[i].y + srcPoints[i].z - pTmpPoints[i].z);
    }
    printf("meanError: %f\n", meanError / stDstPointBuffer.s32PointsNum);
#ifdef USE_ZERO_COPY
    rkALG_freeDrmBuffer(&srcDepthDmaBuf);
    rkALG_freeDrmBuffer(&dstPointsDmaBuf);
#else
    free(stSrcDepthBuffer.virAddr);
    free(stDstPointBuffer.virAddr);
#endif // USE_ZERO_COPY
    //destroy engine
    rkALG_pclDestroy(&pstEngine);
    return 0;
}

int testRemoveNaNFromPointCloud(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " <point number>" << std::endl;
        return -1;
    }
    int numPoints = atoi(argv[1]);
    std::cout << "point number: " << numPoints << std::endl;
    ALG_PCL_ENGINE stEngine;
    ALG_PCL_INIT_S stInitParams;
    stInitParams.s32MaxPointNum = numPoints;
#if USE_ZERO_COPY
    stInitParams.s32IsZeroCopy = 1;
#else
    stInitParams.s32IsZeroCopy = 0;
#endif
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
#if USE_ZERO_COPY
    size_t bpp_point_merged = 8 * sizeof(ALG_PCL_POINT_S);
    ALG_DMA_BUFFER_S srcPointsDmaBuf;
    ALG_DMA_BUFFER_S dstPointsDmaBuf;
    rkALG_allocDrmBuffer(stInitParams.s32MaxPointNum, 1, bpp_point_merged, &srcPointsDmaBuf);
    srcPointBuffer.fd = srcPointsDmaBuf.dmabuf_fd;
    srcPointBuffer.virAddr = srcPointsDmaBuf.virAddr;
    rkALG_allocDrmBuffer(stInitParams.s32MaxPointNum, 1, bpp_point_merged, &dstPointsDmaBuf);
    dstPointBuffer.fd = dstPointsDmaBuf.dmabuf_fd;
    dstPointBuffer.virAddr = dstPointsDmaBuf.virAddr;
#else
    srcPointBuffer.virAddr = aligned_alloc(16, pointsBufferSize);
    dstPointBuffer.virAddr = aligned_alloc(16, pointsBufferSize);
#endif
    for (int i = 0; i < srcPointBuffer.s32PointsNum; ++i) { /* generate some +inf/-inf/NaN */
        float* p = (float*)srcPointBuffer.virAddr + i * 4;
        p[0] = 1.f * rand() / (RAND_MAX + 1.0f);
        p[1] = 1.f * rand() / (RAND_MAX + 1.0f);
        p[2] = 1.f * rand() / (RAND_MAX + 1.0f);
        if (p[0] < 0.2) {
            p[0] = NAN;
        }
        if (p[1] > 0.8) {
            p[1] = INFINITY;
        }
        if (p[2] < 0.6 && p[2] > 0.4) {
            p[2] = -INFINITY;
        }
        p[3] = 1.0f;
    }

    ALG_PCL_BACKEND_E eBackEnd = ALG_CPU_BACKEND;
    std::vector<int> indices;
    indices.reserve(srcPointBuffer.s32PointsNum);
    int validPointsNum = 0;
    ret = rkALG_pclRemoveNaNFromPointCloud(&stEngine, &srcPointBuffer, &dstPointBuffer, indices.data(), &validPointsNum, eBackEnd);
    indices.resize(validPointsNum);

#if USE_ZERO_COPY
    rkALG_freeDrmBuffer(&srcPointsDmaBuf);
    rkALG_freeDrmBuffer(&dstPointsDmaBuf);
#else
    free(srcPointBuffer.virAddr);
    free(dstPointBuffer.virAddr);
#endif
    rkALG_pclDestroy(&stEngine);
    return ret;
}

int32_t main(int argc, char** argv)
{
    char rkPclVersion[128];
    rkALG_pclVersion(rkPclVersion);
#ifdef WIN32
    argv[1] = "D:/LinuxShare/PCL_data/";
#endif // USE_EXE
    if (argc < 2) {
	std::cerr << "Error: Missing required argument" << std::endl;
	std::cout << "usage: " << argv[0] << " <data_directory_path>" << std::endl;
	std::cout << "example: " << argv[0] << "/data/pcl_tmp/" << std::endl;
	return -1;
    }
    std::string filePath = std::string(argv[1]);
    int32_t iterNum = 1;
    int32_t dataNum = 472;
    //init engine
    ALG_PCL_ENGINE pstEngine;
    ALG_PCL_INIT_S stInitParams;
    stInitParams.s32MaxPointNum = 300000;
    stInitParams.s32IsZeroCopy = 0;
    stInitParams.stDepthInfo.s32Flag = 0;
#ifdef USE_ZERO_COPY
    ALG_DMA_BUFFER_S srcPointsDmaBuf;
    ALG_DMA_BUFFER_S dstPointsDmaBuf;
    stInitParams.s32IsZeroCopy = 1;
#endif // USE_ZERO_COPY
    int32_t ret = rkALG_pclInit(&pstEngine, &stInitParams);
    //malloc
    ALG_PCL_POINTS_BUFFER_S pSrcPointBuffer;
    ALG_PCL_POINTS_BUFFER_S pDstPointBuffer;
    uint32_t bufferSize = stInitParams.s32MaxPointNum * sizeof(ALG_PCL_POINT_S);
#ifdef USE_ZERO_COPY
    size_t bpp_point_merged = 8 * sizeof(ALG_PCL_POINT_S);
    rkALG_allocDrmBuffer(stInitParams.s32MaxPointNum, 1, bpp_point_merged, &srcPointsDmaBuf);
    pSrcPointBuffer.fd = srcPointsDmaBuf.dmabuf_fd;
    pSrcPointBuffer.virAddr = srcPointsDmaBuf.virAddr;

    rkALG_allocDrmBuffer(stInitParams.s32MaxPointNum, 1, bpp_point_merged, &dstPointsDmaBuf);
    pDstPointBuffer.fd = dstPointsDmaBuf.dmabuf_fd;
    pDstPointBuffer.virAddr = dstPointsDmaBuf.virAddr;
#else
    pSrcPointBuffer.virAddr = malloc(bufferSize);
    pDstPointBuffer.virAddr = malloc(bufferSize);
#endif // USE_ZERO_COPY
    for(int iter=0;iter< iterNum;iter++)
    {
        sumTime = 0;
        sumTime1 = 0;
        sumTime2 = 0;
        sumTime3 = 0;
        sumTime4 = 0;
        for (int32_t i = 0; i < dataNum; i++)
        {
            std::string dataPath = filePath + "frame_" + std::to_string(i) + ".bin";
            uint64_t bufferSize = 0;
            readBinaryFile2(dataPath.c_str(), pSrcPointBuffer.virAddr, bufferSize);
            //第一种读入方法较多场合如此

            int32_t srcPointsNum = bufferSize / sizeof(ALG_PCL_POINT_S);
            int32_t dstPointsNum = srcPointsNum;
            uint32_t currBufferSize = srcPointsNum * sizeof(ALG_PCL_POINT_S);
            pSrcPointBuffer.s32PointsNum = srcPointsNum;
            pDstPointBuffer.s32PointsNum = dstPointsNum;
    #ifdef PRINT_COUT
            std::cout << "Fram: " << i << "   Count size: " << srcPointsNum << std::endl;
    #endif
            //rk pcl
            rkPclFilter(&pstEngine, &pSrcPointBuffer, &pDstPointBuffer);
            //uniform sampling
            //rkPclUniformSampling(&pstEngine, &pSrcPointBuffer, &pDstPointBuffer);
            //voxel grid
            //rkPclVoxelGrid(&pstEngine, &pSrcPointBuffer, &pDstPointBuffer);
        }
        float meanTime = sumTime / float(dataNum);
        float meanTime1 = sumTime1 / float(dataNum);
        float meanTime2 = sumTime2 / float(dataNum);
        float meanTime3 = sumTime3 / float(dataNum);
        float meanTime4 = sumTime4 / float(dataNum);
#ifdef WIN32
        printf("Final all mean time: %f ms\n", meanTime);
        printf("Final all mean1 time: %f ms\n", meanTime1);
        printf("Final all mean2 time: %f ms\n", meanTime2);
        printf("Final all mean3 time: %f ms\n", meanTime3);
        printf("Final all mean4 time: %f ms\n", meanTime4);
#else
        printf("Final all mean time: %f us\n", meanTime);
        printf("Final all mean1 time: %f us\n", meanTime1);
        printf("Final all mean2 time: %f us\n", meanTime2);
        printf("Final all mean3 time: %f us\n", meanTime3);
        printf("Final all mean4 time: %f us\n", meanTime4);
#endif
    }
#ifdef USE_ZERO_COPY
    rkALG_freeDrmBuffer(&srcPointsDmaBuf);
    rkALG_freeDrmBuffer(&dstPointsDmaBuf);
#else
    free(pSrcPointBuffer.virAddr);
    free(pDstPointBuffer.virAddr);
#endif // USE_ZERO_COPY
    //destroy engine
    rkALG_pclDestroy(&pstEngine);
	return 0;
}
