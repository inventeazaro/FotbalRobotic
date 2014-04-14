#!/usr/local/bin/python
import mosquitto 
import time as Time
import sys, argparse
import os
from struct import *
import random

def main(argv):
	help_text = 'test.py -r robot_id [-b mosquitto_broker] [-l left_motor] [-r right_motor] [-lstep 10] [-rstep 10]'
	mosquitto_broker = '192.168.0.100'
	robot_id = 'r100'
	left_motor = 100
	right_motor = 100
	left_steps = 0
	right_steps = 0
	time = 0
	count = 1
	delay = 50
	parser = argparse.ArgumentParser()
	parser.add_argument("-r", "--rid", help='id of the robot, ex: r1')
	parser.add_argument("-b", "--host", help='mosquitto broker hostname or ip')
	parser.add_argument("-L", "--left", help='left motor speed [0..255]', type=int)
	parser.add_argument("-R", "--right", help='right motor speed [0..255]', type=int)
	parser.add_argument("--lstep", help='left motor steps', type=int)
	parser.add_argument("--rstep", help='right motor steps', type=int)
	parser.add_argument("-t", "--time", help='time to maintain speeds in ms', type=int)
	parser.add_argument("-c", "--count", help='how many times to send this command', type=int)
	parser.add_argument("-d", "--delay", help='delay between resending commands', type=int)
	args = parser.parse_args()

	if args.rid is not None:
			robot_id = args.rid
	if args.host is not None:
			mosquitto_broker = args.host
	if args.left is not None:
			left_motor = args.left
	if args.right is not None:
			right_motor = args.right
	if args.lstep is not None:
			left_steps = args.lstep
	if args.rstep is not None:
			right_steps = args.rstep
	if args.time is not None:
			time = args.time
	if args.count is not None:
			count = args.count
	if args.delay is not None:
			delay = args.delay

	client = mosquitto.Mosquitto("robotctrl-" + str(random.randrange(0,100)))
	client.connect(mosquitto_broker)
	client.publish("status", "robotcrl python connected")

	print "Broker host:", mosquitto_broker
	print "Robot id:", robot_id
	print "Motors: Left:", left_motor, "Right:", right_motor
	print "Steps:  Left:", left_steps, "Right:", right_steps
	print "Time:", time


	i = 0
	while i < count:
		msg = pack("iiiii", left_motor, right_motor, time, left_steps, right_steps)
		client.publish("/" + str(args.rid), msg)
		client.loop()
		Time.sleep(0.05)
		i = i+1
	client.loop()
	client.disconnect()
	Time.sleep(0.05)
	sys.exit(2)

	
if __name__ == "__main__":
	main(sys.argv[1:])

