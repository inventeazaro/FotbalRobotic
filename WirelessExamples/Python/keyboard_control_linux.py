#!/usr/bin/env python
'''
w - forward
a - left
s - backward
d - right
<space> - stop
'''
import mosquitto, time
from struct import *
import sys, tty, termios


robot = "/r3"


client = mosquitto.Mosquitto("kb1234")
client.connect("192.168.0.100")
client.publish("status", "action " + robot)
time.sleep(0.05)

while True:
    try:
        # CODE FOR LINUX
        fd = sys.stdin.fileno()
        # save original terminal settings
        old_settings = termios.tcgetattr(fd)

        # change terminal settings to raw read
        tty.setraw(sys.stdin.fileno())

        aux = sys.stdin.read(1)

        if aux == "w":
            msg  = pack('iiiii', -70, -70, 0, 50, 50)
            client.publish(robot, msg)
        elif aux == "a":
            msg  = pack('iiiii', 25, -25, 0, 50, 50)
            client.publish(robot, msg)
        elif aux == "d":
            msg  = pack('iiiii', -25, 25, 0, 50, 50)
            client.publish(robot, msg)
        elif aux == " ":
            msg  = pack('iiiii', 0, 0, 0, 50, 50)
            client.publish(robot, msg)
        elif aux == "s":
            msg = pack('iiiii', 50, 50, 0, 50, 50)
            client.publish(robot, msg)

        print 'pressed ' + aux

    except KeyboardInterrupt:
        print "CTRL-C exit"
        # restore original terminal settings
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        sys.exit()

