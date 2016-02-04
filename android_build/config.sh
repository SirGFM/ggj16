#!/bin/bash

#===============================================================================
# Prepares the android_build directory with everything needed to build it
#-------------------------------------------------------------------------------
# Note about patching:
#  - To create patches:
#    - diff -ruN orig/ new/ > file.patch
#  - To patch stuff:
#    - patch -s -p0 < file.patch
#-------------------------------------------------------------------------------
# Note about icons:
#  -    ldpi: 36x36
#  -    mdpi: 48x48
#  -    hdpi: 72x72   (ldpi x2)
#  -   xhdpi: 96x96   (mdpi x2)
#  -  xxhdpi: 144x144 (ldpi x4) (mdpi x3)
#  - xxxhdpi: 192x192 (mdpi x4)
#
# Also, remember that the file must be called 'ic_launcher' (or whatever is
# assigned to 'android:icon', on 'AndroidManifest.xml')
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

# Check for a SDL2.0.3 clean package
SDL2_2_0_3_pkg=/home/gfm/Downloads/SDL2-2.0.3.tar.gz
if [ ! -f ${SDL2__2_0_3_pkg} ]; then
    echo "ERROR: Failed to find SDL2-2.0.3 package!"
    exit 1
fi

# Extract the project's template
tar -zxf ~/Downloads/SDL2-2.0.3.tar.gz SDL2-2.0.3/android-project
# Extract the SDL2 src/header (must be recompiled for Android)
tar -zxf ~/Downloads/SDL2-2.0.3.tar.gz SDL2-2.0.3/Android.mk
tar -zxf ~/Downloads/SDL2-2.0.3.tar.gz SDL2-2.0.3/src
tar -zxf ~/Downloads/SDL2-2.0.3.tar.gz SDL2-2.0.3/include

# Put all extracted files into the project directory
mv SDL2-2.0.3/android-project/ .
mv SDL2-2.0.3/ android-project/jni/

# Patch the directory
patch -s -p0 < android-project.patch

# Create all directories
mkdir -p android-project/res/drawable-ldpi/
mkdir -p android-project/res/drawable-xxxhdpi/
mkdir -p android-project/src/com/gfmgamecorner/
mkdir -p android-project/assets/
mkdir -p android-project/jni/include/

# Create a symlink to the SDL2 includes
cd android-project/jni/include/
ln -s ../SDL2-2.0.3/include/ SDL2
cd -

# Download the required libs
C_SYNTH_URL=https://github.com/SirGFM/c_synth/archive/v1.0.2.tar.gz
GFRAME_URL=https://github.com/SirGFM/GFraMe/archive/devel.tar.gz
C_SYNTH_BASEDIR=c_synth-1.0.2
GFRAME_BASEDIR=GFraMe-devel
wget ${C_SYNTH_URL}
mv v1.0.2.tar.gz c_synth.tar.gz
wget ${GFRAME_URL}
mv devel.tar.gz gframe.tar.gz

# Extract the headers
cd android-project/jni/include/
tar -zxf ../../../c_synth.tar.gz ${C_SYNTH_BASEDIR}/include/c_synth
tar -zxf ../../../gframe.tar.gz ${GFRAME_BASEDIR}/include/GFraMe
mv ${C_SYNTH_BASEDIR}/include/c_synth .
mv ${GFRAME_BASEDIR}/include/GFraMe .
rmdir ${C_SYNTH_BASEDIR}/include/ ${C_SYNTH_BASEDIR}
rmdir ${GFRAME_BASEDIR}/include/ ${GFRAME_BASEDIR}
cd -

# Extract all sources
cd android-project/jni/
tar -zxf ../../../c_synth.tar.gz ${C_SYNTH_BASEDIR}/src
tar -zxf ../../../c_synth.tar.gz ${C_SYNTH_BASEDIR}/Android.mk
tar -zxf ../../../gframe.tar.gz ${GFRAME_BASEDIR}/src
tar -zxf ../../../gframe.tar.gz ${GFRAME_BASEDIR}/Android.mk
mv ${C_SYNTH_BASEDIR}/src ./c_synth
mv ${C_SYNTH_BASEDIR}/Android.mk ./c_synth/
mv ${GFRAME_BASEDIR}/src ./GFraMe
mv ${GFRAME_BASEDIR}/Android.mk ./GFraMe/
rmdir ${C_SYNTH_BASEDIR}
rmdir ${GFRAME_BASEDIR}
cd -

# Create the game's main Java class
JAVA_FILE=android-project/src/com/gfmgamecorner/Witchs_Spell.java
touch ${JAVA_FILE}
echo "com.gfmgamecorner.witchs_spell" >> ${JAVA_FILE}
echo "" >> ${JAVA_FILE}
echo "import org.libsdl.app.SDLActivity;" >> ${JAVA_FILE}
echo "" >> ${JAVA_FILE}
echo "public class Witchs_Spell extends SDLActivity { }" >> ${JAVA_FILE}
echo "" >> ${JAVA_FILE}

# Request the user's key.store and key.alias
if [ -z "${KEY_STORE}" ]; then
    echo -n "Insert the path to your key.store: "
    read KEY_STORE
fi
if [ -z "${KEY_ALIAS}" ]; then
    echo -n "Insert the alias of your key: "
    read KEY_ALIAS
fi

if [ -z "${KEY_STORE}" ]; then
    echo "key.store=${KEY_STORE}" > android-project/ant.properties
fi
if [ -z "${KEY_ALIAS}" ]; then
    echo "key.alias=${KEY_ALIS}" > android-project/ant.properties
fi

