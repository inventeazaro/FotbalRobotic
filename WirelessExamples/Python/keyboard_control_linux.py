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
            msg  = pack('iiii', -70, -70, 300, 0)
            client.publish(robot, msg)
        elif aux == "a":
            msg  = pack('iiii', 25, -25, 300, 0)
            client.publish(robot, msg)
        elif aux == "d":
            msg  = pack('iiii', -25, 25, 300, 0)
            client.publish(robot, msg)
        elif aux == " ":
            msg  = pack('iiii', 0, 0, 300, 0)
            client.publish(robot, msg)
        elif aux == "s":
            msg = pack('iiii', 50, 50, 300, 0)
            client.publish(robot, msg)

        print 'pressed ' + aux

    except KeyboardInterrupt:
        print "CTRL-C exit"
        # restore original terminal settings
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        sys.exit()

