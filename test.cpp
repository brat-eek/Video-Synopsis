#include<bits/stdc++.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

static int thold=15;

void prirgb(Mat &frame){
	int r, g, b, val;
    for (int i = 0; i < frame.rows; ++i)
    {
        Vec3b* pixel = frame.ptr<Vec3b>(i); // point to first pixel in row
        for (int j = 0; j < frame.cols; ++j)
        {
            r = (pixel[j][2]);
            g = (pixel[j][1]);
            b = (pixel[j][0]);
            val=pixel[j][2]+pixel[j][1];
            cout<<r<<" "<<g<<" "<<b<<endl;
        	cout<<val<<endl;
        	waitKey(0);
        }

    }
}

void pri(Mat &gframe){
	int piy;
    for (int i = 0; i < gframe.rows; ++i)
    {
        uchar* pixel = gframe.ptr<uchar>(i);  // point to first color in row
        for (int j = 0; j < gframe.cols; ++j)
        {
            piy = pixel[j];
            cout<<piy<<endl;
        	waitKey(0);
        }
        
    }
}

int activity(vector<Mat> &cuboid,int t,int y,int x){
	int act=0;
	int bracket=10;
	int a[2*bracket+1];
	int pos=0;

	for(int i=t-bracket;i<=t+bracket;i++){
		if(i<0){
			a[pos]=cuboid[t].at<uchar>(Point(x,y));
			pos++;
		}
		else{
			a[pos]=cuboid[i].at<uchar>(Point(x,y));
			pos++;	
		}
		
	}
	/*for(int i=0;i<2*bracket+1;i++){
			cout<<a[i]<<" ";
	}
	cout<<endl;*/
	sort(a, a+(2*bracket+1));
	/*for(int i=0;i<2*bracket+1;i++){
			cout<<a[i]<<" ";
	}*/	
	act=a[bracket];
	return act;
}

int main(int argc, char* argv[])
{
	const char* vidname = argc > 1 ? argv[1] : "/home/ps/Videos/demo.avi";
	VideoCapture cap(vidname);
	Mat frame,gframe;
	int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	int l=cap.get(CV_CAP_PROP_FRAME_COUNT);

	cout<<frame_height<<endl;
	cout<<frame_width<<endl;
	cout<<l<<endl;

	
	VideoWriter video("/home/ps/Videos/activity.avi",CV_FOURCC('M','J','P','G'),20, Size(frame_width,frame_height),false);
	if ( !video.isOpened() ) 
	{
	       cout << "ERROR: Failed to write the video" << endl;
	       return -1;
	}
	VideoWriter videobw("/home/ps/Videos/activitybw25.avi",CV_FOURCC('M','J','P','G'),20, Size(frame_width,frame_height),false);
	if ( !videobw.isOpened() ) 
	{
	       cout << "ERROR: Failed to write the video" << endl;
	       return -1;
	}

	//namedWindow("box",0);
	//cap>>frame;
	//cvtColor(frame, gframe, COLOR_BGR2GRAY);
	//pri(gframe);

														/*
													    int i,j,k;
														int ***arr = new int **[frame_height]();
													    for (i = 0; i < frame_height; i++)
													    {
													        arr[i] = new int *[frame_width]();
													        for (j = 0; j < frame_width; j++)
													            arr[i][j] = new int [50]();
													    }
													
													    
													    arr[frame_height-1][frame_width-1][49]=35;
													    cout<<arr[frame_height-1][frame_width-1][49]<<endl;
														*/
													    /*
													    for (i = 0; i < frame_height; i++)
														{
													    	for (j = 0; j < frame_width; j++)
													        	delete[] arr[i][j];
													    	delete[] arr[i];
														}
														delete[] arr;
														*/

	int cnt=0;
	vector<Mat> cuboid;//(l, Mat(frame_height, frame_width, CV_8UC1)); // allocates 592 matrices sized 47 by 47
	for(int i=0; i<l; i++) {
		cap>>frame;
		cvtColor(frame, gframe, COLOR_BGR2GRAY);    
    	cuboid.push_back(gframe.clone());
	}
	//cout<<activity(cuboid,2,2,2)<<endl;
	//cout<<cuboid[794].size()<<endl;
	//cout<<int(cuboid[0].at<uchar>(Point(200,200)))<<endl;  //first col then row
	int intensity=0;
	Mat tempc = Mat::zeros(frame_height,frame_width, CV_8UC1);
	Mat tempcbw = Mat::zeros(frame_height,frame_width, CV_8UC1);
	int act=0;
	for(int t=0; t<l; t++){
		for(int y=0; y<frame_height; y++){
			for(int x=0; x<frame_width; x++){
				act=activity(cuboid,t,y,x);
				intensity=cuboid[t].at<uchar>(Point(x,y));	
				//cout<<act<<" "<<intensity<<endl;
				//waitKey(0);			
				tempc.at<uchar>(Point(x,y))=act;
				if(abs(act-intensity)>thold+10){
					tempcbw.at<uchar>(Point(x,y))=255;
				}
			}
		}
		video.write(tempc);
		videobw.write(tempcbw);
		tempcbw = Mat::zeros(frame_height,frame_width, CV_8UC1);
	}
	cap.release();
	return 0;
}

