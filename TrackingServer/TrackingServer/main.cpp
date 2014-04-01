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
#include <pthread.h>
#include <math.h>
#include <pthread.h>

#include "aruco/aruco.h"
#include "aruco/cvdrawingutils.h"

using namespace cv;
using namespace aruco;

int alfa = 150, alfa2 = 150;
int beta = 50, beta2 = 50;
int idAssocMat[1024] = {1};

#define mqtt_host "localhost"   //"192.168.0.100"
#define mqtt_port 1883

struct robotCoords {
    int id;    // id robot - 1..10
    int x;     // 0...800  - relative la terenul de joc
    int y;     // 0...600
    int angle; // unghi fata de baza ringuluieu as fi avut nevoie de joi pana duminica in special
    int timestamp; // timpul cand au fost calculate - UNIXTIME - Ex: 1352460922
};

struct mosquitto *mosq;
pthread_t threads[5];

const char mainWindow1[] = "FotbalRobotic Tracker camera 1";
const char mainWindowsettings1[] = "FotbalRobotic Tracker settings camera 1";
const char mainWindow2[] = "FotbalRobotic Tracker camera 2";
const char mainWindowsettings2[] = "FotbalRobotic Tracker settings camera 2";

int mid = 0;
int rc = 0;

int onces = 0;
int onced = 0;
int la0s = 0;
int la0d = 0;
int calib_cam2;

aruco::CameraParameters CamParam_stanga,CamParam_dreapta;

Point center_dreapta;
Point center_stanga;
cv::Mat imagine_dreapta;
cv::Mat imagine_stanga;

void* camera1 (void *threadid) {
    int hLow1 = 116;
    int sLow1 = 73;
    int vLow1 = 99;
    int hHigh1 = 141;
    int sHigh1 = 149;
    int vHigh1 = 169;
    cv::Mat HSVImage;
    cv::Mat BallMask;
    MarkerDetector MDetector;
    vector<Marker> Markers;
    float MarkerSize_dr=10; //0.008;
    int cps1x=0, cps1y=0, cps2x=0, cps2y=0;
    long tid;
    tid = (long)threadid;
    cv::Mat imagine_dr;

    while(1) {

    imagine_dr = imagine_dreapta.clone();
    cv::createTrackbar("H Low", mainWindowsettings1, &hLow1, 180, NULL);
    cv::createTrackbar("S Low", mainWindowsettings1, &sLow1, 255, NULL);
    cv::createTrackbar("V Low", mainWindowsettings1, &vLow1, 255, NULL);
    cv::createTrackbar("H High", mainWindowsettings1, &hHigh1, 180, NULL);
    cv::createTrackbar("S High", mainWindowsettings1, &sHigh1, 255, NULL);
    cv::createTrackbar("V High", mainWindowsettings1, &vHigh1, 255, NULL);

    cv::Scalar lowThreshold(hLow1, sLow1, vLow1, 0);   //pt minge
    cv::Scalar highThreshold(hHigh1, sHigh1, vHigh1, 0);
    cv::cvtColor(imagine_dr, HSVImage, CV_RGB2HSV);
    cv::inRange(HSVImage, lowThreshold, highThreshold, BallMask);

    Moments ballMoments = moments(BallMask, false);

    Point   ballPosition;

    float ballArea = ballMoments.m00;

    if(ballArea > 20) {
        ballPosition.x = ballMoments.m10/ballArea;
        ballPosition.y = ballMoments.m01/ballArea;
        circle(imagine_dr, ballPosition, 10, Scalar(255, 255, 255), 2);
        robotCoords coords;
        coords.id = 0; // Id Ball
        coords.y  =  ballPosition.x;
        coords.x  =  ballPosition.y;
        coords.angle = 0;
       // cerr<<ballPosition.x<<" "<<ballPosition.y<<endl;
        coords.timestamp = std::time(0);
        mosquitto_publish(mosq, &mid, "coords", sizeof(coords), &coords, 0, true);
    }


   // imshow("ceva", BallMask);
 //   imshow("verde",VerdeMask);

    bitwise_not(imagine_dr, imagine_dr);

    Mat roi(imagine_dr, Rect(Point (400,0), Point (560,130)));
    rectangle(imagine_dr, Point (400,0), Point (560,100), Scalar(255,0,255),1,8, 0);
    vector<Vec3f> circles;
    // Apply the Hough Transform to find the circles
    cvtColor( roi, roi, CV_BGR2GRAY );

    //filtru sharpen
    cv::Mat tmp1;
    cv::GaussianBlur(imagine_dr, tmp1, cv::Size(3,3), 5);
    cv::addWeighted(imagine_dr, alfa/100, tmp1, -beta/100, 0, imagine_dr);
    cv::createTrackbar("alfa", mainWindowsettings1, &alfa, 300, NULL);
    cv::createTrackbar("beta", mainWindowsettings1, &beta, 300, NULL);

    MDetector.getCandidates();
    MDetector.setMinMaxSize(0.005, 0.5);
    MDetector.detect(imagine_dr,Markers,CamParam_dreapta,MarkerSize_dr);

    //for each marker, draw info and its boundaries in the image
    int nr = 0;
    if (onces ==0)
     {
        for (unsigned int i=0;i<Markers.size();i++) {
            if (Markers[i].id == 1023 || Markers[i].id == 682)
            {
            Markers[i].draw(imagine_dr,Scalar(0,0,255),2);
            cv::Point2f markerCenter;

            markerCenter = Markers[i].getCenter();
            if (markerCenter.x !=0 && markerCenter.y !=0) {
                if (Markers[i].id == 682)
                {
                    cps1y = markerCenter.x;
                    cps1x = markerCenter.y;
                    nr++;
                }
                if (Markers[i].id == 1023)
                {
                    cps2y = markerCenter.x;
                    cps2x = markerCenter.y;
                    nr++;
                }
            }
           //    cerr<<"poarta 1 dreapta: x: "<<cps1x<<" y: "<<cps1y<<endl;

           //    cerr<<"poarta 2 dreapta: x: "<<cps2x<<" y: "<<cps2y<<endl;
            }
        }
        HoughCircles( roi, circles, CV_HOUGH_GRADIENT, 1, 10, 100, 24, 0, 0 );
        cerr<<circles.size();
          // Draw the circles detected
          for( size_t i = 0; i < circles.size(); i++ )
          {
              Point center(cvRound(circles[i][0])+400, cvRound(circles[i][1]));
              int radius = cvRound(circles[i][2]);
              circle( imagine_dr, center, 3, Scalar(0,255,0), -1, 8, 0 );// circle center
              circle( imagine_dr, center, radius, Scalar(0,0,255), 3, 8, 0 );// circle outline
              center_dreapta = center;
           }
    if (nr == 2) {
        onces = 1;
        la0s = (cps1x + cps2x) / 2;
        cps2y = cps1y - cps2y;
        cerr<<"pentru marker de la 0,0: "<< cps1y<<"iar pt marker de la 0,700 "<<cps2y<<" //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////";
        cerr<<"pentru cerc mijloc camera dreapta: "<<center_dreapta.x<<"cu "<<center_dreapta.y<<" (x,y)\n";
        }
    }

        Point center(center_dreapta.x, center_dreapta.y);
        int radius = 33;
        circle( imagine_dr, center, 3, Scalar(0,255,0), -1, 8, 0 );// circle center
        circle( imagine_dr, center, radius, Scalar(0,0,255), 3, 8, 0 );// circle outline

   // cerr<<"\n\n coord cam 2 calib_cam2la mijlocul terenului: "<<center_dreapta.y<<" iar la poarta din dreapta: "<<la0s<<"\n\n";
    for (unsigned int i=0;i<Markers.size();i++) {
        Markers[i].draw(imagine_dr,Scalar(0,0,255),2);
        cv::Point2f markerCenter, p1, p2;
        double pd, angle;
        robotCoords coords;

        markerCenter = Markers[i].getCenter();
        if (Markers[i].id != 682)
            if (Markers[i].id != 1023)
                if (onces == 1) {  //daca nu este markerul de langa poarta

            coords.id = idAssocMat[Markers[i].id];
            coords.y  = markerCenter.x;
            coords.x  = markerCenter.y;
           // if (coords.id ==9) cerr<<"coord nemodif x: "<<coords.x;
            if (coords.x > center_dreapta.y && coords.x < la0s) {

               // cerr<<"cd.y "<<center_dreapta.y<<" la0s: "<< la0s<<"cps2y "<< cps2y;
                //scalare coordonate
                coords.x = (coords.x - center_dreapta.y) * 700 / (la0s - center_dreapta.y);
                coords.x = coords.x + 700;  // de fapt + calib_cam2
                coords.y = cps1y - coords.y;
                coords.y = coords.y * 700 / cps2y;
                //coords.y = (coords.y - (markerdela0,0 +- offset)/ (markerdela500,0 +- offset2) - (markerdela0,0 +- offset))
                p1.x = Markers[i][0].x;
                p1.y = Markers[i][0].y;
                p2.x = Markers[i][1].x;
                p2.y = Markers[i][1].y;

                pd = p1.y-p2.y/p1.x-p2.x;

                angle= atan2(p1.x-p2.x, p1.y-p2.y);
                angle = angle*180/3.1415 + 180;
                angle = fmod(angle+90,360);
                Point center(center_dreapta.x, center_dreapta.y);
                int radius = 33;
                circle( imagine_dr, center, 3, Scalar(0,255,0), -1, 8, 0 );// circle center
                circle( imagine_dr, center, radius, Scalar(0,0,255), 3, 8, 0 );// circle outline
                if (coords.id == 1) cerr<<"\n\nunghiul: "<<angle<<"\n\ncoord robot: x:" << coords.x<<" y: "<<coords.y<<endl;
                coords.angle = angle;
                coords.timestamp = std::time(0);
                mosquitto_publish(mosq, &mid, "coords", sizeof(coords), &coords, 0, true);
            }
        }
    }
    cv::imshow(mainWindow1,imagine_dr);
  //  cv::waitKey(50);
    }
  pthread_exit(NULL);
}

void* camera2(void *threadid) {

    cv::Mat HSVImage2;
    cv::Mat BallMask2;
    int hLow2 = 116;
    int sLow2 = 73;
    int vLow2 = 99;
    int hHigh2 = 141;
    int sHigh2 = 149;
    int vHigh2 = 169;
    int cpd1x=0, cpd1y=0, cpd2x=0, cpd2y=0; //pt partea stanga a terenului
    long tid;
    tid = (long)threadid;
    cv::Mat imagine_st;
    MarkerDetector MDetector_st;
    vector<Marker> Markers2;
    float MarkerSize_st=10; //0.008;

    while(1) {
        imagine_st = imagine_stanga.clone();
        cv::createTrackbar("H Low", mainWindowsettings2, &hLow2, 180, NULL);
        cv::createTrackbar("S Low", mainWindowsettings2, &sLow2, 255, NULL);
        cv::createTrackbar("V Low", mainWindowsettings2, &vLow2, 255, NULL);
        cv::createTrackbar("H High", mainWindowsettings2, &hHigh2, 180, NULL);
        cv::createTrackbar("S High", mainWindowsettings2, &sHigh2, 255, NULL);
        cv::createTrackbar("V High", mainWindowsettings2, &vHigh2, 255, NULL);

        cv::Scalar lowThreshold(hLow2, sLow2, vLow2, 0);   //pt minge
        cv::Scalar highThreshold(hHigh2, sHigh2, vHigh2, 0);

        cv::cvtColor(imagine_st, HSVImage2, CV_RGB2HSV);

        cv::inRange(HSVImage2, lowThreshold, highThreshold, BallMask2);

        Moments ballMoments2 = moments(BallMask2, false);

        Point   ballPosition;

        float ballArea2 = ballMoments2.m00;
        if(ballArea2 > 10) {
                ballPosition.x = ballMoments2.m10/ballArea2;
                ballPosition.y = ballMoments2.m01/ballArea2;
                circle(imagine_st, ballPosition, 10, Scalar(255, 255, 255), 2);
                robotCoords coords;
                coords.id = 0; // Id Ball
                coords.y  =  ballPosition.x;
                coords.x  =  ballPosition.y;
                coords.angle = 0;
            //    cerr<<ballPosition.x<<" "<<ballPosition.y<<endl;
                coords.timestamp = std::time(0);
                mosquitto_publish(mosq, &mid, "coords", sizeof(coords), &coords, 0, true);
            }

        bitwise_not(imagine_st, imagine_st);

        Mat roi2(imagine_st, Rect(Point (400,620), Point (560,719)));
        rectangle(imagine_st, Point (400,620), Point (560,719), Scalar(255,0,255),1,8, 0);
        vector<Vec3f> circles2;
        // Apply the Hough Transform to find the circles
        cvtColor( roi2, roi2, CV_BGR2GRAY );

        //cerr<<center_stanga.x<<" si pe y "<<center_stanga.y;
        //filtru sharpen
        cv::Mat tmp2;
        cv::GaussianBlur(imagine_st, tmp2, cv::Size(3,3), 5);
        cv::addWeighted(imagine_st, alfa2/100, tmp2, -beta2/100, 0, imagine_st);
        cv::createTrackbar("alfa", mainWindowsettings2, &alfa2, 300, NULL);
        cv::createTrackbar("beta", mainWindowsettings2, &beta2, 300, NULL);

        //read marker size if smarkerdela0,0pecified
        //Ok, let's detect
        MDetector_st.getCandidates();
        MDetector_st.setMinMaxSize(0.005, 0.5);

        MDetector_st.detect(imagine_st,Markers2,CamParam_stanga,MarkerSize_st);

        int nr2 = 0;
        if (onced ==0)
         {
            for (unsigned int i=0;i<Markers2.size();i++) {
                if (Markers2[i].id == 1023 || Markers2[i].id == 682)
                {
                Markers2[i].draw(imagine_st,Scalar(0,0,255),2);
                cv::Point2f markerCenter, p1, p2;

                markerCenter = Markers2[i].getCenter();
                if (markerCenter.x !=0 && markerCenter.y !=0) {
                    if (Markers2[i].id == 682)
                    {
                        cpd1y = markerCenter.x;
                        cpd1x = markerCenter.y;
                        nr2++;
                    }
                    if (Markers2[i].id == 1023)
                    {
                        cpd2y = markerCenter.x;
                        cpd2x = markerCenter.y;
                        nr2++;
                    }
                }
          //          cerr<<"poarta 1 stanga: x: "<<cpd1x<<" y: "<<cpd1y<<endl;

          //          cerr<<"poarta 2 stanga: x: "<<cpd2x<<" y: "<<cpd2y<<endl;
                }
            }
            HoughCircles( roi2, circles2, CV_HOUGH_GRADIENT, 1, 10, 200, 24, 0, 0 );
            cerr<<circles2.size();
            // Draw the circles detected
            for( size_t i = 0; i < circles2.size(); i++ )
              {
                  Point center2(cvRound(circles2[i][0])+400, cvRound(circles2[i][1])+620);
                  int radius2 = cvRound(circles2[i][2]);
                  circle( imagine_st, center2, 3, Scalar(0,255,0), -1, 8, 0 );// circle center
                  circle( imagine_st, center2, radius2, Scalar(0,0,255), 3, 8, 0 );// circle outline
                //  cout << "center : " << center2 << "\nradius : " << radius2 << endl;
                  center_stanga = center2;
              }
        if (nr2 == 2) {
            onced = 1;
            la0d = (cpd1x + cpd2x) / 2;
            cpd2y = cpd1y - cpd2y;
            calib_cam2 = (center_stanga.y - la0d) * 700 / (center_stanga.y - la0d);

            cerr<<"POARTA DIN STANGA pentru marker de la 0,0: "<< cpd1y<<"iar pt marker de la 0,700 "<<cpd2y<<" //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////";
            cerr<<"pentru cerc mijloc camera stanga: "<<center_stanga.x<<"cu "<<center_stanga.y<<" (x,y)\n";
            }
        }
        Point center2(center_stanga.x, center_stanga.y);
        int radius2 = 33;
        circle( imagine_st, center2, 3, Scalar(0,255,0), -1, 8, 0 );// circle center
        circle( imagine_st, center2, radius2, Scalar(0,0,255), 3, 8, 0 );// circle outline

         //for each marker, draw info and its boundaries in the image
        for (unsigned int i=0;i<Markers2.size();i++) {
            Markers2[i].draw(imagine_st,Scalar(0,0,255),2);
            cv::Point2f markerCenter, p1, p2;
            double pd, angle;
            robotCoords coords;

            markerCenter = Markers2[i].getCenter();
            if (Markers2[i].id != 682)
                if (Markers2[i].id != 1023) {  //daca nu este markerul de langa poarta
                coords.id = idAssocMat[Markers2[i].id];
                coords.y  = markerCenter.x;
                coords.x  = markerCenter.y;
               // if (coords.id ==6) cerr<<"coord camera 2nemodif x: "<<coords.x;
                if ((coords.x > la0d) && (coords.x < (center_stanga.y + 15))) { //hard ca sa tina cont de primul cerc

                    //scalare coordonate
                    coords.x = (coords.x - la0d) * 700 / (center_stanga.y - la0d);
                    coords.y = cpd1y - coords.y;
                    coords.y = coords.y * 700 / cpd2y;

                    //ca la camera 1 dar cu mici modificari
                    p1.x = Markers2[i][0].x;
                    p1.y = Markers2[i][0].y;
                    p2.x = Markers2[i][1].x;
                    p2.y = Markers2[i][1].y;

                    pd = p1.y-p2.y/p1.x-p2.x;

                    angle= atan2(p1.x-p2.x, p1.y-p2.y);
                    angle = angle*180/3.1415 + 180;
                    angle = fmod(angle+90,360);

             //     if (coords.id == 9) cerr<<"center_stanga pt x este: "<<calib_cam2<<"\n/ncoord robot: x:" << coords.x<<" y: "<<coords.y<<endl<<"/n/n";
                    if (coords.id == 1) cerr<<"\n\nunghiul: "<<angle<<"\n\ncoord robot: x:" << coords.x<<" y: "<<coords.y<<endl;

                    coords.angle = angle;
                    coords.timestamp = std::time(0);
                    mosquitto_publish(mosq, &mid, "coords", sizeof(coords), &coords, 0, true);
                }
            }
        }
        cv::imshow(mainWindow2,imagine_st);
        cv::waitKey(50);
    }
  pthread_exit(NULL);
}
void* preluare_camera_stanga (void *threadid) {

    long tid;
    tid = (long)threadid;
    int oncest = 0;
    VideoCapture vreader2(2);  //camera

    vreader2.set(CV_CAP_PROP_FRAME_WIDTH, 960);
    vreader2.set(CV_CAP_PROP_FRAME_HEIGHT, 720);

    cv::namedWindow(mainWindow2, CV_WINDOW_FULLSCREEN);

    //read camera parameters if specifed
    CamParam_stanga.readFromXMLFile("/etc/fr/camera.yml");

    while(1) {
    if (vreader2.isOpened()) {
        vreader2>>imagine_stanga;
    }
    //at this point, we should have the image in imagine_dreapta
    //if empty, exit
    if (imagine_stanga.total()==0) {
        cerr<<"Could not open input"<<endl;
        return 0;
    }
    else
        if (oncest==0) {
            CamParam_stanga.resize(imagine_stanga.size());
            pthread_create(&threads[3],NULL,camera2,(void*)1);
            oncest++;}

    //cv::imshow(mainWindow2,imagine_stanga);
    cv::waitKey(50);
    }
    pthread_exit(NULL);
}

void* preluare_camera_dreapta (void *threadid) {

    long tid;
    tid = (long)threadid;
    int once = 0;
    VideoCapture vreader(1);

    vreader.set(CV_CAP_PROP_FRAME_WIDTH, 960);
    vreader.set(CV_CAP_PROP_FRAME_HEIGHT, 720);

    cv::namedWindow(mainWindow1, CV_WINDOW_FULLSCREEN);

    CamParam_dreapta.readFromXMLFile("/etc/fr/camera.yml");

    while(1) {
        if (vreader.isOpened()) {
            vreader>>imagine_dreapta;
        }
        //at this point, we should have the image in imagine_dreapta
        //if empty, exit
        if (imagine_dreapta.total()==0) {
            cerr<<"Could not open input"<<endl;
            return 0;
        }
        else
            if (once ==0) {
                pthread_create(&threads[2],NULL,camera1,(void*)3);

                pthread_create(&threads[1],NULL,preluare_camera_stanga,(void*)2);

                CamParam_dreapta.resize( imagine_dreapta.size());
                once++;}

        //cv::imshow(mainWindow1,imagine_dreapta);
        cv::waitKey(50);
    }
    pthread_exit(NULL);
}

int main(int argc,char **argv)
{
    idAssocMat[0]    = 1;
    idAssocMat[81]   = 2;
    idAssocMat[277]  = 3;
    idAssocMat[325]  = 4;
    idAssocMat[321]  = 5;
    idAssocMat[1023] = 6; //sta la 0,700 pentru camera din stanga,

    idAssocMat[5]    = 7;
    idAssocMat[20]   = 8;
    idAssocMat[80]   = 9;
    idAssocMat[336]  = 10;
    idAssocMat[341]  = 11;
    idAssocMat[682]  = 12; //sta la 0,0 pentru camera din stanga,

    // TODO Find a better solution for this
    // system("uvcdynctrl -d video1 -s \"Exposure (Absolute)\" 100");
     pthread_create(&threads[0],NULL,preluare_camera_dreapta,(void*)1);

    char clientid[64]="FotbalRobotic.ro Tracking Server";
    mosquitto_lib_init();

    mosq = mosquitto_new(clientid, true, NULL);

    if(mosq) {
        mosquitto_connect(mosq, mqtt_host, mqtt_port, 60);
    }

    try
    {
        //read the input image
        //try opening first as video
        VideoCapture vreader(0);  //camera
        VideoCapture vreader2(1);  //camera de la 0,0

        cv::namedWindow(mainWindow1, CV_WINDOW_FULLSCREEN);
        cv::namedWindow(mainWindow2, CV_WINDOW_FULLSCREEN);

        // ATENTIE! AICI SETAM LA 1080 CU 720. IN CAMERA PARAMETERS MAI JOS AVEM 640 CU 480 PE CARE LE MODIFICAM LA 1080 CU 720??
        // intrebare: de ce folosim camparam?
        vreader.set(CV_CAP_PROP_FRAME_WIDTH, 960);
        vreader.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
        vreader2.set(CV_CAP_PROP_FRAME_WIDTH, 960);
        vreader2.set(CV_CAP_PROP_FRAME_HEIGHT, 720);

        // TODO important shit!
        // system("guvcview -d /dev/video1 -f MJPEG -s 960x720 -o");
        // system("guvcview -d /dev/video2 -f MJPEG -s 960x720 -o");

        // read camera parameters if specifed
        CamParam_stanga.readFromXMLFile("/etc/fr/camera_stanga.yml");
        CamParam_dreapta.readFromXMLFile("/etc/fr/camera.yml");
        // resizes the parameters to fit the size of the input image
        // FIXME Am comentat liniile de mai jos pentru InImage nu e declarat niciunde si nu se compileaza
        // CamParam_stanga.resize( InImage.size());
        // CamParam_dreapta.resize( InImage.size());

        while(1) {

            // Call mosquitto
            rc = mosquitto_loop(mosq, 1, 50);

            if(rc) {
                sleep(20);
                mosquitto_reconnect(mosq);
            }

        cv::waitKey(99);//wait for key to be pressed

        }

     //   if (argc>=5) cv::imwrite(argv[4],imagine_dreapta);
    } catch (std::exception &ex)

    {
        cout<<"Exception :"<<ex.what()<<endl;
    }
}
