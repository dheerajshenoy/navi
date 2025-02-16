#!/bin/sh

share_dir="/usr/share/navifm"
app_file="/usr/share/applications/navifm.desktop"


if [[ -d $share_dir ]]; then
    sudo rm -rf $share_dir
fi

if [[ -f $app_file ]]; then
    sudo rm -rf $app_file
fi


root=$(pwd)
mkdir -p external
if [[ ! -d "external/KDDockWidgets" ]]; then
    git clone --depth=1 https://github.com/KDAB/KDDockWidgets external/KDDockWidgets
fi
cd external/KDDockWidgets
mkdir -p build
cd build
cmake .. -G Ninja -DKDDockWidgets_QT6=ON \
    -DKDDockWidgets_EXAMPLES=false \
    -DKDDockWidgets_DOCS=false \
    -DKDDockWidgets_FRONTENDS="qtwidgets" \
    -DCMAKE_BUILD_TYPE="Release" \
    -DCMAKE_INSTALL_PREFIX=/usr

cmake --build .
sudo cmake --build . --target install
cd $root
mkdir -p build
cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE="Release"
sudo ninja install
