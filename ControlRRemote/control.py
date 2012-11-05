#!/usr/bin/python

import mosquitto
import pyfirmata
import time
from struct import *

af = pyfirmata.Arduino("/dev/ttyACM0")
it1 = pyfirmata.util.Iterator(af)

it1.start()

af.analog[0].enable_reporting()
af.analog[1].enable_reporting()
af.analog[2].enable_reporting()

client = mosquitto.Mosquitto("ControlRRemote")

client.connect("127.0.0.1")

time.sleep(0.05)

cal_x = 3
cal_y = -2
scale_x = 1
scale_y = 1.011

def read_firmata():
  x_ax  = af.analog[0].read()
  y_ax  = af.analog[1].read()
  btn   = af.analog[2].read()
  jx  = int(x_ax * 500 - 128*2) + cal_x
  jy = int(((y_ax * 500 - 128*2) + cal_y) / scale_y)
  lw = 0
  rw = 0
  if (jx==0 and jy==0):
    lw = 0
    rw = 0
  elif (jy==0):
    lw = jx
    rw = -jx
  elif (jx==0):
    lw = jy
    rw = jy
  else:
    lw = jy + jx
    rw = jy - jx
  print jx
  print jy
  msg   = pack('iii', lw, rw, 0)
  print msg
  client.publish("in7", msg)
  time.sleep(0.1)

while (True):
    try:
        read_firmata()
        #client.loop(50)
    except KeyboardInterrupt:
        print "CTRL-C exit"
        af.exit()
        quit()

