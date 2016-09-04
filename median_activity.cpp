#include <highgui.h>
//#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <iostream>
//#include <algorithm>
//#include <string>

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

static int frame_limit = 250;

int get_activity(VideoCapture &cap, int y, int x, int t){
	int activity=0;
	int bracket=10;
	int a[2*bracket+1][3];
	Mat src;
	cap.set(CV_CAP_PROP_POS_FRAMES, t);
	cap>>src;
	int nr=src.rows;
	int nc=src.cols;
	cout<<nr<<" "<<nc<<endl;
	int pos=0;

	for(int i=t-bracket;i<=t+bracket;i++){
		if(i<=0){
			a[pos][0]=0;
			a[pos][1]=0;
			a[pos][2]=0;
			pos++;
		}
		else{
			cap.set(CV_CAP_PROP_POS_FRAMES, i);
			cap>>src;
			Vec3b color = src.at<Vec3b>(Point(x,y)); // pagalpan pehle col do phir row 
			a[pos][0]=color[0];  //blue
			a[pos][1]=color[1];  //green 
			a[pos][2]=color[2];  //red
			pos++;	
		}
		
	}
	for(int i=0;i<2*bracket+1;i++){
		for(int j=0;j<3;j++){
			cout<<a[i][j]<<" : ";
		}
		cout<<endl;
	}
	//namedWindow("box",0);
	//imshow("box",src);
	//waitKey(0);
	return activity;
}

int get_activitybw(VideoCapture &cap, int y, int x, int t){
	int activity=0;
	int bracket=10;
	int a[2*bracket+1];
	Mat src,greysrc;
	cap.set(CV_CAP_PROP_POS_FRAMES, t);
	cap>>src;
	cvtColor(src, greysrc, COLOR_BGR2GRAY);

	//namedWindow("box",0);
	//imshow("box",greysrc);
	//waitKey(0);
	int nr=src.rows;
	int nc=src.cols;

	//cout<<nr<<" "<<nc<<endl;
	int pos=0;

	for(int i=t-bracket;i<=t+bracket;i++){
		if(i<=0){
			a[pos]=0;
			pos++;
		}
		else{
			cap.set(CV_CAP_PROP_POS_FRAMES, i);
			cap>>src;
			cvtColor(src, greysrc, COLOR_BGR2GRAY);
			a[pos]=greysrc.at<uchar>(x,y);
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
	activity=a[bracket];
	return activity;
}



int main(int argc, char* argv[])
{
	const char* vidname = argc > 1 ? argv[1] : "/home/ps/Videos/demo.avi";
	VideoCapture cap(vidname);
	int i,j,k,p,q;
	
	int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	int l=cap.get(CV_CAP_PROP_FRAME_COUNT);

	//VideoWriter video("/home/ps/Videos/activity.avi",CV_FOURCC('P','I','M','1'),20, Size(100,100),true);
	VideoWriter video("/home/ps/Videos/activity.avi",CV_FOURCC('M','J','P','G'),20, Size(frame_width,frame_height),false);
	if ( !video.isOpened() ) 
	{
	       cout << "ERROR: Failed to write the video" << endl;
	       return -1;
	}

	cout<<"total frames: "<<l<<endl;
	cout<<"frame height: "<<frame_height<<endl;
	cout<<"frame width: "<<frame_width<<endl;
	
	Mat src;
	int activity;
	int x,y,t;
	Mat tempc = Mat::zeros(frame_height,frame_width, CV_8UC1);
	
	//namedWindow("box",0);
	//imshow("box",tempc);
	//waitKey(0);																//query method
/*
	cout<<"Enter pixel row col timeframe details:"<<endl;
	cin>>y>>x>>t;
	activity=get_activitybw(cap,y,x,t);
	cout<<endl<<"activity"<<endl;	
	cout<<activity<<endl;
*/
	return 0;
	int cnt=0;															//video object method
	for(int t=0;t<40;t++){

		for(int y=160;y<180;y++){
			for(int x=160;x<180;x++){
			//cout<<x<<" "<<y<<endl;
			activity=get_activitybw(cap,y,x,t);
			tempc.at<uchar>(x,y)=activity;
			cout<<y<<" : "<<x<<endl;
			cout<<"ac: "<<int(tempc.at<uchar>(x,y))<<endl;
			}
		}
		video.write(tempc);
	}

	cap.release();

																	// alloc dealloc
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

    for (i = 0; i < frame_height; i++)
	{
    	for (j = 0; j < frame_width; j++)
        	delete[] arr[i][j];
    	delete[] arr[i];
	}
	delete[] arr;
	*/

	return 0;
}

