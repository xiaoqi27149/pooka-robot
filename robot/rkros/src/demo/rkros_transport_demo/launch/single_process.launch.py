from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
    declare_msg_type = DeclareLaunchArgument(
        'msg_type',
        default_value='dmabuf',
        description='Message type for publisher and subscriber'
    )

    return LaunchDescription([
        declare_msg_type,
        Node(
            package='rkros_transport_demo',
            executable='single_process_launch',
            output='screen',
            parameters=[{'msg_type': LaunchConfiguration('msg_type')}],
        )
    ])