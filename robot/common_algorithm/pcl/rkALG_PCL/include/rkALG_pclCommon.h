#pragma once
#include <stdint.h>
#define MAX_CLUSTERS_NUM 100

#define RKPCL_ALIGN16 __attribute__((aligned(16))) /* used to satisfy SIMD requirements */

/**
 * @brief PCL algorithm backend
*/
typedef enum rkPCL_BACKEND_E {
    ALG_CPU_BACKEND = 0,  /* CPU backend */
    ALG_GPU_BACKEND = 1   /* GPU backend */
} ALG_PCL_BACKEND_E;

/**
 * @brief Axis selection modes for point cloud passthrough filtering‌
 */
typedef enum rkPCL_PCL_PASS_THROUGH_MODE_E
{
    PCL_PASS_THROUGH_MODE_X = 0,                        // Filter along X-axis
    PCL_PASS_THROUGH_MODE_Y = 1,                        // Filter along Y-axis
    PCL_PASS_THROUGH_MODE_Z = 2,                        // Filter along Z-axis
    PCL_PASS_THROUGH_MODE_XY = 3,                       // Filter in XY-plane
    PCL_PASS_THROUGH_MODE_XZ = 4,                       // Filter in XZ-plane
    PCL_PASS_THROUGH_MODE_YZ = 5,                       // Filter in YZ-plane
    PCL_PASS_THROUGH_MODE_XYZ = 6                       // Filter in 3D space
} ALG_PCL_PASS_THROUGH_MODE_E;

typedef enum rkPCL_PCL_NEIGHBOR_MODE_E
{
    PCL_NEIGHBOR_MODE_KNN = 0,                        // K-nearest neighbors
    PCL_NEIGHBOR_MODE_RADIUS = 1                      // Radius search
} ALG_PCL_NEIGHBOR_MODE_E;

// Camera intrinsic parameters
typedef struct rkALG_CAMERA_MATRIX_S
{
    float f32CenterX;                                   // Optical center X coordinate (pixels)
    float f32CenterY;                                   // Optical center Y coordinate (pixels)
    float f32FocalX;                                    // Focal length in X direction
    float f32FocalY;                                    // Focal length in Y direction
} ALG_CAMERA_MATRIX_S;

// Depth image metadata container
typedef struct rkALG_DEPTH_INF_S
{
    int32_t s32Flag;                                    // Configuration flags bitmask
    int32_t s32StepW;                                   // Width stride
    int32_t s32StepH;                                   // Height stride
    int32_t s32Width;                                   // Image width in pixels
    int32_t s32Height;                                  // Image height in pixels
    float f32DepthRatio;                                // Depth value scaling factor
    ALG_CAMERA_MATRIX_S stCameraMatrix;                 // Camera intrinsic parameters
} ALG_DEPTH_INF_S;

// Depth image buffer descriptor
typedef struct rkALG_DEPTH_IMG_S
{
    int32_t fd;                                         // File descriptor for DMA buffer
    int32_t width;                                      // Image width in pixels
    int32_t height;                                     // Image height in pixels
    void* virAddr;                                      // Virtual address pointer
    void* phyAddr;                                      // Physical address pointer
} ALG_DEPTH_IMG_S;

// Point cloud data buffer descriptor
typedef struct rkALG_PCL_POINTS_BUFFER_S {
    int32_t fd;                                         // DMA buffer file descriptor
    int32_t s32PointsNum;                               // Number of points in buffer
    void* virAddr;                                      // Virtual address of point data
    void* phyAddr;                                      // Physical address of point data
} ALG_PCL_POINTS_BUFFER_S;

// normals data buffer descriptor
typedef ALG_PCL_POINTS_BUFFER_S ALG_PCL_NORMALS_BUFFER_S;

// PointNormals data buffer descriptor
typedef ALG_PCL_POINTS_BUFFER_S ALG_PCL_POINTNORMALS_BUFFER_S;

// indices data buffer descriptor
typedef ALG_PCL_POINTS_BUFFER_S ALG_PCL_INDICES_BUFFER_S;

// 3D point with homogeneous coordinate‌
typedef RKPCL_ALIGN16 struct rkALG_PCL_POINT_S {
    union {
        float data[4];
        struct {
            float x;  // X coordinate
            float y;  // Y coordinate
            float z;  // Z coordinate
        };
    };
} ALG_PCL_POINT_S;

/* A point structure representing normal coordinates and the surface curvature estimate */
typedef RKPCL_ALIGN16 struct rkALG_PCL_NORMAL_S {
    union {
        float data_n[4];
        struct {
            float normal_x;  // Normal X coordinate
            float normal_y;  // Normal Y coordinate
            float normal_z;  // Normal Z coordinate
        };
    };
    union {
        float data_c[4];
        struct {
            float curvature;                            // Surface curvature estimate
        };
    };
} ALG_PCL_NORMAL_S;

typedef RKPCL_ALIGN16 struct rkALG_PCL_POINTNORMALS_S {
    union {
        float data[4];
        struct {
            float x;  // X coordinate
            float y;  // Y coordinate
            float z;  // Z coordinate
        };
    };
    union {
        float data_n[4];
        struct {
            float normal_x;  // Normal X coordinate
            float normal_y;  // Normal Y coordinate
            float normal_z;  // Normal Z coordinate
        };
    };
    union {
        float data_c[4];
        struct {
            float curvature;                            // Surface curvature estimate
        };
    };
} ALG_PCL_POINTNORMAL_S;

// Passthrough filter parameters‌
typedef struct rkALG_PCL_PASS_THROUGH_S {
    float minX;                                         // Minimum X threshold
    float maxX;                                         // Maximum X threshold
    float minY;                                         // Minimum Y threshold
    float maxY;                                         // Maximum Y threshold
    float minZ;                                         // Minimum Z threshold
    float maxZ;                                         // Maximum Z threshold
    int32_t limitsNegative;                             // 0=keep inside range, 1=keep outside
    int32_t passThroughMode;                            // Filtering mode from ALG_PCL_PASS_THROUGH_MODE_E
} ALG_PCL_PASS_THROUGH_S;

// Radius-based outlier removal parameters‌
typedef struct rkALG_PCL_RADIUS_OUTLIER_REMOVAL_S {
    float f32RadiusSearch;                              // Search radius for neighbors
    int32_t s32NeighborsInRadius;                       // Minimum neighbors required
} ALG_PCL_RADIUS_OUTLIER_REMOVAL_S;

// Statistical outlier removal parameters‌
typedef struct rkALG_PCL_STATISTICAL_OUTLIER_REMOVAL_S {
    int32_t s32MeanK;                                   // Number of neighbors to analyze
    float f32StddevMulThresh;                           // Standard deviation multiplier
} ALG_PCL_STATISTICAL_OUTLIER_REMOVAL_S;

// Euclidean clustering parameters‌
typedef struct rkALG_PCL_EUCLIDEAN_CLUSTER_S
{
    float f32ClusterTolerance;                          // Maximum point-to-point distance in cluster
    int32_t s32MinClusterSize;                          // Minimum points per cluster
    int32_t s32MaxClusterSize;                          // Maximum points per cluster
} ALG_PCL_EUCLIDEAN_CLUSTER_S;

// Cluster extraction result container
typedef struct rkALG_PCL_CLUSTER_RESULT_S
{
    int32_t s32ClustersNum;                             // Detected clusters count
    int32_t s32ClustersOffset[MAX_CLUSTERS_NUM];        // Starting indices in buffer
    int32_t s32ClustersPointsNum[MAX_CLUSTERS_NUM];     // Points count per cluster
    ALG_PCL_INDICES_BUFFER_S* pstIndicesBuffer;         // Cluster indices buffer
} ALG_PCL_CLUSTER_RESULT_S;

// Voxel grid downsampling parameters‌
typedef struct rkALG_PCL_VOXEL_GRID_S {
    float f32LeafSizeX;                                 // Voxel size in X dimension
    float f32LeafSizeY;                                 // Voxel size in Y dimension
    float f32LeafSizeZ;                                 // Voxel size in Z dimension
} ALG_PCL_VOXEL_GRID_S;

// Uniform sampling parameters‌
typedef struct rkALG_PCL_UNIFORM_SAMPLING_S {
    float f32RadiusSearch;                              // Sphere radius for sampling
} ALG_PCL_UNIFORM_SAMPLING_S;

/* normal estimation params */
typedef struct rkALG_PCL_NORMAL_ESTITMATION_S {
    int32_t s32K;                                       // Number of neighbors to search (only make sense when eNeighborMode=PCL_NEIGHBOR_MODE_KNN)
    float f32Radius;                                    // Search radius for neighbors (only make sense when eNeighborMode=PCL_NEIGHBOR_MODE_RADIUS)
    ALG_PCL_NEIGHBOR_MODE_E eNeighborMode;              // Neighbor search mode
    float f32ViewPointX;                                // Viewpoint X coordinate. Used to unify normal orientation. A typical viewpoint value is (0, 0, 0)
    float f32ViewPointY;                                // Viewpoint Y coordinate
    float f32ViewPointZ;                                // Viewpoint Z coordinate
} ALG_PCL_NORMAL_ESTIMATION_S;

/* boundary estimation params */
typedef struct rkALG_PCL_BOUNDARY_ESTIMATION_S {
    int32_t s32K;                                       // Number of neighbors to search (only make sense when eNeighborMode=PCL_NEIGHBOR_MODE_KNN)
    float f32Radius;                                    // Search radius for neighbors (only make sense when eNeighborMode=PCL_NEIGHBOR_MODE_RADIUS)
    ALG_PCL_NEIGHBOR_MODE_E eNeighborMode;              // Neighbor search mode
    float f32RadianAngleThreshold;                      // Set the decision boundary (angle threshold) that marks points as boundary or regular.
    const ALG_PCL_NORMALS_BUFFER_S* pNormalsBuffer;
} ALG_PCL_BOUNDARY_ESTIMATION_S;

typedef struct rkALG_PCL_REGION_GROWING_S {
    uint32_t u32MinPointsPerCluster; // the minimum number of points that a cluster needs to contain in order to be considered valid.
    uint32_t u32MaxPointsPerCluster; // the maximum number of points that a cluster needs to contain in order to be considered valid.
    uint8_t u8SmoothModeFlag;        // Flag that signalizes if the smoothness constraint will be used.
    uint8_t u8CurvatureFlag;         // If set to true then curvature test will be done during segmentation.
    uint8_t u8ResidualFlag;          // If set to true then residual test will be done during segmentation.
    float f32RadianThetaThreshold;   // Threshold used for testing the smoothness between points.
    float f32ResidualThreshold;      // Threshold used in residual test.
    float f32CurvatureThrehold;      // Threshold used in curvature test.
    uint32_t u32NeighborNumber;      // Number of neighbours to find.
    const ALG_PCL_NORMALS_BUFFER_S* pNormalsBuffer; // Contains normals of the points that will be segmented.
} ALG_PCL_REGION_GROWING_S;

typedef struct rkALG_PCL_MOMENT_OF_INERTIA_S {
    float f32AngleStep;
    uint8_t u8NormalFlag;
    float f32PointMass;   /* only make sense when u8NormalFlag is false(0) */
} ALG_PCL_MOMENT_OF_INERTIA_S;

typedef struct rkALG_PCL_MOMENT_OF_INERTIA_RESULT_S {
    ALG_PCL_POINT_S massCenter;
    ALG_PCL_POINT_S AABBMinPoint;   /* min point of the axis aligned bounding box */
    ALG_PCL_POINT_S AABBMaxPoint;   /* max point of the axis aligned bounding box */
    ALG_PCL_POINT_S OBBMinPoint;    /* min point of the oriented bounding box */
    ALG_PCL_POINT_S OBBMaxPoint;    /* max point of the oriented bounding box */
    ALG_PCL_POINT_S OBBPosition;    /* position of the oriented bounding box */
    float pf32OBBRotationMatrix[9];
    float major_eigen_value, middle_eigen_value, minor_eigen_value;
    float major_eigen_vector[3], middle_eigen_vector[3], minor_eigen_vector[3];
    float* pf32MomentOfInertia;  /* need to be pre-alloced */
    float* pf32Eccentricity;     /* need to be pre-alloced */
} ALG_PCL_MOMENT_OF_INERTIA_RESULT_S;

typedef struct rkALG_PCL_ICP_PARAM_S {
    float f32MaxCorrespondenceDistance; /* the maximum distance threshold between a point and its nearest neighbor correspondent in order to be considered in the alignment process */
    uint32_t u32MaxIterations; /* the maximum number of iterations the internal optimization should run for */
    float f32TransformationEpsilon; /* maximum allowable translation squared difference between two consecutive transformations */
    float f32EuclideanFitnessEpsilon; /* maximum allowable Euclidean error between two consecutive steps in the ICP loop */
    uint8_t u8GetFitnessScore; /* set it to 1 if fitness score is needed, otherwise the fitness will not be computed */
    uint8_t u8EnableInitGuess; /* initialize transformation according to pf32InitGuess (if u8EnableInitGuess is set) or identity matrix(if u8EnableInitGuess is 0) */
    float pf32InitGuess[16]; /* pointer of init guess matrix(a Eigen::Matrix4f), only make sense when u8EnableInitGuess is set */
} ALG_PCL_ICP_PARAM_S;

typedef struct rkALG_NDT_PARAM_S {
    float f32OutlierRatio; /* the ratio of outliers of points w.r.t. a normal distribution */
    uint32_t u32MaxIterations; /* the maximum number of iterations the internal optimization should run for */
    float f32TransformationEpsilon; /* maximum allowable translation squared difference between two consecutive transformations */
    float f32StepSize;   /* the newton line search maximum step length */
    float f32Resolution; /* voxel grid resolution. */
    uint8_t u8GetFitnessScore; /* set it to 1 if fitness score is needed, otherwise the fitness will not be computed */
    uint8_t u8EnableInitGuess; /* initialize transformation according to pf32InitGuess (if u8EnableInitGuess is set) or identity matrix(if u8EnableInitGuess is 0) */
    float pf32InitGuess[16]; /* pointer of init guess matrix(a Eigen::Matrix4f), only make sense when u8EnableInitGuess is set */
} ALG_PCL_NDT_PARAM_S;

typedef struct rkALG_PCL_REGISTRATION_RESULT_S {
    uint8_t u8IsConverged; /* 1=converged, 0=not converged */
    uint32_t u32FinalIterations; /* final number of iterations */
    float f32Transformation[16]; /* pointer of the transformation matrix(a Eigen::Matrix4f) */
    float f32FitnessScore; /* only make sense if u8GetFitnessScore of param is set */
    ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer; /* Pointer of the output point cloud, need to be pre-allocated for ICP/NDT */
    ALG_PCL_POINTNORMALS_BUFFER_S* pDstPointNormalBuffer; /* Pointer of the output point cloud, need to be pre-allocated for ICPWithNormals */
} ALG_PCL_REGISTRATION_RESULT_S;

// PCL processing engine initialization parameters
typedef struct rkALG_PCL_INIT_S {
    int32_t s32MaxPointNum;                             // Maximum allocatable points
    int32_t s32IsZeroCopy;                              // 1=use zero-copy buffers, 0=normal mode
    ALG_DEPTH_INF_S stDepthInfo;                        // Depth sensor configuration
} ALG_PCL_INIT_S;

// Opaque handle for PCL processing engine‌
typedef struct rkALG_PCL_ENGINE {
    void* p;                                            // Internal implementation pointer
} ALG_PCL_ENGINE;