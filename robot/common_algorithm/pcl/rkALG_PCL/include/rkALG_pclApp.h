#pragma once
#include "rkALG_pclCommon.h"
#ifdef __cplusplus
extern "C" {
#endif

#if defined _WIN32 || defined __CYGWIN__
#ifdef BUILDING_DLL
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__ ((dllexport))
#else
	// Note: actually gcc seems to also supports this syntax.
#define DLL_PUBLIC __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__ ((dllimport))
#else
	// Note: actually gcc seems to also supports this syntax.
#define DLL_PUBLIC
#endif
#define DLL_LOCAL
#endif
#else
#if __GNUC__ >= 4
#define DLL_PUBLIC __attribute__ ((visibility ("default")))
#define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define DLL_PUBLIC
#define DLL_LOCAL
#endif
#endif
	/**
	* @brief Retrieve PCL algorithm library version information
	* @param rkPclVersion Output buffer (recommended 128 bytes)
	* @return 0 success, non-zero error code
	*/
	DLL_PUBLIC int32_t rkALG_pclVersion(char rkPclVersion[128]);

	/**
	* @brief Initialize PCL processing engine with configuration
	* @param pstEngine Engine handle pointer (pre-allocated)
	* @param stInitParams Initialization params (max points/zero-copy flag)
	* @return 0 success, non-zero error code
	*/
	DLL_PUBLIC int32_t rkALG_pclInit(ALG_PCL_ENGINE* pstEngine, ALG_PCL_INIT_S* stInitParams);

	/**
	* @brief Convert depth map to point cloud coordinates
	* @param pSrcDepthImgBuffer Input depth image with camera matrix
	* @param pDstPointBuffer Output point cloud buffer (pre-allocated)
	* @return 0 success, non-zero error code
	*/
	DLL_PUBLIC int32_t rkALG_pclDepth2Points(ALG_PCL_ENGINE* pstEngine, ALG_DEPTH_IMG_S* pSrcDepthImgBuffer, ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer);

	/**
 	* @brief concatenate a point cloud and a normal cloud (for ICP With normal)
	* @param pstEngine Engine handle
	* @param pSrcPointBuffer Input point cloud
	* @param pSrcNormalBuffer Input normal cloud
	* @param pDstPointNormalBuffer Output point normal cloud
	*/
	DLL_PUBLIC int32_t rkALG_pclConcatenatePointAndNormal(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, const ALG_PCL_NORMALS_BUFFER_S* pSrcNormalBuffer, ALG_PCL_POINTNORMALS_BUFFER_S* pDstPointNormalBuffer);

	/**
	* @brief Apply axis-aligned filtering to point cloud
	* @param pPassThroughParams Filter params (axis ranges/mode)
	* @param pSrcPointBuffer Input point cloud
	* @param pDstPointBuffer Filtered output (pre-allocated)
	* @return 0 success, non-zero error code
	*/
	DLL_PUBLIC int32_t rkALG_pclPassThrough(ALG_PCL_ENGINE* pstEngine, ALG_PCL_PASS_THROUGH_S* pPassThroughParams,
		ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer);

	/**
	* @brief Remove NaN/Inf values from point cloud.
	* @param pSrcPointBuffer Input point cloud
	* @param pDstPointBuffer Filtered output (pre-allocated), can be nullptr if not needed
	* @param pIndices Output indices of valid points(pre-allocated needed).
	* @param pNumValidPoints Output number of valid points.
	* @param eBackEnd Backend type (CPU/GPU)
	* @return 0 success, non-zero error code
	* @note  This function can be called with (pSrcPointBuffer == pDstPointBuffer)
	*/
	DLL_PUBLIC int32_t rkALG_pclRemoveNaNFromPointCloud(ALG_PCL_ENGINE* pstEngine,
		const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer, int32_t* pIndices, int32_t* pNumValidPoints,
		ALG_PCL_BACKEND_E eBackEnd);

	/**
	* @brief Remove outliers via neighbor density check
	* @param pRadiusOutlierRemovalParams (search radius/min neighbors)
	* @param pSrcPointBuffer Input point cloud
	* @param pDstPointBuffer Filtered output
	* @return 0 success, non-zero error code
	*/
	DLL_PUBLIC int32_t rkALG_pclRadiusOutlierRemoval(ALG_PCL_ENGINE* pstEngine, ALG_PCL_RADIUS_OUTLIER_REMOVAL_S* pRadiusOutlierRemovalParams,
		ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer);

	/**
	* @brief Remove statistical outliers using distribution analysis
	* @param pStatisticalOutlierRemoval (mean neighbors/stddev threshold)
	* @param pSrcPointBuffer Input point cloud
	* @param pDstPointBuffer Filtered output
	* @return 0 success, non-zero error code
	*/
	DLL_PUBLIC int32_t rkALG_pclStatisticalOutlierRemoval(ALG_PCL_ENGINE* pstEngine, ALG_PCL_STATISTICAL_OUTLIER_REMOVAL_S* pStatisticalOutlierRemoval,
		ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer);

	/**
	* @brief Perform Euclidean cluster segmentation
	* @param pEuclideanClusterExtraction (cluster tolerance/size range)
	* @param pSrcPointBuffer Input point cloud
	* @param pClusterResult Cluster indices storage
	* @return 0 success, non-zero error code
	*/
	DLL_PUBLIC int32_t rkALG_pclEuclideanClusterExtraction(ALG_PCL_ENGINE* pstEngine, ALG_PCL_EUCLIDEAN_CLUSTER_S* pEuclideanClusterExtraction,
		ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_CLUSTER_RESULT_S* pClusterResult);

	/**
	 * @brief Perform uniform sampling on point cloud data using sphere radius criteria‌
	 * @param pUniformSampling Sampling parameters (contains search radius configuration)‌
	 * @param pSrcPointBuffer Input point cloud buffer (must be pre-allocated)
	 * @param pDstPointBuffer Output sampled point buffer (requires pre-allocated memory)
	 * @return 0 for success, non-zero error code for failure conditions‌
	 */
	DLL_PUBLIC int32_t rkALG_pclUniformSampling(ALG_PCL_ENGINE* pstEngine, ALG_PCL_UNIFORM_SAMPLING_S* pUniformSampling,
		ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer);

	/**
	* @brief Apply voxel grid downsampling
	* @param pVoxelGrid (leaf node dimensions)
	* @param pSrcPointBuffer Input point cloud
	* @param pDstPointBuffer Downsampled output
	* @return 0 success, non-zero error code
	*/
	DLL_PUBLIC int32_t rkALG_pclVoxelGrid(ALG_PCL_ENGINE* pstEngine, ALG_PCL_VOXEL_GRID_S* pVoxelGrid,
		ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer);

	/**
	 * @brief Apply point cloud transformation
	 * @param pSrcPointBuffer Input point cloud
	 * @param pDstPointBuffer Transformed output
	 * @param pTransform Pointer of a Eigen::Matrix4f containing the transformation matrix
	 * @param eBackEnd Backend type (CPU/GPU)
	 * @note  This function can be called with (pSrcPointBuffer == pDstPointBuffer)
	 * @return 0 success, non-zero error code
	 */
	DLL_PUBLIC int32_t rkALG_pclTransformPointCloud(ALG_PCL_ENGINE* pstEngine,
		ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer,
		const float* pTransform, ALG_PCL_BACKEND_E eBackEnd);

	/**
	 * @brief Copy point cloud data
	 * @param pSrcPointBuffer Input point cloud
	 * @param pDstPointBuffer Output point cloud
	 * @return 0 success, non-zero error code
	 */
	DLL_PUBLIC int32_t rkALG_pclCopyPointCloud(ALG_PCL_ENGINE* pstEngine,
		const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer,
		ALG_PCL_BACKEND_E eBackEnd);

	/**
	 * @brief Copy point cloud data with indices
	 * @param pSrcPointBuffer Input point cloud
	 * @param pDstPointBuffer Output point cloud
	 * @param pIndex the pointer to mapping (ordered): cloud_out[i] = cloud_in[pIndex[i]]
	 * @param s32NumValidPoint the number of valid points in pIndex
	 * @note Assumes unique indices.
	 * @return 0 success, non-zero error code
	 */
	DLL_PUBLIC int32_t rkALG_pclCopyPointCloudWithIndex(ALG_PCL_ENGINE* pstEngine,
		const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer,
		const int32_t* pIndex, int32_t s32NumValidPoint, ALG_PCL_BACKEND_E eBackEnd);

	/**
	 * @brief Search for k-nearest neighbors for the given query point.
	 * @param pSrcPointBuffer Input point cloud
	 * @param pQueryPoint a given valid (i.e., finite, Homogeneous coordinates(w=1.0)) query point
	 * @param K the number of neighbors to search for
	 * @param pKIndices the resultant indices of the neighboring points (must be pre-allocated to k a priori!)
	 * @param pKSquaredDistances the resultant squared distances of the neighboring points (must be pre-allocated to k a priori!)
	 * @note assumes valid (i.e., finite) data
	 * @return number of neighbors found
	 **/
	DLL_PUBLIC int32_t rkALG_pclKdTreeNearestKSearch(ALG_PCL_ENGINE* pstEngine,
		const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, const ALG_PCL_POINT_S* pQueryPoint, int32_t K,
		int32_t* pKIndices, float* pKSquaredDistances);

	/**
	 * @brief Search for all the nearest neighbors of the query point in a given radius.
	 * @param pSrcPointBuffer Input point cloud
	 * @param pQueryPoint a given valid (i.e., finite, Homogeneous coordinates(w=1.0)) query point
	 * @param radius the radius of the sphere within which to search for neighboring points
	 * @param pIndices the resultant indices of the neighboring points (must be pre-allocated)
	 * @param pSquaredDistances the resultant squared distances of the neighboring points (must be pre-allocated)
	 * @param max_nn if given, bounds the maximum returned neighbors to this value. If max_nn is set to 0 or to a number higher than the number of points in the input cloud, all neighbors in radius will be returned.
	 * @note assumes valid (i.e., finite) data
	 * @return number of neighbors found
	 */
	DLL_PUBLIC int32_t rkALG_pclKdTreeRadiusSearch(ALG_PCL_ENGINE* pstEngine,
		const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, const ALG_PCL_POINT_S* pQueryPoint, float radius,
		int32_t* pIndices, float* pSquaredDistances, uint32_t max_nn);

	/**
	 * @brief Compute the Least-Squares plane fit for a given set of points, using their indices, and return the estimated plane parameters together with the surface curvature.
	 * @param pSrcPointBuffer Input point cloud
	 * @param indices the point cloud indices that need to be used
	 * @param numPoints the number of points to be used
	 * @param pNormal the resultant normal and curvature of the plane
	 * @return 0 success, non-zero error code
	 */
	DLL_PUBLIC int32_t rkALG_pclComputePointNormal(ALG_PCL_ENGINE* pstEngine,
		const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, const int32_t* indices, int32_t numPoints,
		ALG_PCL_NORMAL_S* pNormal);

	/**
	 * @brief Estimates local surface properties (surface normals and curvatures)at each 3D point.
	 * @param pNormalEstimationParam Normal estimation parameters
	 * @param pSrcPointBuffer Input point cloud
	 * @param pDstNormalsBuffer Output normal and curvature buffer
	 * @note In situations where not enough neighbors are found, the normal and curvature values are set to NaN.
	 * @return 0 success, non-zero error code
	 */
	DLL_PUBLIC int32_t rkALG_pclNormalEstimation(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_NORMAL_ESTIMATION_S* pNormalEstimationParam,
		const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_NORMALS_BUFFER_S* pDstNormalsBuffer);

	/**
	 * @brief Estimate the boundary of the point cloud
	 * @param pBoundaryEstimationParam Boundary estimation parameters
	 * @param pSrcPointBuffer Input point cloud
	 * @param pDstIsBoundary Output boundary buffer (must be pre-allocated to the same size as the input point cloud!). 1: is boundary/ 0: is not boundary.
	 * @return 0 success, non-zero error code
	 */
	DLL_PUBLIC int32_t rkALG_pclBoundaryEstimation(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_BOUNDARY_ESTIMATION_S* pBoundaryEstimationParam,
		const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, uint8_t* pDstIsBoundary);

	/**
	 * @brief Implements the well known Region Growing algorithm used for segmentation.
	 * @param pSrcPointBuffer Input point cloud
	 * @param pRegionGrowingParams Region growing parameters
	 * @param pClusterResult Output cluster result
	 * @note pClusterResult->pstIndicesBuffer must be pre allocated to the same size as the input point cloud.
	 * @return 0 success, non-zero error code
	*/
	DLL_PUBLIC int32_t rkALG_pclRegionGrowing(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer,
		const ALG_PCL_REGION_GROWING_S* pRegionGrowingParams, ALG_PCL_CLUSTER_RESULT_S* pClusterResult);

	/**
	 * @brief Get the minimum and maximum values on each of the 3 (x-y-z) dimensions in a given pointcloud.
	 * @param pSrcPointBuffer Input point cloud
	 * @param pMinPoint the resultant minimum bounds
	 * @param pMaxPoint the resultant maximum bounds
	 * @return 0 success, non-zero error code
	*/
	DLL_PUBLIC int32_t rkALG_pclGetMinMax3D(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer,
		ALG_PCL_POINT_S* pMinPoint, ALG_PCL_POINT_S* pMaxPoint);

	/**
	 * @brief Get the minimum and maximum values on each of the 3 (x-y-z) dimensions in a given pointcloud.
	 * @param pSrcPointBuffer Input point cloud
	 * @param pIndex the vector of point indices to use from cloud
	 * @param s32NumValidPoint the number of valid points in pIndex
	 * @param pMinPoint the resultant minimum bounds
	 * @param pMaxPoint the resultant maximum bounds
	 * @return 0 success, non-zero error code
	*/
	DLL_PUBLIC int32_t rkALG_pclGetMinMax3DWithIndex(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer,
		const int32_t* pIndex, int32_t s32NumValidPoint,
		ALG_PCL_POINT_S* pMinPoint, ALG_PCL_POINT_S* pMaxPoint);

	/**
	 * @brief extract features based on moment of inertia. AABB, OBB and eccentricity of the point cloud also calculated.
	 * @param pMomentOfInertiaParam Moment of inertia parameters
	 * @param pSrcPointBuffer Input point cloud
	 * @param pMomentOfInertiaResult result. moment of inertia, AABB, OBB, eccentricity of the point cloud are included.
	 * @note pMomentOfInertiaResult->pf32MomentOfInertia, pMomentOfInertiaResult->pf32Eccentricity must be pre-allocaed to (int)(360 / f32AngleStep + 1) * (int)(90 / f32AngleStep + 1)
	 * @return 0 success, non-zero error code
	*/
	DLL_PUBLIC int32_t rkALG_pclMomentOfInertia(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_MOMENT_OF_INERTIA_S* pMomentOfInertiaParam,
		const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_MOMENT_OF_INERTIA_RESULT_S* pMomentOfInertiaResult);

	/**
	 * @brief Obtain the maximum segment in a given point cloud, and return the minimum and maximum points.
	 * @param pSrcPointBuffer Input point cloud
	 * @param pMinPoint the coordinates of the "minimum" point in cloud (one end of the segment)
	 * @param pMaxPoint the coordinates of the "maximum" point in cloud (the other end of the segment)
	 * @param length the length of segment
	 * @return 0 success, non-zero error code
	*/
	DLL_PUBLIC int32_t rkALG_pclGetMaxSegment(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer,
		ALG_PCL_POINT_S* pMinPoint, ALG_PCL_POINT_S* pMaxPoint, double* length);

	/**
	 * @brief estimates the transformation(ICP) and returns the transformed point cloud as output.
	 * @param pICPParam ICP parameters
	 * @param pSrcPointBuffer Input point cloud
	 * @param pTargetPointBuffer Target point cloud
	 * @param pICPResult Output ICP result
	 * @param eBackEnd Backend type (CPU/GPU)
	 * @return 0 success, non-zero error code
	*/
	DLL_PUBLIC int32_t rkALG_pclIterativeClosestPoint(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_ICP_PARAM_S* pICPParam, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer,
		const ALG_PCL_POINTS_BUFFER_S* pTargetPointBuffer, ALG_PCL_REGISTRATION_RESULT_S* pICPResult, ALG_PCL_BACKEND_E eBackEnd);

	/**
	 * @brief estimates the transformation(ICPWithNormals) and returns the transformed point cloud as output.
	 * @param pICPParam ICP parameters
	 * @param pSrcPointNormalBuffer Input point cloud with normals. Generate via rkALG_pclConcatenatePointAndNormal.
	 * @param pTargetPointNormalBuffer Target point cloud with normals. Generate via rkALG_pclConcatenatePointAndNormal.
	 * @param pICPResult Output ICP result
	 * @param eBackEnd Backend type (CPU/GPU)
	 * @return 0 success, non-zero error code
	*/
	DLL_PUBLIC int32_t rkALG_pclIterativeClosestPointWithNormals(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_ICP_PARAM_S* pICPParam, const ALG_PCL_POINTNORMALS_BUFFER_S* pSrcPointNormalBuffer,
		const ALG_PCL_POINTNORMALS_BUFFER_S* pTargetPointNormalBuffer, ALG_PCL_REGISTRATION_RESULT_S* pICPResult, ALG_PCL_BACKEND_E eBackEnd);

	/**
	 * @brief Estimate the transformation (NDT-based) and returns the transformed point cloud as output.
	 * @param pNDTParam NDT parameters
	 * @param pSrcPointBuffer Input point cloud
	 * @param pTargetPointBuffer Target point cloud
	 * @param pNDTResult Output NDT result
	 * @param eBackEnd Backend type (only CPU backend supported now)
	 * @return 0 success, non-zero error code
	*/
	DLL_PUBLIC int32_t rkALG_pclNormalDistributionsTransform(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_NDT_PARAM_S* pNDTParam, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer,
		const ALG_PCL_POINTS_BUFFER_S* pTargetPointBuffer, ALG_PCL_REGISTRATION_RESULT_S* pNDTResult, ALG_PCL_BACKEND_E eBackEnd);

	/**
	 * @brief implements a simple 2D rigid transformation estimation (x, y, theta) for a given pair of datasets.
	 * @param pSrcPointBuffer Input point cloud
	 * @param pTargetPointBuffer Target point cloud
	 * @param pTransform pointer to resultant transformation matrix (4x4)
	 * @param eBackEnd Backend type (CPU/GPU)
	 * @return 0 success, non-zero error code
	 * @note pSrcPointBuffer and pTargetPointBuffer must have the same number of points
	*/
	DLL_PUBLIC int32_t rkALG_pclTransformationEstimation2D(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer,
		const ALG_PCL_POINTS_BUFFER_S* pTargetPointBuffer, float* pTransform, ALG_PCL_BACKEND_E eBackEnd);

	/**
	 * @brief implements a simple 3D rigid transformation estimation (x, y, z, roll, pitch, yaw) for a given pair of datasets.
	 * @param pSrcPointBuffer Input point cloud
	 * @param pTargetPointBuffer Target point cloud
	 * @param pTransform pointer to resultant transformation matrix (4x4)
	 * @return 0 success, non-zero error code
	 * @note pSrcPointBuffer and pTargetPointBuffer must have 3 points
	*/
	DLL_PUBLIC int32_t rkALG_pclTransformationEstimation3Point(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer,
		const ALG_PCL_POINTS_BUFFER_S* pTargetPointBuffer, float* pTransform);

	/**
	* @brief Release PCL engine resources
	* @param pstEngine Engine handle to destroy
	* @return 0 success, non-zero error code
	*/
	DLL_PUBLIC int32_t rkALG_pclDestroy(ALG_PCL_ENGINE* pstEngine);
#ifdef __cplusplus
} /* extern "C" { */
#endif