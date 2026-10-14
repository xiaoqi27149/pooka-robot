from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode
import os

def generate_launch_description():
    declare_width = DeclareLaunchArgument('width', default_value='1920')
    declare_height = DeclareLaunchArgument('height', default_value='1080')
    declare_gen_fps = DeclareLaunchArgument('gen_fps', default_value='30.0')
    declare_pub_fps = DeclareLaunchArgument('pub_fps', default_value='30.0')

    container = ComposableNodeContainer(
        name='vcam_container',
        namespace='vcam',
        package='rclcpp_components',
        executable='component_container',
        composable_node_descriptions=[
            ComposableNode(
                package='rkros_vcam',
                plugin='rkros_vcam::VCamNode',
                name='vcam_node',
                parameters=[
                    {'width': LaunchConfiguration('width')},
                    {'height': LaunchConfiguration('height')},
                    {'gen_fps': LaunchConfiguration('gen_fps')},
                    {'publish_fps': LaunchConfiguration('pub_fps')},
                    {'topics': [
                        # topic:format:msgtype
                        # format: rgb yuv jpeg
                        # msgtype: sensor dmabuf
                        '/rkros/vcam/image_raw:rgb:sensor'
                    ]},
                ],
            )
        ],
        output='screen',
    )

    return LaunchDescription([
        declare_width,
        declare_height,
        declare_gen_fps,
        declare_pub_fps,
        container
    ])
