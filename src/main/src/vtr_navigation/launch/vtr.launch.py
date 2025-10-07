import os
import os.path as osp

from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch import LaunchDescription

from launch.conditions import IfCondition
from launch.substitutions import EqualsSubstitution, NotEqualsSubstitution
# from launch.conditions import LaunchConfigurationEquals, LaunchConfigurationNotEquals
from launch_ros.actions import Node


def generate_launch_description():
    # config directory
    config_dir = osp.join(os.getenv('VTRSRC'), 'config')
    # temp directory
    temp_dir = os.getenv('VTRTEMP')

    commonNodeArgs = {
        "package": 'vtr_navigation',
        "namespace": 'vtr',
        "executable": 'vtr_navigation',
        "output": 'screen',
        #"prefix": ["gdbserver localhost:3000"]
        # "prefix": 'xterm -e gdb -ex run --args',
        #"prefix": 'valgrind --tool=callgrind',
    }

    camera_tf_publisher1 = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='camera_tf_publisher',
        arguments=[
            '0.3', '0', '0', '1.57', '-3.14', '1.57', 'default_mount', 'camera'   # '0', '0', '0.25', '1.57', '-3.14', '1.57', 'default_mount', 'camera'
        ],
        remappings=[
            ('/tf_static', '/vtr/tf_static')
        ]
    )
    camera_tf_publisher2 = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='camera_tf_publisher',
        arguments=[
            '0', '0', '0.25', '0', '0', '0', 'base_link', 'default_mount'
        ],
        remappings=[
            ('/tf_static', '/vtr/tf_static')
        ]
    )
    camera_tf_publisher3 = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='camera_tf_publisher',
        arguments=[
            '0', '0', '0', '0', '0', '0', 'robot','base_link'
        ],
        remappings=[
            ('/tf_static', '/vtr/tf_static')
        ]
    )

    return LaunchDescription([
        DeclareLaunchArgument('data_dir', default_value='', description='directory to store graph, if blank use setup UI'),
        DeclareLaunchArgument('start_new_graph', default_value='false', description='whether to start a new pose graph'),
        DeclareLaunchArgument('use_sim_time', default_value='false', description='use simulated time for playback'),
        DeclareLaunchArgument('planner', default_value='cbit', description='use no planner. Publish zero'),
        DeclareLaunchArgument('base_params', default_value="config.yaml", description='base parameter file (sensor, robot specific)'),
        DeclareLaunchArgument('override_params', default_value='', description='scenario specific parameter overrides'),

        # Added camera transform publisher
        camera_tf_publisher1,
        camera_tf_publisher2,
        camera_tf_publisher3,
        # Note how we choose which parameters to use based on whether we have entered a data_dir variable.
        Node(**commonNodeArgs,
            parameters=[
                PathJoinSubstitution((config_dir, LaunchConfiguration("base_params"))),
              {
                    "data_dir": LaunchConfiguration("data_dir"),
                    "start_new_graph": LaunchConfiguration("start_new_graph"),
                    "use_sim_time": LaunchConfiguration("use_sim_time"),
                    "path_planning.type": LaunchConfiguration("planner"),
              },
                PathJoinSubstitution((config_dir, LaunchConfiguration("override_params")))
            ],
            remappings=[('/tf','/vtr/tf'),('/tf_static','/vtr/tf_static'), ('/vtr/command', '/a300_00047/joy_teleop/cmd_vel')], #, ('/vtr/command', '/a200_0656/joy_teleop/cmd_vel')
            condition=IfCondition(NotEqualsSubstitution(LaunchConfiguration('data_dir'), ''))
            # condition=LaunchConfigurationNotEquals('data_dir', '')
        ),
        Node(**commonNodeArgs,
            parameters=[
                PathJoinSubstitution((config_dir, LaunchConfiguration("base_params"))),
                PathJoinSubstitution((temp_dir, "setup_params.yaml")),
                {
                    "start_new_graph": LaunchConfiguration("start_new_graph"),
                    "use_sim_time": LaunchConfiguration("use_sim_time"),
                    "path_planning.type": LaunchConfiguration("planner"),

                },
                PathJoinSubstitution((config_dir, LaunchConfiguration("override_params")))
            ],
            remappings=[('/tf','/vtr/tf'),('/tf_static','/vtr/tf_static'), ('/vtr/command', '/a300_00047/joy_teleop/cmd_vel')], #, ('/vtr/command', '/a200_0656/joy_teleop/cmd_vel')
            # condition=LaunchConfigurationEquals('data_dir', '')
            condition=IfCondition(EqualsSubstitution(LaunchConfiguration('data_dir'), ''))
        ),
    ])
