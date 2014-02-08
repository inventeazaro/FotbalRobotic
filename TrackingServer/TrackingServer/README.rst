==============
TrackingServer
==============

Building the project
====================

Linux:

Depenencies:

add the mosquitto debian repository using instructions from http://mosquitto.org/2013/01/mosquitto-debian-repository/

then 

sudo apt-get install libssl-dev mosquitto mosquitto-clients libmosquittopp-dev libmosquitto-dev

install aruco from the ext folder 
cd FotbalRobotic/TrackingServer/ext
tar xvzf aruco-1.2.4.tgz
cd aruco-1.2.4
mkdir build
cd build
cmake ..
make
sudo make install 
 

MacOSX:
TBD
