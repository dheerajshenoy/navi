#!/bin/sh

root=$(pwd)

PARSED=$(getopt -o a:f --long argument:,flag -- "$@") || exit 1
eval set -- "$PARSED"

show_help() {
    echo "Usage: $0 [OPTIONS]"
    echo "Options:"
    echo "  --clean            Cleans installation"
    echo "  --help             Show this help message"
    exit 0
}

while true; do
    case "$1" in
        -c|--clean) shift 2 ;;
        -h|--help) shift ;;
        --) shift; break ;;
        *) break ;;
    esac
done

if [[ ! -d "external" ]]; then
    mkdir external
fi

git clone --depth=1 https://github.com/KDAB/KDDockWidgets external/KDDockWidgets
cd external/KDDockWidgets

if [[ ! -d "build" ]]; then
    mkdir build
fi

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

if [[ ! -d "build" ]]; then
    mkdir build
fi

cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE="Release"
sudo ninja install
