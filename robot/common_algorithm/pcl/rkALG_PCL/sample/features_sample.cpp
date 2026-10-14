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


int testNormalEstimationAndBoundaryEstimation(int argc, char** argv) {
    ALG_PCL_ENGINE stEngine;
    ALG_PCL_INIT_S stInitParams;

    int maxPointsNum = 300000;
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

    /* estimate normals first */
    ALG_PCL_NORMAL_ESTIMATION_S stNormalEstimationParam;
    stNormalEstimationParam.eNeighborMode = PCL_NEIGHBOR_MODE_KNN;
    stNormalEstimationParam.s32K = 20;
    stNormalEstimationParam.f32Radius = 1.f;
    stNormalEstimationParam.f32ViewPointX = 0;
    stNormalEstimationParam.f32ViewPointY = 0;
    stNormalEstimationParam.f32ViewPointZ = 0;

    ALG_PCL_NORMALS_BUFFER_S dstNormalsBuffer;
    dstNormalsBuffer.s32PointsNum = pointsNum;
    size_t normalsBufferSize = pointsNum * sizeof(ALG_PCL_NORMAL_S);
    dstNormalsBuffer.virAddr = aligned_alloc(16, normalsBufferSize);

    /* pcl::NormalEstimation::compute */
    rkALG_pclNormalEstimation(&stEngine, &stNormalEstimationParam, &srcPointBuffer, &dstNormalsBuffer);

    /* then estimate boundary */
    ALG_PCL_BOUNDARY_ESTIMATION_S stBoundaryEstimationParam;
    stBoundaryEstimationParam.eNeighborMode = PCL_NEIGHBOR_MODE_KNN;
    stBoundaryEstimationParam.s32K = 20;
    stBoundaryEstimationParam.f32Radius = 1.f;
    stBoundaryEstimationParam.pNormalsBuffer = &dstNormalsBuffer;
    stBoundaryEstimationParam.f32RadianAngleThreshold = 90.f * 3.1415926535f / 180.f; /* radian */

    std::vector<uint8_t> boundaries(pointsNum, 0);
    /* pcl::BoundaryEstimation::compute */
    rkALG_pclBoundaryEstimation(&stEngine, &stBoundaryEstimationParam, &srcPointBuffer, boundaries.data());

    free(srcPointBuffer.virAddr);
    free(dstNormalsBuffer.virAddr);
    rkALG_pclDestroy(&stEngine);

    return 0;
}

int testMomentOfInertia(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " <step>" << std::endl;
        return -1;
    }
    float step = atof(argv[1]);
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
    srcPointBuffer.virAddr = aligned_alloc(16, maxPointsNum * sizeof(ALG_PCL_POINT_S));
    size_t bufferSize = 0;
    readBinaryFile2("/data/pcl_tmp/frame_0.bin", srcPointBuffer.virAddr, bufferSize);
    int pointsNum = bufferSize / sizeof(ALG_PCL_POINT_S);
    srcPointBuffer.s32PointsNum = pointsNum;

    ALG_PCL_MOMENT_OF_INERTIA_S stMomentOfInertiaParams;
    stMomentOfInertiaParams.f32AngleStep = step;
    stMomentOfInertiaParams.u8NormalFlag = 1;
    stMomentOfInertiaParams.f32PointMass = 0.5f;

    ALG_PCL_MOMENT_OF_INERTIA_RESULT_S stMomentOfInertiaResult;
    memset(&stMomentOfInertiaResult, 0, sizeof(stMomentOfInertiaResult));
    int length = (int)(360.f / stMomentOfInertiaParams.f32AngleStep + 1) * (int)(90.f / stMomentOfInertiaParams.f32AngleStep + 1);
    stMomentOfInertiaResult.pf32MomentOfInertia = (float*)malloc(sizeof(float) * length);
    stMomentOfInertiaResult.pf32Eccentricity = (float*)malloc(sizeof(float) * length);

    rkALG_pclMomentOfInertia(&stEngine, &stMomentOfInertiaParams, &srcPointBuffer, &stMomentOfInertiaResult);

    printf("mass center: %f, %f, %f\n", stMomentOfInertiaResult.massCenter.x, stMomentOfInertiaResult.massCenter.y, stMomentOfInertiaResult.massCenter.z);
    printf("AABB\n");
    printf("minPoint: %f, %f, %f\n", stMomentOfInertiaResult.AABBMinPoint.x, stMomentOfInertiaResult.AABBMinPoint.y, stMomentOfInertiaResult.AABBMinPoint.z);
    printf("maxPoint: %f, %f, %f\n", stMomentOfInertiaResult.AABBMaxPoint.x, stMomentOfInertiaResult.AABBMaxPoint.y, stMomentOfInertiaResult.AABBMaxPoint.z);
    printf("EigenValues:\n");
    printf("major_value: %f\n", stMomentOfInertiaResult.major_eigen_value);
    printf("middle_value: %f\n", stMomentOfInertiaResult.middle_eigen_value);
    printf("minor_value: %f\n", stMomentOfInertiaResult.minor_eigen_value);
    printf("EigenVectors:\n");
    printf("major_vector: %f, %f, %f\n", stMomentOfInertiaResult.major_eigen_vector[0], stMomentOfInertiaResult.major_eigen_vector[1], stMomentOfInertiaResult.major_eigen_vector[2]);
    printf("middle_vector: %f, %f, %f\n", stMomentOfInertiaResult.middle_eigen_vector[0], stMomentOfInertiaResult.middle_eigen_vector[1], stMomentOfInertiaResult.middle_eigen_vector[2]);
    printf("minor_vector: %f, %f, %f\n", stMomentOfInertiaResult.minor_eigen_vector[0], stMomentOfInertiaResult.minor_eigen_vector[1], stMomentOfInertiaResult.minor_eigen_vector[2]);

    float* pMomentOfInertia = stMomentOfInertiaResult.pf32MomentOfInertia;
    float* pEccentricity = stMomentOfInertiaResult.pf32Eccentricity;
    for (int i = 0; i < 10; ++i) {
        printf("moment_of_inertia[%d]: %f\n", i, pMomentOfInertia[i]);
        printf("eccentricity[%d]: %f\n", i, pEccentricity[i]);
    }
    printf("moment_of_inertia[-1]: %f\n", pMomentOfInertia[length - 1]);
    printf("eccentricity[-1]: %f\n", pEccentricity[length - 1]);

    printf("OBB\n");
    printf("minPoint: %f, %f, %f\n", stMomentOfInertiaResult.OBBMinPoint.x, stMomentOfInertiaResult.OBBMinPoint.y, stMomentOfInertiaResult.OBBMinPoint.z);
    printf("maxPoint: %f, %f, %f\n", stMomentOfInertiaResult.OBBMaxPoint.x, stMomentOfInertiaResult.OBBMaxPoint.y, stMomentOfInertiaResult.OBBMaxPoint.z);
    printf("position: %f, %f, %f\n", stMomentOfInertiaResult.OBBPosition.x, stMomentOfInertiaResult.OBBPosition.y, stMomentOfInertiaResult.OBBPosition.z);
    Eigen::Map<Eigen::Matrix3f> rotation_matrix_OBB(stMomentOfInertiaResult.pf32OBBRotationMatrix);
    std::cout << "rotation_matrix_OBB:\n" << rotation_matrix_OBB << std::endl;

    free(srcPointBuffer.virAddr);
    free(stMomentOfInertiaResult.pf32MomentOfInertia);
    free(stMomentOfInertiaResult.pf32Eccentricity);
    rkALG_pclDestroy(&stEngine);

    return 0;
}

int main(int argc, char** argv) {
    // testNormalEstimationAndBoundaryEstimation(argc, argv);
    testMomentOfInertia(argc, argv);
    return 0;
}
