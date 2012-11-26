#!/usr/bin/python
import threading
import mosquitto
import time
from struct import *

client = mosquitto.Mosquitto("ThreadingPY")

client.connect("192.168.1.72")

time.sleep(0.05)

  
class ThreadClass(threading.Thread):
  def __init__(self, number):
    super(ThreadClass, self).__init__()
    self.number=number	
    print "Robot" + str(self.number)
  def run(self):
    print "Topic: "+"in"+str(self.number)
    while True:
	do_stuff("in"+str(self.number))
        
def do_stuff(topic):
  lw = 0
  rw = 0
  msg   = pack('iii', lw, rw, 0)
  client.publish(topic, msg)
  client.loop(0)
  time.sleep(0.5)


  lw = 80
  rw = 80
  msg   = pack('iii', lw, rw, 0)
  client.publish(topic, msg)
  client.loop(0)
  time.sleep(0.7)

  lw = 0
  rw = 0
  msg   = pack('iii', lw, rw, 0)
  client.publish(topic, msg)
  client.loop(0)
  time.sleep(0.5)

  lw = 80
  rw = -80
  msg   = pack('iii', lw, rw, 0)
  client.publish(topic, msg)
  client.loop(0)
  time.sleep(0.7)

  lw = 0
  rw = 0
  msg   = pack('iii', lw, rw, 0)
  client.publish(topic, msg)
  client.loop(0)
  time.sleep(0.5)
 
  lw = -80
  rw = -80
  msg   = pack('iii', lw, rw, 0)
  client.publish(topic, msg)
  client.loop(0)
  time.sleep(0.7)

  lw = 0
  rw = 0
  msg   = pack('iii', lw, rw, 0)
  client.publish(topic, msg)
  client.loop(0)
  time.sleep(0.5)

for i in range(3, 5):
  t = ThreadClass(i)
  t.start()

while (True):
    try:
        client.loop(0)
    except KeyboardInterrupt:
        lw = 0
        rw = 0
        time.sleep(0.5)
        print "CTRL-C exit"
        quit()

