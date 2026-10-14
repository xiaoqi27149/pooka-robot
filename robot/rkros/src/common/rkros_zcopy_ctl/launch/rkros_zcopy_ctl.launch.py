from launch import LaunchDescription
from launch.conditions import IfCondition, UnlessCondition
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare
from launch.actions import SetEnvironmentVariable, ExecuteProcess

def generate_launch_description():
    return LaunchDescription([
        SetEnvironmentVariable(
            name='RMW_IMPLEMENTATION',
            value='rmw_fastrtps_cpp'),

        SetEnvironmentVariable(
            name='FASTRTPS_DEFAULT_PROFILES_FILE',
            value=PathJoinSubstitution([
                FindPackageShare('rkros_zcopy_ctl'),
                'config',
                'fastdds_zcopy.xml'])),

        SetEnvironmentVariable(
            name='RMW_FASTRTPS_USE_QOS_FROM_XML',
            value='1'),

        SetEnvironmentVariable(
            name='ROS_DISABLE_LOANED_MESSAGES',
            value='0'),

        ExecuteProcess(
            cmd=['echo', '>>> Zero-copy ENABLED <<<'],
            output='screen'),
    ])