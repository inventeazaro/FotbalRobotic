#!/usr/bin/python

# Test whether a PUBLISH to a topic with QoS 2 results in the correct packet flow.

import subprocess
import socket
import time

import inspect, os, sys
# From http://stackoverflow.com/questions/279237/python-import-a-module-from-a-folder
cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0],"..")))
if cmd_subfolder not in sys.path:
    sys.path.insert(0, cmd_subfolder)

import mosq_test

rc = 1
keepalive = 60
connect_packet = mosq_test.gen_connect("test-helper", keepalive=keepalive)
connack_packet = mosq_test.gen_connack(rc=0)

mid = 312
publish_packet = mosq_test.gen_publish("qos2/disconnect/test", qos=2, mid=mid, payload="disconnect-message")
pubrec_packet = mosq_test.gen_pubrec(mid)
pubrel_packet = mosq_test.gen_pubrel(mid)
pubcomp_packet = mosq_test.gen_pubcomp(mid)

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(("localhost", 1888))
sock.send(connect_packet)
connack_recvd = sock.recv(len(connack_packet))

if mosq_test.packet_matches("helper connack", connack_recvd, connack_packet):
    sock.send(publish_packet)
    pubrec_recvd = sock.recv(32)

    if mosq_test.packet_matches("helper pubrec", pubrec_recvd, pubrec_packet):
        sock.send(pubrel_packet)
        pubcomp_recvd = sock.recv(len(pubcomp_packet))

        if mosq_test.packet_matches("helper pubcomp", pubcomp_recvd, pubcomp_packet):
            rc = 0

sock.close()

exit(rc)

