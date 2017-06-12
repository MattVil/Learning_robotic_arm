#include "opencv/highgui.h"
#include "opencv/cv.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//entre 1 et 19
#define SMOOTH_LVL 19
 
int main() {
  
        char key;
        IplImage *image, *smooth;
        CvCapture *capture;
        capture = cvCaptureFromCAM(0);
  
        if (!capture) {
            printf("Ouverture du flux vidéo impossible !\n");
           return 1;
         }
         
        image = cvQueryFrame(capture);
        smooth = cvCloneImage(image);
         
        cvNamedWindow("Smooth", CV_WINDOW_AUTOSIZE);        
        cvNamedWindow("Camera", CV_WINDOW_AUTOSIZE);
         
        while(key != 'q' && key != 'Q') {        
            
            image = cvQueryFrame(capture);
            smooth = cvCloneImage(image);

            cvSmooth(image, smooth, CV_GAUSSIAN, SMOOTH_LVL);
                 
            cvShowImage( "Camera", image);
            cvShowImage( "Smooth", smooth);
            key = cvWaitKey(10);
         }

        cvReleaseImage(&smooth);
        cvReleaseImage(&image);
  
        cvReleaseCapture(&capture);
        cvDestroyWindow("Camera");
        cvDestroyWindow("Smooth");
  
        return 0;
  
}