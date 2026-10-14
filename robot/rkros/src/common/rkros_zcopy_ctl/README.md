# 开启零拷贝环境

## 功能介绍

`rkros_zcopy_ctl`用于开启ROS2节点的零拷贝功能

## 使用方法

### 通过python launch脚本启动（推荐）

当你的节点是通过launch脚本启动时候，可以在脚本中添加如下代码来启用零拷贝功能。

```python
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    return LaunchDescription([
        # 你的其它代码
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                PathJoinSubstitution([
                    FindPackageShare('rkros_zcopy_ctl'),
                    'launch',
                    'rkros_zcopy_ctl.launch.py']))
        ),
        # 你的其它代码
    ])
```

`rkros_zcopy_ctl`包内提供了测试脚本`test_zero_copy_python.launch.py`，可用于测试零拷贝功能的开启：
```bash
$ ros2 launch rkros_zcopy_ctl test_zero_copy_python.launch.py
[INFO] [echo-1]: process started with pid [550216]
[INFO] [bash-2]: process started with pid [550218]
[echo-1] >>> Zero-copy ENABLED <<<
[INFO] [echo-1]: process has finished cleanly [pid 550216]
[bash-2] RMW_IMPLEMENTATION=rmw_fastrtps_cpp
[bash-2] FASTRTPS_DEFAULT_PROFILES_FILE=/data/rkros_ws/install/rkros_zcopy_ctl/share/rkros_zcopy_ctl/config/fastdds_zcopy.xml
[bash-2] RMW_FASTRTPS_USE_QOS_FROM_XML=1
[bash-2] ROS_DISABLE_LOANED_MESSAGES=0
```

### 通过节点直接启动

当你的节点不能通过launch脚本启动，而是通过`ros2 run`等命令直接启动，此时需要修改你的代码，在main函数中添加如下代码来启用零拷贝功能。
```cpp
#include "rkros_zcopy_ctl/rkros_zcopy_ctl.hpp"

int main() {
    rkros_zcopy_ctl::enable();
    // 你的其它代码
}
```

`rkros_zcopy_ctl`包内提供了测试程序`test_zero_copy_cpp`，可用于测试零拷贝功能的开启：
```bash
$ ros2 run rkros_zcopy_ctl test_zero_copy_cpp
>>> Zero-copy ENABLED <<<
RMW_IMPLEMENTATION=rmw_fastrtps_cpp
FASTRTPS_DEFAULT_PROFILES_FILE=/data/rkros_ws/install/rkros_zcopy_ctl/share/rkros_zcopy_ctl/config/fastdds_zcopy.xml
RMW_FASTRTPS_USE_QOS_FROM_XML=1
ROS_DISABLE_LOANED_MESSAGES=0
```
