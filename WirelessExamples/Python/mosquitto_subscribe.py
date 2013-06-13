#!/usr/bin/python

import sys
import mosquitto
import time
from struct import *

##def controll():
##    while True:
##        
##        lw = int(raw_input('lw:'))
##        rw = int(raw_input('rw='))
##        do_stuff(lw,rw)


def on_message(msgmosq, obj, msg):
        print unpack('iiiii', msg.payload)
        id,x,y,grad,t = unpack('iiiii', msg.payload)
        print "id robot: ",id
        print "x: ",x
        print "y: ",y
        print "grad: ",grad
        print "t: ",t

client = mosquitto.Mosquitto("")
client.connect("192.168.1.72")
client.subscribe("coords", 2)
client.on_message = on_message

while client.loop() == 0:
    pass

def do_stuff(lw, rw):
    msg = pack('iii', lw, rw, 0)
    print msg
    client.publish("in4", msg)
    



