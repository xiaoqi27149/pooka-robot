# launch/rmsl32x_component.launch.py
from launch import LaunchDescription
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
    # 声明启动参数
    declare_cam_name = DeclareLaunchArgument(
        'cam_name',
        default_value='rmsl321_cam',
        description='Camera name for topic namespace'
    )

    declare_publish_rate = DeclareLaunchArgument(
        'publish_rate',
        default_value='15.0',
        description='Publishing rate in Hz'
    )

    declare_rgb_width = DeclareLaunchArgument(
        'rgb_width',
        default_value='640',
        description='RGB image width'
    )

    declare_rgb_height = DeclareLaunchArgument(
        'rgb_height',
        default_value='540',
        description='RGB image height'
    )

    declare_enable_depth = DeclareLaunchArgument(
        'enable_depth',
        default_value='true',
        description='Enable depth image publishing'
    )

    declare_enable_pointcloud = DeclareLaunchArgument(
        'enable_pointcloud',
        default_value='false',
        description='Enable pointcloud publishing'
    )

    # 组件容器
    container = ComposableNodeContainer(
        name='rmsl32x_container',
        namespace='',
        package='rclcpp_components',
        executable='component_container',
        composable_node_descriptions=[
            ComposableNode(
                package='rkros_rgbd_cam',
                plugin='RMSLNode',
                name='rmsl32x_node',
                parameters=[{
                    'cam_name': LaunchConfiguration('cam_name'),
                    'publish_rate': LaunchConfiguration('publish_rate'),
                    'rgb_width': LaunchConfiguration('rgb_width'),
                    'rgb_height': LaunchConfiguration('rgb_height'),
                    'enable_depth': LaunchConfiguration('enable_depth'),
                    'enable_pointcloud': LaunchConfiguration('enable_pointcloud'),
                }]
            )
        ],
        output='screen'
    )

    return LaunchDescription([
        declare_cam_name,
        declare_publish_rate,
        declare_rgb_width,
        declare_rgb_height,
        declare_enable_depth,
        declare_enable_pointcloud,
        container
    ])