from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    """Launch web-gui, socket-server, and socket-client."""
   # Define your three nodes
    node1 = Node(
        package='vtr_gui',
        namespace='vtr',
        executable='web_server',
        name='web_server',
        output='screen'
    )
    
    node2 = Node(
        package='vtr_gui',
        namespace='vtr',
        executable='socket_server',
        name='socket_server',
        output='screen'
    )
    
    node3 = Node(
        package='vtr_gui',
        namespace='vtr',
        executable='socket_client',
        name='socket_client',
        output='screen'
    )
    
    # Return the launch description with all three nodes
    return LaunchDescription([
        node1,
        node2,
        node3
    ])
