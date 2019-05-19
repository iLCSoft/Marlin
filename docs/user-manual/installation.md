
# Dependencies

Marlin has a few dependencies:

- [iLCUtil](https://github.com/iLCSoft/iLCUtil): For the streamlog library and CMake macros
- [LCIO](https://github.com/iLCSoft/lcio): The event data model

Marlin can also be compiled with optional packages to enable additional components:

- [Gear](https://github.com/iLCSoft/gear): The old geometry package for linear colliders (deprecated)
- [DD4hep](https://github.com/AIDASoft/DD4hep): The new geometry package for high energy physics
- [Qt](www.qt.io): To build the Marlin graphical user interface

## Installation order

If you wish to install only the required packages, the installation order is the following:

- iLCUtil
- LCIO. Dependencies:
     - iLCUtil

With all optional dependencies, the order would be the following:

- Qt
- iLCUtil
- XercesC
- ROOT. Dependencies:
     - Qt (optional)
- Geant4. Dependencies:
     - Qt (optional)
     - XercesC (optional)
- Gear. Dependencies:
     - ROOT (optional)
- LCIO. Dependencies:
     - iLCUtil
     - ROOT (optional)
- DD4hep. Dependencies:
     - ROOT
     - Geant4 (optional but recommended)
     - XercesC (optional but recommended)
     - Gear (optional)
     - LCIO (optional but recommended)


## Dependencies installation


### Qt

Qt can be downloaded with `git`:

```shell
$ git clone git://code.qt.io/qt/qt.git
$ cd qt
```

We recommend to use the version 4.7.4:

```shell
$ git checkout v4.7.4
```

Many options are available for compiling Qt. We recommend the following:

```shell
$ mkdir install
$ ./configure \
    -prefix ./install \
    -no-webkit \
    -opensource \
    -confirm-license \
    -prefix-install \
    -fast \
    -make libs \
    -no-separate-debug-info \
    -no-xkb \
    -no-xinerama
$ make install
```

### iLCUtil

iLCUtil can be downloaded with `git`:

```shell
$ git clone https://github.com/rete/iLCUtil.git
$ cd iLCUtil
```

We recommend to use the development branch `streamlog-thread-safe`

```shell
$ git checkout streamlog-thread-safe
```

To compile the package:

```shell
$ mkdir build
$ cd build
$ cmake ..
$ make install
```

### XercesC

XercesC can be downloaded with `git`:

```shell
$ git clone https://github.com/apache/xerces-c.git
$ cd xerces-c
```

We recommend to use the version `3.2.2`

```shell
$ git checkout Xerces-C_3_2_2
```

To compile the package:

```shell
$ mkdir build
$ cd build
$ cmake ..
$ make install
```

### ROOT

ROOT can be downloaded with `git`:

```shell
$ git clone https://github.com/root-project/root.git
$ cd root
```

We recommend to use the version `v6-08-06`:

```shell
$ git checkout v6-08-06
```

To compile the package:

```shell
$ mkdir root_build
$ cd root_build
$ cmake \
  -Dgsl_shared=ON \
  -Dgdml=ON \
  -Dminuit2=ON \
  -Droofit=ON \
  -Dunuran=ON \
  -Dxrootd=ON \
  -Dbuiltin_xrootd=OFF \
  -Dfortran=OFF \
  -Dmysql=OFF \
  ..
$ make install
```

For more details on ROOT dependencies, see [https://root.cern.ch/build-prerequisites](https://root.cern.ch/build-prerequisites).

### Geant4

Geant4 can be downloaded with `git`:

```shell
$ git clone https://gitlab.cern.ch/geant4/geant4.git
$ cd geant4
```

We recommend to use the version `v10.3.2`:

```shell
$ git checkout v10.3.2
```

To compile the package:

```shell
$ mkdir geant4_build
$ cd geant4_build
$ cmake \
  -DGEANT4_INSTALL_DATA=ON \
  -DGEANT4_USE_SYSTEM_EXPAT=OFF \
  -DGEANT4_USE_OPENGL_X11=ON \
  -DGEANT4_USE_QT=ON \
  -DGEANT4_BUILD_CXXSTD="c++11" \
  -DGEANT4_USE_SYSTEM_CLHEP=OFF \
  -DQT_QMAKE_EXECUTABLE=/path/to/Qt/bin/qmake \
  -DXERCESC_ROOT_DIR=/path/to/XercesC \
  -DGEANT4_USE_GDML=ON \
  ..
$ make install
```

### Gear

Gear can be downloaded with `git`:

```shell
$ git clone https://github.com/iLCSoft/gear.git
$ cd gear
```

We recommend to use the version `v01-08`:

```shell
$ git checkout v01-08
```

To compile the package:

```shell
$ mkdir build
$ cd build
$ cmake \
  -DINSTALL_DOC=OFF \
  -DGEAR_TGEO=ON \
  ..
$ make install
```

### LCIO

LCIO can be downloaded with `git`:

```shell
$ git clone https://github.com/rete/lcio.git
$ cd lcio
```

We recommend to use the development branch `lcio-thread-safe`

```shell
$ git checkout lcio-thread-safe
```

To compile the package:

```shell
$ mkdir build
$ cd build
$ cmake \
  -DINSTALL_JAR=OFF \
  -DLCIO_JAVA_USE_MAVEN=OFF \
  -DBUILD_LCIO_EXAMPLES=ON \
  -DBUILD_F77_TESTJOBS=OFF \
  -DLCIO_GENERATE_HEADERS=OFF \
  -DBUILD_ROOTDICT=ON \
  ..
$ make install
```

### DD4hep

DD4hep can be downloaded with `git`:

```shell
$ git clone https://github.com/AIDASoft/DD4hep.git
$ cd DD4hep
```

We recommend to use the version `v01-07-02`

```shell
$ git checkout v01-07-02
```

To compile the package:

```shell
$ mkdir build
$ cd build
$ cmake \
  -DDD4HEP_USE_GEANT4=ON \
  -DDD4HEP_USE_LCIO=ON \
  -DDD4HEP_USE_XERCESC=ON \
  -DDD4HEP_USE_PYROOT=OFF \
  -DDD4HEP_USE_GEAR=ON \
  -DDD4HEP_USE_BOOST=ON \
  -DDD4HEP_USE_CXX11=ON \
  ..
$ make install
```

# Installing Marlin

Marlin can be downloaded with `git`:

```shell
$ git clone https://github.com/rete/Marlin.git
$ cd Marlin
```

We recommend to use the development branch `marlin-mt`

```shell
$ git checkout marlin-mt
```

To compile the package without optional dependencies:

```shell
$ mkdir build
$ cd build
$ cmake \
  -DMARLIN_DD4HEP=OFF \
  -DMARLIN_GEAR=OFF \
  ..
$ make install
```

With optional dependencies:

```shell
$ mkdir build
$ cd build
$ cmake \
  -DMARLIN_DD4HEP=ON \
  -DMARLIN_GEAR=ON \
  ..
$ make install
```

%
