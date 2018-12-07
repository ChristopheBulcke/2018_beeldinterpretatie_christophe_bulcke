#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <string>

using namespace std;
using namespace cv;

vector<Point> track;
CascadeClassifier face_cascade_haar;
CascadeClassifier face_cascade_lbp;

Mat faceDetector(Mat);
Mat personDetector(Mat frame, vector<Point> track);

int main(int argc, const char** argv)
{
    ///Adding a little help option and command line parser input
    CommandLineParser parser(argc, argv,
        "{help h usage ?    |    | show this message}"
        "{video_faces vidf   |    | (required) path to video}"
        "{video_people vidp  |    | (required) path to video}"
        "{haar_cascade haar    |    | (required) path to haar_cascade}"
        "{lbp_cascade lbp    |    | (required) path to haar_cascade}"
    );

    ///Print help message
    if(parser.has("help")){
        parser.printMessage();
        cerr <<"Use absolute paths for video."<<endl;
        cerr <<"Use absolute paths for haar cascade."<<endl;
        return -1;
    }

    ///Empty string warning
    string vidf_name(parser.get<string>("video_faces"));
    string vidp_name(parser.get<string>("video_people"));
    string haar_name(parser.get<string>("haar_cascade"));
    string lbp_name(parser.get<string>("lbp_cascade"));

    if(vidf_name.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for video!"<<endl;
        return -1;
    }
    else{
        cerr <<"Video parameters correct."<<endl;
    }

    if(vidp_name.empty()){
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

    if(lbp_name.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for lbp_cascade!"<<endl;
        return -1;
    }
    else{
        cerr <<"lbp_cascade parameters correct."<<endl;
    }

    ///Load the cascades
    if( !face_cascade_haar.load( haar_name ) )
    {
        cout << "Error loading haar cascade" <<endl;
        return -1;
    };
    if( !face_cascade_lbp.load( lbp_name ) )
    {
        cout << "Error loading lbp cascade" <<endl;
        return -1;
    };

    ///Video interface
    string vid_name;
    bool faces = false;

    if (faces)
    {
        vid_name = vidf_name;
    }
    else
    {
        vid_name = vidp_name;
    }

    VideoCapture cap(vid_name);
    if( !cap.isOpened()){
         cout << "Cannot open the video file " << vid_name << endl;
         return -1;
    }

    double count = cap.get(CV_CAP_PROP_FRAME_COUNT); //get the frame count
    //cap.set(CV_CAP_PROP_POS_FRAMES,count-1); //Set index to last frame
    namedWindow("MyVideo",CV_WINDOW_AUTOSIZE);

    while(1)
    {
        Mat frame_in, frame_out;
        bool success = cap.read(frame_in);
        if (!success){
          cout << "Cannot read frame " << endl;
          break;
        }
        if (faces)
        {
            frame_out = faceDetector(frame_in.clone());
        }
        else
        {
            frame_out = personDetector(frame_in.clone(), track);
        }
        imshow("MyVideo", frame_out);
        if(waitKey(10) >= 0) break;
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
        Mat faceROI = frame( facesHAAR[i] );
    }

    temp = 0;

    for ( size_t i = 0; i < facesLBP.size(); i++ )
    {
        rectangle(frame, Point(facesLBP[i].x, facesLBP[i].y), Point(facesLBP[i].x + facesLBP[i].width, facesLBP[i].y + facesLBP[i].height), Scalar( 255, 255, 0 ), 3);
        temp = (int)scoreLBP[i];
        putText( frame, to_string(temp), Point(facesLBP[i].x + facesLBP[i].width, facesLBP[i].y),FONT_HERSHEY_SIMPLEX, 1, Scalar( 255, 255, 0 ), 2);
        Mat faceROI = frame( facesLBP[i] );
    }

    putText( frame, "LBP", Point(20, 25),FONT_HERSHEY_SIMPLEX, 1, Scalar( 255, 255, 0 ));
    putText( frame, "HAAR", Point(20, 50),FONT_HERSHEY_SIMPLEX, 1, Scalar( 255, 0, 255 ));

    return frame;
}

Mat personDetector(Mat frame, vector<Point> track)
{
    Mat frame_temp = frame.clone();
    int temp = 0;

    vector<Rect> person;
    vector<double> score;

    HOGDescriptor hog;
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

    hog.detectMultiScale(frame_temp, person, score );

    for( size_t i = 0; i < person.size(); i++ )
    {
        Rect r = person[i];
        rectangle(frame, person[i], cv::Scalar(255,0,255), 3);
        temp = (int)score[i];
        putText(frame, to_string(temp),Point(person[i].x,person[i].y+50), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,255));
        track.push_back(Point(person[i].x+person[i].width/2,person[i].y+person[i].height/2));
    }

    /// plot the track so far
    for(size_t i = 1; i < track.size(); i++){
        line(frame, track[i-1], track[i], Scalar(255,255,0), 2);
    }

    return frame;
}
