# Visual Teach & Repeat 
This is my implementation of the vtr3 framework developed by ASRL. This branch is specifically meant to not include the ability for deep learnt features using torch.
## Installing the framework:
clone this repository with recursive modules enabled:
```
git clone --recurse-submodules git@github.com:adamspeedy/vtr3.git
```
To build the ros2 packages navigate to the 'main' directory and build all of the packages using the below command:
```
colcon build  --symlink-install --executor sequential
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
export VTRROOT= ~/vtr3    # or wherever you have saved it
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
docker build -t vtr3\
  --build-arg USERID=$(id -u) \
  --build-arg GROUPID=$(id -g) \
  --build-arg USERNAME=$(whoami) \
  --build-arg HOMEDIR=${HOME}  .

docker run -it --name vtr3_container \
  --privileged \
  --network=host \
  --ipc=host \
  --runtime=nvidia \
  -e DISPLAY=$DISPLAY \
  -e NVIDIA_DRIVER_CAPABILITIES=all \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  -v /home/orin/code/vtr3_speedy_docker:/home/vtr:rw \
  -v /dev:/dev \
  -v /home/orin/code/zed_speedy_docker:/home/zed:rw \
  -v /usr/local/zed/resources/:/usr/local/zed/resources/ \
  -v /usr/local/zed/settings/:/usr/local/zed/settings/ \
  -v /etc/systemd/system/zed_x_daemon.service:/etc/systemd/system/zed_x_daemon.service \
  -v /var/nvidia/nvcam/settings/:/var/nvidia/nvcam/settings/ \
  vtr3_image
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

or through an ssh tunnel you can view it at:

http://192.168.131.18:5200/index.html

or through an ssh tunnel you can view it at:

http://192.168.131.18:5200/index.html

### Launch the navigator
We can run this using some bag files on our computer using the command below:
```
ros2 launch vtr_navigation vtr.launch.py base_params:=config.yaml data_dir:=${VTRTEMP}/vision start_new_graph:=true use_sim_time:=true path_planning.type:=stationary
```

Or if we really back ourselves we can use the below command, that would be done on the robot itself:
```
ros2 launch vtr_navigation vtr.launch.py base_params:=config.yaml start_new_graph:=false use_sim_time:=false planner:="cbit" 
```

### Viewing in RVIZ2
You can launch the rviz2 viewer using the default config file using the following commond:
```
rviz2 -d viewer.rviz --ros-args -r __ns:=/vtr -r /tf:=/vtr/tf -r /tf_static:=/vtr/tf_static
```
### ROS2 middleware
An important part of this framework is the use of the CycloneDDS as the ros2 middleware. The main setup can be done using stereo labs suggested parameters: https://www.stereolabs.com/docs/ros2/dds_and_network_tuning

### Useful command for running ZED camera:
This repo tries to make an attempt at launching the zed camera from within the docker container, this is not currently working, but to build the zed wrapper the following code must be used for the wrapper repo:
The current wrapper is taken from: https://github.com/stereolabs/zed-ros2-wrapper/blob/humble-v4.2.5/docker/Dockerfile.l4t-humble

```
sudo apt-get install python-pip
sudo pip install -U rosdep
sudo rosdep init
rosdep update
rosdep install --from-paths src --ignore-src -r -y # install dependencies
colcon build --symlink-install --cmake-args=-DCMAKE_BUILD_TYPE=Release
```

```
RMW_IMPLEMENTATION=rmw_cyclonedds_cpp ros2 launch zed_wrapper zed_camera.launch.py camera_model:=zedx pos_tracking.enable:=true publish_odom:=true publish_odom_tf:=true
```