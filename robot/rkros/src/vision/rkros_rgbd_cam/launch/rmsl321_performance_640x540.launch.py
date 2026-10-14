import os
import yaml
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, OpaqueFunction, GroupAction
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import PushRosNamespace, ComposableNodeContainer, Node
from launch_ros.descriptions import ComposableNode


def load_yaml(file_path):
    with open(file_path, 'r') as f:
        return yaml.safe_load(f)

def merge_params(default_params, yaml_params):
    for key, value in yaml_params.items():
        if key in default_params:
            default_params[key] = value
    return default_params

def convert_value(value):
    if isinstance(value, str):
        try:
            return int(value)
        except ValueError:
            pass
        try:
            return float(value)
        except ValueError:
            pass
        if value.lower() == 'true':
            return True
        elif value.lower() == 'false':
            return False
    return value

def load_parameters(context, args):
    default_params = {arg.name: LaunchConfiguration(arg.name).perform(context) for arg in args}
    config_file_path = LaunchConfiguration('config_file_path').perform(context)
    if config_file_path:
        yaml_params = load_yaml(config_file_path)
        default_params = merge_params(default_params, yaml_params)
    skip_convert = {'config_file_path', 'usb_port', 'serial_number'}
    return {
        key: (value if key in skip_convert else convert_value(value))
        for key, value in default_params.items()
    }

def generate_launch_description():
    args = [
        DeclareLaunchArgument('unused_component', default_value='true'),
        DeclareLaunchArgument('name_space', default_value='', description='Namespace for the camera nodes, such as camera.'),

        DeclareLaunchArgument('enable_muliti_camera', default_value='false'),
        DeclareLaunchArgument('serial_number', default_value=''),
        DeclareLaunchArgument('usb_port', default_value=''),
        DeclareLaunchArgument('device_num', default_value='1'),

        DeclareLaunchArgument('point_cloud_qos', default_value='default'),
        DeclareLaunchArgument('config_file_path', default_value=''),

        DeclareLaunchArgument('cam_name', default_value='rmsl321_cam'),
        DeclareLaunchArgument('rgb_width', default_value='640'),
        DeclareLaunchArgument('rgb_height', default_value='540'),
        DeclareLaunchArgument('ir_width', default_value='640'),
        DeclareLaunchArgument('ir_height', default_value='352'),

        DeclareLaunchArgument('rgb_fps', default_value='15'),
        DeclareLaunchArgument('publish_rate', default_value='15.0'),

        DeclareLaunchArgument('enable_color', default_value='true'),
        DeclareLaunchArgument('enable_depth', default_value='true'),
        DeclareLaunchArgument('enable_pointcloud', default_value='false'),
        DeclareLaunchArgument('enable_depth_pseudo', default_value='false'),
        DeclareLaunchArgument('enable_depth_aligned', default_value='true'),
        DeclareLaunchArgument('enable_pointcloud_aligned', default_value='false'),
        DeclareLaunchArgument('enable_pointcloud_rgbd', default_value='false'),

    ]

    def get_params(context, args):
        return [load_parameters(context, args)]

    def create_node_action(context, args):
        params = get_params(context, args)
        # ros_distro = os.environ.get("ROS_DISTRO", "humble")
        if unused_component := LaunchConfiguration("unused_component").perform(context) == 'true':
            return [
                Node(
                    package="rkros_rgbd_cam",
                    executable="rmsl32x_node",
                    name="rk_rmsl321_node",
                    namespace=LaunchConfiguration("name_space"),
                    parameters=params,
                    output="screen",
                )
            ]
        else:
            return [
                GroupAction([
                    PushRosNamespace(LaunchConfiguration("name_space")),
                    ComposableNodeContainer(
                        name="camera_container",
                        namespace="",
                        package="rclcpp_components",
                        executable="component_container",
                        composable_node_descriptions=[
                            ComposableNode(
                                package='rkros_rgbd_cam',
                                plugin='RMSLNode',
                                name=LaunchConfiguration("cam_name"),
                                parameters=params,
                            ),
                        ],
                        output="screen",
                    )
                ])
            ]

    return LaunchDescription(
        args + [
            OpaqueFunction(function=lambda context: create_node_action(context, args))
        ]
    )
