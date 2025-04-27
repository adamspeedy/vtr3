#!/bin/bash

# Exit immediately if any command fails
set -e

ITERATION=11

TARGET_DIR="/home/adam/Desktop/CurrentBranch/src/main"
cd $TARGET_DIR

echo "Running a loop from 1 to $ITERATION:"
for ((i=1; i<=$ITERATION; i++))
do
    echo "Iteration $i of $ITERATION"
    VTR_PIPELINE=VISION colcon build --packages-select vtr_vision --cmake-args -DITERATION_BUILD_VISION=${i}
    wait $!
done

# Done
echo " Build finished for vtr_vision with MY_PARAM=${MY_PARAM}"