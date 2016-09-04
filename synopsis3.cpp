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

// struct definitons
struct obj{
  int q=0;
  int location[1000][5];   //1000 is max for no of frame appearances the object can have
}tmp;
typedef struct obj obj;
obj things[200];			// assuming 200 objects as the cap

//globals
int thresh = 50;
int num = 0;
int frame_cnt = 1;
int max_thresh = 255;
int open = num;
int fframe;

//global arrays
int objarrs[200];
int rectarrs[200];

//flags
int flag_first_frame = 0;
int flag_compare = 0;

// Mat objects
Mat frame; 
Mat fgMaskMOG2; 
Ptr<BackgroundSubtractor> pMOG2;

// functions
// for the first rect objects
void initialise_func(vector<Rect> boundRectnew, int newrects, int frame_no){

	for(int i=0; i<newrects; i++){
		things[num].location[things[num].q][0]=(boundRectnew[i].tl()).x;
        things[num].location[things[num].q][1]=(boundRectnew[i].br()).x;
        things[num].location[things[num].q][2]=(boundRectnew[i].tl()).y;
        things[num].location[things[num].q][3]=(boundRectnew[i].br()).y;
        things[num].location[things[num].q][4]=frame_no;
        things[num].q += 1;
        num++;
	}
	cout<<things[2].location[things[2].q-1][3]<<endl;
	open=num;
}

// to add rect entries for various objects at diff frames
void add_func(vector<Rect> boundRectnew, int rectno, int objid, int frame_no){

		things[objid].location[things[objid].q][0]=(boundRectnew[rectno].tl()).x;
        things[objid].location[things[objid].q][1]=(boundRectnew[rectno].br()).x;
        things[objid].location[things[objid].q][2]=(boundRectnew[rectno].tl()).y;
        things[objid].location[things[objid].q][3]=(boundRectnew[rectno].br()).y;
        things[objid].location[things[objid].q][4]=frame_no;
        things[objid].q += 1;

        cout<<things[objid].location[things[objid].q-1][3]<<endl;
}


int dist(vector<Rect> boundRectnew, int rectno, int objno){
	int finalsum;
	int val1,val2,val3,val4;
	val1 = things[objno].location[things[objno].q-1][0]-(boundRectnew[rectno].tl()).x;
    val2 = things[objno].location[things[objno].q-1][1]-(boundRectnew[rectno].br()).x;
    val3 = things[objno].location[things[objno].q-1][2]-(boundRectnew[rectno].tl()).y;
    val4 = things[objno].location[things[objno].q-1][3]-(boundRectnew[rectno].br()).y;
    finalsum = abs(val1+val2+val3+val4);
    return finalsum;
}

// periodically show all objects
void dispobj(){
	int iterator=0;
	cout<<"Displaying all objects uptil now: "<<endl<<endl;
	for(int i=0; i<num; i++){
		iterator=0;
		cout<<"Object: "<<i<<endl;
		if(objarrs[i]==0){
			cout<<"Status: open"<<endl;
		}
		else{
			cout<<"Status: closed"<<endl;
		}
		while(things[i].q - iterator != 0){
			cout<<"xmin: "<<things[i].location[iterator][0]<<" ";
			cout<<"xmax: "<<things[i].location[iterator][1]<<" ";
			cout<<"ymin: "<<things[i].location[iterator][0]<<" ";
			cout<<"ymax: "<<things[i].location[iterator][0]<<endl;
			iterator++;
		}
	}
}


// tracker for rect objects
void compare_func(vector<Rect> boundRectnew, int newrects, int frame_no){
	cout<<"compare func called"<<endl;
	cout<<"no of rects "<<newrects<<" frame no "<<frame_no<<" open objects: "<<open<<endl;
	int temp,objid,rectid;
	int rectno=0;
	int objno=0;
	int cost=1000000;
													// rect objects are less than open objects
	if(newrects <= open){                          // for each rect go thru all open objs
		while(rectno!=newrects){
			objno=0;
			cost=1000000;
			while(objno!=num){
				if(objarrs[objno]==0){
					temp=dist(boundRectnew, rectno, objno);
					if(temp<cost){
						objid=objno;
						cost=temp;
					}
				}
				objno++;			
			}
			cout<<"assigning rect no: "<<rectno<<"to: "<<objid<<endl;
			objarrs[objid]=1;
			add_func(boundRectnew,rectno,objid,frame_no);
			rectno++;
		}
	}
	else											// more rects than  open objects
	{
		cout<<"Jhingalala hoo hoo "<<endl;         // for each open obj go thru all rects
		for(int t=0; t<newrects; t++){
			rectarrs[t]=0;
		}
		while(objno!=num){
			cost=100000;
			rectno=0;
			if(objarrs[objno]==0){
				while(rectno!=newrects){
					if(rectarrs[rectno]==0){
						temp=dist(boundRectnew, rectno, objno);
						if(temp<cost){
							rectid=rectno;
							cost=temp;
						}
					}
					rectno++;
				}
			cout<<"assigning rect no: "<<rectid<<"to: "<<objno<<endl;
			rectarrs[rectid]=1;
			objarrs[objno]=1;
			add_func(boundRectnew,rectid,objno,frame_no);
			}
			objno++;
		}
		//assigning left over rects to new objects
		for(int t=0; t<newrects; t++){
			cout<<"rect index: "<<t<<" rect-assign: "<<rectarrs[t]<<endl;
			if(rectarrs[t]==0){
				objarrs[num]=1;
				things[num].location[things[num].q][0]=(boundRectnew[t].tl()).x;
        		things[num].location[things[num].q][1]=(boundRectnew[t].br()).x;
        		things[num].location[things[num].q][2]=(boundRectnew[t].tl()).y;
        		things[num].location[things[num].q][3]=(boundRectnew[t].br()).y;
        		things[num].location[things[num].q][4]=frame_no;
        		things[num].q += 1;
        		num++;

			}
		}
	}
	cout<<"Total objects: "<<num<<endl;
}


// controller program
void routine(Mat &diffImage, Mat &fr){
	
  Mat threshold_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  int k,br1,br2,tl1,tl2;
  	
  threshold( diffImage, threshold_output, thresh, 255, THRESH_BINARY );
  	//imshow("bw",threshold_output);
  findContours( threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
		
  	vector<vector<Point> > contours_poly( contours.size() );
  	vector<Rect> boundRect( contours.size() );
    vector<Rect> boundRectnew( contours.size() );

  	for(int i=0; i<contours.size(); i++){
  		approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
      boundRect[i] = boundingRect( Mat(contours_poly[i]) );
  	}

  	namedWindow( "Contours", 0 );
    cout<<"frame : "<<frame_cnt<<endl;
  	Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );

  	int newrects=0;

  	for( int i = 0; i< contours.size(); i++ ){
       //Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       Scalar color = Scalar( 0, 255, 0 );
       if((boundRect[i].br()).x-(boundRect[i].tl()).x > 22 && (boundRect[i].br()).y-(boundRect[i].tl()).y > 30  && frame_cnt!=1){
       	  if(flag_first_frame==0){
       	  	fframe=frame_cnt;
       	  	flag_first_frame=1;
       	  }
          rectangle( fr, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
          //cout<<boundRect[i].tl()<<" to "<<boundRect[i].br()<<endl; 
          boundRectnew[newrects]=boundRect[i];
          cout<<newrects+1<<endl;
          cout<<boundRectnew[newrects].tl()<<" to "<<boundRectnew[newrects].br()<<endl;
          newrects++;
        }
     }
     cout<<endl;
     
     if(frame_cnt==fframe){
     	initialise_func(boundRectnew,newrects,frame_cnt);
     }
     else{
     	compare_func(boundRectnew, newrects,frame_cnt);
     	open=0;
     	for(int t=0; t<num; t++){ //use num for all current objects
     		if(objarrs[t]==1){
     			objarrs[t]=0;  //open
     			open++;        // update open variable 
     		}
     		else{
     			objarrs[t]=2; //closed
     		}
     	}
     }
     cout<<endl;
     imshow( "Contours", fr);
}

int main(int argc, char* argv[])
{
	int t;
	char p;
	const char* vidname = argc > 1 ? argv[1] : "/home/ps/Videos/demo.avi";
	VideoCapture capture(vidname);

	for(t=0; t<100; t++){
		objarrs[t]=0;
	}
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
        if(frame_cnt%15==0){
        	cout<<"Press y to see objects created till now: "<<endl;
        	cin>>p;
        	if(p=='y'){
        		//cout<<"print func called"<<endl;
        		dispobj();
        	}
        }
        waitKey(0);
    }
    capture.release();
    return 0;
}























