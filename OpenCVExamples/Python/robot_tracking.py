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

mosq = mosquitto.Mosquitto("RoboTracker")
#mosq.connect("192.168.1.72")
mosq.connect("127.0.0.1")

ColorRed_Low = (175, 18, 60)
ColorRed_High = (21, 180, 180)

ColorGreen_Low = (67, 103, 46)
ColorGreen_High = (100, 209, 184)

ColorBlue_Low = (79, 186, 84)
ColorBlue_High = (146, 243, 244)

ColorYellow_Low = (19, 159, 81)
ColorYellow_High = (56, 244, 255)

ColorOrange_Low = (3, 110, 122)
ColorOrange_High = (5, 233, 212)

ColorDarkBlue_Low = (41, 40, 31)
ColorDarkBlue_High = (141, 129, 80)

ColorIndigo_Low = (137, 91, 43)
ColorIndigo_High = (156, 233, 132)


R4= {}
R4["minRed"] = ColorRed_Low
R4["maxRed"] = ColorRed_High

R4["minBlue"] = ColorBlue_Low
R4["maxBlue"] = ColorBlue_High

R4["minGreen"] = ColorGreen_Low
R4["maxGreen"] = ColorGreen_High



def RedMinMaxThread():
	client.subscribe("red/min", 0)
	client.subscribe("red/max", 0)


	return



def getthresholdedimgRGeneric(R, imhsv):		
	# A little change here. Creates images for blue and yellow (or whatever color you like).
	imgRed =cv.CreateImage(cv.GetSize(imhsv),8,1)
	imgBlue=cv.CreateImage(cv.GetSize(imhsv),8,1)
	imgGreen=cv.CreateImage(cv.GetSize(imhsv),8,1)
	
	imgthreshold=cv.CreateImage(cv.GetSize(imhsv),8,1)
	
	cv.InRangeS(imghsv,cv.Scalar(R["minRed"][0], R["minRed"][1], R["minRed"][2]),cv.Scalar(R["maxRed"][0], R["maxRed"][1], R["maxRed"][2]),imgRed)	# Select a range of yellow color
	cv.InRangeS(imghsv,cv.Scalar(R["minBlue"][0], R["minBlue"][1], R["minBlue"][2]),cv.Scalar(R["maxBlue"][0], R["maxBlue"][1], R["maxBlue"][2]),imgBlue)	# Select a range of blue color
	cv.InRangeS(imghsv,cv.Scalar(R["minGreen"][0], R["minGreen"][1], R["minGreen"][2]),cv.Scalar(R["maxGreen"][0], R["maxGreen"][1], R["maxGreen"][2]),imgGreen)	# Select a range of blue color
	
	cv.Add(imgRed,imgBlue,imgthreshold)
	cv.Add(imgGreen, imgGreen, imgthreshold)
	return imgthreshold



def CenterFunction(R, imghsv):
	imgyellowthresh=getthresholdedimgRGeneric(R, imghsv) # creaza mastile de culoare, aici specifice robotului R4

	cv.Erode(imgyellowthresh,imgyellowthresh,None,3)#filtru 
	cv.Dilate(imgyellowthresh,imgyellowthresh,None,6)#filtru 
	img2=cv.CloneImage(imgyellowthresh)#cloneaza imaginea in img2, useless
	storage = cv.CreateMemStorage(0)#creaza un loc in memorie unde sa stocheze, necesar ptr FindContours
	contour = cv.FindContours(imgyellowthresh, storage, cv.CV_RETR_CCOMP, cv.CV_CHAIN_APPROX_SIMPLE)#gaseste contururile robotilor
	points = []	

#	This is the new part here. ie Use of cv.BoundingRect()
	while contour:
		# Draw bounding rectangles
		bound_rect = cv.BoundingRect(list(contour)) #creaza un patratel din punctele din contur, ptr afisare/debug
		#bound_rect = cv.BoundingRect(contour)

		# for more details about cv.BoundingRect,see documentation
		pt1 = (bound_rect[0], bound_rect[1]) #
		pt2 = (bound_rect[0] + bound_rect[2], bound_rect[1] + bound_rect[3])
		points.append(pt1)
		points.append(pt2)
		cv.Rectangle(color_image, pt1, pt2, cv.CV_RGB(255,0,0), 1)
		#pana aici s-a desenat patratul
	#	Calculating centroids
	
		centroidx=cv.Round((pt1[0]+pt2[0])/2)
		centroidy=cv.Round((pt1[1]+pt2[1])/2)
		area = cv.ContourArea(list(contour))
		#print "CentroidXY:" + str(centroidx) +":" +str(centroidy) + "A:" + str(area)
		if(area > 100):
			print "CentroidXY:" + str(centroidx) +":" +str(centroidy) + "A:" + str(area)
			coords = pack('iiiii', 4,centroidx, centroidy, 0, int(time.time()))
			mosq.publish("coords", coords, 0)
	
		contour = contour.h_next()	
		print contour
		#	Identifying if blue or yellow blobs and adding centroids to corresponding lists	
		if (169<cv.Get2D(imghsv,centroidy,centroidx)[0]<180):
			red.append((centroidx,centroidy))
		elif (100<cv.Get2D(imghsv,centroidy,centroidx)[0]<120):
			blue.append((centroidx,centroidy))
		elif (67<cv.Get2D(imghsv,centroidy,centroidx)[0]<100):
			green.append((centroidx, centroidy))

	return


def getthresholdedimgR4(imhsv):		
	# A little change here. Creates images for blue and yellow (or whatever color you like).
	imgRed =cv.CreateImage(cv.GetSize(imhsv),8,1)
	imgBlue=cv.CreateImage(cv.GetSize(imhsv),8,1)
	imgGreen=cv.CreateImage(cv.GetSize(imhsv),8,1)
	
	imgthreshold=cv.CreateImage(cv.GetSize(imhsv),8,1)
	
	cv.InRangeS(imghsv,cv.Scalar(169, 168, 115),cv.Scalar(180, 243, 200),imgRed)	# Select a range of yellow color
	cv.InRangeS(imghsv,cv.Scalar(100,100,100),cv.Scalar(120,255,255),imgBlue)	# Select a range of blue color
	cv.InRangeS(imghsv,cv.Scalar(67, 103, 46),cv.Scalar(100, 209, 184),imgGreen)	# Select a range of blue color
	
	cv.Add(imgRed,imgBlue,imgthreshold)
	cv.Add(imgGreen, imgGreen, imgthreshold)
	return imgthreshold

def getthresholdedimgR7(imhsv):		
	# A little change here. Creates images for blue and yellow (or whatever color you like).
	imgYellow =cv.CreateImage(cv.GetSize(imhsv),8,1)
	imgBlue=cv.CreateImage(cv.GetSize(imhsv),8,1)
	imgGreen=cv.CreateImage(cv.GetSize(imhsv),8,1)
	
	imgthreshold=cv.CreateImage(cv.GetSize(imhsv),8,1)
	imgInterim  =cv.CreateImage(cv.GetSize(imhsv),8,1)
	
	
	cv.InRangeS(imghsv,cv.Scalar(20,100,100),cv.Scalar(30,255,255),imgYellow)	# Select a range of yellow color
	cv.InRangeS(imghsv,cv.Scalar(67, 103, 46),cv.Scalar(100, 209, 184),imgGreen)	# Select a range of blue color
	cv.InRangeS(imghsv,cv.Scalar(100,100,100),cv.Scalar(120,255,255),imgBlue)	# Select a range of blue color

	
	cv.Add(imgYellow,imgBlue,imgInterim)
	cv.Add(imgGreen, imgInterim, imgthreshold)
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

capture=cv.CaptureFromCAM(1)
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
	color_image = cv.QueryFrame(capture) #1 frame
	imdraw=cv.CreateImage(cv.GetSize(frame),8,3) #poza goala
	cv.SetZero(imdraw)#goleste poza goala
	#cv.Flip(color_image,color_image,-1)#intoarce imaginea sus jos, stg dr
	cv.Smooth(color_image, color_image, cv.CV_GAUSSIAN, 3, 0)#filtru blur
	imghsv=cv.CreateImage(cv.GetSize(color_image),8,3)#creaza o imagine goala de dimensiunea originalului (color_image)
	cv.CvtColor(color_image,imghsv,cv.CV_BGR2HSV) #transforma din rgb in hsv 
	
	CenterFunction(R4, imghsv)
	
		contour = contour.h_next()
		print contour
	#	Identifying if blue or yellow blobs and adding centroids to corresponding lists	
		if (169<cv.Get2D(imghsv,centroidy,centroidx)[0]<180):
			red.append((centroidx,centroidy))
		elif (100<cv.Get2D(imghsv,centroidy,centroidx)[0]<120):
			blue.append((centroidx,centroidy))
		elif (67<cv.Get2D(imghsv,centroidy,centroidx)[0]<100):
			green.append((centroidx, centroidy))

# 		Now drawing part. Exceptional handling is used to avoid IndexError.	After drawing is over, centroid from previous part is
#		removed from list by pop. So in next frame,centroids in this frame become initial points of line to draw.		
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
