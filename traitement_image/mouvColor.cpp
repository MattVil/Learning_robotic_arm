#include "opencv/highgui.h"
#include "opencv/cv.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define NUM_CAM 0

#define TOLERANCE_COLOR 25
#define TOLERANCE_MOUVEMENT 20
#define BRUIT 5

CvPoint isolateColor(IplImage* image);
CvPoint isolateMouvement(IplImage* image, IplImage* pastImage);
void applyFilterOnImage(IplImage* image, const char* filter, int lvl);
void getColor(int event, int x, int y, int flags, void *param);
void showCentroid(CvPoint centroidColor, CvPoint centroidMouvement, const char* window);

//image from the camera
IplImage *image;
//variable for the selected color
int h = 0, s = 0, v = 0;

int main() {
  
        char key;
        CvCapture *capture;
        capture = cvCaptureFromCAM(NUM_CAM);

        CvPoint centroidColor, centroidMouvement;
  
        if (!capture) {
            printf("ERROR ! Can't open the video stream !\n");
           return 1;
        }

        image = cvQueryFrame(capture);

        printf("height : %d\twidth : %d\n", image->height, image->width);
            
        cvNamedWindow("Origin", CV_WINDOW_AUTOSIZE);
        cvNamedWindow("MaskMouvement", CV_WINDOW_AUTOSIZE);
        cvNamedWindow("MaskColor", CV_WINDOW_AUTOSIZE);
        cvNamedWindow("Centroid", CV_WINDOW_AUTOSIZE);
        cvMoveWindow("Origin", 50, 0);
        cvMoveWindow("Centroid", 750, 0);
        cvMoveWindow("MaskMouvement", 50, 600);
        cvMoveWindow("MaskColor", 750, 600);

        //mouse event for the color selection
        cvSetMouseCallback("Origin", getColor);    
         
        while(key != 'q' && key != 'Q') {   

            IplImage* pastImage = cvCloneImage(image);
            image = cvQueryFrame(capture);     
            cvShowImage("Origin", image);

            
            applyFilterOnImage(image, "CV_GAUSSIAN", 19);
            centroidMouvement = isolateMouvement(image, pastImage);
            centroidColor = isolateColor(image);

            showCentroid(centroidColor, centroidMouvement, "Centroid");

            cvReleaseImage(&pastImage);

            key = cvWaitKey(10);
        }


        cvReleaseImage(&image);
  
        cvReleaseCapture(&capture);
        cvDestroyWindow("Origin");
        cvDestroyWindow("MaskMouvement");
        cvDestroyWindow("MaskColor");
  
        return 0;
  
}

/*
This fonction isolate the selected color on the video stream and return the centroid of 
the isolates pixels
*/
CvPoint isolateColor(IplImage* image){
    int x, y;
    CvScalar pixel;
    IplImage *hsv, *maskColor;
    IplConvKernel *kernel;
    int sommeX = 0, sommeY = 0;
    int nbPixels = 0;
    
    maskColor = cvCreateImage(cvGetSize(image), image->depth, 1);
 
    //copy and conversion in HSV
    hsv = cvCloneImage(image); 
    cvCvtColor(image, hsv, CV_BGR2HSV);
 
    //padding of the mask with the pixels with the good color of image
    cvInRangeS(hsv, cvScalar(h - TOLERANCE_COLOR -1, s - TOLERANCE_COLOR, 100), cvScalar(h + TOLERANCE_COLOR -1, s + TOLERANCE_COLOR, 255), maskColor);
 
    //create a kernel of size 5*5
    kernel = cvCreateStructuringElementEx(5, 5, 2, 2, CV_SHAPE_ELLIPSE);
    
    //erosion and dilatation
    cvErode(maskColor, maskColor, kernel, 1);  
    cvDilate(maskColor, maskColor, kernel, 1); 
    cvDilate(maskColor, maskColor, kernel, 1); 


    
    //summe of the coordinate of our white pixels
    for(x = 0; x < maskColor->width; x++) {
        for(y = 0; y < maskColor->height; y++) { 
 
            if(((uchar *)(maskColor->imageData + y*maskColor->widthStep))[x] == 255) {
                sommeX += x;
                sommeY += y;
                nbPixels++;
            }
        }
    }
    
    //return the centroid or (-1,-1)
    CvPoint centroid;

    if(nbPixels > BRUIT){
        centroid = cvPoint((int)(sommeX / nbPixels), (int)(sommeY / nbPixels));
        //showCentroid(maskColor, centroid, CV_RGB(255, 0, 0), "MaskColor");
    }
    else{
        centroid = cvPoint(-1, -1);
    }

     cvShowImage("MaskColor", maskColor);
 
    cvReleaseStructuringElement(&kernel);
 
    cvReleaseImage(&maskColor);
    cvReleaseImage(&hsv);

    return centroid;
}


/*
This function isolate the zone witch move in the video stream
*/
CvPoint isolateMouvement(IplImage* image, IplImage* pastImage){

    int x, y;
    int sommeX = 0, sommeY = 0;
    int nbPixels = 0;

    IplImage *maskMouvement;
    //IplImage *hsv, *hsvPast;

    //pixels for the different images
    CvScalar pxCurrent, pxPast, pxDif;

    //conversion in HSV
    //hsv = cvCloneImage(image); 
    //hsvPast = cvCloneImage(pastImage);
    //cvCvtColor(image, hsv, CV_BGR2HSV);
    //cvCvtColor(pastImage, hsvPast, CV_BGR2HSV);

    maskMouvement = cvCloneImage(image);

    int i, j;
    for (i = 0; i < image->height; i++){
        for(j=0; j < image->width; j++){
            pxCurrent = cvGet2D(image, i, j);
            pxPast = cvGet2D(pastImage, i, j);

            pxDif.val[0] = abs(cvRound(pxCurrent.val[0] - pxPast.val[0]));
            pxDif.val[1] = abs(cvRound(pxCurrent.val[1] - pxPast.val[1]));
            pxDif.val[2] = abs(cvRound(pxCurrent.val[2] - pxPast.val[2]));

            //cvSet2D(maskMouvement, i, j, pxDif);

            /* if the diffrence between past and current pixel is higher as a tolerance,
                we print it in white, else in black */
            if(pxDif.val[0] > TOLERANCE_MOUVEMENT || pxDif.val[1] > TOLERANCE_MOUVEMENT || pxDif.val[2] > TOLERANCE_MOUVEMENT){
                CvScalar white;
                white.val[0] = 255;
                white.val[1] = 255;
                white.val[2] = 255;

                //We summe the number of white pixels to calcul the centroid
                sommeX += i;
                sommeY += j;
                nbPixels++;

                cvSet2D(maskMouvement, i, j, white);
            }
            else{
                CvScalar black;
                black.val[0] = 0;
                black.val[1] = 0;
                black.val[2] = 0;

                cvSet2D(maskMouvement, i, j, black);
            }
        }
    }

    //create a kernel of size 5*5
    IplConvKernel* kernel = cvCreateStructuringElementEx(5, 5, 2, 2, CV_SHAPE_ELLIPSE);
    
    //erosion and dilatation
    //cvErode(maskMouvement, maskMouvement, kernel, 1);
    //cvDilate(maskMouvement, maskMouvement, kernel, 1);
    cvDilate(maskMouvement, maskMouvement, kernel, 1);
    cvShowImage("MaskMouvement", maskMouvement);
    
    //return the centroid or (-1,-1)
    CvPoint centroid;

    if(nbPixels > BRUIT){
        centroid = cvPoint((int)(sommeY / nbPixels), (int)(sommeX / nbPixels));
        //showCentroid(maskColor, centroid, CV_RGB(255, 0, 0), "MaskColor");
    }
    else{
        centroid = cvPoint(-1, -1);
    }

    //cvReleaseImage(&hsv);
    //cvReleaseImage(&hsvPast);
    cvReleaseImage(&maskMouvement);

    return centroid;
}

/*
This function transform a IplImage with diffrent filter
*/
void applyFilterOnImage(IplImage* image, const char* filter, int lvl){

    IplImage* tmp = cvCloneImage(image);

    if(filter == "CV_GAUSSIAN"){
        cvSmooth(image, tmp, CV_GAUSSIAN, lvl);
        cvCopy(tmp, image);
    }
    else if(filter == "CV_BLUR_NO_SCALE"){
        cvSmooth(image, tmp, CV_BLUR_NO_SCALE, lvl);
        cvCopy(tmp, image);
    }
    else if(filter == "CV_BLUR"){
        cvSmooth(image, tmp, CV_BLUR, lvl);
        cvCopy(tmp, image);
    }
    else if(filter == "CV_MEDIAN"){
        cvSmooth(image, tmp, CV_MEDIAN, lvl);
        cvCopy(tmp, image);
    }
    else if(filter == "CV_BILATERAL"){
        cvSmooth(image, tmp, CV_BILATERAL, lvl);
        cvCopy(tmp, image);
    }
    else{
        printf("ERROR with filtering !\n");
    }

    cvReleaseImage(&tmp);
}


/*
This function initilize the h s v color variable with the color of the pixel we clicked on
*/
void getColor(int event, int x, int y, int flags, void *param) {
 
    CvScalar pixel;
    IplImage *hsv;
    
    //left clic
    if(event == CV_EVENT_LBUTTONUP) {

        //hsv convertion of the image
        hsv = cvCloneImage(image);
        cvCvtColor(image, hsv, CV_BGR2HSV);
        
        //extraction of the pixel
        pixel = cvGet2D(hsv, y, x);
        
        //initialization of the color variable
        h = (int)pixel.val[0];
        s = (int)pixel.val[1];
        v = (int)pixel.val[2];
 
        cvReleaseImage(&hsv);
    }
}


/*
This function add a point on the video stream to see the position of the centroid
*/
void showCentroid(CvPoint centroidColor, CvPoint centroidMouvement, const char* window){
    IplImage* centroidImage;
    centroidImage = cvCloneImage(image);

    cvDrawCircle(centroidImage, centroidColor, 10, CV_RGB(255, 0, 0), -1);
    cvDrawCircle(centroidImage, centroidMouvement, 10, CV_RGB(0, 0, 255), -1);
    cvShowImage(window, centroidImage);

    cvReleaseImage(&centroidImage);
}