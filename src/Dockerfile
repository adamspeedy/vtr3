FROM nvidia/cuda:11.8.0-devel-ubuntu22.04

CMD ["/bin/bash"]

# Args for setting up non-root users, example command to use your own user:
# docker build -t <name: vtr3> \
#   --build-arg USERID=$(id -u) \
#   --build-arg GROUPID=$(id -g) \
#   --build-arg USERNAME=$(whoami) \
#   --build-arg HOMEDIR=${HOME} \
#   --build-arg CUDA_ARCH=${nvidia-smi --query-gpu=compute_cap --format=csv,noheader} .
ARG GROUPID=0
ARG USERID=0
ARG USERNAME=root
ARG HOMEDIR=/root
ARG CUDA_ARCH="87"

RUN if [ ${GROUPID} -ne 0 ]; then addgroup --gid ${GROUPID} ${USERNAME}; fi \
 && if [ ${USERID} -ne 0 ]; then adduser --disabled-password --gecos '' --uid ${USERID} --gid ${GROUPID} ${USERNAME}; fi

# Default number of threads for make build
ARG NUMPROC=8

ENV DEBIAN_FRONTEND=noninteractive

## Switch to specified user to create directories
USER ${USERID}:${GROUPID}

ENV VTRROOT=${HOMEDIR}/vtr3
ENV VTRSRC=${VTRROOT}/src \
  VTRDATA=${VTRROOT}/data \
  VTRTEMP=${VTRROOT}/temp \
  VTRMODELS=${VTRROOT}/models \
  VTRUI=${VTRSRC}/main/src/vtr_gui/vtr_gui/vtr-gui

RUN echo "alias build_ui='npm --prefix ${VTRUI} install ${VTRUI}; npm --prefix ${VTRUI} run build'" >> ~/.bashrc
RUN echo "alias build_vtr='source /opt/ros/humble/setup.bash; cd ${VTRSRC}/main; colcon build --symlink-install --executor sequential'" >> ~/.bashrc

## Switch to root to install dependencies
USER 0:0

## Dependencies
RUN apt update && apt upgrade -q -y
RUN apt update && apt install -q -y cmake git build-essential lsb-release curl gnupg2
RUN apt update && apt install -q -y libboost-all-dev libomp-dev
RUN apt update && apt install -q -y libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
RUN apt update && apt install -q -y freeglut3-dev
RUN apt update && apt install -q -y python3 python3-distutils python3-pip
RUN apt update && apt install -q -y libeigen3-dev
RUN apt update && apt install -q -y libsqlite3-dev sqlite3
RUN apt install -q -y libc6-dbg gdb valgrind

## Dependency for navtech radar
RUN apt update && apt install -q -y apt libbotan-2-dev

## Install PROJ (8.2.0) (this is for graph_map_server in vtr_navigation)
RUN apt update && apt install -q -y cmake libsqlite3-dev sqlite3 libtiff-dev libcurl4-openssl-dev
RUN mkdir -p ${HOMEDIR}/proj && cd ${HOMEDIR}/proj \
  && git clone https://github.com/OSGeo/PROJ.git . && git checkout 8.2.0 \
  && mkdir -p ${HOMEDIR}/proj/build && cd ${HOMEDIR}/proj/build \
  && cmake .. && cmake --build . -j${NUMPROC} --target install
ENV LD_LIBRARY_PATH=/usr/local/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}

## Install ZED SDK
# arguments for ZED sdk
#ARG UBUNTU_MAJOR=22
ARG CUDA_MAJOR=11
ARG CUDA_MINOR=8
ARG ZED_SDK_MAJOR=4
ARG ZED_SDK_MINOR=2
ARG ZED_SDK_PATCH=3
ARG L4T_MAJOR=35  
#36
ARG L4T_MINOR=4
#3
# Set time zone (optional)
ENV TZ=Africa/Harare
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Install dependencies and ZED SDK
#RUN apt-get update -y && apt-get install -y --no-install-recommends lsb-release wget less zstd udev sudo python3 python3-pip libpng-dev libgomp1 
#RUN python3 -m pip install --no-cache-dir numpy opencv-python 
#ENV ZED_SDK_URL="https://stereolabs.sfo2.cdn.digitaloceanspaces.com/zedsdk/${ZED_SDK_MAJOR}.${ZED_SDK_MINOR}/ZED_SDK_Ubuntu${UBUNTU_MAJOR}_cuda${CUDA_MAJOR}.${CUDA_MINOR}_v${ZED_SDK_MAJOR}.${ZED_SDK_MINOR}.${ZED_SDK_PATCH}.zstd.run"

# Setup the ZED SDK
# RUN apt-get update -y || true && \
#   apt-get install --no-install-recommends dialog bash-completion lsb-release wget less udev sudo  build-essential cmake zstd python3 python3-pip libpng-dev libgomp1 -y && \
#   python3 -m pip install --upgrade pip; python3 -m pip install numpy opencv-python-headless && \
#   wget -q -O ZED_SDK_Linux_Ubuntu.run ${ZED_SDK_URL} && \
#   chmod +x ZED_SDK_Linux_Ubuntu.run && \
#   ./ZED_SDK_Linux_Ubuntu.run -- silent skip_tools skip_cuda && \
#   ln -sf /lib/x86_64-linux-gnu/libusb-1.0.so.0 /usr/lib/x86_64-linux-gnu/libusb-1.0.so && \
#   rm ZED_SDK_Linux_Ubuntu.run && \
#   rm -rf /var/lib/apt/lists/*
# RUN wget -q -O ZED_SDK_Linux_Ubuntu${UBUNTU_RELEASE_YEAR}.run https://download.stereolabs.com/zedsdk/${ZED_SDK_MAJOR}.${ZED_SDK_MINOR}/cu${CUDA_MAJOR}${CUDA_MINOR%.*}/ubuntu${UBUNTU_RELEASE_YEAR} 
# RUN chmod +x ZED_SDK_Linux_Ubuntu${UBUNTU_RELEASE_YEAR}.run ; ./ZED_SDK_Linux_Ubuntu${UBUNTU_RELEASE_YEAR}.run -- silent skip_tools skip_cuda 
# RUN rm ZED_SDK_Linux_Ubuntu${UBUNTU_RELEASE_YEAR}.run 
# RUN rm -rf /var/lib/apt/lists/*

# Set environment variables required by ZED SDK (optional but recommended)
# ENV LD_LIBRARY_PATH=/usr/local/zed/lib:$LD_LIBRARY_PATH
# ENV ZED_SDK_ROOT=/usr/local/zed

## Install ROS2
# UTF-8
RUN apt-get install -q -y locales 
RUN locale-gen en_US en_US.UTF-8 
RUN update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
ENV LANG=en_US.UTF-8
# Add ROS2 key and install from Debian packages
RUN curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key  -o /usr/share/keyrings/ros-archive-keyring.gpg \
  && echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(lsb_release -cs) main" | tee /etc/apt/sources.list.d/ros2.list > /dev/null \
  && apt update && apt install -q -y ros-humble-desktop

## Install VTR specific ROS2 dependencies
RUN apt update && apt install -q -y \
  ros-humble-xacro \
  ros-humble-vision-opencv \
  ros-humble-perception-pcl ros-humble-pcl-ros \
  ros-humble-rmw-cyclonedds-cpp

RUN apt install ros-humble-tf2-tools

## Install misc dependencies
RUN apt update && apt install -q -y \
  tmux \
  nodejs npm protobuf-compiler \
  libboost-all-dev libomp-dev \
  libpcl-dev \
  libcanberra-gtk-module libcanberra-gtk3-module \
  libbluetooth-dev libcwiid-dev \
  python3-colcon-common-extensions \
  virtualenv \
  texlive-latex-extra \
  clang-format

## Install python dependencies
RUN pip3 install \
  tmuxp \
  pyyaml \
  pyproj \
  scipy \
  zmq \
  flask \
  flask_socketio \
  eventlet \
  python-socketio \
  python-socketio[client] \
  websocket-client

#added by sherry

RUN apt install wget
RUN apt install nano

# Install ZED SDK 
# ENV ZED_SDK_URL="https://stereolabs.sfo2.cdn.digitaloceanspaces.com/zedsdk/${ZED_SDK_MAJOR}.${ZED_SDK_MINOR}/ZED_SDK_Tegra_L4T${L4T_MAJOR}.${L4T_MINOR}_v${ZED_SDK_MAJOR}.${ZED_SDK_MINOR}.${ZED_SDK_PATCH}.zstd.run"
# # Check that this SDK exists
# RUN echo "SDK link: $ZED_SDK_URL"
# RUN echo "# R${L4T_MAJOR} (release), REVISION: ${L4T_MINOR}" > /etc/nv_tegra_release && \
#   apt update && \
#   apt install -y --no-install-recommends zstd wget less cmake curl gnupg2 \
#   build-essential python3 python3-pip python3-dev python3-setuptools libusb-1.0-0-dev \ 
#   libgeographic-dev libdraco-dev zlib1g-dev -y && \
#   pip install protobuf && \
#   wget -q --no-check-certificate -O ZED_SDK_Linux_JP.run \
#   ${ZED_SDK_URL} && \
#   chmod +x ZED_SDK_Linux_JP.run ; ./ZED_SDK_Linux_JP.run silent skip_tools && \
#   rm -rf /usr/local/zed/resources/* && \
#   rm -rf ZED_SDK_Linux_JP.run #&& \
#  rm -rf /var/lib/apt/lists/*



## sam install opencv 4.10.0
RUN apt install -q -y libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev python3-dev python3-numpy


RUN mkdir -p ${HOMEDIR}/opencv && cd ${HOMEDIR}/opencv \
&& git clone https://github.com/opencv/opencv.git . 

RUN cd ${HOMEDIR}/opencv && git checkout 4.10.0
RUN mkdir -p ${HOMEDIR}/opencv_contrib && cd ${HOMEDIR}/opencv_contrib \
&& git clone https://github.com/opencv/opencv_contrib.git . 
RUN cd ${HOMEDIR}/opencv_contrib && git checkout 4.10.0 


RUN apt install -q -y build-essential cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev python3-dev python3-numpy
# # generate Makefiles (note that install prefix is customized to: /usr/local/opencv_cuda)

RUN mkdir -p ${HOMEDIR}/opencv/build && cd ${HOMEDIR}/opencv/build \
&& cmake -D CMAKE_BUILD_TYPE=RELEASE \
-D CMAKE_INSTALL_PREFIX=/usr/local/opencv_cuda \
-D OPENCV_EXTRA_MODULES_PATH=${HOMEDIR}/opencv_contrib/modules \
-D PYTHON_DEFAULT_EXECUTABLE=/usr/bin/python3.10 \
-DBUILD_opencv_python2=OFF \
-DBUILD_opencv_python3=ON \
-DWITH_OPENMP=ON \
-DWITH_CUDA=ON \
-D CUDA_TOOLKIT_ROOT_DIR=/usr/local/cuda-11.8 \
-DOPENCV_ENABLE_NONFREE=ON \
-D OPENCV_GENERATE_PKGCONFIG=ON \
-DWITH_TBB=ON \
-DWITH_GTK=ON \
-DWITH_OPENMP=ON \
-DWITH_FFMPEG=ON \
-DBUILD_opencv_cudacodec=OFF \
-D BUILD_EXAMPLES=OFF \
-D CUDA_ARCH_BIN=$CUDA_ARCH ..  && make -j16 && make install


ENV LD_LIBRARY_PATH=/usr/local/opencv_cuda/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}

RUN mkdir -p ${HOMEDIR}/.matplotcpp && cd ${HOMEDIR}/.matplotcpp \
  && git clone https://github.com/lava/matplotlib-cpp.git . \
  && mkdir -p ${HOMEDIR}/.matplotcpp/build && cd ${HOMEDIR}/.matplotcpp/build \
  && cmake .. && cmake --build . -j${NUMPROC} --target install
  
##Install LibTorch
RUN curl https://download.pytorch.org/libtorch/cu118/libtorch-cxx11-abi-shared-with-deps-2.0.0%2Bcu118.zip --output libtorch.zip
RUN unzip libtorch.zip -d /opt/torch
RUN rm libtorch.zip
ENV TORCH_LIB=/opt/torch/libtorch
ENV LD_LIBRARY_PATH=$TORCH_LIB/lib:${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}
ENV CMAKE_PREFIX_PATH=$TORCH_LIB:$CMAKE_PREFIX_PATH
  
RUN apt install swig liblapack-dev libmetis-dev -y -q --install-recommends
RUN mkdir -p ${HOMEDIR}/.casadi && cd ${HOMEDIR}/.casadi \
  && git clone https://github.com/utiasASRL/casadi.git .
RUN cd ${HOMEDIR}/.casadi \
  && mkdir -p build && cd build \
  && cmake build -DWITH_PYTHON=ON -DWITH_PYTHON3=ON -DWITH_IPOPT=ON -DWITH_BUILD_IPOPT=ON -DWITH_BUILD_REQUIRED=ON -DWITH_SELFCONTAINED=ON .. \
  && make -j${NUMPROC} install
ENV PYTHONPATH=${PYTHONPATH}:/usr/local
ENV LD_LIBRARY_PATH=/usr/local/casadi:${LD_LIBRARY_PATH}


ENV NVIDIA_DRIVER_CAPABILITIES compute,utility,graphics

RUN apt install -q -y vim htop

## Switch to specified user
USER ${USERID}:${GROUPID}