from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='rkros_mipi_cam',
            executable='rkros_mipi_node',
            name='rk_mipi_camera',
            parameters=[{
                "dev_id": 0,
                "pipe_id": 0,
                "channel_id": 1,
                # 基本参数
                "width": 1280,
                "height": 960,
                "loop_count": 0,

                # 像素格式和缓冲区
                "pixel_format": 0,  # RK_FMT_YUV420SP
                "buffer_count": 5,
                # "memory_type": 1,   # VI_V4L2_MEMORY_TYPE_DMABUF
                # "capture_type": 1,  # VI_V4L2_CAPTURE_TYPE_VIDEO_CAPTURE

                # 区域设置
                "enable_region": False,
                "region_count": 1,

                "publish_rgb": True,
                "enable_rkros_zcopy": False,

            }]
        )
    ])