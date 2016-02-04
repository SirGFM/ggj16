#!/bin/bash

#===============================================================================
# Prepares the android_build directory with everything needed to build it
#===============================================================================

# Make sure we are in the correct directory
ORIGIN_URL=`git config --get remote.origin.url`
if [ "${ORIGIN_URL}" != "git@github.com:SirGFM/ggj16" ]; then
    echo "ERROR: Not running inside ggj16 repo!"
    exit 1
fi
PWD=`pwd`
if [ "${PWD##*/}" != "android_build" ]; then
    cd android_build
    if [ $? -ne 0 ]; then
        echo "ERROR: Failed to find 'android_build' directory!"
        exit 1
    fi
fi

# Check for a SDL2.0.3 clean package and extract it
SDL2_2_0_3_pkg=/home/gfm/Downloads/SDL2-2.0.3.tar.gz
if [ ! -f ${SDL2__2_0_3_pkg} ]; then
    echo "ERROR: Failed to find SDL2-2.0.3 package!"
    exit 1
fi
tar -zxf ~/Downloads/SDL2-2.0.3.tar.gz SDL2-2.0.3/android-project
mv SDL2-2.0.3/android-project/* .
rmdir SDL2-2.0.3/android-project/
rmdir SDL2-2.0.3/

