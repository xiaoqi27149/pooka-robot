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

int icp_test(int argc, char** argv) {
    int numPoints = 1000;
    if (argc >= 2) {
        numPoints = atoi(argv[1]);
    }
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
    ALG_PCL_POINTS_BUFFER_S srcPointBuffer, targetPointBuffer, dstPointBuffer;
    srcPointBuffer.s32PointsNum = numPoints;
    size_t pointsBufferSize = srcPointBuffer.s32PointsNum * sizeof(ALG_PCL_POINT_S);
    srcPointBuffer.virAddr = aligned_alloc(16, pointsBufferSize);
    targetPointBuffer.s32PointsNum = numPoints;
    targetPointBuffer.virAddr = aligned_alloc(16, pointsBufferSize);
    dstPointBuffer.s32PointsNum = numPoints;
    dstPointBuffer.virAddr = aligned_alloc(16, pointsBufferSize);

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-150.0f, 150.0f);

    for (int i = 0; i < numPoints; ++i) {
        float* pSrc = (float*)srcPointBuffer.virAddr + i * 4;
        pSrc[0] = dist(rng);
        pSrc[1] = dist(rng);
        pSrc[2] = dist(rng);
        pSrc[3] = 1.0f;
        float* pTarget = (float*)targetPointBuffer.virAddr + i * 4;
        pTarget[0] = pSrc[0] + 1.0f;
        pTarget[1] = pSrc[1] - 0.5f;
        pTarget[2] = pSrc[2] + 1.2f;
        pTarget[3] = 1.0f;
    }

    Eigen::Matrix4f initGuess = Eigen::Matrix4f::Identity();
    initGuess << 1, 0, 0, -0.5,
                0, 0, 1, 0.5,
                0, 1, 0, -0.5,
                0, 0, 0, 1;
    rkALG_PCL_ICP_PARAM_S ICPParam;
    memset(&ICPParam, 0, sizeof(rkALG_PCL_ICP_PARAM_S));
    ICPParam.u32MaxIterations = 10;
    // ICPParam.f32MaxCorrespondenceDistance = 1.3;
    ICPParam.f32TransformationEpsilon = 1e-6;
    ICPParam.f32EuclideanFitnessEpsilon = 1e-6;
    ICPParam.u8GetFitnessScore = 0;
    ICPParam.u8EnableInitGuess = 1;
    memcpy(ICPParam.pf32InitGuess, initGuess.data(), sizeof(float) * 16);

    rkALG_PCL_REGISTRATION_RESULT_S ICPResult;
    memset(&ICPResult, 0, sizeof(rkALG_PCL_REGISTRATION_RESULT_S));
    ICPResult.pDstPointBuffer = &dstPointBuffer;

    ALG_PCL_BACKEND_E eBackEnd = ALG_GPU_BACKEND;

    ret = rkALG_pclIterativeClosestPoint(&stEngine, &ICPParam, &srcPointBuffer, &targetPointBuffer, &ICPResult, eBackEnd);

    std::cout << "icp.u8IsConverged: " << +ICPResult.u8IsConverged << std::endl;
    Eigen::Map<Eigen::Matrix4f> transform(ICPResult.f32Transformation);
    std::cout << "icp.transform" << std::endl << transform << std::endl;
    if (ICPParam.u8GetFitnessScore) {
        std::cout << "icp.fitnessScore: " << ICPResult.f32FitnessScore << std::endl;
    }

    free(srcPointBuffer.virAddr);
    free(targetPointBuffer.virAddr);
    free(dstPointBuffer.virAddr);
    rkALG_pclDestroy(&stEngine);

    return 0;
}

int icp_normals_test(int argc, char** argv) {
    int numPoints = 20;
    if (argc >= 2) {
        numPoints = atoi(argv[1]);
    }
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
    ALG_PCL_POINTS_BUFFER_S srcPointBuffer, targetPointBuffer;
    ALG_PCL_NORMALS_BUFFER_S srcNormalBuffer, targetNormalBuffer;
    ALG_PCL_POINTNORMALS_BUFFER_S srcPointNormalBuffer, targetPointNormalBuffer;
    srcPointBuffer.s32PointsNum = numPoints;
    srcPointBuffer.virAddr = aligned_alloc(16, srcPointBuffer.s32PointsNum * sizeof(ALG_PCL_POINT_S));
    srcNormalBuffer.s32PointsNum = numPoints;
    srcNormalBuffer.virAddr = aligned_alloc(16, srcNormalBuffer.s32PointsNum * sizeof(ALG_PCL_NORMAL_S));
    srcPointNormalBuffer.s32PointsNum = numPoints;
    srcPointNormalBuffer.virAddr = aligned_alloc(16, srcPointNormalBuffer.s32PointsNum * sizeof(ALG_PCL_POINTNORMAL_S));
    targetPointBuffer.s32PointsNum = numPoints;
    targetPointBuffer.virAddr = aligned_alloc(16, targetPointBuffer.s32PointsNum * sizeof(ALG_PCL_POINT_S));
    targetNormalBuffer.s32PointsNum = numPoints;
    targetNormalBuffer.virAddr = aligned_alloc(16, targetNormalBuffer.s32PointsNum * sizeof(ALG_PCL_NORMAL_S));
    targetPointNormalBuffer.s32PointsNum = numPoints;
    targetPointNormalBuffer.virAddr = aligned_alloc(16, targetPointNormalBuffer.s32PointsNum * sizeof(ALG_PCL_POINTNORMAL_S));

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-50.0f, 50.0f);
    for (int i = 0; i < numPoints; ++i) {
        float* p = (float*)srcPointBuffer.virAddr + i * 4;
        p[0] = dist(rng);
        p[1] = dist(rng);
        p[2] = dist(rng);
        p[3] = 1.0f;
    }

    Eigen::Matrix4f manualT = Eigen::Matrix4f::Identity();
    manualT <<  1, 0, 0, 0.8,
                0, 1, 0, 0.8,
                0, 0, 1, -0.9,
                0, 0, 0, 1;

    Eigen::Vector3f axis(1.0f, 2.0f, 2.0f);
    axis.normalize();
    float angle = 30.f / 180.f * M_PI;
    Eigen::Matrix3f rotation_3x3 = Eigen::AngleAxisf(angle, axis).toRotationMatrix();
    manualT.block<3, 3>(0, 0) = rotation_3x3;
    std::cout << "manualT:\n" << manualT << std::endl;

    Eigen::Matrix4f initGuess = Eigen::Matrix4f::Identity();
    initGuess << 1, 0, 0, 0.2,
                0, 1, 0, 0.2,
                0, 0, 1, -0.5,
                0, 0, 0, 1;
    angle *= 1.5f;
    rotation_3x3 = Eigen::AngleAxisf(angle, axis).toRotationMatrix();
    initGuess.block<3, 3>(0, 0) = rotation_3x3;
    std::cout << "initGuess:\n" << initGuess << std::endl << std::endl;

    /* estimate normals first */
    ALG_PCL_NORMAL_ESTIMATION_S stNormalEstimationParam;
    stNormalEstimationParam.eNeighborMode = PCL_NEIGHBOR_MODE_KNN;
    // stNormalEstimationParam.eNeighborMode = PCL_NEIGHBOR_MODE_RADIUS;
    stNormalEstimationParam.s32K = 10;
    stNormalEstimationParam.f32Radius = 40.f;
    stNormalEstimationParam.f32ViewPointX = 0;
    stNormalEstimationParam.f32ViewPointY = 0;
    stNormalEstimationParam.f32ViewPointZ = 0;
    ret = rkALG_pclNormalEstimation(&stEngine, &stNormalEstimationParam, &srcPointBuffer, &srcNormalBuffer);

    ret = rkALG_pclConcatenatePointAndNormal(&stEngine, &srcPointBuffer, &srcNormalBuffer, &srcPointNormalBuffer);

    /* generate target point cloud and normal */
    ret = rkALG_pclTransformPointCloud(&stEngine, &srcPointBuffer, &targetPointBuffer, manualT.data(), ALG_CPU_BACKEND);
    ret = rkALG_pclNormalEstimation(&stEngine, &stNormalEstimationParam, &targetPointBuffer, &targetNormalBuffer);
    ret = rkALG_pclConcatenatePointAndNormal(&stEngine, &targetPointBuffer, &targetNormalBuffer, &targetPointNormalBuffer);

    /* ICP With normals */
    ALG_PCL_ICP_PARAM_S ICPParam;
    memset(&ICPParam, 0, sizeof(ALG_PCL_ICP_PARAM_S));
    ICPParam.u32MaxIterations = 10;
    ICPParam.f32MaxCorrespondenceDistance = 100.f;
    ICPParam.f32TransformationEpsilon = 1e-6;
    ICPParam.f32EuclideanFitnessEpsilon = 1e-6;
    ICPParam.u8GetFitnessScore = 0;
    ICPParam.u8EnableInitGuess = 1;
    memcpy(ICPParam.pf32InitGuess, initGuess.data(), sizeof(float) * 16);

    ALG_PCL_POINTNORMALS_BUFFER_S dstPointNormalBuffer;
    dstPointNormalBuffer.s32PointsNum = numPoints;
    dstPointNormalBuffer.virAddr = aligned_alloc(16, dstPointNormalBuffer.s32PointsNum * sizeof(ALG_PCL_POINTNORMAL_S));

    ALG_PCL_REGISTRATION_RESULT_S ICPResult;
    memset(&ICPResult, 0, sizeof(ALG_PCL_REGISTRATION_RESULT_S));
    ICPResult.pDstPointNormalBuffer = &dstPointNormalBuffer;

    { // GPU ICP with normals
        ALG_PCL_BACKEND_E eBackEnd = ALG_GPU_BACKEND;
        ret = rkALG_pclIterativeClosestPointWithNormals(&stEngine, &ICPParam, &srcPointNormalBuffer, &targetPointNormalBuffer, &ICPResult, eBackEnd);
        std::cout << "icp.u8IsConverged: " << +ICPResult.u8IsConverged << std::endl;
        std::cout << "icp.finalIter: " << ICPResult.u32FinalIterations << std::endl;
        Eigen::Map<Eigen::Matrix4f> transform(ICPResult.f32Transformation);
        std::cout << "icp.transform:" << std::endl << transform << std::endl;
        if (ICPParam.u8GetFitnessScore) {
            std::cout << "icp.fitnessScore: " << ICPResult.f32FitnessScore << std::endl;
        }
    }

    { // CPU ICP with normals
        ALG_PCL_BACKEND_E eBackEnd = ALG_CPU_BACKEND;
        ret = rkALG_pclIterativeClosestPointWithNormals(&stEngine, &ICPParam, &srcPointNormalBuffer, &targetPointNormalBuffer, &ICPResult, eBackEnd);
        std::cout << "icp.u8IsConverged: " << +ICPResult.u8IsConverged << std::endl;
        std::cout << "icp.finalIter: " << ICPResult.u32FinalIterations << std::endl;
        Eigen::Map<Eigen::Matrix4f> transform(ICPResult.f32Transformation);
        std::cout << "icp.transform:" << std::endl << transform << std::endl;
        if (ICPParam.u8GetFitnessScore) {
            std::cout << "icp.fitnessScore: " << ICPResult.f32FitnessScore << std::endl;
        }
    }

    if (0) { /* print output */
        ALG_PCL_POINTNORMAL_S* pDstPointNormal = (ALG_PCL_POINTNORMAL_S*)dstPointNormalBuffer.virAddr;
        for (int i = 0; i < dstPointNormalBuffer.s32PointsNum; ++i) {
            printf("%d:(%f, %f, %f) (%f, %f, %f - %f)\n", i,
                pDstPointNormal[i].x, pDstPointNormal[i].y, pDstPointNormal[i].z,
                pDstPointNormal[i].normal_x, pDstPointNormal[i].normal_y, pDstPointNormal[i].normal_z, pDstPointNormal[i].curvature);
        }
    }

    free(srcPointBuffer.virAddr);
    free(srcNormalBuffer.virAddr);
    free(srcPointNormalBuffer.virAddr);
    free(targetPointBuffer.virAddr);
    free(targetNormalBuffer.virAddr);
    free(targetPointNormalBuffer.virAddr);
    free(dstPointNormalBuffer.virAddr);
    rkALG_pclDestroy(&stEngine);
    return 0;
}

int ndt_test(int argc, char** argv) {
    int numPoints = 20000;
    if (argc >= 2) {
        numPoints = atoi(argv[1]);
    }
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
    ALG_PCL_POINTS_BUFFER_S srcPointBuffer, targetPointBuffer, dstPointBuffer;
    srcPointBuffer.s32PointsNum = numPoints;
    size_t pointsBufferSize = srcPointBuffer.s32PointsNum * sizeof(ALG_PCL_POINT_S);
    srcPointBuffer.virAddr = aligned_alloc(16, pointsBufferSize);
    targetPointBuffer.s32PointsNum = numPoints;
    targetPointBuffer.virAddr = aligned_alloc(16, pointsBufferSize);
    dstPointBuffer.s32PointsNum = numPoints;
    dstPointBuffer.virAddr = aligned_alloc(16, pointsBufferSize);

    size_t bufferSize = 0;
    readBinaryFile2(DATA_DIR"room_scan_src.bin", srcPointBuffer.virAddr, bufferSize);
    srcPointBuffer.s32PointsNum = bufferSize / sizeof(ALG_PCL_POINT_S);

    memcpy(targetPointBuffer.virAddr, srcPointBuffer.virAddr, bufferSize);
    targetPointBuffer.s32PointsNum = srcPointBuffer.s32PointsNum;

    Eigen::Matrix4f manualT = Eigen::Matrix4f::Identity();
    manualT <<  1, 0, 0, 0.8,
                0, 1, 0, 0.8,
                0, 0, 1, -0.9,
                0, 0, 0, 1;

    Eigen::Vector3f axis(1.0f, 0.0f, 0.0f);  // rotation axis: x
    float angle = M_PI / 8.0f;
    Eigen::Matrix3f rotation_3x3 = Eigen::AngleAxisf(angle, axis).toRotationMatrix();
    manualT.block<3, 3>(0, 0) = rotation_3x3;

    ret = rkALG_pclTransformPointCloud(&stEngine, &targetPointBuffer, &targetPointBuffer, manualT.data(), ALG_CPU_BACKEND);
    std::cout << "manualT:" << manualT << std::endl;

    Eigen::Matrix4f initGuess = Eigen::Matrix4f::Identity();
    initGuess << 1, 0, 0, 0.6,
                 0, 1, 0, 0.7,
                 0, 0, 1, -0.7,
                 0, 0, 0, 1;
    angle *= 1.2f;
    rotation_3x3 = Eigen::AngleAxisf(angle, axis).toRotationMatrix();
    initGuess.block<3, 3>(0, 0) = rotation_3x3;

    ALG_PCL_NDT_PARAM_S NDTParam;
    memset(&NDTParam, 0, sizeof(ALG_PCL_NDT_PARAM_S));
    NDTParam.f32OutlierRatio = 0.55;
    NDTParam.f32Resolution = 1.0;
    NDTParam.f32StepSize = 0.08;
    NDTParam.f32TransformationEpsilon = 0.00001;
    NDTParam.u32MaxIterations = 15;
    NDTParam.u8GetFitnessScore = 0;
    NDTParam.u8EnableInitGuess = 1;
    memcpy(NDTParam.pf32InitGuess, initGuess.data(), sizeof(float) * 16);

    ALG_PCL_REGISTRATION_RESULT_S NDTResult;
    memset(&NDTResult, 0, sizeof(ALG_PCL_REGISTRATION_RESULT_S));
    NDTResult.pDstPointBuffer = &dstPointBuffer;
    ALG_PCL_BACKEND_E eBackEnd = ALG_CPU_BACKEND;

    ret = rkALG_pclNormalDistributionsTransform(&stEngine, &NDTParam, &srcPointBuffer, &targetPointBuffer, &NDTResult, eBackEnd);

    std::cout << "ndt.u8IsConverged: " << +NDTResult.u8IsConverged << std::endl;
    std::cout << "ndt.finalIter: " << NDTResult.u32FinalIterations << std::endl;
    Eigen::Map<Eigen::Matrix4f> transform(NDTResult.f32Transformation);
    std::cout << "ndt.transform" << std::endl << transform << std::endl;
    Eigen::Quaternionf quat(transform.block<3, 3>(0, 0));

    /* Missing output stream operator for Eigen::Quaternionf,
     * while eigen3 version < 3.4.0.
     * std::cout << "ndt quat: " << std::endl << quat << std::endl;
     */
    std::cout << "ndt quat: " << std::endl;
    std::cout << "w: " << quat.w() << ", x: " << quat.x() << ", y: " << quat.y() << ", z: " << quat.z() << std::endl;
    if (NDTParam.u8GetFitnessScore) {
        std::cout << "ndt.fitnessScore: " << NDTResult.f32FitnessScore << std::endl;
    }

    free(srcPointBuffer.virAddr);
    free(targetPointBuffer.virAddr);
    free(dstPointBuffer.virAddr);
    rkALG_pclDestroy(&stEngine);

    return 0;
}

int transformation_estimation_2d_test(int argc, char** argv) {
    int numPoints = 5000;
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " <point number>" << std::endl;
        return -1;
    }
    numPoints = atoi(argv[1]);
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
    ALG_PCL_POINTS_BUFFER_S srcPointBuffer, targetPointBuffer;
    srcPointBuffer.s32PointsNum = numPoints;
    size_t pointsBufferSize = srcPointBuffer.s32PointsNum * sizeof(ALG_PCL_POINT_S);
    srcPointBuffer.virAddr = aligned_alloc(16, pointsBufferSize);
    targetPointBuffer.s32PointsNum = numPoints;
    targetPointBuffer.virAddr = aligned_alloc(16, pointsBufferSize);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    for (int i = 0; i < numPoints; ++i) {
        float* p = (float*)srcPointBuffer.virAddr + i * 4;
        p[0] = dist(rng);
        p[1] = dist(rng);
        p[2] = dist(rng);
        p[3] = 1.0f;
        float* q = (float*)targetPointBuffer.virAddr + i * 4;
        memcpy(q, p, sizeof(float) * 4);
    }

    Eigen::Matrix4f manualT = Eigen::Matrix4f::Identity();
    manualT <<  1, 0, 0, 0.8,
                0, 1, 0, -0.4,
                0, 0, 1, 0.2,
                0, 0, 0, 1;
    Eigen::Vector3f axis(-0.4f, 0.3f, 1.0f);
    axis.normalize();
    float angle = M_PI / 8.0f;
    Eigen::Matrix3f rotation_3x3 = Eigen::AngleAxisf(angle, axis).toRotationMatrix();
    manualT.block<3, 3>(0, 0) = rotation_3x3;
    ret = rkALG_pclTransformPointCloud(&stEngine, &targetPointBuffer, &targetPointBuffer, manualT.data(), ALG_CPU_BACKEND);
    std::cout << "manualT: \n" << manualT << std::endl;

    float transform[16];
    {
        ret = rkALG_pclTransformationEstimation2D(&stEngine, &srcPointBuffer, &targetPointBuffer, transform, ALG_GPU_BACKEND);
        Eigen::Map<Eigen::Matrix4f> transformEstimation(transform);
        std::cout << "transformEstimation2D GPU: \n" << transformEstimation << std::endl;
    }
    {
        ret = rkALG_pclTransformationEstimation2D(&stEngine, &srcPointBuffer, &targetPointBuffer, transform, ALG_CPU_BACKEND);
        Eigen::Map<Eigen::Matrix4f> transformEstimation(transform);
        std::cout << "transformEstimation2D CPU: \n" << transformEstimation << std::endl;
    }

    free(srcPointBuffer.virAddr);
    free(targetPointBuffer.virAddr);
    rkALG_pclDestroy(&stEngine);

    return 0;
}

int transformation_estimation_3point_test(int argc, char** argv) {
    int numPoints = 3;
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
    ALG_PCL_POINTS_BUFFER_S srcPointBuffer, targetPointBuffer;
    srcPointBuffer.s32PointsNum = numPoints;
    size_t pointsBufferSize = srcPointBuffer.s32PointsNum * sizeof(ALG_PCL_POINT_S);
    srcPointBuffer.virAddr = aligned_alloc(16, pointsBufferSize);
    targetPointBuffer.s32PointsNum = numPoints;
    targetPointBuffer.virAddr = aligned_alloc(16, pointsBufferSize);

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    for (int i = 0; i < numPoints; ++i) {
        float* p = (float*)srcPointBuffer.virAddr + i * 4;
        p[0] = dist(rng);
        p[1] = dist(rng);
        p[2] = dist(rng);
        p[3] = 1.0f;
        float* q = (float*)targetPointBuffer.virAddr + i * 4;
        memcpy(q, p, sizeof(float) * 4);
    }

    Eigen::Matrix4f manualT = Eigen::Matrix4f::Identity();
    manualT <<  1, 0, 0, 0.8,
                0, 1, 0, -0.4,
                0, 0, 1, 0.2,
                0, 0, 0, 1;
    Eigen::Vector3f axis(-0.4f, 0.3f, 1.0f);
    axis.normalize();
    float angle = M_PI / 8.0f;
    Eigen::Matrix3f rotation_3x3 = Eigen::AngleAxisf(angle, axis).toRotationMatrix();
    manualT.block<3, 3>(0, 0) = rotation_3x3;
    std::cout << "manualT: \n" << manualT << std::endl;

    float transform[16];
    ret = rkALG_pclTransformationEstimation3Point(&stEngine, &srcPointBuffer, &targetPointBuffer, transform);
    Eigen::Map<Eigen::Matrix4f> transformEstimation(transform);
    std::cout << "transformEstimation: \n" << transformEstimation << std::endl;

    free(srcPointBuffer.virAddr);
    free(targetPointBuffer.virAddr);
    rkALG_pclDestroy(&stEngine);

    return 0;
}

int main(int argc, char** argv) {
    // icp_test(argc, argv);
    icp_normals_test(argc, argv);
    // ndt_test(argc, argv);
    transformation_estimation_2d_test(argc, argv);
    transformation_estimation_3point_test(argc, argv);
    return 0;
}