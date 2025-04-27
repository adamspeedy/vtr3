# Visual Teach & Repeat 
This is my implementation of the vtr3 framework developed by ASRL.

## Running the framework
### Launch the webserver:
* ros2 run vtr_gui socket_server  --ros-args -r __ns:=/a200_0656/vtr
* ros2 run vtr_gui socket_client  --ros-args -r __ns:=/a200_0656/vtr
* ros2 run vtr_gui web_server  --ros-args -r __ns:=/a200_0656/vtr

### Launch the navigator
* ros2 launch vtr_navigation vtr.launch.py base_params:=config.yaml data_dir:=${VTRTEMP}/vision start_new_graph:=true use_sim_time:=true path_planning.type:=stationary
