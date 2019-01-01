#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

vector<Point> boardCorners;

void CallBackFuncCorners(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN )
    {
    //add element
        if(boardCorners.size() == 4)
        {
            cout << "You can only add 4 corners." << endl;
        }
        else{
            cout << "Added corner (" << x << ", " << y << ")" << endl;
            //push_back: add element
            boardCorners.push_back(Point(x,y));
        }
    }
    else if  ( event == EVENT_RBUTTONDOWN )
    {
    //delete previous corner
        if(boardCorners.size() > 0)
        {
            cout << "Deleted last added corner. " << boardCorners.size()-1 << " entries left." << endl;
            //pop_back: delete previous element
            boardCorners.pop_back();
        }
        else
        {
            cout << "There are no corners left in the list." << endl;
        }
    }
}

int main(int argc, const char** argv)
{
    ///Adding a little help option and command line parser input
    CommandLineParser parser(argc, argv,
        "{help h usage ?    |    | show this message}"
        "{whiteboard_image img    |    | (required) path to image}"
    );

    ///Print help message
    if(parser.has("help")){
        parser.printMessage();
        cerr <<"Try and use absolute paths for images or put the image in the current working folder."<<endl;
        return -1;
    }

    ///Empty string warning
    string img_name(parser.get<string>("whiteboard_image"));
    if(img_name.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for whiteboard_image path!"<<endl;
        return -1;
    }
    else{
        cerr <<"Parameters correct."<<endl;
    }

    ///Read images and check if read is succsfull
    Mat img = imread(img_name);
    if(img.empty()){
        cerr<< img_name + " not found"<<endl;
        return -1;
    }

    ///Define whiteboard area
    //new window
    string cornersWindow("Select the 4 corners of the whiteboard clockwise, starting top left.");
    namedWindow(cornersWindow);

//    putText(img,
//            "Click on the 4 corners of the whiteboard, clockwise!.",
//            Point(5,20), // Coordinates
//            FONT_HERSHEY_COMPLEX_SMALL, // Font
//            0.9, // Scale. 2.0 = 2x bigger
//            Scalar(255,255,255), // BGR Color
//            1, // Line Thickness (Optional)
//            CV_AA);

    //define mouse callback
    setMouseCallback(cornersWindow, CallBackFuncCorners, NULL);
    imshow(cornersWindow,img);
    waitKey(0);

    Mat mask = Mat::zeros(img.rows,img.cols,CV_8UC3);
    Mat boundingRectangle = Mat::zeros(img.rows,img.cols,CV_8UC3);

    fillConvexPoly( mask, boardCorners, Scalar(255,255,255), LINE_AA );

    imshow("mask",mask);
    waitKey(0);

    Mat board = Mat::zeros(img.rows,img.cols,CV_8UC3);
    multiply(img,mask/255,board);

    imshow("board",board);
    waitKey(0);

    ///Transform perspective

    Mat M, transformed;
    M = Mat::zeros(img.rows,img.cols,img.type());

    Rect r1 = boundingRect(boardCorners);
    vector<Point> boundingRectanglePoints;
    boundingRectanglePoints.push_back(Point(r1.x , r1.y));    //top left
    boundingRectanglePoints.push_back(Point(r1.x+r1.width , r1.y));    //top right
    boundingRectanglePoints.push_back(Point(r1.x+r1.width , r1.y+r1.height));    //bottom right
    boundingRectanglePoints.push_back(Point(r1.x , r1.y+r1.height));    //bottom left

    Point2f temp1[4];
    Point2f temp2[4];

    temp1[0] = boardCorners.at(0);
    temp1[1] = boardCorners.at(1);
    temp1[2] = boardCorners.at(2);
    temp1[3] = boardCorners.at(3);

    temp2[0] = boundingRectanglePoints.at(0);
    temp2[1] = boundingRectanglePoints.at(1);
    temp2[2] = boundingRectanglePoints.at(2);
    temp2[3] = boundingRectanglePoints.at(3);

    M = getPerspectiveTransform(temp1,temp2);
    warpPerspective(board,transformed,M,transformed.size());
    imshow("transformed mask",transformed);
    waitKey(0);

    Mat transformedCropped = Mat::zeros(r1.height,r1.width,img.type());

    Mat tmp = transformed(Rect(r1.x,r1.y,r1.width,r1.height));
    tmp.copyTo(transformedCropped);
    imshow("transformedCropped",transformedCropped);
    waitKey(0);


    ///Convert to HSV to segment different colors
    Mat hsv;
    cvtColor(transformedCropped,hsv,COLOR_BGR2HSV);



    return 0;
}

