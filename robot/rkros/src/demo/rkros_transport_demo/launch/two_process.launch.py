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

    publisher_node = Node(
        package='rkros_transport_demo',
        executable='publisher_node',
        name='publisher_node',
        output='screen',
        parameters=[{'msg_type': LaunchConfiguration('msg_type')}],
    )

    subscriber_node = Node(
        package='rkros_transport_demo',
        executable='subscriber_node',
        name='subscriber_node',
        output='screen',
        parameters=[{'msg_type': LaunchConfiguration('msg_type')}],
    )

    return LaunchDescription([
        declare_msg_type,
        publisher_node,
        subscriber_node,
    ])