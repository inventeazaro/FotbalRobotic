#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>


int g_slider_position_h1 = 0;
int g_slider_position_s1 = 0;
int g_slider_position_v1 = 0;

int g_slider_position_h2 = 255;
int g_slider_position_s2 = 255;
int g_slider_position_v2 = 255;

int g_slider_position_smooth = 0;

int g_slider_position_cam = 1 ;

IplImage *src_image;
IplImage *hsv_image;

CvScalar  hsv_min = cvScalar(120, 120, 120, 120);
CvScalar  hsv_max = cvScalar(134, 134, 134, 134);


int main(  )
{

    int key = 0;

    CvCapture* capture = cvCaptureFromCAM(0);
    CvSize sz =  cvGetSize(cvQueryFrame( capture));

    src_image = cvCreateImage( sz, 8, 3);
    hsv_image = cvCreateImage( sz, 8, 3);
    IplImage* hsv_filt  = cvCreateImage( sz, IPL_DEPTH_8U , 1);
    IplImage* hsv_smooth  = cvCreateImage( sz, IPL_DEPTH_8U , 1);

    cvNamedWindow( "src",0);
    cvNamedWindow( "filterd",1);

    while( key != 27)
    {

        cvCreateTrackbar("Hue 1","src",&g_slider_position_h1,180,NULL);
        cvCreateTrackbar("Sat 1","src",&g_slider_position_s1,255,NULL);
        cvCreateTrackbar("Val 1","src",&g_slider_position_v1,255,NULL);

        cvCreateTrackbar("Hue 2","src",&g_slider_position_h2,180,NULL);
        cvCreateTrackbar("Sat 2","src",&g_slider_position_s2,255,NULL);
        cvCreateTrackbar("Val 2","src",&g_slider_position_v2,255,NULL);

        cvCreateTrackbar("Smooth","src",&g_slider_position_smooth,100,NULL);

        cvCreateTrackbar("Cam ","src",&g_slider_position_cam,1,NULL);


        hsv_min.val[0] = g_slider_position_h1;
        hsv_min.val[1] = g_slider_position_s1;
        hsv_min.val[2] = g_slider_position_v1;

        hsv_max.val[0] = g_slider_position_h2;
        hsv_max.val[1] = g_slider_position_s2;
        hsv_max.val[2] = g_slider_position_v2;

        src_image = cvQueryFrame( capture);
        cvCvtColor(src_image, hsv_image, CV_BGR2HSV);
        cvInRangeS (hsv_image, hsv_min, hsv_max, hsv_filt);

        if(g_slider_position_smooth == 0 )
        {
            cvShowImage("src",src_image);
            cvShowImage("filterd",hsv_filt);
        }
        else if(g_slider_position_smooth%2 != 0 )
        {
            cvSmooth( hsv_filt, hsv_smooth, CV_MEDIAN,g_slider_position_smooth, 0, 0, 0 );
            cvShowImage("src",src_image);
            cvShowImage("filterd",hsv_smooth);
        }
        else
        {
            cvSmooth( hsv_filt, hsv_smooth, CV_MEDIAN,g_slider_position_smooth+1, 0, 0, 0 );
            cvShowImage("src",src_image);
            cvShowImage("filterd",hsv_smooth);

        }

        if(g_slider_position_cam == 0)
        {
            cvReleaseCapture( &capture);
            while((g_slider_position_cam == 0)&& key != 27) key = cvWaitKey(100);
            capture = cvCaptureFromCAM(0);
        }

        else if(g_slider_position_cam == 1)key = cvWaitKey( 30 );
    }

return 1;
}
