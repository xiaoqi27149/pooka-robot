from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        # Declare launch arguments with default values
        DeclareLaunchArgument(
            'width',
            default_value='1920',
            description='Input video width'
        ),
        DeclareLaunchArgument(
            'height',
            default_value='1080',
            description='Input video height'
        ),
        DeclareLaunchArgument(
            'disp_width',
            default_value='1280',
            description='Display/output width'
        ),
        DeclareLaunchArgument(
            'disp_height',
            default_value='720',
            description='Display/output height'
        ),
        DeclareLaunchArgument(
            'channel_id',
            default_value='0',
            description='Camera channel ID'
        ),
        DeclareLaunchArgument(
            'encode_type',
            default_value='h264',
            description='Encoding type (h264, h265, or mjpeg)'
        ),
        DeclareLaunchArgument(
            'frame_count',
            default_value='-1',
            description='Number of frames to process (-1 for infinite)'
        ),
        DeclareLaunchArgument(
            'enable_publish_encoder',
            default_value='true',
            description='Enable/disable publishing of encoded stream to ROS2 topic'
        ),

        # Define the video_rtsp node
        Node(
            package='rkros_rtsp_demo',
            executable='video_rtsp',
            name='video_rtsp',
            output='screen',
            parameters=[{
                'width': LaunchConfiguration('width'),
                'height': LaunchConfiguration('height'),
                'disp_width': LaunchConfiguration('disp_width'),
                'disp_height': LaunchConfiguration('disp_height'),
                'channel_id': LaunchConfiguration('channel_id'),
                'encode_type': LaunchConfiguration('encode_type'),
                'frame_count': LaunchConfiguration('frame_count'),
                'enable_publish_encoder': LaunchConfiguration('enable_publish_encoder'),
            }],
        )
    ])