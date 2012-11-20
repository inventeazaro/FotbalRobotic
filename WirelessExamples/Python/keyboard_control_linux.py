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


robot = "in10"


client = mosquitto.Mosquitto("status")
client.connect("192.168.1.72")
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

        if   aux == "w":
            msg  = pack('iii', 70, 70, 0)
            client.publish(robot, msg)
        elif aux == "a":
            msg  = pack('iii', -17, 17, 0)
            client.publish(robot, msg)
        elif aux == "d":
            msg  = pack('iii', 17, -17, 0)
            client.publish(robot, msg)
        elif aux == " ":
            msg  = pack('iii', 0, 0, 0)
            client.publish(robot, msg)
        elif aux == "s":
            msg = pack('iii', -50, -50, 0)
            client.publish(robot, msg)

        print 'pressed ' + aux

    except KeyboardInterrupt:
        print "CTRL-C exit"
        # restore original terminal settings
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        sys.exit()

