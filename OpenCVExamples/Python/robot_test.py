#!/usr/bin/python
''' multi_color_two_object.py

RobotTracker prototype
Written by Ionut Cotoi
This code is under GPL licence by inventeaza.ro
'''
###############################################################################################################################

import cv
import mosquitto
import time
import numpy as np
from struct import *

global imghsv

#mosq = mosquitto.Mosquitto("RoboTracker")
#mosq.connect("192.168.1.72")

def getthresholdedimgR4(imhsv):		
	# A little change here. Creates images for blue and yellow (or whatever color you like).
	imgRed   =cv.CreateImage(cv.GetSize(imhsv),8,1)
	imgYellow=cv.CreateImage(cv.GetSize(imhsv),8,1)
	imgGreen =cv.CreateImage(cv.GetSize(imhsv),8,1)
	
	imgthreshold=cv.CreateImage(cv.GetSize(imhsv),8,1)
	
	cv.InRangeS(imghsv,cv.Scalar(0, 190, 130),cv.Scalar(18, 255, 190),imgRed)	# Select a range of red color
	cv.InRangeS(imghsv,cv.Scalar(100,100,100),cv.Scalar(120,255,255),  imgYellow)	# Select a range of blue color
	cv.InRangeS(imghsv,cv.Scalar(67, 103, 46),cv.Scalar(100, 209, 184),imgGreen)	# Select a range of green color
	
	storage = cv.CreateMemStorage(0)
	redContour = cv.FindContours(imgRed, storage, cv.CV_RETR_CCOMP, cv.CV_CHAIN_APPROX_SIMPLE)
	points = []
	
	while redContour:
		# Draw bounding rectangles
		bound_rect = cv.BoundingRect(list(redContour))
		#bound_rect = cv.BoundingRect(contour)

		# for more details about cv.BoundingRect,see documentation
		pt1 = (bound_rect[0], bound_rect[1])
		pt2 = (bound_rect[0] + bound_rect[2], bound_rect[1] + bound_rect[3])
		points.append(pt1)
		points.append(pt2)
		cv.Rectangle(imhsv, pt1, pt2, cv.CV_RGB(255,0,0), 1)
	
		#Calculating centroids
		centroidx=cv.Round((pt1[0]+pt2[0])/2)
		centroidy=cv.Round((pt1[1]+pt2[1])/2)
		area = cv.ContourArea(list(redContour))
		
		redContour = redContour.h_next()
	
	while yellowContour:
		# Draw bounding rectangles
		bound_rect = cv.BoundingRect(list(redContour))
		#bound_rect = cv.BoundingRect(contour)

		# for more details about cv.BoundingRect,see documentation
		pt1 = (bound_rect[0], bound_rect[1])
		pt2 = (bound_rect[0] + bound_rect[2], bound_rect[1] + bound_rect[3])
		points.append(pt1)
		points.append(pt2)
		cv.Rectangle(imhsv, pt1, pt2, cv.CV_RGB(255,0,0), 1)
	
		#Calculating centroids
		centroidx=cv.Round((pt1[0]+pt2[0])/2)
		centroidy=cv.Round((pt1[1]+pt2[1])/2)
		area = cv.ContourArea(list(redContour))
		
		redContour = redContour.h_next()
	
	
	cv.ShowImage("Ceva",imhsv)
	
	
	cv.Add(imgRed,imgBlue,imgthreshold)
	cv.Add(imgGreen, imgGreen, imgthreshold)
	return imgthreshold

def getthresholdedimg(imhsv):	
	#Get component colors
	imgyellow=cv.CreateImage(cv.GetSize(imhsv),8,1)
	imgblue=cv.CreateImage(cv.GetSize(imhsv),8,1)
	
	imgthreshold=cv.CreateImage(cv.GetSize(imhsv),8,1)
	
	cv.InRangeS(imghsv,cv.Scalar(20,100,100),cv.Scalar(30,255,255),imgyellow)	# Select a range of yellow color
	cv.InRangeS(imghsv,cv.Scalar(100,100,100),cv.Scalar(120,255,255),imgblue)	# Select a range of blue color
	cv.Add(imgyellow,imgblue,imgthreshold)
	return imgthreshold

capture=cv.CaptureFromCAM(0)
frame = cv.QueryFrame(capture)
frame_size = cv.GetSize(frame)
test=cv.CreateImage(cv.GetSize(frame),8,3)
img2=cv.CreateImage(cv.GetSize(frame),8,3)
cv.NamedWindow("Real",0)
cv.NamedWindow("Threshold",0)
cv.NamedWindow("final",0)

#	Create two lists to store co-ordinates of blobs
blue=[]
yellow=[]
red=[]
green=[]
orange=[]

while(1):
	color_image = cv.QueryFrame(capture)
	imdraw=cv.CreateImage(cv.GetSize(frame),8,3)
	cv.SetZero(imdraw)
	#cv.Flip(color_image,color_image,-1)
	cv.Smooth(color_image, color_image, cv.CV_GAUSSIAN, 3, 0)
	imghsv=cv.CreateImage(cv.GetSize(color_image),8,3)
	cv.CvtColor(color_image,imghsv,cv.CV_BGR2HSV)
	imgyellowthresh=getthresholdedimgR4(imghsv)
	cv.Erode(imgyellowthresh,imgyellowthresh,None,3)
	cv.Dilate(imgyellowthresh,imgyellowthresh,None,6)
	img2=cv.CloneImage(imgyellowthresh)
	storage = cv.CreateMemStorage(0)
	contour = cv.FindContours(imgyellowthresh, storage, cv.CV_RETR_CCOMP, cv.CV_CHAIN_APPROX_SIMPLE)
	points = []	

#	This is the new part here. ie Use of cv.BoundingRect()
	while contour:
		# Draw bounding rectangles
		bound_rect = cv.BoundingRect(list(contour))
		#bound_rect = cv.BoundingRect(contour)

		# for more details about cv.BoundingRect,see documentation
		pt1 = (bound_rect[0], bound_rect[1])
		pt2 = (bound_rect[0] + bound_rect[2], bound_rect[1] + bound_rect[3])
		points.append(pt1)
		points.append(pt2)
		cv.Rectangle(color_image, pt1, pt2, cv.CV_RGB(255,0,0), 1)
	
	#	Calculating centroids
	
		centroidx=cv.Round((pt1[0]+pt2[0])/2)
		centroidy=cv.Round((pt1[1]+pt2[1])/2)
		area = cv.ContourArea(list(contour))
		#print "CentroidXY:" + str(centroidx) +":" +str(centroidy) + "A:" + str(area)
		if(area > 100):
			print "CentroidXY:" + str(centroidx) +":" +str(centroidy) + "A:" + str(area)
			coords = pack('iiiii', 4,centroidx, centroidy, 0, int(time.time()))
			#mosq.publish("coords", coords, 0)
	
		contour = contour.h_next()
		print contour
	#	Identifying if blue or yellow blobs and adding centroids to corresponding lists	
		if (169<cv.Get2D(imghsv,centroidy,centroidx)[0]<180):
			red.append((centroidx,centroidy))
		elif (100<cv.Get2D(imghsv,centroidy,centroidx)[0]<120):
			blue.append((centroidx,centroidy))
		elif (67<cv.Get2D(imghsv,centroidy,centroidx)[0]<100):
			green.append((centroidx, centroidy))

# 		Now drawing part. Exceptional handling is used to avoid IndexError.	After drawing is over, centroid from previous part is #		removed from list by pop. So in next frame,centroids in this frame become initial points of line to draw.		
	try:
		cv.Circle(imdraw,red[1],5,(0,255,255))
		cv.Line(imdraw,red[0],red[1],(0,255,255),3,8,0)
		red.pop(0)
	except IndexError:
		print "Just wait for red"
	try:
		cv.Circle(imdraw,yellow[1],5,(0,255,255))
		cv.Line(imdraw,yellow[0],yellow[1],(0,255,255),3,8,0)
		yellow.pop(0)
	except IndexError:
		print "Just wait for yellow"
		
	try:
		cv.Circle(imdraw,blue[1],5,(255,0,0))
		cv.Line(imdraw,blue[0],blue[1],(255,0,0),3,8,0)
		blue.pop(0)			
	except IndexError:
		print "just wait for blue"
	try:
		cv.Circle(imdraw,green[1],5,(255,0,0))
		cv.Line(imdraw,green[0],green[1],(255,0,0),3,8,0)
		green.pop(0)			
	except IndexError:
		print "just wait for green"		
	cv.Add(test,imdraw,test)

	cv.ShowImage("Real",color_image)
	cv.ShowImage("Threshold",img2)
	cv.ShowImage("final",test)
	if cv.WaitKey(33)==1048603:
		cv.DestroyWindow("Real")
		cv.DestroyWindow("Threshold")
		cv.DestroyWindow("final")
		break
###########################################################################################################################
