#!/bin/bash

# Exit immediately if any command fails
set -e

ITERATION=11

TARGET_DIR="/home/orin/code/vtr3/src/main"
cd $TARGET_DIR

echo "Running a loop from 1 to $ITERATION:"
for ((i=1; i<=$ITERATION; i++))
do
    echo "Iteration $i of $ITERATION"
    colcon build --packages-select vtr_vision --symlink-install --cmake-args -DITERATION_BUILD_VISION=${i} 
    wait $!
done

# Done
echo " Build finished for vtr_vision with MY_PARAM=${MY_PARAM}"