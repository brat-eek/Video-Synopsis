#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/video.hpp>
using namespace cv;
using namespace std;

struct obj{
  int q=0;
  int location[1000][5];
}tmp;

typedef struct obj obj;


int thresh = 50;
int thing_cnt=0;
int frame_cnt=1;
int max_thresh = 255;
obj things[200];


Mat frame; 
Mat fgMaskMOG2; 
Ptr<BackgroundSubtractor> pMOG2;

void routine(Mat &diffImage, Mat &fr){
	
	Mat threshold_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  int k,br1,br2,tl1,tl2;
  	
  threshold( diffImage, threshold_output, thresh, 255, THRESH_BINARY );
  	/// Find contours
  	//imshow("bw",threshold_output);
  findContours( threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
  	//cout<<contours.size()<<endl; 	
  	//waitKey(0);	
  	vector<vector<Point> > contours_poly( contours.size() );
  	vector<Rect> boundRect( contours.size() );
    vector<Rect> boundRectnew( contours.size() );

  	for(int i=0; i<contours.size(); i++){
  		approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
      boundRect[i] = boundingRect( Mat(contours_poly[i]) );
  	}

    /*for(int i=0; i<contours.size()-1; i++){
      br1=(boundRect[i].br()).x;
      br2=(boundRect[i+1].br()).x;
      tl1=(boundRect[i].tl()).x;
      tl2=(boundRect[i+1].tl()).x;
      if((br1<br2 && br1>tl2) || (br2<br1 && br2>tl1)){
        cout<<"yo"<<endl;
      }
    }*/
  	namedWindow( "Contours", 0 );
  	Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
  	for( int i = 0; i< contours.size(); i++ ){
       //Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       Scalar color = Scalar( 0, 255, 0 );
       if((boundRect[i].br()).x-(boundRect[i].tl()).x > 22 && (boundRect[i].br()).y-(boundRect[i].tl()).y > 30){
          rectangle( fr, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
          /*if(frame_cnt==1){
            things[thing_cnt].location[things[thing_cnt].q][0]=(boundRect[i].tl()).x;
            things[thing_cnt].location[things[thing_cnt].q][1]=(boundRect[i].br()).x;
            things[thing_cnt].location[things[thing_cnt].q][2]=(boundRect[i].tl()).y;
            things[thing_cnt].location[things[thing_cnt].q][3]=(boundRect[i].br()).y;
            things[thing_cnt].location[things[thing_cnt].q][4]=frame_cnt;
          }*/
          //waitKey(0);
        }
     }

     imshow( "Contours", fr);
}


int main(int argc, char* argv[])
{
	const char* vidname = argc > 1 ? argv[1] : "/home/ps/Videos/demo.avi";
	VideoCapture capture(vidname);
	
  /*obj var1;
  var1.location[0][0]=5;
  cout<<var1.location[0][0]<<endl;
  waitKey(0);*/

  int keyboard;
  pMOG2 = createBackgroundSubtractorMOG2();
  while(1){
        if(!capture.read(frame)) {
            cout << "Unable to read next frame." << endl;
            cout << "Exiting..." << endl;
            return 0;
        }
        //update the background model
        //pMOG->apply(frame, fgMaskMOG);
        pMOG2->apply(frame, fgMaskMOG2);
        routine(fgMaskMOG2,frame);
        frame_cnt++;
        waitKey(0);
    }

    capture.release();
    return 0;
}























