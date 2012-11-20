import mosquitto
import time
from struct import *
import sys    
from msvcrt import getch

client = mosquitto.Mosquitto("status")
client.connect("192.168.1.72")
client.publish("status", "action in5")
time.sleep(0.05)

"""
w-forward
s-backward
d-right
a-left
<space> - stop
"""
while 1:
    #aux = raw_input()[0]
    aux = getch()
    if aux == "w":
        msg  = pack('iii', 60, 60, 0)
        client.publish("in5", msg)
    if aux == "d":
        msg  = pack('iii', 15, -15, 0)
        client.publish("in5", msg)
    if aux == "a":
        msg  = pack('iii', -15, 15, 0)
        client.publish("in5", msg)
    if aux == " ":
        msg  = pack('iii', 0, 0, 0)
        client.publish("in5", msg)
    if aux == "s":
        msg = pack('iii', -30, -30, 0)
        client.publish("in5", msg)
