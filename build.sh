#!/bin/bash

apt -y install debconf-utils build-essential cmake dos2unix unixodbc-dev libjsoncpp-dev uuid-dev

mkdir -p build_ && cd build_
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
make
make install
make package

