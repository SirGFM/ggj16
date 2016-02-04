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

# Check for a SDL2.0.3 clean package and extract it
SDL2_2_0_3_pkg=/home/gfm/Downloads/SDL2-2.0.3.tar.gz
if [ ! -f ${SDL2__2_0_3_pkg} ]; then
    echo "ERROR: Failed to find SDL2-2.0.3 package!"
    exit 1
fi
tar -zxf ~/Downloads/SDL2-2.0.3.tar.gz SDL2-2.0.3/android-project
mv SDL2-2.0.3/android-project/ .
rmdir SDL2-2.0.3/

# TODO Patch the directory

# Create all directories
mkdir -p android-project/res/drawable-ldpi/
mkdir -p android-project/res/drawable-xxxhdpi/
mkdir -p android-project/src/com/gfmgamecorner/
mkdir -p android-project/assets/

# Create the game's main Java class
JAVA_FILE=android-project/src/com/gfmgamecorner/Witchs_Spell.java
touch ${JAVA_FILE}
echo "com.gfmgamecorner.witchs_spell" >> ${JAVA_FILE}
echo "" >> ${JAVA_FILE}
echo "import org.libsdl.app.SDLActivity;" >> ${JAVA_FILE}
echo "" >> ${JAVA_FILE}
echo "public class Witchs_Spell extends SDLActivity { }" >> ${JAVA_FILE}
echo "" >> ${JAVA_FILE}

