from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    param_file =  '/home/adam/Desktop/CurrentBranch/src/config/config.yaml'
    return LaunchDescription([
        Node(
            package='vtr_navigation',
            namespace='a200_0656/vtr',
            executable='vtr_navigation',
            output='screen',
            parameters=[param_file],
            remappings=[('/tf','/a200_0656/tf'),('/tf_static','/a200_0656/tf_static')]
        ),
    ])