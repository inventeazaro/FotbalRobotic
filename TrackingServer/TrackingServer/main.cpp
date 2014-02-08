/*****************************
Copyright 2011 Rafael Muñoz Salinas. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY Rafael Muñoz Salinas ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Rafael Muñoz Salinas OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of Rafael Muñoz Salinas.
********************************/
/************************************
 *
 *
 *
 *
 ************************************/
#include <iostream>
#include <ctime>
#include <mosquitto.h>
#include <stdlib.h>
#include <unistd.h>

#include "aruco/aruco.h"
#include "aruco/cvdrawingutils.h"

using namespace cv;
using namespace aruco;


int hLow = 116;
int sLow = 73;
int vLow = 99;
int hHigh = 141;
int sHigh = 149;
int vHigh = 169;


int idAssocMat[1024] = {1};

#define mqtt_host "localhost"
#define mqtt_port 1883

struct robotCoords {
    int id;    // id robot - 1..10
    int x;     // 0...800  - relative la terenul de joc
    int y;     // 0...600
    int angle; // unghi fata de baza ringuluieu as fi avut nevoie de joi pana duminica in special
    int timestamp; // timpul cand au fost calculate - UNIXTIME - Ex: 1352460922
};

struct mosquitto *mosq;

#define TEST_FILE "/home/cotty/dev/arutest/test.png"

int main(int argc,char **argv)
{

    idAssocMat[0]    = 1;
    idAssocMat[81]   = 2;
    idAssocMat[277]  = 3;
    idAssocMat[325]  = 4;
    idAssocMat[321]  = 5;
    idAssocMat[1023] = 6;

    idAssocMat[5]    = 7;
    idAssocMat[20]   = 8;
    idAssocMat[80]   = 9;
    idAssocMat[336]  = 10;
    idAssocMat[341]  = 11;
    idAssocMat[682]  = 12;

    // TODO Find a better solution for this
    // system("uvcdynctrl -d video1 -s \"Exposure (Absolute)\" 100");

    char clientid[64]="FotbalRobotic.ro Tracking Server";
    mosquitto_lib_init();

    mosq = mosquitto_new(clientid, true, NULL);

    if(mosq) {
        mosquitto_connect(mosq, mqtt_host, mqtt_port, 60);
    }

    try
    {
        aruco::CameraParameters CamParam;
        MarkerDetector MDetector;
        vector<Marker> Markers;
        float MarkerSize=0.008;
        //read the input image
        cv::Mat InImage;
        //try opening first as video
        VideoCapture vreader(1);
        const char mainWindow[] = "FotbalRobotic Tracker";
        cv::namedWindow(mainWindow, CV_WINDOW_FULLSCREEN);
        cv::Mat HSVImage;
        cv::Mat BallMask;

        int mid = 0;
        int rc = 0;

        MarkerSize=10;

        vreader.set(CV_CAP_PROP_FRAME_WIDTH, 1080);
        vreader.set(CV_CAP_PROP_FRAME_HEIGHT, 720);

        //vreader.open("http://admin:admin@192.168.20.36/oneshotimage.jpg");


        while(1) {
        if (vreader.isOpened()) {
            vreader.grab();
            vreader.retrieve(InImage);
            //InImage=cv::imread(TEST_FILE);
        }

//        cv::GaussianBlur(InImage, temp, cv::Size(0, 0), valSigmaX, valSigmaY);
//        cv::addWeighted(InImage, 1.5, temp, -0.5, 0, InImage);
          //cv::erode(InImage, InImage, Mat(), Point(-1,-1), 2);

          //cv::dilate(InImage, InImage, 0, 2);

        //cv::imshow("enhanced", temp);

        //at this point, we should have the image in InImage
        //if empty, exit
        if (InImage.total()==0) {
            cerr<<"Could not open input"<<endl;
            return 0;
        }


        cv::createTrackbar("H Low", mainWindow, &hLow, 180, NULL);
        cv::createTrackbar("S Low", mainWindow, &sLow, 255, NULL);
        cv::createTrackbar("V Low", mainWindow, &vLow, 255, NULL);
        cv::createTrackbar("H High", mainWindow, &hHigh, 180, NULL);
        cv::createTrackbar("S High", mainWindow, &sHigh, 255, NULL);
        cv::createTrackbar("V High", mainWindow, &vHigh, 255, NULL);

        cv::Scalar lowThreshold(hLow, sLow, vLow, 0);
        cv::Scalar highThreshold(hHigh, sHigh, vHigh, 0);

        cv::cvtColor(InImage, HSVImage, CV_RGB2HSV);
        cv::inRange(HSVImage, lowThreshold, highThreshold, BallMask);
        //cv::GaussianBlur(BallMask, BallMask, cv::Size(1, 1), 2);
        //cv::erode(BallMask, BallMask, Mat(), Point(-1,-1), 1);
        //cv::dilate(BallMask, BallMask, Mat(), Point(-1,-1), 1);

//        vector<Vec3f> circles;

//        HoughCircles(BallMask, circles, CV_HOUGH_GRADIENT, 2, BallMask.rows, 70, 70, 0, 50);
//        for( size_t i = 0; i < circles.size(); i++ )
//        {
//             Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
//             int radius = cvRound(circles[i][2]);
//             // draw the circle center
//             circle( InImage, center, 3, Scalar(0,255,0), -1, 8, 0 );
//             // draw the circle outline
//             circle( InImage, center, radius, Scalar(0,0,255), 3, 8, 0 );
//        }
//        CBlobResult blobs;
//        CBlob blob;
//        IplImage img = BallMask;
//        blobs = CBlobResult( img, NULL, 255);
//        // Determine Min & Max blob size
//        blobs.Filter( blobs, B_INCLUDE, CBlobGetArea(), B_LESS, 599);
//        blobs.Filter( blobs, B_INCLUDE, CBlobGetArea(), B_GREATER, 59);
//        int totalBlobs = blobs.GetNumBlobs();

//        for(int b=0; b<totalBlobs; b++)
//        {
//            blob = blobs.GetBlob(b);
//            // ICOTOI blob size
//            //cout<<"Blob: "<<b<<"BlobSize: "<<blob.Area()<<std::endl;

//            // Draw rectangle around each blob
//            rectangle(InImage, cvPoint(blob.MinX(), blob.MinY()),
//            cvPoint(blob.MaxX(), blob.MaxY() ), CV_RGB(0,255,0), 1 );
//        }

        Moments ballMoments = moments(BallMask, false);
        Point   ballPosition;


        float ballArea = ballMoments.m00;
        if(ballArea > 10) {
            ballPosition.x = ballMoments.m10/ballArea;
            ballPosition.y = ballMoments.m01/ballArea;

            //circle(InImage, ballPosition, 10, Scalar(255, 255, 255), 2);
            robotCoords coords;

            coords.id = 0; // Id Ball
            coords.x  =  ballPosition.x;
            coords.y  =  ballPosition.y;
            coords.angle = 0;
            cerr<<ballPosition.x<<" "<<ballPosition.y<<endl;
            coords.timestamp = std::time(0);
            mosquitto_publish(mosq, &mid, "coords", sizeof(coords), &coords, 0, true);
        }

        //imshow("ceva", BallMask);

        //read camera parameters if specifed

        CamParam.readFromXMLFile("/home/bogdan/dev/FotbalRobotic/TrackingServer/arutest/camera.yml");
        //resizes the parameters to fit the size of the input image
        CamParam.resize( InImage.size());
        bitwise_not(InImage, InImage);
        //read marker size if specified
        //Ok, let's detect
        MDetector.getCandidates();
        MDetector.setMinMaxSize(0.005, 0.5);

        MDetector.detect(InImage,Markers,CamParam,MarkerSize);
        //for each marker, draw info and its boundaries in the image
        for (unsigned int i=0;i<Markers.size();i++) {
            //cout<<Markers[i]<<endl;
            Markers[i].draw(InImage,Scalar(0,0,255),2);
            double position[3];
            double orientation[4];

            cv::Point2f markerCenter, p1, p2;
            double pd, angle;
            robotCoords coords;

            markerCenter = Markers[i].getCenter();
            coords.id = idAssocMat[Markers[i].id];
            coords.x  = markerCenter.x;
            coords.y  = markerCenter.y;


            p1.x = Markers[i][0].x;
            p1.y = Markers[i][0].y;
            p2.x = Markers[i][1].x;
            p2.y = Markers[i][1].y;

            pd = p1.y-p2.y/p1.x-p2.x;

            angle= atan2(p1.x-p2.x, p1.y-p2.y);
            angle = angle*180/3.1415 + 180;
            cout<<"Id:"<< coords.id << "Ang: "<<angle<<endl;


             //coords.angle = Markers[i].Rvec.ptr<float>(0)[0] * 180/3.1415;
             coords.angle = angle;

//            Markers[i].OgreGetPoseParameters(position, orientation);
//            cout<<"Id: "<<coords.id<<"Orientation: "<<orientation[0]<<":"<<orientation[1]<<":"<<orientation[2]<<":"<<orientation[3];
//            cout<<"Id: "<<coords.id<<"Angle: "<<coords.angle<<endl;
            coords.timestamp = std::time(0);
            mosquitto_publish(mosq, &mid, "coords", sizeof(coords), &coords, 0, true);
        }

        // Call mosquitto
        rc = mosquitto_loop(mosq, 1, 50);

        if(rc) {
            sleep(20);
            mosquitto_reconnect(mosq);
        }

        //draw a 3d cube in each marker if there is 3d info
        //if (  CamParam.isValid() && MarkerSize!=-1)
        //    for (unsigned int i=0;i<Markers.size();i++) {
        //        CvDrawingUtils::draw3dCube(InImage,Markers[i],CamParam);
        //    }
        //show input with augmented information

        //Cerc verde 0,0
        circle(InImage, Point(15,30), 5, Scalar(0, 255, 255), 2);

        //Poarta stanga
        Point goalTopLeft(15, 280);
        Point goalLowLeft(15, 420);
        Point goalTopRight(35, 280);
        Point goalLowRight(35, 420);

        circle(InImage, goalTopLeft, 2, Scalar(255, 0, 0), 2);
        circle(InImage, goalLowLeft, 2, Scalar(255, 0, 0), 2);
        circle(InImage, goalTopRight, 2, Scalar(255, 0, 0), 2);
        circle(InImage, goalLowRight, 2, Scalar(255, 0, 0), 2);

        //Poarta dreapta


        cv::imshow("FotbalRobotic Tracker",InImage);

        //show also the internal image resulting from the threshold operation
        //cv::imshow("thes", MDetector.getThresholdedImage() );
        cv::waitKey(1);//wait for key to be pressed

        }

        if (argc>=5) cv::imwrite(argv[4],InImage);
    } catch (std::exception &ex)

    {
        cout<<"Exception :"<<ex.what()<<endl;
    }
}
