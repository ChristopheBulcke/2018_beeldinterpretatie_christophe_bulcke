#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <string>

using namespace std;
using namespace cv;

CascadeClassifier face_cascade_haar;
CascadeClassifier face_cascade_lbp;
Mat faceDetector(Mat);

int main(int argc, const char** argv)
{
    ///Adding a little help option and command line parser input
    CommandLineParser parser(argc, argv,
        "{help h usage ?    |    | show this message}"
        "{video vid    |    | (required) path to video}"
        "{haar_cascade casc    |    | (required) path to haar_cascade}"
    );

    ///Print help message
    if(parser.has("help")){
        parser.printMessage();
        cerr <<"Use absolute paths for video."<<endl;
        cerr <<"Use absolute paths for haar cascade."<<endl;
        return -1;
    }

    ///Empty string warning
    string vid_name(parser.get<string>("video"));
    string haar_name(parser.get<string>("haar_cascade"));

    if(vid_name.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for video!"<<endl;
        return -1;
    }
    else{
        cerr <<"Video parameters correct."<<endl;
    }

    if(haar_name.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for haar_cascade!"<<endl;
        return -1;
    }
    else{
        cerr <<"haar_cascade parameters correct."<<endl;
    }

    ///Load the cascades
    if( !face_cascade_haar.load( haar_name ) )
    {
        cout << "Error loading face cascade" <<endl;
        return -1;
    };
    if( !face_cascade_lbp.load( haar_name ) )
    {
        cout << "Error loading face cascade" <<endl;
        return -1;
    };

    ///Video interface

    VideoCapture cap(vid_name);
    if( !cap.isOpened()){
         cout << "Cannot open the video file " << vid_name << endl;
         return -1;
    }

    double count = cap.get(CV_CAP_PROP_FRAME_COUNT); //get the frame count
    //cap.set(CV_CAP_PROP_POS_FRAMES,count-1); //Set index to last frame
    namedWindow("MyVideo",CV_WINDOW_AUTOSIZE);

//    while(1)
//    {
//        Mat frame_in, frame_out;
//        bool success = cap.read(frame_in);
//        if (!success){
//          cout << "Cannot read  frame " << endl;
//          break;
//        }
//        frame_out = faceDetector(frame_in.clone());
//        imshow("MyVideo", frame_out);
//        if(waitKey(25) >= 0) break;
//    }


    while(1)
    {
        Mat frame_in, frame_out;
        bool success = cap.read(frame_in);
        if (!success){
          cout << "Cannot read  frame " << endl;
          break;
        }
        frame_out = hogDetector(frame_in.clone());
        imshow("MyVideo", frame_out);
        if(waitKey(25) >= 0) break;
    }

    return 0;
}

Mat faceDetector( Mat frame )
{
    Mat frame_temp = frame.clone();
    //cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    //equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    vector<Rect> facesHAAR, facesLBP;
    vector<int> scoreHAAR, scoreLBP;

    double scaleFactor = 1.05;
    int minNeighbors = 3;

    face_cascade_haar.detectMultiScale( frame_temp, facesHAAR, scoreHAAR, scaleFactor, minNeighbors );
    face_cascade_lbp.detectMultiScale( frame_temp, facesLBP, scoreLBP, scaleFactor, minNeighbors );

    int temp;

    for ( size_t i = 0; i < facesHAAR.size(); i++ )
    {
        Point center( facesHAAR[i].x + facesHAAR[i].width/2, facesHAAR[i].y + facesHAAR[i].height/2 );
        ellipse( frame, center, Size( facesHAAR[i].width/2, facesHAAR[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 255 ), 3 );
        temp = (int)scoreHAAR[i];
        putText( frame, to_string(temp), Point(facesHAAR[i].x, facesHAAR[i].y),FONT_HERSHEY_SIMPLEX, 1, Scalar( 255, 0, 255 ), 2);
        Mat faceROI = frame_gray( facesHAAR[i] );
    }

    temp = 0;

    for ( size_t i = 0; i < facesLBP.size(); i++ )
    {
        rectangle(frame, Point(facesLBP[i].x, facesLBP[i].y), Point(facesLBP[i].x + facesLBP[i].width, facesLBP[i].y + facesLBP[i].height), Scalar( 255, 255, 0 ), 3);

        putText( frame, to_string(temp), Point(facesLBP[i].x + facesLBP[i].width, facesLBP[i].y),FONT_HERSHEY_SIMPLEX, 1, Scalar( 255, 255, 0 ), 2);
        Mat faceROI = frame_gray( facesLBP[i] );
    }

    putText( frame, "LBP", Point(20, 25),FONT_HERSHEY_SIMPLEX, 1, Scalar( 255, 255, 0 ));
    putText( frame, "HAAR", Point(20, 50),FONT_HERSHEY_SIMPLEX, 1, Scalar( 255, 0, 255 ));

    return frame;
}

Mat hogDetector(Mat frame)
{
    Mat frame_temp = frame.clone();

    return frame;
}
