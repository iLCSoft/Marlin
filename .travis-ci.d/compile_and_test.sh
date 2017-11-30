#!/bin/bash

ILCSOFT=/cvmfs/clicdp.cern.ch/iLCSoft/builds/current/CI_${COMPILER}
source $ILCSOFT/init_ilcsoft.sh

cd /Package
mkdir build
cd build
cmake -GNinja -C $ILCSOFT/ILCSoft.cmake -DCMAKE_CXX_FLAGS="-fdiagnostics-color=always -Werror" .. && \
ninja -k0 && \
ninja install && \
ctest --output-on-failure
