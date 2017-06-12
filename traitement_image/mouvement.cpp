#include "opencv/highgui.h"
#include "opencv/cv.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
 
int main() {
  
        char key;
        IplImage *image, *fond;
        CvCapture *capture;
        capture = cvCaptureFromCAM(1);
  
        if (!capture) {
            printf("Ouverture du flux vidéo impossible !\n");
           return 1;
         }
         
        image = cvQueryFrame(capture);
        fond = cvCloneImage(image);
         
        cvNamedWindow("Mouvement", CV_WINDOW_AUTOSIZE);        
        cvNamedWindow("Camera", CV_WINDOW_AUTOSIZE);
 
        IplImage *moi = cvCloneImage(image);
         
        while(key != 'q' && key != 'Q') {        
            fond = cvCloneImage(image);
            image = cvQueryFrame(capture); 
                         
            cvCopyImage(image, moi);
            CvScalar px1, px2, dif;
            for (int i =0;i<image->height;i++){
                for (int j =0;j<image->width;j++){                       
                    px1 = cvGet2D(image,i,j);        
                    px2 = cvGet2D(fond,i,j);
                    dif.val[0] = abs(cvRound(px1.val[0] - px2.val[0]));
                    dif.val[1] = abs(cvRound(px1.val[1] - px2.val[1]));
                    dif.val[2] = abs(cvRound(px1.val[2] - px2.val[2]));
                    cvSet2D(moi,i,j,dif);        
                } 
            }
                 
            cvShowImage( "Camera", image);
            cvShowImage( "Mouvement", moi);
            key = cvWaitKey(10);
         }


         cvReleaseImage(&moi);
         cvReleaseImage(&fond);
  
        cvReleaseCapture(&capture);
        cvDestroyWindow("Camera");
        cvDestroyWindow("Mouvement");
  
        return 0;
  
}