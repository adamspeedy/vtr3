# Visual Teach & Repeat 
This is my implementation of the vtr3 framework developed by ASRL.
## Installing the framework:
To build the ros2 packages navigate to the 'main' directory and build all of the packages using the below command:
```
VTR_PIPELINE=VISION colcon build  --symlink-install 
```
Since we are in a battle over our memory constraints when building the vtr_vision package, we can try to build this package incrementally using the 'build_vtr3_vision.sh' file.
```
./build_vtr3_vision.sh
```



## Running the framework
### Launch the webserver:

You can launch all of the necessary nodes,(web-server, socket-server and socket-client), using the command below:
```
ros2 launch vtr_gui web_gui.launch.py
```
You can also specify where we want to save our pose_graph bag files using the below command
```
ros2 run vtr_gui setup_server  --ros-args -r __ns:=/a200_0656/vtr
```


### Launch the navigator
We can run this using some bag files on our computer using the command below:
```
ros2 launch vtr_navigation vtr.launch.py base_params:=config.yaml data_dir:=${VTRTEMP}/vision start_new_graph:=true use_sim_time:=true path_planning.type:=stationary
```

Or if we really back ourselves we can use the below command, that would be done on the robot itself:
```
ros2 launch vtr_navigation vtr.launch.py base_params:=bumblebee_grizzly_default.yaml start_new_graph:=false use_sim_time:=false planner:="cbit" model_dir:=${VTRROOT}/models
```
