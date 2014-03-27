#!/usr/bin/python

import mosquitto
import time
from struct import *

client = mosquitto.Mosquitto("Go4Ever")

client.connect("192.168.0.100")

time.sleep(0.05)

def do_stuff():
  lw = 80
  rw = 80
  msg   = pack('iiii', lw, rw, 0, 0)
  print msg
  client.publish("/r5", msg)
  time.sleep(0.7)

while (True):
    try:
        do_stuff()
        client.loop(0)
    except KeyboardInterrupt:
        lw = 0
        rw = 0
        msg   = pack('iiii', lw, rw, 0, 0)
        print msg
        client.publish("/r1", msg)
        time.sleep(0.5)
        print "CTRL-C exit"
        quit()

