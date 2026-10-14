# Rockchip 点云处理算法调试指南

文件标识：RK-KF-SF-048
发布版本：V1.1.14

日期：2025-09-29

文件密级：□绝密 □秘密 □内部资料 ■公开

**免责声明**

本文档按“现状”提供，瑞芯微电子股份有限公司（“本公司”，下同）不对本文档的任何陈述、信息和内容的准确性、可靠性、完整性、适销性、特定目的性和非侵权性提供任何明示或暗示的声明或保证。本文档仅作为使用指导的参考。

由于产品版本升级或其他原因，本文档将可能在未经任何通知的情况下，不定期进行更新或修改。

**商标声明**

“Rockchip”、“瑞芯微”、“瑞芯”均为本公司的注册商标，归本公司所有。

本文档可能提及的其他所有注册商标或商标，由其各自拥有者所有。

**版权所有 © 2025 瑞芯微电子股份有限公司**

超越合理使用范畴，非经本公司书面许可，任何单位和个人不得擅自摘抄、复制本文档内容的部分或全部，并不得以任何形式传播。

瑞芯微电子股份有限公司

Rockchip Electronics Co., Ltd.

地址： 福建省福州市铜盘路软件园A区18号

网址： [www.rock-chips.com](http://www.rock-chips.com)

客户服务电话： +86-4007-700-590

客户服务传真： +86-591-83951833

客户服务邮箱： [fae@rock-chips.com](mailto:fae@rock-chips.com)

---

**产品版本**

与本文档相对应的产品版本如下：

| **产品名称** | **产品版本** |
| -------- | -------- |
| RK3576   |          |
| RK3588   |          |

**修订记录**

| **版本**   | **作者**        |**修改日期**       | **修改说明** |
| ---------- | ------ | ----------- | -------- |
| V1.1.0 | Mingpei Liu | 2025.03.10 | 初始版本 |
| V1.1.12 | Jieke Chen | 2025.05.29 | 新增算子 |
| V1.1.13 | Jieke Chen | 2025.09.17 | 新增算子 |
| V1.1.14 | Jieke Chen | 2025.09.29 | 新增配准算子 |

---

**目录**

[TOC]

---

## 算法概述

瑞芯微点云处理算法通过对点云数据进行预处理实现滤波、聚类、配准等功能。该算法充分利用芯片平台上的GPU模块进行加速，降低传统点云处理算法运行在CPU上的资源占用和耗时。文档中提到的瑞芯微点云处理算法统一用rkPCL代替用于与点云处理算法库PCL进行区分，如下表所示，为当前rkPCL已实现的点云处理算子及其对应的PCL算子情况。

| rkPCL                                                        | PCL                                               | 功能              |
| ------------------------------------------------------------ | ------------------------------------------------- | ----------------- |
| rkALG_pclDepth2Points                                        | pcl::depthToPointCloud                            | 深度数据转点云    |
| rkALG_pclConcatenatePointAndNormal                           | pcl::concatenateFields                            | 数据域连接        |
| rkALG_pclTransformPointCloud                                 | pcl::transformPointCloud                          | 点云刚体变换      |
| rkALG_pclCopyPointCloud<br />rkALG_pclCopyPointCloudWithIndex | pcl::copyPointCloud                               | 点云拷贝          |
| rkALG_pclGetMinMax3D<br />rkALG_pclGetMinMax3DWithIndex      | pcl::GetMinMax3D                                  | 点云外包围盒计算  |
| rkALG_pclGetMaxSegment                                       | pcl::getMaxSegment                                | 最大点对距离计算  |
| rkALG_pclKdTreeNearestKSearch                                | pcl::KdTreeFLANN::nearestKSearch                  | K近邻搜索         |
| rkALG_pclKdTreeRadiusSearch                                  | pcl::KdTreeFLANN::radiusSearch                    | 半径搜索          |
| rkALG_pclPassThrough                                         | pcl::PassThrough                                  | 直通滤波          |
| rkALG_pclRemoveNaNFromPointCloud                             | pcl::removeNaNFromPointCloud                      | 无效点滤除        |
| rkALG_pclRadiusOutlierRemoval                                | pcl::RadiusOutlierRemoval                         | 半径滤波          |
| rkALG_pclStatisticalOutlierRemoval                           | pcl::StatisticalOutlierRemoval                    | 统计滤波          |
| rkALG_pclUniformSampling                                     | pcl::UniformSampling                              | 点云均匀采样滤波  |
| rkALG_pclVoxelGrid                                           | pcl::VoxelGrid                                    | 体素滤波          |
| rkALG_pclComputePointNormal                                  | pcl::NormalEstimation::computePointNormal         | 估计局部点云法线  |
| rkALG_pclNormalEstimation                                    | pcl::NormalEstimation::compute                    | 估计全点云法线    |
| rkALG_pclBoundaryEstimation                                  | pcl::BoundaryEstimation                           | 估计点云边界      |
| rkALG_pclMomentOfInertia                                     | pcl::MomentOfInertia                              | 惯性矩/偏心率估计 |
| rkALG_pclEuclideanClusterExtraction                          | pcl::EuclideanClusterExtraction                   | 欧几里得聚类      |
| rkALG_pclRegionGrowing                                       | pcl::RegionGrowing                                | 点云区域生长分割  |
| rkALG_pclIterativeClosestPoint                               | pcl::IterativeClosestPoint                        | ICP点云配准       |
| rkALG_pclIterativeClosestPointWithNormals                    | pcl::IterativeClosestPointWithNormals             | ICP带法线点云配准 |
| rkALG_pclNormalDistributionsTransform                        | pcl::NormalDistributionsTransform                 | NDT点云配准       |
| rkALG_pclTransformationEstimation2D                          | pcl::registration::TransformationEstimation2D     | 点云2D配准        |
| rkALG_pclTransformationEstimation3Point                      | pcl::registration::TransformationEstimation3Point | 3点配准           |

## 数学原理

### rkALG_pclDepth2Points

该算子用于将深度数据$D(d)$转换为点云数据$P(x,y,z,w)$,转换公式如下。
$$
\begin{aligned}
& z=d*r \\
& x=(u-c_x)/f_x*z \\
& y=(v-c_y)/f_y*z \\
& w=0.0
\end{aligned}
$$
其中$(u,v)$为深度图D的二维坐标，$d$为深度值；$(c_x,c_y)$为相机光心坐标，单位为像素；$(f_x,f_y)$为相机焦距，单位为像素；$r$为深度数据的比例系数取数范围为[0,1]。

### rkALG_pclPassThrough

该算子对点云数据进行直通滤波，通过指定点云中某一坐标轴或者某几个坐标轴$(x,y,z,xy,xz,yz,xyz)$的取值范围，保留该维度上数值在设定区间内的点，其余点全部剔除‌，其数学公式如下。
$$
\ P_f = \begin{cases}
\ {p\in P_i | min \leq p_a \leq max} , \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \text{if limitsNegative = 0}\\
\ {p\in P_i | p_a < min \ \ || \ \ p_a > max} , \ \ \ \ \ \ \  \text{if limitsNegative = 1}\\
\end{cases}
$$
其中$P_f$为滤波后的点集，$P_i$为滤波前的点集。

### rkALG_pclRadiusOutlierRemoval

该算子对点云数据进行半径滤波，剔除半径内相邻点数目不足的点云。

通过对点云中每个点$p_i$，计算其在半径$r$范围内邻域点的数量$N(p_i,r)$,若 $N(p_i,r)\geq m$，则保留该点，否则视为离群点并剔除‌。

首先计算每个点附近邻域点的数量：
$$
N(p_i,r)=\{p_j\in P \ | \ d(p_i,p_j)\leq r \}
$$
其中$d(p_i,p_j)$表示两点间的欧式距离。

然后以邻域点数阈值进行筛选，保留符合要求的点云：
$$
p_i \in P_f \Leftarrow\Rightarrow N(p_i,r) \geq m
$$
其中$P_f$为滤波后的点云数据集，$m$为邻域点数阈值。

### rkALG_pclStatisticalOutlierRemoval

该算子对点云数据进行统计滤波，其基本原理是对点云中每个点$p_i$，计算其$k$最近邻的所有点到$p_i$的平均距离$d_i$，并通过统计所有点的平均距离的分布特性（均值$u$和方差$\delta$）确定离群点的阈值范围，然后通过阈值进行筛选，删除离群点。

首先找到每个点离其最近的$k$个点的集合$N_k(p_i)$，然后计算该集合内所有点距离该点距离的平均值：
$$
d_i=\frac{1}{k} \sum_{p_j \in N_k(p_i)}^{k}d(p_i,p_j)
$$
其中$d(p_i,p_j)$表示两点间的欧式距离，$N_k(p_i)$表示距离点$p_i$最近的$k$个点。

然后根据平均距离计算所有点分布的均值$u$和方差$\delta$：
$$
u=\frac{1}{m} \sum_{i=1}^{m} d_i \\
\sigma = \sqrt{\frac{1}{m} \sum_{i=1}^{m}(d_i-u)^2}
$$
根据统计得到的均值和方差计算筛选阈值$T$:
$$
T=u+ t_i *\sigma
$$
其中$t_i$为输入的标准差比例系数。

最后通过阈值$T$进行筛选，保留符合要求的点云：
$$
p_i \in P_f \Leftarrow\Rightarrow d_i \leq T
$$

### rkALG_pclEuclideanClusterExtraction

该算子对点云数据集进行欧几里得聚类，如果两点之间的欧式距离小于给定的阈值$d_t$，则认为这两点属于同一个集合，遍历所有点，并得到所有可能的集合，当集合内的点小于或者大于给定的阈值$n_{min}$和$n_{max}$时剔除该集合。

首先计算两点间的欧式距离并聚类：
$$
d(p_i,p_j)\leq \epsilon \\
d(p_i,p_j) = \sqrt{(x_i-x_j)^2 + (y_i-y_j)^2 + (z_i - z_j)^2} \\
C_k=\{p_a,p_b,...,p_m \}
$$
然后判断所有类的内点数是否满足要求并进行筛选，保留符合阈值要求的类：
$$
c_i \in C_f \Leftarrow\Rightarrow n_{min}\leq k \leq n_{max}
$$
其中$k$为类$c_i$的内点数量，$c_i$为最终保留类$C_f$的子集。

### rkALG_pclUniformSampling

该算子是一种基于体素化（Voxelization）的点云均匀采样方法，通过将点云空间划分为等尺寸的立方体网格（体素），并在每个非空体素中选择距离该体素质心最近的一个点进行保留，实现对原始点云的降采样。该方法通过控制体素尺寸平衡点云密度与计算效率，适用于大规模点云数据的高效精简‌。

首先对点云空间划分为边长为$\iota$的立方体网格，统计每个网格中的点集$v_i$：
$$
v_i=\{p_1,p_2,...,p_n \}
$$
然后计算每个体素网格的质心$c_i$:
$$
c_i=\frac{1}{n} \sum_{j=1} ^n p_j
$$
最后查询每个体素网格中距离质心最近的点并保留，其他点筛除：
$$
p_k=argmin(p_i-c_i), p_i\in v_i
$$

### rkALG_pclVoxelGrid

该算子原理与rkALG_pclUniformSampling类似，不同点在于，该算子保留每个体素网格内的质心作为每个体素网格的滤波点，经过该算子后的点云数据不再保留任何原始数据而是保留每个体素网格内均值滤波后的点。

| 算子                     | 采样策略                   | 输出位置           |
| ------------------------ | -------------------------- | ------------------ |
| rkALG_pclUniformSampling | 选择距离质心最近的点       | 保留原始点云中的点 |
| rkALG_pclVoxelGrid       | 计算体素内点的均值生成新点 | 生成新的虚拟点     |

### rkALG_pclTransformPointCloud

对点云进行刚体变换，包括旋转和平移两种几何变换，将一个 4X4 变换矩阵 $M$ 作用于点云内所有点。
$$
M_{4 \times 4} =
\begin{bmatrix}
R_{3 \times 3} & t_{3 \times 1} \\
\mathbf{0} & 1
\end{bmatrix} \\
p_{dst} = Mp_{src}
$$

### rkALG_pclComputePointNormal

通过指定索引，估计选定点（一般为已知的邻域点）所拟合出的平面法线与曲率。

① 计算邻域点的质心
给定一个点的邻域 $\mathcal{N} = \{ \mathbf{p}_1, \mathbf{p}_2, \dots, \mathbf{p}_n \}$，其中每个点 $\mathbf{p}_i \in \mathbb{R}^3$，计算其质心：
$$
\mathbf{\bar{p}} = \frac{1}{n} \sum_{i=1}^{n} \mathbf{p}_i
$$
② 构建协方差矩阵
将每个邻域点相对于质心进行偏移：
$$
\mathbf{q}_i = \mathbf{p}_i - \mathbf{\bar{p}}
$$

然后计算 3x3 的协方差矩阵：

$$
\mathbf{Cov} = \frac{1}{n} \sum_{i=1}^{n} \mathbf{q}_i \mathbf{q}_i^T
$$
③ 对协方差矩阵进行特征值分解
$$
\mathbf{Cov} \cdot \mathbf{v}_i = \lambda_i \cdot \mathbf{v}_i
$$

得到 3 个特征值 $\lambda_0 \leq \lambda_1 \leq \lambda_2$ 及对应的特征向量 $\mathbf{v}_0, \mathbf{v}_1, \mathbf{v}_2$，其中最小特征值 $\lambda_0$ 对应的特征向量 $\mathbf{v}_0$ 意味点云在该方向上变化最小，即法线方向$\mathbf n$。

曲率由 $curvature = \frac{\lambda_0}{\lambda_0+\lambda_1+\lambda_2}$ 计算得到。

### rkALG_pclNormalEstimation

为所有点根据邻域信息估计法线，对于点云 $C$ 中每个点 $p_i$，通过 K 近邻搜索或半径搜索取得其邻域 $\mathcal{N}$，然后进行单平面的法线计算，步骤参考 rkALG_pclComputePointNormal 算子。

计算完成后，如果指定了视点位置 $\mathbf{v}$，可以将法线朝向该视点修正：
$$
\text{if } (\mathbf{n} \cdot (\mathbf{v} - \mathbf{\bar{p}})) < 0 \Rightarrow \mathbf{n} := -\mathbf{n}
$$
确保所有法线指向同一侧，避免可视化时出现朝向不一的情况。

### rkALG_pclBoundaryEstimation

基于点云的法向量和邻域分布，估计每个点是否为边界点。

① 构建局部正交坐标系

对于点云 $C$ 中的每个点 $\mathbf{p}_i \in \mathbb{R}^3$ 及其法向量 $\mathbf{n}_i \in \mathbb{R}^3$ ，构建局部正交坐标系：$\{\mathbf{u}, \mathbf{v}, \mathbf{n}_i\}$, 其中 $\mathbf{u}, \mathbf{v}$ 两个单位向量定义了一个切平面，用于后续邻域点投影。

② 计算邻域点投影角

对于 $\mathbf{p}_i$ 的每个邻域点 $\mathbf{p}_j$，计算向量 $\mathbf{d}_j = p_j - p_i$ 相对于切平面的投影角 $\theta_j \in [-\pi, \pi]$。
$$
\theta_j = \mathrm{arctan}(\frac {\mathbf{d}_j \cdot \mathbf{v}}{ \mathbf{d}_j \cdot \mathbf{u} })
$$
③ 分析角度间隙

将所有 $\theta_j$ 升序排列，定义相邻角度差 $\Delta \theta_j = \theta_{j+1} - \theta_j$ 和头尾闭合角度差 $\Delta_{warp} = \theta_0 + 2\pi-\theta_{N-1}$. 选取最大角度间隙来判定边界点。
$$
\Delta\theta_{\max} = \max(\{\Delta\theta_k\}_{k=1}^{k=N-1}, \Delta\theta_{\mathrm{wrap}}) \\
\mathrm{isBoundary} =    \begin{cases}    1 & \text{if } \Delta\theta_{\max} > \theta_{\mathrm{thresh}} \\   0 & \text{otherwise}   \end{cases}
$$

### rkALG_pclRegionGrowing

该算子基于法向量平滑性，通过区域生长的方式来进行点云分割，参考论文*"Segmentation of point clouds using smoothness constraint"* —— T. Rabbani, F.A. van den Heuvel, G. Vosselman.

将全点云中具有最低曲率的点作为生长种子，判断其邻域点是否满足法线平滑性约束，辅以曲率测试/残差测试来判定邻域点是否能作为新种子。

① 法线平滑性约束

由种子点法向量 $\vec{ n_p}$ 和邻域点法向量 $\vec {n_q}$ 的夹角 $\theta$ 决定，接受条件：
$$
|\vec{n_p}·\vec{n_q}| > \cos(\theta_{thresh})
$$
其中 $\theta_{thresh}$ 为预设法线夹角的弧度阈值，接受的邻域点将被纳入同个分割区域。

② 曲率测试

由邻域点曲率 $curvature$ 决定，用于确保平滑表面，接收条件：
$$
curvature < c_{thresh}
$$
其中 $c_{thresh}$ 为预设曲率阈值，拒绝的邻域点不能成为新种子。

③ 残差测试

由邻域点残差决定，用于过滤掉不贴合局部平面的跳变点或孤立点。

当前点 $p$ 的所有邻域点可以拟合出一个平面 $\pi$，对于领域点 $q$，其残差 $r(q) = |\vec {n_p} · (p - q))|$，接受条件：
$$
r(q) < r_{thresh}
$$
其中 $r_{thresh}$ 为预设残差阈值，拒绝的邻域点不能成为新种子。

### rkALG_pclMomentOfInertia

该算子主要求解惯性矩和偏心率信息。

① 首先统计点云的质心$\bar {\mathbf {p}}$和协方差矩阵$\mathbf{Cov}$，分解 $\mathbf {Cov}$ 得到 3 个正交单位向量，即局部坐标系。

② 构造旋转方向 $(\theta, \phi)$，其中$\theta \in [0, 90]$表示纬度，$\phi \in [0, 360]$ 表示经度，分别按照角度步长递增。使用该旋转角度对角度对单位轴进行旋转，等价于在单位球面上采样各方向。

对于任意旋转轴 $\vec a$，计算当前方向的惯性矩
$$
I_{\vec a}=m \sum_{i=1}^N||(\mathbf p_i - \mathbf {\bar{p}})\times\vec a||^2
$$
将点云投影到垂直当前方向的平面上
$$
\mathbf {p}^{proj}_i = \mathbf {p}_i-(\vec a ·(\mathbf p_i - \bar {\mathbf {p}}) )\vec a
$$
，对投影点计算协方差矩阵，分解得到两个特征值，计算偏心率
$$
e = \sqrt{1-(\frac{\lambda_2}{\lambda_1})^2}
$$

## API接口及定义

### 结构体定义

```C++
/**
 * @brief PCL algorithm backend
*/
typedef enum rkPCL_BACKEND_E {
    ALG_CPU_BACKEND = 0,  /* CPU backend */
    ALG_GPU_BACKEND = 1   /* GPU backend */
} ALG_PCL_BACKEND_E;
```

定义 rkPCL 算子后端，部分算子支持选择后端实现，目前开放 CPU 和 GPU 供选择。

```C++
typedef enum rkPCL_PCL_NEIGHBOR_MODE_E
{
    PCL_NEIGHBOR_MODE_KNN = 0,                        // K-nearest neighbors
    PCL_NEIGHBOR_MODE_RADIUS = 1                      // Radius search
} ALG_PCL_NEIGHBOR_MODE_E;
```

定义两种近邻搜索的策略，包括 K 最近邻和半径搜索。

```c++
// Camera intrinsic parameters
typedef struct rkALG_CAMERA_MATRIX_S
{
    float f32CenterX;               // Optical center X coordinate (pixels)
    float f32CenterY;               // Optical center Y coordinate (pixels)
    float f32FocalX;                // Focal length in X direction
    float f32FocalY;                // Focal length in Y direction
}ALG_CAMERA_MATRIX_S;
```

该结构体定义了相机的内参，包括光心和焦距。

```c++
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
}ALG_DEPTH_INF_S;
```

该结构体定义了深度图的信息。
s32Flag用于表示是否需要将深度图转换为点云数据，为1表示算法内部会预开深度图转点云数据所需的buffer，为0表示算法内部不会预开深度图转点云数据所需的buffer；
s32StepW和s32StepH表示点云数据转换过程中水平方向和垂直方向的采样步长；
s32Width和s32Height表示深度图的宽和高；
f32DepthRatio为深度数据的比例系数；
stCameraMatrix为相机内参矩阵。

```c++
/**
 * @brief Axis selection modes for point cloud passthrough filtering
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
```

该枚举定义rkALG_pclPassThrough滤波的方向，可以选择单个方向或者多个方向同时滤波。

```c++
// Depth image buffer descriptor
typedef struct rkALG_DEPTH_IMG_S
{
    int32_t fd;                                         // File descriptor for DMA buffer
    int32_t width;                                      // Image width in pixels
    int32_t height;                                     // Image height in pixels
    void* virAddr;                                      // Virtual address pointer
    void* phyAddr;                                      // Physical address pointer
}ALG_DEPTH_IMG_S;
```

该结构体定义了深度图的内存。

```c++
// Point cloud data buffer descriptor
typedef struct rkALG_PCL_POINTS_BUFFER_S {
    int32_t fd;                                         // DMA buffer file descriptor
    int32_t s32PointsNum;                               // Number of points in buffer
    void* virAddr;                                      // Virtual address of point data
    void* phyAddr;                                      // Physical address of point data
} ALG_PCL_POINTS_BUFFER_S;
```

该数据定义了点云数据的内存。

```c++
// 3D point with homogeneous coordinate‌
typedef RKPCL_ALIGN16 struct rkALG_PCL_POINT_S {
    union {
        float data[4];
        struct {
            float x;                                            // X coordinate
            float y;                                            // Y coordinate
            float z;                                            // Z coordinate
        };
    };
} ALG_PCL_POINT_S;
```

该结构体定义了点云数据的结构，注意虽然只有 3 个维度，但是用 float[4] 对齐存储，这和 `pcl::PointXYZ` 在内存中的数据排布保持一致，所以 ALG_PCL_POINT_S 和 pcl::PointXYZ 可以相互引用指针并解析。

```C++
/* A point structure representing normal coordinates and the surface curvature estimate */
typedef RKPCL_ALIGN16 struct rkALG_PCL_NORMAL_S {
    union {
        float data_n[4];
        struct {
            float normal_x;                                   // Normal X coordinate
            float normal_y;                                   // Normal Y coordinate
            float normal_z;                                   // Normal Z coordinate
        };
    };
    union {
        float data_c[4];
        struct {
            float curvature;                            // Surface curvature estimate
        };
    };
} ALG_PCL_NORMAL_S;
```

定义法线数据结构，同样和 `pcl::Normal` 的定义保持一致，曲率信息需对齐按 4 个 float 对齐，一共对齐到 8 个 float。

```C++
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
```

带法线点云的数据结构定义，和 `pcl::PointNormal` 保持一致。

```C++
// normals data buffer descriptor
typedef ALG_PCL_POINTS_BUFFER_S ALG_PCL_NORMALS_BUFFER_S;
```

法线数据内存与点云数据类似.

```C++
// PointNormals data buffer descriptor
typedef ALG_PCL_POINTS_BUFFER_S ALG_PCL_POINTNORMALS_BUFFER_S;
```

带法线点云数据内存与点云数据类似。

> [!note]
>
> 1. 申请空间时，相同点数下法线所需空间为点云的 2 倍，带法线点云所需空间为点云的 3 倍。
> 2. 为确保首地址按需对齐，非零拷贝模式下，申请地址时应按以下代码所示。
>
> ```C++
> ALG_PCL_POINTS_BUFFER_S srcPointBuffer; /* alloc memory for point cloud */
> srcPointBuffer.s32PointsNum = numPoints;
> size_t pointsBufferSize = srcPointBuffer.s32PointsNum * sizeof(ALG_PCL_POINT_S);
> srcPointBuffer.virAddr = malloc(pointsBufferSize);
>
> ALG_PCL_NORMALS_BUFFER_S dstNormalsBuffer; /* alloc memory for normals */
> dstNormalsBuffer.s32PointsNum = numPoints;
> size_t normalsBufferSize = dstNormalsBuffer.s32PointsNum * sizeof(ALG_PCL_NORMAL_S);
> dstNormalsBuffer.virAddr = aligned_alloc(16, normalsBufferSize);
>
> ALG_PCL_POINTNORMALS_BUFFER_S dstPointNormalsBuffer; /* alloc memory for pointNormals */
> dstPointNormalsBuffer.s32PointsNum = numPoints;
> size_t pointNormalsBufferSize = dstPointNormalsBuffer.s32PointsNum * sizeof(ALG_PCL_POINTNORMAL_S);
> dstPointNormalsBuffer.virAddr = aligned_alloc(16, pointNormalsBufferSize);
> ```

```C++
// indices data buffer descriptor
typedef ALG_PCL_POINTS_BUFFER_S ALG_PCL_INDICES_BUFFER_S;
```

索引数据内存与点云数据内存类似，申请空间时可以按 int 计算所需字节量.

```c++
// Passthrough filter parameters
typedef struct rkALG_PCL_PASS_THROUGH_S {
    float minX;                       // Minimum X threshold
    float maxX;                       // Maximum X threshold
    float minY;                       // Minimum Y threshold
    float maxY;                       // Maximum Y threshold
    float minZ;                       // Minimum Z threshold
    float maxZ;                       // Maximum Z threshold
    int32_t limitsNegative;           // 0=keep inside range, 1=keep outside
    int32_t passThroughMode;          // Filtering mode from OCL_PCL_PASS_THROUGH_MODE_E
} ALG_PCL_PASS_THROUGH_S;
```

该结构体定义了直通滤波器的配置参数。limitsNegative用来控制保留点的范围，0时保留滤波范围内的点，1时保留滤波范围外的点；passThroughMode控制滤波的模式，支持单方向和多方向的滤波。

```c++
// Radius-based outlier removal parameters
typedef struct rkALG_PCL_RADIUS_OUTLIER_REMOVAL_S {
    float f32RadiusSearch;                       // Search radius for neighbors
    int32_t s32NeighborsInRadius;                // Minimum neighbors required
} ALG_PCL_RADIUS_OUTLIER_REMOVAL_S;
```

该结构体定义了半径滤波的配置参数。f32RadiusSearch为滤波半径，s32NeighborsInRadius为滤波半径内最小邻域点数，当滤波半径内邻域点数小于该值时目标点被滤除，反之则保留。

```c++
// Statistical outlier removal parameters
typedef struct rkALG_PCL_STATISTICAL_OUTLIER_REMOVAL_S {
    int32_t s32MeanK;                          // Number of neighbors to analyze
    float f32StddevMulThresh;                  // Standard deviation multiplier
} ALG_PCL_STATISTICAL_OUTLIER_REMOVAL_S;
```

该结构体定义了统计滤波的配置参数。s32MeanK为最近邻点数，最大值为50；s32MeanKf32StddevMulThresh为统计方差的乘法系数。

```c++
// Euclidean clustering parameters
typedef struct rkALG_PCL_EUCLIDEAN_CLUSTER_S
{
    float f32ClusterTolerance;               // Maximum point-to-point distance in cluster
    int32_t s32MinClusterSize;               // Minimum points per cluster
    int32_t s32MaxClusterSize;               // Maximum points per cluster
}ALG_PCL_EUCLIDEAN_CLUSTER_S;
```

该结构体定义了聚类的配置参数。f32ClusterTolerance为聚类所需的相邻两点间的最大距离，小于等于该值为同一集合，s32MinClusterSize为判别某一类的最少内点数，s32MaxClusterSize为某一类的最大内点数。

```c++
// Cluster extraction result container
typedef struct rkALG_PCL_CLUSTER_RESULT_S
{
    int32_t s32ClustersNum;                             // Detected clusters count
    int32_t s32ClustersOffset[MAX_CLUSTERS_NUM];        // Starting indices in buffer
    int32_t s32ClustersPointsNum[MAX_CLUSTERS_NUM];     // Points count per cluster
    ALG_PCL_INDICES_BUFFER_S* pstIndicesBuffer;         // Cluster indices buffer
}ALG_PCL_CLUSTER_RESULT_S;
```

该结构体定义了聚类/分割的输出信息。s32ClustersNum为检测到聚类后集合的个数，s32ClustersOffset为类内点idx在pstIndicesBuffer内存上的起始位置，s32ClustersPointsNum为类内点的数量，pstIndicesBuffer为所有聚类结果点的索引。

```c++
// Voxel grid downsampling parameters
typedef struct rkALG_PCL_VOXEL_GRID_S {
    float f32LeafSizeX;                     // Voxel size in X dimension
    float f32LeafSizeY;                     // Voxel size in Y dimension
    float f32LeafSizeZ;                     // Voxel size in Z dimension
} ALG_PCL_VOXEL_GRID_S;
```

该结构体定义了体素均值滤波的配置参数，f32LeafSizeX为体素网格x方向的宽度，f32LeafSizeY为体素网格y方向的宽度，f32LeafSizeZ为体素网格z方向的宽度。

```c++
// Uniform sampling parameters
typedef struct rkALG_PCL_UNIFORM_SAMPLING_S {
    float f32RadiusSearch;                 // Sphere radius for sampling
} ALG_PCL_UNIFORM_SAMPLING_S;
```

该结构体定义了体素滤波的配置参数，f32RadiusSearch为体素网格的边长。

```C++
/* normal estimation params */
typedef struct rkALG_PCL_NORMAL_ESTITMATION_S {
    int32_t s32K;                          // Number of neighbors to search (only make sense when eNeighborMode=PCL_NEIGHBOR_MODE_KNN)
    float f32Radius;                       // Search radius for neighbors (only make sense when eNeighborMode=PCL_NEIGHBOR_MODE_RADIUS)
    ALG_PCL_NEIGHBOR_MODE_E eNeighborMode; // Neighbor search mode
    float f32ViewPointX;                   // Viewpoint X coordinate. Used to unify normal orientation. A typical viewpoint value is (0, 0, 0)
    float f32ViewPointY;                   // Viewpoint Y coordinate
    float f32ViewPointZ;                   // Viewpoint Z coordinate
} ALG_PCL_NORMAL_ESTIMATION_S;
```

定义法线估计的配置参数，K 近邻数和搜索半径只有其中一个会生效，根据近邻搜索模式 `eNeighborMode` 选定。视点坐标用于统一法线朝向。

```C++
/* boundary estimation params */
typedef struct rkALG_PCL_BOUNDARY_ESTIMATION_S {
    int32_t s32K;                                   // Number of neighbors to search (only make sense when eNeighborMode=PCL_NEIGHBOR_MODE_KNN)
    float f32Radius;                                // Search radius for neighbors (only make sense when eNeighborMode=PCL_NEIGHBOR_MODE_RADIUS)
    ALG_PCL_NEIGHBOR_MODE_E eNeighborMode;          // Neighbor search mode
    float f32RadianAngleThreshold;                  // Set the decision boundary (angle threshold) that marks points as boundary or regular.
    const ALG_PCL_NORMALS_BUFFER_S* pNormalsBuffer;
} ALG_PCL_BOUNDARY_ESTIMATION_S;
```

定义边界估计的配置参数，K 近邻数和搜索半径只有其中一个会生效，根据近邻搜索模式 `eNeighborMode` 选定。`f32RadianAngleThreshold` 是弧度制的边界点判决阈值。

```C++
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
```

定义区域生长分割的配置参数。
`u8SmoothModeFlag` 控制平滑性约束时使用当前种子点（ON）还是初始种子点（OFF），
`f32RadianThetaThreshold` 是弧度制表示的平滑性约束夹角阈值，始终生效，
`f32ResidualThreshold` 是残差阈值，仅在开启残差测试(`u8ResidualFlag`)时生效，
`f32CurvatureThrehold` 是曲率阈值，仅在开启曲率测试(`u8CurvatureFlag`)时生效。

```C++
typedef struct rkALG_PCL_MOMENT_OF_INERTIA_S {
    float f32AngleStep;
    uint8_t u8NormalFlag;
    float f32PointMass;   /* only make sense when u8NormalFlag is false(0) */
} ALG_PCL_MOMENT_OF_INERTIA_S;
```

定义点云惯性矩估计的配置参数。
`f32AngleStep` 是以度数表示的步进角度，
`u8NormalFlag` 控制是否开启点云质量归一化，开启时点质量默认为 $\frac {1}{N^2}$.
`f32PointMass` 是点质量，仅在点云质量归一化关闭时生效。

```C++
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
```

定义惯性矩/偏心率估计结果。包括点云质心、坐标轴对齐外包围盒、带方向包围盒、特征值、特征向量、惯性矩、偏心率信息。

```C++
typedef struct rkALG_PCL_ICP_PARAM_S {
    float f32MaxCorrespondenceDistance; /* the maximum distance threshold between a point and its nearest neighbor correspondent in order to be considered in the alignment process */
    uint32_t u32MaxIterations; /* the maximum number of iterations the internal optimization should run for */
    float f32TransformationEpsilon; /* maximum allowable translation squared difference between two consecutive transformations */
    float f32EuclideanFitnessEpsilon; /* maximum allowable Euclidean error between two consecutive steps in the ICP loop */
    uint8_t u8GetFitnessScore; /* set it to 1 if fitness score is needed, otherwise the fitness will not be computed */
    uint8_t u8EnableInitGuess; /* initialize transformation according to pf32InitGuess (if u8EnableInitGuess is set) or identity matrix(if u8EnableInitGuess is 0) */
    float pf32InitGuess[16]; /* pointer of init guess matrix(a Eigen::Matrix4f), only make sense when u8EnableInitGuess is set */
} ALG_PCL_ICP_PARAM_S;
```

定义ICP点云配准算法的配置参数。
`f32MaxCorrespondenceDistance` 控制对应点搜索距离阈值；
`u32MaxIterations` 控制最大迭代次数；
`f32TransformationEpsilon` 控制相邻两次迭代过程中，取得的变换矩阵平移量差值约束；
`f32EuclideanFitnessEpsilon` 控制相邻两次迭代过程的相对欧式距离误差。误差的计算方法为：(对应点的欧式距离平方和/点对数目)；
`u8GetFitnessScore` 控制当前 ICP 算子是否计算配准分数，如果不关心配准分数，可以将其设为 0 以跳过分数计算；
`u8EnableInitGuess` 控制是否传入初始预测位姿；
`pf32InitGuess` 为初始预测位姿，仅当启用 `u8EnableInitGuess` 时生效，按列主序排布，可直接由 `Eigen::Matrix4f.data()` 拷贝 `16 * sizeof(float)` 而得。

```C++
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
```

定义NDT点云配准算法的配置参数。
`f32OutlierRatio` 预估离群点比例；
`u32MaxIterations` 最大迭代次数；
`f32StepSize` 最大迭代步长；
`f32Resolution` 体素网格尺寸；
`u8GetFitnessScore` 当前 NDT 算子是否计算配准分数，如果不关心配准分数，可以将其设为 0 以跳过分数计算；
`u8EnableInitGuess` 控制是否传入初始预测位姿；
`pf32InitGuess` 为初始预测位姿，仅当启用 `u8EnableInitGuess` 时生效，按列主序排布，可直接由 `Eigen::Matrix4f.data()` 拷贝 `16 * sizeof(float)` 而得。

```C++
typedef struct rkALG_PCL_REGISTRATION_RESULT_S {
    uint8_t u8IsConverged; /* 1=converged, 0=not converged */
    uint32_t u32FinalIterations; /* final number of iterations */
    float f32Transformation[16]; /* pointer of the transformation matrix(a Eigen::Matrix4f) */
    float f32FitnessScore; /* only make sense if u8GetFitnessScore of param is set */
    ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer; /* Pointer of the output point cloud, need to be pre-allocated for ICP/NDT */
    ALG_PCL_POINTNORMALS_BUFFER_S* pDstPointNormalBuffer; /* Pointer of the output point cloud, need to be pre-allocated for ICPWithNormals */
} ALG_PCL_REGISTRATION_RESULT_S;
```

点云配准结果结构体，ICP 和 NDT 通用。
`u8IsConverged` 表示迭代是否收敛；
`u32FinalIterations` 为迭代次数；
`f32Transformation` 为点云变换矩阵的首地址（列主序）；
`f32FitnessScore` 为点云变换分数，计算规则同 pcl，数值越小表示配准所得的变换矩阵拟合程度越高；
`pDstPointBuffer` 为变换后的结果点云，通过 ICP/NDT 进行配准时需要为其预申请空间；
`pDstPointNormalBuffer` 为变换后的结果带法线点云，通过 ICP With Normals 进行配准时需要为其预申请空间。

```c++
// PCL processing engine initialization parameters
typedef struct rkALG_PCL_INIT_S {
    int32_t s32MaxPointNum;                // Maximum allocatable points
    int32_t s32IsZeroCopy;                 // 1=use zero-copy buffers, 0=normal mode
    ALG_DEPTH_INF_S stDepthInfo;           // Depth sensor configuration
} ALG_PCL_INIT_S;
```

该结构体定义了rkPCL算法的初始化参数。s32MaxPointNum为处理点云的最大点数；s32IsZeroCopy为是否采用零拷贝的数据结构，0表示不采用零拷贝的数据类型，1表示采用零拷贝的数据类型；stDepthInfo定义了深度图的信息。

```c++
// Opaque handle for PCL processing engine
typedef struct rkALG_PCL_ENGINE {
    void* p;                                 // Internal implementation pointer
} ALG_PCL_ENGINE;
```

该结构体定义了rkPCL的计算引擎。

### 函数接口

```c++
/**
* @brief Retrieve PCL algorithm library version information
* @param rkPclVersion Output buffer (recommended 128 bytes)
* @return 0 success, non-zero error code
*/
int32_t rkALG_pclVersion(char rkPclVersion[128]);
```

该函数可以获取当前算法库的版本信息。

```c++
/**
* @brief Initialize PCL processing engine with configuration
* @param pstEngine Engine handle pointer (pre-allocated)
* @param stInitParams Initialization params (max points/zero-copy flag)
* @return 0 success, non-zero error code
*/
int32_t rkALG_pclInit(ALG_PCL_ENGINE* pstEngine, ALG_PCL_INIT_S* stInitParams);
```

该函数用于初始化rkPCL的计算引擎，内部会预开计算所需的buffer。

```c++
/**
* @brief Convert depth map to point cloud coordinates
* @param pSrcDepthImgBuffer Input depth image with camera matrix
* @param pDstPointBuffer Output point cloud buffer (pre-allocated)
* @return 0 success, non-zero error code
*/
int32_t rkALG_pclDepth2Points(ALG_PCL_ENGINE* pstEngine,
        ALG_DEPTH_IMG_S* pSrcDepthImgBuffer, ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer);
```

该函数用于将深度图转换为点云数据。

```c++
/**
* @brief concatenate a point cloud and a normal cloud (for ICP With normal)
* @param pstEngine Engine handle
* @param pSrcPointBuffer Input point cloud
* @param pSrcNormalBuffer Input normal cloud
* @param pDstPointNormalBuffer Output point normal cloud
*/
int32_t rkALG_pclConcatenatePointAndNormal(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, const ALG_PCL_NORMALS_BUFFER_S* pSrcNormalBuffer, ALG_PCL_POINTNORMALS_BUFFER_S* pDstPointNormalBuffer);
```

将普通点云和法线连接成带法线点云，用于带法线信息的配准（ICP With Normals）。

```c++
/**
* @brief Apply axis-aligned filtering to point cloud
* @param pPassThroughParams Filter params (axis ranges/mode)
* @param pSrcPointBuffer Input point cloud
* @param pDstPointBuffer Filtered output (pre-allocated)
* @return 0 success, non-zero error code
*/
int32_t rkALG_pclPassThrough(ALG_PCL_ENGINE* pstEngine,
        					 ALG_PCL_PASS_THROUGH_S* pPassThroughParams,
        					 ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer,
        					 ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer);
```

该函数用于对点云数据进行直通滤波。

```C++
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
int32_t rkALG_pclRemoveNaNFromPointCloud(ALG_PCL_ENGINE* pstEngine,
	const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer, int32_t* pIndices, int32_t* pNumValidPoints,
	ALG_PCL_BACKEND_E eBackEnd);
```

用于移除点云中的 NaN/+Inf/-Inf，有效点的索引通过 `pIndices` 返回。

> [!note]
>
> 1. 无需输出点云时，可以将输出点云置空 (`pDstPointBuffer = nullptr`).
> 2. 支持原地操作(in-place operation)，输入和输出点云可以相同 (`pSrcPointBuffer == pDstPointBuffer`).
> 3. 提供基于 CPU 和 GPU 进行加速的 2 种后端实现供选择.
> 4. `pIndices` 需提前按最大点数申请好空间（参照示例代码）。

```c++
/**
* @brief Remove outliers via neighbor density check
* @param pRadiusOutlierRemovalParams (search radius/min neighbors)
* @param pSrcPointBuffer Input point cloud
* @param pDstPointBuffer Filtered output
* @return 0 success, non-zero error code
*/
int32_t rkALG_pclRadiusOutlierRemoval(ALG_PCL_ENGINE* pstEngine, 						   							ALG_PCL_RADIUS_OUTLIER_REMOVAL_S* pRadiusOutlierRemovalParams,
						 ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer,
                         ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer);
```

该函数用于对点云数据进行半径滤波。

```c++
/**
* @brief Remove statistical outliers using distribution analysis
* @param pStatisticalOutlierRemoval (mean neighbors/stddev threshold)
* @param pSrcPointBuffer Input point cloud
* @param pDstPointBuffer Filtered output
* @return 0 success, non-zero error code
*/
int32_t rkALG_pclStatisticalOutlierRemoval(ALG_PCL_ENGINE* pstEngine, 							ALG_PCL_STATISTICAL_OUTLIER_REMOVAL_S* pStatisticalOutlierRemoval,
		ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer,
        ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer);
```

该函数用于对点云数据进行统计滤波。

```c++
/**
* @brief Perform Euclidean cluster segmentation
* @param pEuclideanClusterExtraction (cluster tolerance/size range)
* @param pSrcPointBuffer Input point cloud
* @param pClusterResult Cluster indices storage
*/
int32_t rkALG_pclEuclideanClusterExtraction(ALG_PCL_ENGINE* pstEngine, 												ALG_PCL_EUCLIDEAN_CLUSTER_S* pEuclideanClusterExtraction,
							ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer,
                			ALG_PCL_CLUSTER_RESULT_S* pClusterResult);
```

该函数对点云数据进行聚类分割。

```c++
/**
* @brief Perform uniform sampling on point cloud data using sphere radius criteria
* @param pUniformSampling Sampling parameters (contains search radius configuration)
* @param pSrcPointBuffer Input point cloud buffer (must be pre-allocated)
* @param pDstPointBuffer Output sampled point buffer (requires pre-allocated memory)
* @return 0 for success, non-zero error code for failure conditions
*/
int32_t rkALG_pclUniformSampling(ALG_PCL_ENGINE* pstEngine,
                                 ALG_PCL_UNIFORM_SAMPLING_S* pUniformSampling,
								 ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer,
                                 ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer);
```

该函数对点云数据进行体素滤波。

```c++
/**
* @brief Apply voxel grid downsampling
* @param pVoxelGrid (leaf node dimensions)
* @param pSrcPointBuffer Input point cloud
* @param pDstPointBuffer Downsampled output
* @return 0 success, non-zero error code
*/
int32_t rkALG_pclVoxelGrid(ALG_PCL_ENGINE* pstEngine, ALG_PCL_VOXEL_GRID_S* pVoxelGrid,
							ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer,
                            ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer);
```

该函数对点云数据进行体素均值滤波。

```C++
/**
 * @brief Apply point cloud transformation
 * @param pSrcPointBuffer Input point cloud
 * @param pDstPointBuffer Transformed output
 * @param pTransform Pointer of a Eigen::Matrix4f containing the transformation matrix
 * @param eBackEnd Backend type (CPU/GPU)
 * @note  This function can be called with (pSrcPointBuffer == pDstPointBuffer)
 * @return 0 success, non-zero error code
 */
 int32_t rkALG_pclTransformPointCloud(ALG_PCL_ENGINE* pstEngine,
 	ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer,
 	const float* pTransform, ALG_PCL_BACKEND_E eBackEnd);
```

该函数对点云数据进行刚体变换。

> [!note]
>
> 1. 调用该函数时，需确保点云内没有 NaN/Inf 数据（如存在非法数据，可先调用 `removeNaNFromPointCloud` 接口）.
> 2. 该算子支持原地操作(in-place operation)，输入和输出点云可以相同 (`pSrcPointBuffer == pDstPointBuffer`).
> 3. 提供基于 CPU 和 GPU 进行加速的 2 种后端实现供选择.
> 4. `pTransform` 可以直接使用 `Eigen::Matrix4f::data()` 作为数据指针。

```C++
/**
 * @brief Copy point cloud data
 * @param pSrcPointBuffer Input point cloud
 * @param pDstPointBuffer Output point cloud
 * @return 0 success, non-zero error code
 */
int32_t rkALG_pclCopyPointCloud(ALG_PCL_ENGINE* pstEngine,
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
int32_t rkALG_pclCopyPointCloudWithIndex(ALG_PCL_ENGINE* pstEngine,
	const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_POINTS_BUFFER_S* pDstPointBuffer,
	const int32_t* pIndex, int32_t s32NumValidPoint, ALG_PCL_BACKEND_E eBackEnd);
```

点云拷贝。

> [!note]
>
> 1. 根据是否带索引(index)分化为两个函数.
> 2. 提供基于 CPU 和 GPU 进行加速的 2 种后端实现供选择.

```C++
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
int32_t rkALG_pclKdTreeNearestKSearch(ALG_PCL_ENGINE* pstEngine,
	const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, const ALG_PCL_POINT_S* pQueryPoint, int32_t K,
	int32_t* pKIndices, float* pKSquaredDistances);
```

基于 k-dimensional Tree 的 K 近邻查找。

> [!note]
>
> 1. 调用该函数时，需确保点云内没有 NaN/Inf 数据（如存在非法数据，可先调用 `removeNaNFromPointCloud` 接口）.
> 2. `pKIndices` 和 `pKSquaredDistances` 需提前按照目标近邻数目 `K` 申请好空间（参照示例代码）。

```C++
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
int32_t rkALG_pclKdTreeRadiusSearch(ALG_PCL_ENGINE* pstEngine,
	const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, const ALG_PCL_POINT_S* pQueryPoint, float radius,
	int32_t* pIndices, float* pSquaredDistances, uint32_t max_nn);
```

基于 k-dimensional Tree 的半径查找。

> [!note]
>
> 1. 调用该函数时，需确保点云内没有 NaN/Inf 数据（如存在非法数据，可先调用 `removeNaNFromPointCloud` 接口）.
> 2. `pKIndices` 和 `pKSquaredDistances` 需提前申请好空间（参照示例代码），若结果数量未知，可以按照最大点数申请。

```C++
/**
 * @brief Compute the Least-Squares plane fit for a given set of points, using their indices, and return the estimated plane parameters together with the surface curvature.
 * @param pSrcPointBuffer Input point cloud
 * @param indices the point cloud indices that need to be used
 * @param numPoints the number of points to be used
 * @param pNormal the resultant normal and curvature of the plane
 * @return 0 success, non-zero error code
 */
int32_t rkALG_pclComputePointNormal(ALG_PCL_ENGINE* pstEngine,
	const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, const int32_t* indices, int32_t numPoints,
	ALG_PCL_NORMAL_S* pNormal);
```

对所选的指定点（一般为邻域点）进行法线估计。

```C++
/**
 * @brief Estimates local surface properties (surface normals and curvatures)at each 3D point.
 * @param pSrcPointBuffer Input point cloud
 * @param pNormalEstimationParam Normal estimation parameters
 * @param pDstNormalsBuffer Output normal and curvature buffer
 * @note In situations where not enough neighbors are found, the normal and curvature values are set to NaN.
 * @return 0 success, non-zero error code
 */
int32_t rkALG_pclNormalEstimation(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_NORMAL_ESTIMATION_S* pNormalEstimationParam,
	const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_NORMALS_BUFFER_S* pDstNormalsBuffer);
```

根据指定的近邻搜索策略，对全点云进行法线估计。

> [!note]
>
> 1. 如果近邻数目不足以进行法线估计，当前点处的法线值和曲率值会以 `NaN` 返回。

```C++
/**
 * @brief Estimate the boundary of the point cloud
 * @param pBoundaryEstimationParam Boundary estimation parameters
 * @param pSrcPointBuffer Input point cloud
 * @param pDstIsBoundary Output boundary buffer (must be pre-allocated to the same size as the input point cloud!). 1: is boundary/ 0: is not boundary.
 * @return 0 success, non-zero error code
 */
int32_t rkALG_pclBoundaryEstimation(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_BOUNDARY_ESTIMATION_S* pBoundaryEstimationParam,
	const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, uint8_t* pDstIsBoundary);
```

根据法线和最大投影角阈值，估计点云边界。

> [!note]
>
> 1. 该算子不会内部计算法线，需要先用 `rkALG_pclNormalEstimation` 接口估计法线后传入。
> 2. `pDstIsBoundary` 包含了是否为点云边界的布尔值信息。1 表示边界点，0 表示非边界点，需要预先按点数申请好空间。

```C++
/**
 * @brief Get the minimum and maximum values on each of the 3 (x-y-z) dimensions in a given pointcloud.
 * @param pSrcPointBuffer Input point cloud
 * @param pMinPoint the resultant minimum bounds
 * @param pMaxPoint the resultant maximum bounds
 * @return 0 success, non-zero error code
*/
int32_t rkALG_pclGetMinMax3D(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_POINT_S* pMinPoint, ALG_PCL_POINT_S* pMaxPoint);

/**
 * @brief Get the minimum and maximum values on each of the 3 (x-y-z) dimensions in a given pointcloud.
 * @param pSrcPointBuffer Input point cloud
 * @param pIndex the vector of point indices to use from cloud
 * @param s32NumValidPoint the number of valid points in pIndex
 * @param pMinPoint the resultant minimum bounds
 * @param pMaxPoint the resultant maximum bounds
 * @return 0 success, non-zero error code
*/
int32_t rkALG_pclGetMinMax3DWithIndex(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, const int32_t* pIndex, int32_t s32NumValidPoint, ALG_PCL_POINT_S* pMinPoint, ALG_PCL_POINT_S* pMaxPoint);
```

计算点云的轴对齐外包围盒，根据是否带索引分为两个函数。

```C++
/**
 * @brief: Obtain the maximum segment in a given point cloud, and return the minimum and maximum points.
 * @param pSrcPointBuffer Input point cloud
 * @param pMinPoint the coordinates of the "minimum" point in cloud (one end of the segment)
 * @param pMaxPoint the coordinates of the "maximum" point in cloud (the other end of the segment)
 * @param length the length of segment
 * @return 0 success, non-zero error code
*/
DLL_PUBLIC int32_t rkALG_pclGetMaxSegment(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_POINT_S* pMinPoint, ALG_PCL_POINT_S* pMaxPoint, double* length);
```

计算点云的最大点对距离。

```C++
/**
 * @brief Implements the well known Region Growing algorithm used for segmentation.
 * @param pSrcPointBuffer Input point cloud
 * @param pRegionGrowingParams Region growing parameters
 * @param pClusterResult Output cluster result
 * @note pClusterResult->pstIndicesBuffer must be pre allocated to the same size as the input point cloud.
 * @return 0 success, non-zero error code
*/
int32_t rkALG_pclRegionGrowing(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, const ALG_PCL_REGION_GROWING_S* pRegionGrowingParams, ALG_PCL_CLUSTER_RESULT_S* pClusterResult);
```

基于法向量平滑性进行区域生长分割。

> [!note]
>
> 1. 该算子不会内部计算法线，需要先用 `rkALG_pclNormalEstimation` 接口估计法线后传入。
> 2. 需要预先按点数为 `pClusterResult->pstIndicesBuffer` 申请好空间.

```C++
/**
 * @brief extract features based on moment of inertia. AABB, OBB and eccentricity of the point cloud also calculated.
 * @param pMomentOfInertiaParam Moment of inertia parameters
 * @param pSrcPointBuffer Input point cloud
 * @param pMomentOfInertiaResult result. moment of inertia, AABB, OBB, eccentricity of the point cloud are included.
 * @note pMomentOfInertiaResult->pf32MomentOfInertia, pMomentOfInertiaResult->pf32Eccentricity must be pre-allocaed to (int)(360 / f32AngleStep + 1) * (int)(90 / f32AngleStep + 1)
 * @return 0 success, non-zero error code
*/
int32_t rkALG_pclMomentOfInertia(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_MOMENT_OF_INERTIA_S* pMomentOfInertiaParam,
	const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, ALG_PCL_MOMENT_OF_INERTIA_RESULT_S* pMomentOfInertiaResult);
```

点云惯性矩/偏心率估计。输出结果包括点云质心、坐标轴对齐最小包围盒、带方向包围盒、特征值、特征向量、惯性矩、偏心率信息。

> [!note]
>
> 1. 需要预先按照结果长度为 `pMomentOfInertiaResult->pf32MomentOfInertia`, `pMomentOfInertiaResult->pf32Eccentricity` 申请好空间。
>    长度计算规则： `length = (int)(360 / f32AngleStep + 1) * (int)(90 / f32AngleStep + 1)`
>
> 2. 该算子同时会提取 AABB 特征，但如果只需要 AABB，则应使用速度更快的 `rkALG_pclGetMinMax3D`。
>
> 3. 该算子同时会提取 OBB 特征，但提取出的 OBB 可能并非最小的边界框。
>
> 4. 输出结果中的 OBB 旋转矩阵结果 `pMomentOfInertiaResult->pf32OBBRotationMatrix` 按列主序存储，可以通过 `Eigen::Map` 从中获取 `Eigen::Matrix3f`, 如
>
>    ```C++
>    Eigen::Map<Eigen::Matrix3f> rotation(pMomentOfInertiaResult->pf32OBBRotationMatrix);
>    ```

```C++
/**
* @brief estimates the transformation(ICP) and returns the transformed point cloud as output.
* @param pICPParam ICP parameters
* @param pSrcPointBuffer Input point cloud
* @param pTargetPointBuffer Target point cloud
* @param pICPResult Output ICP result
* @param eBackEnd Backend type (CPU/GPU)
* @return 0 success, non-zero error code
*/
int32_t rkALG_pclIterativeClosestPoint(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_ICP_PARAM_S* pICPParam, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, const ALG_PCL_POINTS_BUFFER_S* pTargetPointBuffer, ALG_PCL_REGISTRATION_RESULT_S* pICPResult, ALG_PCL_BACKEND_E eBackEnd);
```

ICP点云配准。

> [!note]
>
> 1. 需要预先按点数为 `pICPResult->pDstPointBuffer` 申请好空间。
>
> 2. 如果不关心配准分数，可以将 `pICPParam->u8GetFitnessScore` 设为 0 以节省计算；否则需将其设为非 0 值，配准分数可以通过 `pICPResult->f32FitnessScore` 查询。
>
> 3. 输出结果中的点云变换矩阵 `pICPResult->f32Transformation` 按照列主序存储，可以通过 `Eigen::Map` 从中获取 `Eigen::Matrix4f`，如
>
>    ```C++
>    Eigen::Map<Eigen::Matrix4f> transformation(pICPResult->f32Transformation);
>    ```
>
> 4. 提供基于 CPU 和 GPU 进行加速的 2 种后端实现供选择.

```c++
/**
* @brief estimates the transformation(ICPWithNormals) and returns the transformed point cloud as output.
* @param pICPParam ICP parameters
* @param pSrcPointNormalBuffer Input point cloud with normals. Generate via rkALG_pclConcatenatePointAndNormal.
* @param pTargetPointNormalBuffer Target point cloud with normals. Generate via rkALG_pclConcatenatePointAndNormal.
* @param pICPResult Output ICP result
* @param eBackEnd Backend type (CPU/GPU)
* @return 0 success, non-zero error code
* @note
*/
int32_t rkALG_pclIterativeClosestPointWithNormals(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_ICP_PARAM_S* pICPParam, const ALG_PCL_POINTNORMALS_BUFFER_S* pSrcPointNormalBuffer,
const ALG_PCL_POINTNORMALS_BUFFER_S* pTargetPointNormalBuffer, ALG_PCL_REGISTRATION_RESULT_S* pICPResult, ALG_PCL_BACKEND_E eBackEnd);
```

带法线信息的 ICP 点云配准。

> [!note]
>
> 1. 需要预先按点数为 `pICPResult->pDstPointNormalBuffer` 申请好空间。
>
> 2. 如果不关心配准分数，可以将 `pICPParam->u8GetFitnessScore` 设为 0 以节省计算；否则需将其设为非 0 值，配准分数可以通过 `pICPResult->f32FitnessScore` 查询。
>
> 3. `pSrcPointNormalBuffer`/`pTargetPointNormalBuffer` 可通过 `rkALG_pclConcatenatePointAndNormal` 接口生成，也可自行按对齐规则拼接点云和法线数据。
>
> 4. 输出结果中的点云变换矩阵 `pICPResult->f32Transformation` 按照列主序存储，可以通过 `Eigen::Map` 从中获取 `Eigen::Matrix4f`，如
>
>    ```C++
>    Eigen::Map<Eigen::Matrix4f> transformation(pICPResult->f32Transformation);
>    ```
>
> 5. 提供基于 CPU 和 GPU 进行加速的 2 种后端实现供选择.

```C++
/**
* @brief Estimate the transformation (NDT-based) and returns the transformed point cloud as output.
* @param pNDTParam NDT parameters
* @param pSrcPointBuffer Input point cloud
* @param pTargetPointBuffer Target point cloud
* @param pNDTResult Output NDT result
* @param eBackEnd Backend type (only CPU backend supported now)
* @return 0 success, non-zero error code
*/
int32_t rkALG_pclNormalDistributionsTransform(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_NDT_PARAM_S* pNDTParam, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer, const ALG_PCL_POINTS_BUFFER_S* pTargetPointBuffer, ALG_PCL_REGISTRATION_RESULT_S* pNDTResult, ALG_PCL_BACKEND_E eBackEnd);
```

NDT 点云配准。

> [!note]
>
> 1. 需要预先按点数为 `pNDTResult->pDstPointBuffer` 申请好空间。
>
> 2. 如果不关心配准分数，可以将 `pNDTParam->u8GetFitnessScore` 设为 0 以节省计算；否则需将其设为非 0 值，配准分数可以通过 `pNDTResult->f32FitnessScore` 查询。
>
> 3. 输出结果中的点云变换矩阵 `pICPResult->f32Transformation` 按照列主序存储，可以通过 `Eigen::Map` 从中获取 `Eigen::Matrix4f`，如
>
>    ```C++
>    Eigen::Map<Eigen::Matrix4f> transformation(pICPResult->f32Transformation);
>    ```
>
> 4. 暂时仅支持 CPU 后端。

```C++
/**
* @brief implements a simple 2D rigid transformation estimation (x, y, theta) for a given pair of datasets.
* @param pSrcPointBuffer Input point cloud
* @param pTargetPointBuffer Target point cloud
* @param pTransform pointer to resultant transformation matrix (4x4)
* @param eBackEnd Backend type (CPU/GPU)
* @return 0 success, non-zero error code
* @note pSrcPointBuffer and pTargetPointBuffer must have the same number of points
*/
int32_t rkALG_pclTransformationEstimation2D(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer,
	const ALG_PCL_POINTS_BUFFER_S* pTargetPointBuffer, float* pTransform, ALG_PCL_BACKEND_E eBackEnd);
```

点云 2D 配准。

> [!note]
>
> 1. 输出结果中的点云变换矩阵 `pTransform` 按照列主序存储，可以通过 `Eigen::Map` 从中获取 `Eigen::Matrix4f`，如
>
>    ```C++
>    Eigen::Map<Eigen::Matrix4f> transformation(pTransform);
>    ```
>
> 2. 需要确保 src 点云和 target 点云的点数相同。
>
> 3. 提供基于 CPU 和 GPU 进行加速的 2 种后端实现供选择.

```C++
/**
* @brief implements a simple 3D rigid transformation estimation (x, y, z, roll, pitch, yaw) for a given pair of datasets.
* @param pSrcPointBuffer Input point cloud
* @param pTargetPointBuffer Target point cloud
* @param pTransform pointer to resultant transformation matrix (4x4)
* @return 0 success, non-zero error code
* @note pSrcPointBuffer and pTargetPointBuffer must have 3 points
*/
int32_t rkALG_pclTransformationEstimation3Point(ALG_PCL_ENGINE* pstEngine, const ALG_PCL_POINTS_BUFFER_S* pSrcPointBuffer,
	const ALG_PCL_POINTS_BUFFER_S* pTargetPointBuffer, float* pTransform);
```

3 点配准。

> [!note]
>
> 1. 输出结果中的点云变换矩阵 `pTransform` 按照列主序存储，可以通过 `Eigen::Map` 从中获取 `Eigen::Matrix4f`，如
>
>    ```C++
>    Eigen::Map<Eigen::Matrix4f> transformation(pTransform);
>    ```
>
> 2. 需要确保 src 点云和 target 点云的点数都为 3。

### 示例

demo中使用的“frame_xxx.bin"测试文件，是按(x,y,z,w)紧凑排列的float32 二进制数据，数据排布与结构体ALG_PCL_POINT_S一致。一个“frame_xxx.bin"的数据段如下所示，分别显示了二进制数据以及按float32解析的数据。

```bash
address		00 01 02 03 04 05 06 07	08 09 10 11 12 13 14 15
00000000	b8 dc ef 3e fe 39 e1 be	20 8a 12 be 00 00 80 3f
00000010	e8 ca e8 3e e5 58 ed 3e	84 6e 05 be 00 00 80 3f
00000020	bf b9 e9 3e 73 7b da 3e	64 75 07 be 00 00 80 3f
00000030	b2 aa e8 3e 8f a2 91 3e	0c 7a 08 be 00 00 80 3f
00000040	5a 99 e9 3e b8 30 7d 3e	34 83 0a be 00 00 80 3f
00000050	41 68 ea 3e 46 f8 6f 3e	6c e7 0a be 00 00 80 3f

address		00 01 02 03 	04 05 06 07		08 09 10 11 	12 13 14 15
00000000	0.4684808      -0.4398956      -0.1431050       1.000000
00000010	0.4546731       0.4635688      -0.1303044       1.000000
00000020	0.4564953       0.4267231      -0.1322837       1.000000
00000030	0.4544273       0.2844433      -0.1332781       1.000000
00000040	0.4562481       0.2472562      -0.1352661       1.000000
00000050	0.4578266       0.2343455      -0.1356484       1.000000
```

对于pcd、ply等点云格式的文件，可以用pcl库读取到 pcl::PointCloud\<pcl::PointXYZ\> 对象，随后直接使用points.data()指针。

```c++
pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
std::string dataPath = "xxx.pcd";
if (-1 == pcl::io::loadPCDFile(dataPath.c_str(), *cloud)) {
    std::cout << "error input!" << std::endl;
    return -1;
}
int32_t srcPointsNum = cloud->points.size();
uint32_t bufferSize = srcPointsNum * sizeof(float) * 4;

// If do not use ZERO_COPY_BUFFER
ALG_PCL_POINTS_BUFFER_S pSrcPointBuffer;
pSrcPointBuffer.virAddr = cloud->points.data();
pSrcPointBuffer.s32PointsNum = srcPointsNum;
```

一个基于“frame_xxx.bin"的完整调用demo如下。

```c++
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include "rkALG_pclApp.h"
#include "rkALG_drmBuffer.h"

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

int readBinaryFile2(const char* filename, void* pData, size_t& targetSize) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Failed to open file '%s'\n", filename);
        return -1;
    }
    if (targetSize == 0) {
        fseek(fp, 0, SEEK_END);     /* move fp to end of file */
        targetSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);     /* move fp back to beginning of file */
    }
    size_t readSize = fread(pData, sizeof(unsigned char), targetSize, fp);
    if (readSize != targetSize) {
        printf("read size (%zu) != target size (%zu). The file '%s' might be incomplete!\n", readSize, targetSize, filename);
    }
    fclose(fp);
    return 0;
}

int writeBinaryFile2(const char* filename, void* pData, size_t targetSize) {
    FILE* fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Failed to open file '%s'\n", filename);
        return -1;
    }
    size_t writeSize = fwrite(pData, sizeof(unsigned char), targetSize, fp);
    if (writeSize != targetSize) {
        printf("write size (%zu) != target size (%zu). The file '%s' might be incomplete!\n", writeSize, targetSize, filename);
    }
    fclose(fp);
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

int32_t main(int argc, char** argv)
{
    char rkPclVersion[128];
    rkALG_pclVersion(rkPclVersion);
    std::string filePath = std::string(argv[1]);
    int32_t iterNum = 1;
    int32_t dataNum = 472;
    //init engine
    ALG_PCL_ENGINE pstEngine;
    ALG_PCL_INIT_S stInitParams;
    stInitParams.s32MaxPointNum = 6000;
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
```

其他算子调用示例参考随附的 sample。

## FAQ

**Q1**：虚地址和 fd，应该使用哪个？

**A1**：建议优先使用虚地址。因为大部分 pcl 算法不适合 GPU 运行。输入的数据来源基本上已经是经过 pcl 库处理的数据结构，所以这样可以避免一次拷贝。只有当点云数据量很大且调用的 rkPCL 算子有 GPU 后端实现的时候，才考虑用 fd 的 zero-copy 方式。客户也可以分别测试虚地址方式和 fd 方式的效率差异，这跟点云规模、形态分布有关。

**Q2**：为什么有些算子 GPU 效率不如 CPU 后端？

**A2**：此现象和 GPU 算法实现、点算规模有关，部分 rkPCL 的 GPU 算法涉及到随机访问的操作，对 GPU 不友好。客户需根据优先目的是想减轻 CPU 负担，还是想要更快的算子速度，进行实验后自由选择。

## 调频命令

### RK3588

#### CPU

```bash
（1）获取CPU支持的频率：
获得4个小核：
cat /sys/devices/system/cpu/cpufreq/policy0/scaling_available_frequencies
有以下几种频率选择：
408000 600000 816000 1008000 1200000 1416000 1608000 1800000

获得2个大核：
cat /sys/devices/system/cpu/cpufreq/policy4/scaling_available_frequencies
有以下几种频率选择：
408000 600000 816000 1008000 1200000 1416000 1608000 1800000 2016000 2208000 2304000

获得2个大核：
cat /sys/devices/system/cpu/cpufreq/policy6/scaling_available_frequencies
有以下几种频率选择：
408000 600000 816000 1008000 1200000 1416000 1608000 1800000 2016000 2208000 2304000

（2）获取CPU支持的调频模式
cat /sys/devices/system/cpu/cpufreq/policy0/scaling_available_governors
cat /sys/devices/system/cpu/cpufreq/policy4/scaling_available_governors
cat /sys/devices/system/cpu/cpufreq/policy6/scaling_available_governors
有以下几种模式：
conservative ondemand userspace powersave performance schedutil
默认是自动变频模式：schedutil（恢复的话设置为该模式即可）

（3）设置手动定频模式：userspace
echo userspace > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
echo userspace > /sys/devices/system/cpu/cpufreq/policy4/scaling_governor
echo userspace > /sys/devices/system/cpu/cpufreq/policy6/scaling_governor

设置目标频率：
echo 1800000 > /sys/devices/system/cpu/cpufreq/policy0/scaling_setspeed
echo 2208000 > /sys/devices/system/cpu/cpufreq/policy4/scaling_setspeed
echo 2208000 > /sys/devices/system/cpu/cpufreq/policy6/scaling_setspeed

（4）查看cpu当前频率，确认是否设置成功 ：
cat /sys/devices/system/cpu/cpufreq/policy0/cpuinfo_cur_freq
cat /sys/devices/system/cpu/cpufreq/policy4/cpuinfo_cur_freq
cat /sys/devices/system/cpu/cpufreq/policy6/cpuinfo_cur_freq
```

#### GPU

```bash
（1）获取当前GPU支持的频率：
cat /sys/devices/platform/fb000000.gpu/devfreq/fb000000.gpu/available_frequencies
有以下几种频率选择：
1000000000 900000000 800000000 700000000 600000000 500000000 400000000 300000000

（2）获取GPU支持的调频模式
cat /sys/devices/platform/fb000000.gpu/devfreq/fb000000.gpu/available_governors
有以下几种模式：
userspace powersave performance simple_ondemand

（3）设置手动定频模式：userspace
echo userspace > /sys/devices/platform/fb000000.gpu/devfreq/fb000000.gpu/governor

设置目标频率：
echo 1000000000 > /sys/devices/platform/fb000000.gpu/devfreq/fb000000.gpu/min_freq
echo 1000000000 >  /sys/devices/platform/fb000000.gpu/devfreq/fb000000.gpu/max_freq

（4）查看GPU当前频率，确认是否设置成功 ：
cat /sys/devices/platform/fb000000.gpu/devfreq/fb000000.gpu/cur_freq
GPU最小最大频率
cat /sys/devices/platform/fb000000.gpu/devfreq/fb000000.gpu/min_freq
cat /sys/devices/platform/fb000000.gpu/devfreq/fb000000.gpu/max_freq
查看当前GPU调频模式
cat /sys/devices/platform/fb000000.gpu/devfreq/fb000000.gpu/governor

（5）
查看GPU负载
cat /sys/devices/platform/fb000000.gpu/devfreq/fb000000.gpu/load
查看GPU利用率
cat /sys/devices/platform/fb000000.gpu/utilisation
```
