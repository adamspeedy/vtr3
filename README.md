# Visual Teach & Repeat 
This is my implementation of the vtr3 framework developed by ASRL.
## Installing the framework:
To build the ros2 packages navigate to the 'main' directory and build all of the packages using the below command:
```
VTR_PIPELINE=VISION colcon build  --symlink-install --executor sequential
```
It is important to use symlink install otherwise we will need to manually set some directories, especially for our web application in the vtr_gui package.

Since we are in a battle over our memory constraints when building the vtr_vision package, we can try to build this package incrementally using the 'build_vtr3_vision.sh' file.
```
./build_vtr3_vision.sh
```
### Build the web application 
Using npm, navigate to the directory with our package.json in the vtr_gui package and run the following commands:
```
npm install
npm run build
```
### Some useful code to add to your bashrc file:
```
export VTRROOT= ~/CurrentBranch    # or wherever you have saved it
export VTRSRC=${VTRROOT}/src       # source code (this repo)
export VTRTEMP=${VTRROOT}/temp     # default output directory
export VTRMODELS=${VTRROOT}/models # .pt models for TorchScript  
export VTRUI=${VTRSRC}/main/src/vtr_gui/vtr_gui/vtr-gui
```

We need to also create some directories to save data to:
```
cd ${VTRROOT}
mkdir temp log Debug models
```
## Building with Docker:
The main bits of code you will need are:
```
cd ${VTRSRC}
docker build -t vtr3 --build-arg HOMEDIR=${HOME} .
docker run -it --name vtr3 \
  --privileged \
  --network=host \
  --ipc=host \
  --runtime=nvidia \
  -e DISPLAY=$DISPLAY \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  -v ${VTRROOT}:${VTRROOT}:rw \
  -v /dev:/dev \
  vtr3
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
You can then view this web server in your browser at:

http://localhost:5200/index.html

or through an ssh tunnel you can view it at:

http://192.168.131.18:5200/index.html


### Launch the navigator
We can run this using some bag files on our computer using the command below:
```
ros2 launch vtr_navigation vtr.launch.py base_params:=config.yaml data_dir:=${VTRTEMP}/vision start_new_graph:=true use_sim_time:=true path_planning.type:=stationary
```

Or if we really back ourselves we can use the below command, that would be done on the robot itself:
```
ros2 launch vtr_navigation vtr.launch.py base_params:=config.yaml start_new_graph:=false use_sim_time:=false planner:="cbit" model_dir:=${VTRROOT}/models
```

Another important part to note is the transform for our camera frame, this is manually run using:
```
ros2 run tf2_ros static_transform_publisher 0 0 0.25 1.57 -3.14 1.57 default_mount camera --ros-args -r /tf_static:=/a200_0656/tf_static 
```


