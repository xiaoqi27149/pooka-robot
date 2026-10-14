# Transport Demo

用于测试进程内(Intra-Process)/进程间(Inter-Process)的消息传递耗时情况

## 进程内(intra-process)消息传递

### 未开启进程内零拷贝

在进程中直接添加订阅节点和发布节点，使它们跑在同一进程内，但不开启进程内零拷贝

执行命令：

```
ros2 run rkros_transport_demo single_process_launch no_intra
```

输出日志：
```
$ ros2 run rkros_transport_demo single_process_launch no_intra
[INFO] [1756449541.194001927] [rkros_transport_pub]: normal message: 0
[INFO] [1756449541.199268021] [rkros_transport_sub]: msg: 0, cost time: 5079 us
[INFO] [1756449541.213982457] [rkros_transport_pub]: normal message: 1
[INFO] [1756449541.217011153] [rkros_transport_sub]: msg: 1, cost time: 2951 us
[INFO] [1756449541.233979903] [rkros_transport_pub]: normal message: 2
[INFO] [1756449541.237058767] [rkros_transport_sub]: msg: 2, cost time: 3036 us
[INFO] [1756449541.253963350] [rkros_transport_pub]: normal message: 3
[INFO] [1756449541.257051255] [rkros_transport_sub]: msg: 3, cost time: 3041 us
[INFO] [1756449541.273962838] [rkros_transport_pub]: normal message: 4
[INFO] [1756449541.275370727] [rkros_transport_sub]: msg: 4, cost time: 1367 us
[INFO] [1756449541.293962910] [rkros_transport_pub]: normal message: 5
[INFO] [1756449541.295346007] [rkros_transport_sub]: msg: 5, cost time: 1345 us
[INFO] [1756449541.313962690] [rkros_transport_pub]: normal message: 6
[INFO] [1756449541.315337328] [rkros_transport_sub]: msg: 6, cost time: 1337 us
[INFO] [1756449541.333961886] [rkros_transport_pub]: normal message: 7
[INFO] [1756449541.335331275] [rkros_transport_sub]: msg: 7, cost time: 1331 us
[INFO] [1756449541.353963999] [rkros_transport_pub]: normal message: 8
[INFO] [1756449541.355365471] [rkros_transport_sub]: msg: 8, cost time: 1361 us
[INFO] [1756449541.373962904] [rkros_transport_pub]: normal message: 9
[INFO] [1756449541.375346876] [rkros_transport_sub]: msg: 9, cost time: 1338 us
[INFO] [1756449541.393971143] [rkros_transport_pub]: normal message: 10
[INFO] [1756449541.395363573] [rkros_transport_sub]: msg: 10, cost time: 1351 us
```

可以看到消息传递的耗时达到了毫秒级

### 使用组件（Component）机制 + 容器加载

把发布节点和订阅节点跑在同一个容器内，并开启零拷贝

优点：可以在运行时候动态组合节点

执行命令：

```
ros2 launch rkros_transport_demo container.launch.py
```

在`container.launch.py`脚本中实现了在一个容器中加载两个节点，运行后输出如下：
```
$ ros2 launch rkros_transport_demo container.launch.py
[INFO] [launch]: All log files can be found below /home/linaro/.ros/log/2025-08-29-06-37-55-896565-linaro-alip-5505
[INFO] [launch]: Default logging verbosity is set to INFO
[INFO] [component_container-1]: process started with pid [5517]
[component_container-1] [INFO] [1756449476.248985095] [rkros_transport_container]: Load Library: /home/linaro/rkros_ws/install/lib/libpublisher_component.so
[component_container-1] [INFO] [1756449476.250196690] [rkros_transport_container]: Found class: rclcpp_components::NodeFactoryTemplate<rkros_transport_demo::PublisherComponent>
[component_container-1] [INFO] [1756449476.250249191] [rkros_transport_container]: Instantiate class: rclcpp_components::NodeFactoryTemplate<rkros_transport_demo::PublisherComponent>
[INFO] [launch_ros.actions.load_composable_nodes]: Loaded node '/rkros_transport_pub' in container '/rkros_transport_container'
[component_container-1] [INFO] [1756449476.411355755] [rkros_transport_container]: Load Library: /home/linaro/rkros_ws/install/lib/libsubscriber_component.so
[component_container-1] [INFO] [1756449476.413035188] [rkros_transport_container]: Found class: rclcpp_components::NodeFactoryTemplate<rkros_transport_demo::SubscriberComponent>
[component_container-1] [INFO] [1756449476.413094105] [rkros_transport_container]: Instantiate class: rclcpp_components::NodeFactoryTemplate<rkros_transport_demo::SubscriberComponent>
[INFO] [launch_ros.actions.load_composable_nodes]: Loaded node '/rkros_transport_sub' in container '/rkros_transport_container'
[component_container-1] [INFO] [1756449476.430403231] [rkros_transport_pub]: intra message: 0
[component_container-1] [INFO] [1756449476.431146697] [rkros_transport_sub]: msg: 0, cost time: 621 us
[component_container-1] [INFO] [1756449476.450326591] [rkros_transport_pub]: intra message: 1
[component_container-1] [INFO] [1756449476.450628469] [rkros_transport_sub]: msg: 1, cost time: 231 us
[component_container-1] [INFO] [1756449476.468708770] [rkros_transport_pub]: intra message: 2
[component_container-1] [INFO] [1756449476.468948231] [rkros_transport_sub]: msg: 2, cost time: 193 us
[component_container-1] [INFO] [1756449476.488707964] [rkros_transport_pub]: intra message: 3
[component_container-1] [INFO] [1756449476.488940133] [rkros_transport_sub]: msg: 3, cost time: 187 us
[component_container-1] [INFO] [1756449476.508695200] [rkros_transport_pub]: intra message: 4
[component_container-1] [INFO] [1756449476.508915993] [rkros_transport_sub]: msg: 4, cost time: 179 us
[component_container-1] [INFO] [1756449476.528711311] [rkros_transport_pub]: intra message: 5
[component_container-1] [INFO] [1756449476.528930938] [rkros_transport_sub]: msg: 5, cost time: 178 us
[component_container-1] [INFO] [1756449476.548710505] [rkros_transport_pub]: intra message: 6
[component_container-1] [INFO] [1756449476.548923715] [rkros_transport_sub]: msg: 6, cost time: 173 us
[component_container-1] [INFO] [1756449476.568702991] [rkros_transport_pub]: intra message: 7
[component_container-1] [INFO] [1756449476.568917951] [rkros_transport_sub]: msg: 7, cost time: 175 us
[component_container-1] [INFO] [1756449476.588701018] [rkros_transport_pub]: intra message: 8
[component_container-1] [INFO] [1756449476.588910437] [rkros_transport_sub]: msg: 8, cost time: 169 us
[component_container-1] [INFO] [1756449476.608692629] [rkros_transport_pub]: intra message: 9
[component_container-1] [INFO] [1756449476.608907881] [rkros_transport_sub]: msg: 9, cost time: 172 us
[component_container-1] [INFO] [1756449476.628711073] [rkros_transport_pub]: intra message: 10
[component_container-1] [INFO] [1756449476.628922242] [rkros_transport_sub]: msg: 10, cost time: 170 us
```

消息的传递时间在百微秒级别。

### 添加多个节点到 **Executor** 中运行

在代码中直接添加订阅节点和发布节点，使它们跑在同一进程内，并开启进程内零拷贝

执行命令：

```
ros2 run rkros_transport_demo single_process_launch
```

输出日志：
```
$ ros2 run rkros_transport_demo single_process_launch
[INFO] [1756449728.278651022] [rkros_transport_pub]: intra message: 0
[INFO] [1756449728.279161736] [rkros_transport_sub]: msg: 0, cost time: 308 us
[INFO] [1756449728.298564200] [rkros_transport_pub]: intra message: 1
[INFO] [1756449728.298747077] [rkros_transport_sub]: msg: 1, cost time: 116 us
[INFO] [1756449728.318538046] [rkros_transport_pub]: intra message: 2
[INFO] [1756449728.318696423] [rkros_transport_sub]: msg: 2, cost time: 102 us
[INFO] [1756449728.336945041] [rkros_transport_pub]: intra message: 3
[INFO] [1756449728.337086209] [rkros_transport_sub]: msg: 3, cost time: 98 us
[INFO] [1756449728.358541155] [rkros_transport_pub]: intra message: 4
[INFO] [1756449728.358691948] [rkros_transport_sub]: msg: 4, cost time: 98 us
[INFO] [1756449728.376944066] [rkros_transport_pub]: intra message: 5
[INFO] [1756449728.377081443] [rkros_transport_sub]: msg: 5, cost time: 97 us
[INFO] [1756449728.396933079] [rkros_transport_pub]: intra message: 6
[INFO] [1756449728.397058205] [rkros_transport_sub]: msg: 6, cost time: 84 us
[INFO] [1756449728.416938133] [rkros_transport_pub]: intra message: 7
[INFO] [1756449728.417065302] [rkros_transport_sub]: msg: 7, cost time: 90 us
[INFO] [1756449728.436936479] [rkros_transport_pub]: intra message: 8
[INFO] [1756449728.437058981] [rkros_transport_sub]: msg: 8, cost time: 83 us
[INFO] [1756449728.456925200] [rkros_transport_pub]: intra message: 9
[INFO] [1756449728.457043327] [rkros_transport_sub]: msg: 9, cost time: 79 us
[INFO] [1756449728.476928505] [rkros_transport_pub]: intra message: 10
[INFO] [1756449728.477048089] [rkros_transport_sub]: msg: 10, cost time: 80 us
```

消息的传递时间在百微秒级别。

## 进程间(inter-process)消息传递

### 多个节点跑在不同进程，且未开启零拷贝

我们先来看下不使用零拷贝的情况，也就是把publisher和subscriber分别跑在不同的进程当中，且未开启零拷贝

执行命令：

```
ros2 launch rkros_transport_demo two_process.launch.py
```

输出日志：
```
$ ros2 launch rkros_transport_demo two_process.launch.py
[INFO] [launch]: All log files can be found below /home/linaro/.ros/log/2025-08-29-06-44-36-584118-linaro-alip-5587
[INFO] [launch]: Default logging verbosity is set to INFO
[INFO] [publisher_node-1]: process started with pid [5588]
[INFO] [subscriber_node-2]: process started with pid [5590]
[publisher_node-1] [INFO] [1756449876.856595363] [publisher_node]: normal message: 0
[publisher_node-1] [INFO] [1756449876.876592541] [publisher_node]: normal message: 1
[publisher_node-1] [INFO] [1756449876.896571635] [publisher_node]: normal message: 2
[subscriber_node-2] [INFO] [1756449876.901564731] [subscriber_node]: msg: 2, cost time: 4876 us
[publisher_node-1] [INFO] [1756449876.916576396] [publisher_node]: normal message: 3
[subscriber_node-2] [INFO] [1756449876.921249239] [subscriber_node]: msg: 3, cost time: 4599 us
[publisher_node-1] [INFO] [1756449876.936573574] [publisher_node]: normal message: 4
[subscriber_node-2] [INFO] [1756449876.939612774] [subscriber_node]: msg: 4, cost time: 2975 us
[publisher_node-1] [INFO] [1756449876.956573085] [publisher_node]: normal message: 5
[subscriber_node-2] [INFO] [1756449876.959635618] [subscriber_node]: msg: 5, cost time: 2993 us
[publisher_node-1] [INFO] [1756449876.976579596] [publisher_node]: normal message: 6
[subscriber_node-2] [INFO] [1756449876.979678296] [subscriber_node]: msg: 6, cost time: 3023 us
[publisher_node-1] [INFO] [1756449876.996571815] [publisher_node]: normal message: 7
[subscriber_node-2] [INFO] [1756449876.999630849] [subscriber_node]: msg: 7, cost time: 2990 us
[publisher_node-1] [INFO] [1756449877.016572493] [publisher_node]: normal message: 8
[subscriber_node-2] [INFO] [1756449877.019627151] [subscriber_node]: msg: 8, cost time: 2984 us
[publisher_node-1] [INFO] [1756449877.036572296] [publisher_node]: normal message: 9
[subscriber_node-2] [INFO] [1756449877.039622871] [subscriber_node]: msg: 9, cost time: 2983 us
[publisher_node-1] [INFO] [1756449877.056572973] [publisher_node]: normal message: 10
[subscriber_node-2] [INFO] [1756449877.059628215] [subscriber_node]: msg: 10, cost time: 2987 us
```

可以看到消息传递的耗时达到了毫秒级

### 多个节点跑在不同进程，且开启零拷贝

接下来看下开启共享内存零拷贝的情况

执行命令：

```
ros2 launch rkros_transport_demo two_process_zcopy.launch.py
```

输出日志：
```
$ ros2 launch rkros_transport_demo two_process_zcopy.launch.py
[INFO] [launch]: All log files can be found below /home/linaro/.ros/log/2025-08-29-06-46-25-083429-linaro-alip-5616
[INFO] [launch]: Default logging verbosity is set to INFO
[INFO] [echo-1]: process started with pid [5617]
[INFO] [echo-1]: process has finished cleanly [pid 5617]
[INFO] [publisher_node-2]: process started with pid [5619]
[INFO] [subscriber_node-3]: process started with pid [5621]
[echo-1] >>> Zero-copy ENABLED <<<
[publisher_node-2] [INFO] [1756449985.372209197] [publisher_node]: inter message: 0
[subscriber_node-3] [WARN] [1756449985.372677619] [subscriber_node]: Loaned messages are only safe with const ref subscription callbacks. If you are using any other kind of subscriptions, set the ROS_DISABLE_LOANED_MESSAGES environment variable to 1 (the default).
[subscriber_node-3] [INFO] [1756449985.373130582] [subscriber_node]: msg: 0, cost time: 711 us
[publisher_node-2] [INFO] [1756449985.392358334] [publisher_node]: inter message: 1
[subscriber_node-3] [INFO] [1756449985.392662546] [subscriber_node]: msg: 1, cost time: 222 us
[publisher_node-2] [INFO] [1756449985.412145217] [publisher_node]: inter message: 2
[subscriber_node-3] [INFO] [1756449985.412437762] [subscriber_node]: msg: 2, cost time: 220 us
[publisher_node-2] [INFO] [1756449985.432140935] [publisher_node]: inter message: 3
[subscriber_node-3] [INFO] [1756449985.432404022] [subscriber_node]: msg: 3, cost time: 200 us
[publisher_node-2] [INFO] [1756449985.452080653] [publisher_node]: inter message: 4
[subscriber_node-3] [INFO] [1756449985.452331489] [subscriber_node]: msg: 4, cost time: 193 us
[publisher_node-2] [INFO] [1756449985.472244082] [publisher_node]: inter message: 5
[subscriber_node-3] [INFO] [1756449985.472505710] [subscriber_node]: msg: 5, cost time: 200 us
[publisher_node-2] [INFO] [1756449985.490488031] [publisher_node]: inter message: 6
[subscriber_node-3] [INFO] [1756449985.490722242] [subscriber_node]: msg: 6, cost time: 178 us
[publisher_node-2] [INFO] [1756449985.510465958] [publisher_node]: inter message: 7
[subscriber_node-3] [INFO] [1756449985.510666627] [subscriber_node]: msg: 7, cost time: 154 us
[publisher_node-2] [INFO] [1756449985.530459635] [publisher_node]: inter message: 8
[subscriber_node-3] [INFO] [1756449985.530646303] [subscriber_node]: msg: 8, cost time: 142 us
[publisher_node-2] [INFO] [1756449985.550460895] [publisher_node]: inter message: 9
[subscriber_node-3] [INFO] [1756449985.550636480] [subscriber_node]: msg: 9, cost time: 136 us
[publisher_node-2] [INFO] [1756449985.570451071] [publisher_node]: inter message: 10
[subscriber_node-3] [INFO] [1756449985.570623448] [subscriber_node]: msg: 10, cost time: 132 us
```

消息的传递时间在百微秒级别。