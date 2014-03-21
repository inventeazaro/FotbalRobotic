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

#include "aruco/aruco.h"
#include "aruco/cvdrawingutils.h"

using namespace cv;
using namespace aruco;

int hLow1 = 116;
int sLow1 = 73;
int vLow1 = 99;
int hHigh1 = 141;
int sHigh1 = 149;
int vHigh1 = 169;

int hLow2 = 116;
int sLow2 = 73;
int vLow2 = 99;
int hHigh2 = 141;
int sHigh2 = 149;
int vHigh2 = 169;

int alfa = 150, alfa2 = 150;
int beta = 50, beta2 = 50;

//adaugat parametrii culoare verde
int hLow_00 = 70;
int sLow_00 = 89;
int vLow_00 = 89;
int hHigh_00 = 91;
int sHigh_00 = 255;
int vHigh_00 = 255;


// TODO calcul coordonate relativ la dimensiunea fiecarei jumatati de teren

int idAssocMat[1024] = {1};

#define mqtt_host "192.168.0.100"
#define mqtt_port 1883

struct robotCoords {
    int id;    // id robot - 1..10
    int x;     // 0...800  - relative la terenul de joc
    int y;     // 0...600
    int angle; // unghi fata de baza ringuluieu as fi avut nevoie de joi pana duminica in special
    int timestamp; // timpul cand au fost calculate - UNIXTIME - Ex: 1352460922
};

struct mosquitto *mosq;

const char mainWindow1[] = "FotbalRobotic Tracker camera 1";
const char mainWindowsettings1[] = "FotbalRobotic Tracker settings camera 1";
const char mainWindow2[] = "FotbalRobotic Tracker camera 2";
const char setariverde1[] = "Setari verde 1";
cv::Mat InImage;
cv::Mat InImage2;

cv::Mat HSVImage;
cv::Mat BallMask;
cv::Mat HSVImage2;
cv::Mat BallMask2;
cv::Mat VerdeMask;
cv::Mat VerdeMask2;
cv::Mat VerdeCerc;

int mid = 0;
int rc = 0;

int onces = 0;
int onced = 0;
int la0s = 0;
int la0d = 0;
int calib_cam2;
int cps1x=0, cps1y=0, cps2x=0, cps2y=0;

aruco::CameraParameters CamParam_stanga,CamParam_dreapta;
MarkerDetector MDetector;
vector<Marker> Markers, Markers2;
float MarkerSize=10; //0.008;

Point center_dreapta;

void camera1 () {

    cv::createTrackbar("H Low", mainWindowsettings1, &hLow1, 180, NULL);
    cv::createTrackbar("S Low", mainWindowsettings1, &sLow1, 255, NULL);
    cv::createTrackbar("V Low", mainWindowsettings1, &vLow1, 255, NULL);
    cv::createTrackbar("H High", mainWindowsettings1, &hHigh1, 180, NULL);
    cv::createTrackbar("S High", mainWindowsettings1, &sHigh1, 255, NULL);
    cv::createTrackbar("V High", mainWindowsettings1, &vHigh1, 255, NULL);

 /*   cv::createTrackbar("H Low", setariverde1, &hLow_00, 180, NULL);
    cv::createTrackbar("S Low", setariverde1, &sLow_00, 255, NULL);
    cv::createTrackbar("V Low", setariverde1, &vLow_00, 255, NULL);
    cv::createTrackbar("H High", setariverde1, &hHigh_00, 180, NULL);
    cv::createTrackbar("S High", setariverde1, &sHigh_00, 255, NULL);
    cv::createTrackbar("V High", setariverde1, &vHigh_00, 255, NULL);
*/
    cv::Scalar lowThreshold(hLow1, sLow1, vLow1, 0);   //pt minge
    cv::Scalar highThreshold(hHigh1, sHigh1, vHigh1, 0);

  //  cv::Scalar lowThreshold_00(hLow_00, sLow_00, vLow_00, 0);   //pt verde
  //  cv::Scalar highThreshOpenCV Error: Bad flag (parameter or structure field) (Unrecognized or unsupported array type) in cvGetMat, file /build/buildd/opencv-2.3.1/modules/core/src/array.cpp, line 2482

    cv::cvtColor(InImage, HSVImage, CV_RGB2HSV);

    cv::inRange(HSVImage, lowThreshold, highThreshold, BallMask);

//    cv::inRange(HSVImage, lowThreshold_00, highThreshold_00, VerdeMask);   //varde mask

    Moments ballMoments = moments(BallMask, false);

 //   Moments verdeMoments = moments(VerdeMask, false);

    Point   ballPosition;

 //   Point   verdePosition;

    float ballArea = ballMoments.m00;
//    float verdeArea = verdeMoments.m00;
    //int cps1x=0, cps1y=0, cps2x=0, cps2y=0;

    if(ballArea > 20) {
        ballPosition.x = ballMoments.m10/ballArea;
        ballPosition.y = ballMoments.m01/ballArea;
        circle(InImage, ballPosition, 10, Scalar(255, 255, 255), 2);
        robotCoords coords;
        coords.id = 0; // Id Ball
        coords.y  =  ballPosition.x;
        coords.x  =  ballPosition.y;
        coords.angle = 0;
       // cerr<<ballPosition.x<<" "<<ballPosition.y<<endl;
        coords.timestamp = std::time(0);
        mosquitto_publish(mosq, &mid, "coords", sizeof(coords), &coords, 0, true);
    }

 /*   if (verdeArea > 20) {
        verdePosition.y = verdeMoments.m10/verdeArea;
        verdePosition.x = verdeMoments.m01/verdeArea;
        circle(InImage, verdePosition, 27.3, Scalar(255, 255, 255), 2);
    }*/

    //facem regula de trei simpla astfel incat sa avem 0-500, 501-1000 coordonatele terenului.
    //mai trebuie sa luam coordonatele portilor pentru a stii exact unde este 0 si respectiv 1000

    imshow("ceva", BallMask);
 //   imshow("verde",VerdeMask);

    bitwise_not(InImage, InImage);

    Mat roi(InImage, Rect(Point (400,0), Point (560,130)));
    rectangle(InImage, Point (400,0), Point (560,100), Scalar(255,0,255),1,8, 0);
    vector<Vec3f> circles;
    // Apply the Hough Transform to find the circles
    cvtColor( roi, roi, CV_BGR2GRAY );


    //filtru sharpen
    cv::Mat tmp1;
    cv::GaussianBlur(InImage, tmp1, cv::Size(3,3), 5);
    cv::addWeighted(InImage, alfa/100, tmp1, -beta/100, 0, InImage);
    cv::createTrackbar("alfa", mainWindowsettings1, &alfa, 300, NULL);
    cv::createTrackbar("beta", mainWindowsettings1, &beta, 300, NULL);

    //read marker size if specified
    //Ok, let's detect
    MDetector.getCandidates();
    MDetector.setMinMaxSize(0.005, 0.5);

    MDetector.detect(InImage,Markers,CamParam_dreapta,MarkerSize);
    //cum consider ca ar fi bine: fac de doua ori, dar a doua oara adaug un parametru la coordonate.

    //for each marker, draw info and its boundaries in the image
    int nr = 0;
    if (onces ==0)
     {
        for (unsigned int i=0;i<Markers.size();i++) {
            if (Markers[i].id == 1023 || Markers[i].id == 682)
            {
            Markers[i].draw(InImage,Scalar(0,0,255),2);
            cv::Point2f markerCenter, p1, p2;

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
           //    cerr<<"poarta 1 stanga: x: "<<cps1x<<" y: "<<cps1y<<endl;

           //    cerr<<"poarta 2 stanga: x: "<<cps2x<<" y: "<<cps2y<<endl;
            }
        }
        HoughCircles( roi, circles, CV_HOUGH_GRADIENT, 1, 10, 100, 24, 0, 0 );
        cerr<<circles.size();
          // Draw the circles detected
          for( size_t i = 0; i < circles.size(); i++ )
          {
              Point center(cvRound(circles[i][0])+400, cvRound(circles[i][1]));
              int radius = cvRound(circles[i][2]);
              circle( InImage, center, 3, Scalar(0,255,0), -1, 8, 0 );// circle center
              circle( InImage, center, radius, Scalar(0,0,255), 3, 8, 0 );// circle outline
             // cout << "center : " << center << "\nradius : " << radius << endl;
              center_dreapta = center;
           //   cerr<<"\njtxj "<<center_dreapta.y;  //asta trb sa devina 700

           }
         ///
    if (nr == 2) {
        onces = 1;
        la0s = (cps1x + cps2x) / 2;
        cps2y = cps1y - cps2y;
        cerr<<"pentru marker de la 0,0: "<< cps1y<<"iar pt marker de la 0,700 "<<cps2y<<" //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////";
        }
    }

   // cerr<<"\n\n coord cam 2 calib_cam2la mijlocul terenului: "<<center_dreapta.y<<" iar la poarta din dreapta: "<<la0s<<"\n\n";
    for (unsigned int i=0;i<Markers.size();i++) {
        //cout<<Markers[i]<<endl;
        Markers[i].draw(InImage,Scalar(0,0,255),2);
        double position[3];
        double orientation[4];

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
                //cout<<"Id:"<< coords.id << "Ang: "<<angle<<endl;
                if (coords.id == 1) cerr<<"\n\nunghiul: "<<angle<<"\n\ncoord robot: x:" << coords.x<<" y: "<<coords.y<<endl;
                coords.angle = angle;
                coords.timestamp = std::time(0);
                mosquitto_publish(mosq, &mid, "coords", sizeof(coords), &coords, 0, true);
            }
        }

    }


}

Point center_stanga;
int cpd1x=0, cpd1y=0, cpd2x=0, cpd2y=0; //pt partea stanga a terenului

void camera2() {
    cv::createTrackbar("H Low", mainWindow2, &hLow2, 180, NULL);
    cv::createTrackbar("S Low", mainWindow2, &sLow2, 255, NULL);
    cv::createTrackbar("V Low", mainWindow2, &vLow2, 255, NULL);
    cv::createTrackbar("H High", mainWindow2, &hHigh2, 180, NULL);
    cv::createTrackbar("S High", mainWindow2, &sHigh2, 255, NULL);
    cv::createTrackbar("V High", mainWindow2, &vHigh2, 255, NULL);

    cv::Scalar lowThreshold(hLow2, sLow2, vLow2, 0);   //pt minge
    cv::Scalar highThreshold(hHigh2, sHigh2, vHigh2, 0);

   // cv::Scalar lowThreshold_00(hLow_00, sLow_00, vLow3737_00, 0);   //pt verde
  //  cv::Scalar highThreshold_00(hHigh_00, sHigh_00, vHigh_00, 0);

    cv::cvtColor(InImage2, HSVImage2, CV_RGB2HSV);

    cv::inRange(HSVImage2, lowThreshold, highThreshold, BallMask2);

   // cv::inRange(HSVImage2, lowThreshold_00, highThreshold_00, VerdeMask2);   //varde mask

    Moments ballMoments2 = moments(BallMask2, false);

    //Moments verdeMoments2 = moments(VerdeMask2, false);

    Point   ballPosition;

  //  Point   verdePosition2;

    float ballArea2 = ballMoments2.m00;
  //  float verdeArea2 = verdeMoments2.m00;

    if(ballArea2 > 10) {
            ballPosition.x = ballMoments2.m10/ballArea2;
            ballPosition.y = ballMoments2.m01/ballArea2;
            circle(InImage2, ballPosition, 10, Scalar(255, 255, 255), 2);
            robotCoords coords;
            coords.id = 0; // Id Ball
            coords.y  =  ballPosition.x;
            coords.x  =  ballPosition.y;
            coords.angle = 0;
        //    cerr<<ballPosition.x<<" "<<ballPosition.y<<endl;
            coords.timestamp = std::time(0);
            mosquitto_publish(mosq, &mid, "coords", sizeof(coords), &coords, 0, true);
        }

    //facem regula de trei simpla astfel incat sa avem 0-500, 501-1000 coordonatele terenului.
    //mai trebuie sa luam coordonatele portilor pentru a stii exact unde este 0 si respectiv 1000

    bitwise_not(InImage2, InImage2);

    Mat roi2(InImage2, Rect(Point (400,620), Point (560,720)));
    rectangle(InImage2, Point (400,620), Point (560,720), Scalar(255,0,255),1,8, 0);
    vector<Vec3f> circles2;
    // Apply the Hough Transform to find the circles
    cvtColor( roi2, roi2, CV_BGR2GRAY );

    //cerr<<center_stanga.x<<" si pe y "<<center_stanga.y;
    //filtru sharpen
    cv::Mat tmp2;
    cv::GaussianBlur(InImage2, tmp2, cv::Size(3,3), 5);
    cv::addWeighted(InImage2, alfa2/100, tmp2, -beta2/100, 0, InImage2);
    cv::createTrackbar("alfa", mainWindow2, &alfa2, 300, NULL);
    cv::createTrackbar("beta", mainWindow2, &beta2, 300, NULL);

    //read marker size if smarkerdela0,0pecified
    //Ok, let's detect
    MDetector.getCandidates();
    MDetector.setMinMaxSize(0.005, 0.5);

    MDetector.detect(InImage2,Markers2,CamParam_stanga,MarkerSize); //este posibil sa fie bine asa, daca nu reinitializeaza vectorul markers. altfel va fi mai enervant

    int nr2 = 0;
    if (onced ==0)
     {
        for (unsigned int i=0;i<Markers2.size();i++) {
            if (Markers2[i].id == 1023 || Markers2[i].id == 682)
            {
            Markers2[i].draw(InImage,Scalar(0,0,255),2);
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
      //          cerr<<"muierhgepoarta 1 stanga: x: "<<cpd1x<<" y: "<<cpd1y<<endl;

      //          cerr<<"miweeuiwpoarta 2 stanga: x: "<<cpd2x<<" y: "<<cpd2y<<endl;
            }
        }
        HoughCircles( roi2, circles2, CV_HOUGH_GRADIENT, 1, 10, 200, 24, 0, 0 );
        //cerr<<circles2.size();
        // Draw the circles detected
        for( size_t i = 0; i < circles2.size(); i++ )
          {
              Point center2(cvRound(circles2[i][0])+400, cvRound(circles2[i][1])+620);
              int radius2 = cvRound(circles2[i][2]);
              circle( InImage2, center2, 3, Scalar(0,255,0), -1, 8, 0 );// circle center
              circle( InImage2, center2, radius2, Scalar(0,0,255), 3, 8, 0 );// circle outline
            //  cout << "center : " << center2 << "\nradius : " << radius2 << endl;
              center_stanga = center2;
          }
    if (nr2 == 2) {
        onced = 1;
        la0d = (cpd1x + cpd2x) / 2;
        cpd2y = cpd1y - cpd2y;
        calib_cam2 = (center_stanga.y - la0d) * 700 / (center_stanga.y - la0d);

       // cerr<<"pentru marker de la 0,0: "<< cpd1y<<"iar pt marker de la 0,700 "<<cpd2y<<" //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////";
        }
    }
     //for each marker, draw info and its boundaries in the image
    for (unsigned int i=0;i<Markers2.size();i++) {
        //cout<<Markers2[i]<<endl;
        Markers2[i].draw(InImage2,Scalar(0,0,255),2);
        double position[3];
        double orientation[4];

        cv::Point2f markerCenter, p1, p2;
        double pd, angle;
        robotCoords coords;

        markerCenter = Markers2[i].getCenter();
        if (Markers2[i].id != 682)
            if (Markers2[i].id != 1023) {  //daca nu este markerul de langa poarta

            coords.id = idAssocMat[Markers2[i].id];
            coords.y  = markerCenter.x;
            coords.x  = markerCenter.y;
           // cerr<<coords.id;
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

            //    cout<<"Id:"<< coords.id << "Ang: "<<angle<<endl;
           //     if (coords.id == 9) cerr<<"center_stanga pt x este: "<<calib_cam2<<"\n/ncoord robot: x:" << coords.x<<" y: "<<coords.y<<endl<<"/n/n";
                if (coords.id == 1) cerr<<"\n\nunghiul: "<<angle<<"\n\ncoord robot: x:" << coords.x<<" y: "<<coords.y<<endl;

                coords.angle = angle;
                coords.timestamp = std::time(0);
                mosquitto_publish(mosq, &mid, "coords", sizeof(coords), &coords, 0, true);
            }
        }
    }
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
        VideoCapture vreader(1);  //camera
        VideoCapture vreader2(2);  //camera de la 0,0

        cv::namedWindow(mainWindow1, CV_WINDOW_FULLSCREEN);
        cv::namedWindow(mainWindow2, CV_WINDOW_FULLSCREEN);

        //ATENTIE! AICI SETAM LA 1080 CU 720. IN CAMERA PARAMETERS MAI JOS AVEM 640 CU 480 PE CARE LE MODIFICAM LA 1080 CU 720??
        //intrebare: de ce folosim camparam?
        vreader.set(CV_CAP_PROP_FRAME_WIDTH, 960);
        vreader.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
        vreader2.set(CV_CAP_PROP_FRAME_WIDTH, 960);
        vreader2.set(CV_CAP_PROP_FRAME_HEIGHT, 720);

        //TODO important shit!
        //system("guvcview -d /dev/video1 -f MJPEG -s 960x720 -o");
        //system("guvcview -d /dev/video2 -f MJPEG -s 960x720 -o");

        //read camera parameters if specifed
        CamParam_stanga.readFromXMLFile("/etc/fr/camera_stanga.yml");
        CamParam_dreapta.readFromXMLFile("/etc/fr/camera.yml");
        //resizes the parameters to fit the size of the input image
        CamParam_stanga.resize( InImage.size());
        CamParam_dreapta.resize( InImage.size());

        while(1) {

        if (vreader.isOpened()) {
            vreader>>InImage;
        }
        if (vreader2.isOpened()) {
            vreader2>>InImage2;
        }

        //at this point, we should have the image in InImage
        //if empty, exit
        if (InImage.total()==0) {
            cerr<<"Could not open input"<<endl;
            return 0;
        }

        if (InImage2.total()==0) {
            cerr<<"Could not open input"<<endl;
            return 0;
        }

        camera1();

        camera2(); //cea de la 0,0 adica din stanga

        // Call mosquitto
        rc = mosquitto_loop(mosq, 1, 50);

        if(rc) {
            sleep(20);
            mosquitto_reconnect(mosq);
        }

        cv::imshow(mainWindow1,InImage);

        cv::imshow(mainWindow2,InImage2);

        cv::imshow(mainWindowsettings1,InImage);
        //show also the internal image resulting from the threshold operation
        //cv::imshow("thes", MDetector.getThresholdedImage() );
        cv::waitKey(99);//wait for key to be pressed

        }

        if (argc>=5) cv::imwrite(argv[4],InImage);
    } catch (std::exception &ex)

    {
        cout<<"Exception :"<<ex.what()<<endl;
    }
}
