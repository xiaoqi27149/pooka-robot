# launch/container_launch.py
from launch import LaunchDescription
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
    declare_msg_type = DeclareLaunchArgument(
        'msg_type',
        default_value='dmabuf',
        description='Message type for publisher and subscriber'
    )

    # 一个进程内的容器
    container = ComposableNodeContainer(
        name='rkros_transport_container',
        namespace='',
        package='rclcpp_components',
        executable='component_container',   # 官方容器可执行文件
        composable_node_descriptions=[
            # Publisher 插件
            ComposableNode(
                package='rkros_transport_demo',
                plugin='rkros_transport_demo::PublisherComponent',
                name='rkros_transport_pub',
                extra_arguments=[{'use_intra_process_comms': True}],
                parameters=[{'msg_type': LaunchConfiguration('msg_type')}],
            ),
            # Subscriber 插件
            ComposableNode(
                package='rkros_transport_demo',
                plugin='rkros_transport_demo::SubscriberComponent',
                name='rkros_transport_sub',
                extra_arguments=[{'use_intra_process_comms': True}],
                parameters=[{'msg_type': LaunchConfiguration('msg_type')}],
            ),
        ],
        output='screen',
    )

    return LaunchDescription([
        declare_msg_type,
        container,
    ])