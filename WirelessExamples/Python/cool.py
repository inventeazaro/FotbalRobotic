#!/usr/bin/python
import threading
import mosquitto
import time
from struct import *

client = mosquitto.Mosquitto("Goalkeeper")

client.connect("192.168.1.72")

time.sleep(0.05)

def do_stuff():
  lw = 80
  rw = 80
  msg   = pack('iii', lw, rw, 0)
  print msg
  client.publish("in13", msg)
  client.publish("in5", msg)
  time.sleep(0.7)

  lw = 0
  rw = 0
  msg   = pack('iii', lw, rw, 0)
  print msg
  client.publish("in13", msg)
  client.publish("in4", msg)
  time.sleep(0.5)

  lw = -80
  rw = -80
  msg   = pack('iii', lw, rw, 0)
  print msg
  client.publish("in13", msg)
  client.publish("in5", msg)
  time.sleep(0.7)

  lw = 0
  rw = 0
  msg   = pack('iii', lw, rw, 0)
  print msg
  client.publish("in13", msg)
  client.publish("in4", msg)
  client.publish("in5", msg)
  time.sleep(0.5)

while (True):
    try:
        do_stuff()
        client.loop(50)
    except KeyboardInterrupt:
        lw = 0
        rw = 0
        msg   = pack('iii', lw, rw, 0)
        print msg
        client.publish("in13", msg)
        time.sleep(0.5)
        print "CTRL-C exit"
        quit()

