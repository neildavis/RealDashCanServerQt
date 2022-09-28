# Simple RealDash CAN Server

## Overview

A simple server to send some CAN frame data to a [RealDash](https://realdash.net/index.php) application via TCP/IP. 

Uses [Qt QtNetwork](https://doc.qt.io/qt-5/qtnetwork-index.html) API (QTcpServer, QTcpSocket) to accept socket connections from and send CAN frames to RealDash clients using the custom [RealDash CAN](https://realdash.net/manuals/realdash_can.php) protocol.

Exposes an interface over [DBus](https://en.wikipedia.org/wiki/D-Bus) to allow updating of metrics from a remote process.

## Features
Currently the server supports four basic metrics corresponding to the following [CAN target identifiers](https://realdash.net/manuals/targetid.php)

|Metric|Category|CAN target ID|D-Bus method name|
|------|--------|-------------|-----------------|
|RPM|ENGINE/ECU |37|setRevs|
|Speed|TRANSMISSION|136|setSpeed|
Fuel Level %|BODY ELECTRONICS|170|setFuelLevel|
Selected Gear|TRANSMISSION|140|setGear|

These are delivered in a single CAN frame of 8 bytes, according to the RealDash CAN configuration described in the ```realdash_can.xml``` file in the ```config``` directory.

This can easily be modified and/or expanded to additional metrics and CAN frames. See the [RealDash docs](https://github.com/janimm/RealDash-extras/tree/master/RealDash-CAN) and feel free to send pull requests ;)


## Compatibility

This has been developed on macOS for a target of [Raspbian Linux](https://www.raspbian.org/)

The build system is [CMake](https://cmake.org/) which supports building via generators (e.g. Xcode, Unix Makefiles etc) and installation onto Debian linux using [systemd](https://systemd.io/) D-Bus activation.

It's hoped that the use of CMake will facilitate expansion of compatibility to other platforms where the pre-requisite D-Bus and Qt libraries are available. Again, please feel free to send pull requests ;)

## Prerequisites
Install the following via your preferred package manager:
1. Qt5
2. D-Bus
3. CMake

## Building

### Command line
From the root directory, create a build dir and use cmake to create the makefiles and make to build the project:
```bash
mkdir build
cd build
cmake -G "Unix Makefiles" ..
make
```
### Xcode (macOS)
From the root directory, create a build dir and use cmake to create your Xcode project file and open with Xcode
```bash
mkdir build
cd build
cmake -G "Xcode" ..
open RealDashCanServerQt.xcodeproj
```
Build & Run in the usual way from Xcode
## Installation
Installation on Linux is currently supported by creating a systemd service using D-Bus activation. This means the service is started automatically when clients start to communicate with the server via D-Bus.

After building, simply use make to install from the build dir:
```bash
make install
```
## Sending metrics to the server ##
The server receives data via D-Bus, so you can use any language that has D-Bus binding library support. 

The ```service```, ```interface``` and ```object path``` names are currently defined by constants within the code:
```c
#define CAN_SERVER_SERVICE_NAME "nd.realdash.canserver"
#define CAN_SERVER_INTERFACE    "nd.realdash.canserver.DashBoard"
#define CAN_SERVER_OBJECT_PATH  "/nd/realdash/canserver"
```
An example in C++ of usage can be found in the [dashboard implementation](https://github.com/neildavis/cannonball/blob/real_dash/src/main/realdash/realdashclient.cpp) of my fork of the cannonball engine.
## Contributing
Did I mention pull requests are welcomed? ;)