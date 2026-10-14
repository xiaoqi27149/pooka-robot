from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, ExecuteProcess
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution, LaunchConfiguration
from launch_ros.substitutions import FindPackageShare
from launch.conditions import IfCondition
from launch.actions import DeclareLaunchArgument

def generate_launch_description():
    zero_copy = LaunchConfiguration('zero_copy')

    return LaunchDescription([
        DeclareLaunchArgument(
            'zero_copy',
            default_value='True',
            description='Enable inter-process zero-copy via shared memory'),

        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                PathJoinSubstitution([
                    FindPackageShare('rkros_zcopy_ctl'),
                    'launch',
                    'rkros_zcopy_ctl.launch.py'])),
            condition=IfCondition(zero_copy),
        ),
        ExecuteProcess(
            cmd=['bash', '-c', ' \
                    echo "RMW_IMPLEMENTATION=${RMW_IMPLEMENTATION}" && \
                    echo "FASTRTPS_DEFAULT_PROFILES_FILE=${FASTRTPS_DEFAULT_PROFILES_FILE}" && \
                    echo "RMW_FASTRTPS_USE_QOS_FROM_XML=${RMW_FASTRTPS_USE_QOS_FROM_XML}" && \
                    echo "ROS_DISABLE_LOANED_MESSAGES=${ROS_DISABLE_LOANED_MESSAGES}"'],
            output='screen')
    ])