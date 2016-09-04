#include <highgui.h>
#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	const char* vidname = argc > 1 ? argv[1] : "/home/ps/Videos/demo.avi";
	VideoCapture cap(vidname);
	int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	int l=cap.get(CV_CAP_PROP_FRAME_COUNT);
	int lout=0;
	cout<<l<<endl;
	waitKey(0);
	VideoWriter video("/home/ps/Videos/synopsisstage1.avi",CV_FOURCC('P','I','M','1'),20, Size(frame_width,frame_height),true);
	if ( !video.isOpened() ) 
	{
	       cout << "ERROR: Failed to write the video" << endl;
	       return -1;
	}
	if(!cap.isOpened())
	{
	    cout<<"failed to open image sequence";
	    return 1;
	}
	char c;
	int newf=0;
	float dist;
	Mat diffImage, tempc,tempn;
	Mat foregroundMask;
	Mat fcurrent, fnext;
	float threshold = 50.0f;
	int tval=3000;
	int cnt=0;
	cap>>tempc;
	cvtColor(tempc, fcurrent, CV_BGR2GRAY);
	while(1)
	{
		cap>>tempn;
		if(tempn.empty()){
			break;
		}
		cvtColor(tempn, fnext, CV_BGR2GRAY);
		
		absdiff(fcurrent, fnext, diffImage);
    	foregroundMask = Mat::zeros(diffImage.rows, diffImage.cols, CV_8UC1);

    	for(int j=0; j<diffImage.rows; ++j)
    	{
        	for(int i=0; i<diffImage.cols; ++i)
    		{
            	float intensity = diffImage.at<uchar>(j, i);
            	dist=intensity;
            	if(dist>threshold)
            	{
                	foregroundMask.at<uchar>(j,i) = 255;
                	cnt++;
            	}
        	}
        }
        cout<<"Activity in "<<cnt<<" pixels"<<endl;
        if(cnt>tval){
        	video.write(tempc);
        	tempc=tempn.clone();
        	cvtColor(tempc, fcurrent, CV_BGR2GRAY);
        	newf++;
        }
        cnt=0;


	}
	video.write(tempc);
	newf++;
	cout<<"original: "<<l<<"new: "<<newf<<endl;
	return 0;
}

	