#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

#define NCOLORS 6
#define SUPPER 255
#define SLOWER 0
#define VUPPER 250
#define VLOWER 10

const String window_name = "Finaal";
vector<Point> boardCorners;
const int max_value_H = 360/2;
const int max_value = 255;

int low_H = 10, low_S = 100, low_V = 0;
int high_H = 168, high_S = max_value, high_V = max_value;
int canny_l = 100, canny_h = 500, canny_max = 500;
static void on_low_H_thresh_trackbar(int, void *)
{
    low_H = min(high_H-1, low_H);
    setTrackbarPos("Low H", window_name, low_H);
}
static void on_high_H_thresh_trackbar(int, void *)
{
    high_H = max(high_H, low_H+1);
    setTrackbarPos("High H", window_name, high_H);
}
static void on_low_S_thresh_trackbar(int, void *)
{
    low_S = min(high_S-1, low_S);
    setTrackbarPos("Low S", window_name, low_S);
}
static void on_high_S_thresh_trackbar(int, void *)
{
    high_S = max(high_S, low_S+1);
    setTrackbarPos("High S", window_name, high_S);
}
static void on_low_V_thresh_trackbar(int, void *)
{
    low_V = min(high_V-1, low_V);
    setTrackbarPos("Low V", window_name, low_V);
}
static void on_high_V_thresh_trackbar(int, void *)
{
    high_V = max(high_V, low_V+1);
    setTrackbarPos("High V", window_name, high_V);
}

static void on_canny_l_trackbar(int, void *)
{
    canny_l = min(canny_h-1, canny_l);
    setTrackbarPos("Canny Low", "canny edge", canny_l);
}

static void on_canny_h_trackbar(int, void *)
{
    high_V = max(canny_h, canny_l+1);
    setTrackbarPos("Canny High", "canny edge", canny_h);
}

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

    namedWindow(window_name);

    //create trackbars
    createTrackbar("Low H", window_name, &low_H, max_value_H, on_low_H_thresh_trackbar);
    createTrackbar("High H", window_name, &high_H, max_value_H, on_high_H_thresh_trackbar);
    createTrackbar("Low S", window_name, &low_S, max_value, on_low_S_thresh_trackbar);
    createTrackbar("High S", window_name, &high_S, max_value, on_high_S_thresh_trackbar);
    createTrackbar("Low V", window_name, &low_V, max_value, on_low_V_thresh_trackbar);
    createTrackbar("High V", window_name, &high_V, max_value, on_high_V_thresh_trackbar);

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

    Mat sharpened,smoothed;
    float kdata1[] = {-1, -1, -1, -1, 9, -1, -1, -1, -1};
    float kdata2[] = {0, -1, 0, -1, 5, -1, 0, -1, 0};
    Mat kernel(3,3,CV_32F, kdata1);  //laplace kernel for sharpening the image
    filter2D(transformedCropped,sharpened,-1,kernel);
    imshow("sharpened",sharpened);
    waitKey(0);

//    bilateralFilter ( sharpened, smoothed, 20, 255, 80 );//bilateral because it preserves edges
//    imshow("smoothed",smoothed);
//    waitKey(0);

    GaussianBlur(sharpened,smoothed,Size(3,3), 0, 0);
    imshow("gaussian",smoothed);
    waitKey(0);

    ///Convert to HSV to segment different colors
    Mat hsv, mask_white;
    vector<Mat> channels;

    mask_white = Mat::zeros(r1.height,r1.width,transformedCropped.type());

    cvtColor(smoothed,hsv,CV_BGR2HSV);
    split(hsv,channels);

    inRange(hsv, Scalar(0,0,160), Scalar(180,255,255), mask_white);
    bitwise_not(mask_white, mask_white);
    Point anchor = Point(-1,-1);
//    dilate(mask_white,mask_white,Mat(),anchor,1);
    imshow("white mask",mask_white);
    waitKey(0);

    Mat result = Mat::ones(r1.height,r1.width,transformedCropped.type());
    transformedCropped.copyTo(result,mask_white);

    imshow("result",result);
    waitKey(0);

    //Canny
    Mat edges = Mat::zeros(r1.height,r1.width,CV_8UC1);
    GaussianBlur(transformedCropped,transformedCropped,Size(3,3), 0, 0);
    namedWindow("canny edge");

    createTrackbar("Canny Low", "canny edge", &canny_l, canny_max, on_canny_l_trackbar);
    createTrackbar("Canny High", "canny edge", &canny_h, canny_max, on_canny_h_trackbar);

//    while(true)
//    {
//        Canny(transformedCropped,edges,canny_l,canny_h,3);
//
//        imshow("canny edge",edges);
//        //use a key entry to stop the programm
//        char key = (char) waitKey(30);
//        if (key == 'q' || key == 27)
//        {
//            break;
//        }
//    }

    canny_l = 0;
    canny_h = 78;
    Canny(transformedCropped,edges,canny_l,canny_h,3,true);
    imshow("canny", edges);
    waitKey(0);

    //canny invullen

    dilate(edges,edges,Mat(),anchor,2);
    erode(edges,edges,Mat(),anchor,1);
    imshow("canny filled",edges);
    waitKey(0);

//
//    Mat H = channels[0];
//    Mat S = channels[1];
//    Mat V = channels[2];
//
//    Mat S_th, V_th, th_temp1, th_temp2, H_th_total;
//    H_th_total = Mat::zeros(r1.height,r1.width,CV_8UC1);
//    Mat H_th[NCOLORS];
//    float lRange = 0;
//    float hRange = 0;
//    float angle = 180/NCOLORS;
//    float halfAngle = angle/2;
//
//     while(true) {
//
//    //inRange(H,high_H,180,H_th1);
//    //inRange(H,0,low_H,H_th2);
//    inRange(S,low_S,high_S,S_th);
//    inRange(V,low_V,high_V,V_th);
//
//    //inRange(S,SLOWER,SUPPER,S_th);
//    //inRange(V,VLOWER,VUPPER,V_th);
//
//    int H_drawValues[NCOLORS];  //values used for drawing the scanned whiteboard, we use quantised colors for the scanned image
//    int S_drawValue = 100;
//    int V_drawValue = 50;
//
//    for(int i = 0; i < NCOLORS; i++)
//    {
//        if(i == 0)
//        {
//            inRange(H,180-halfAngle,0,th_temp1);    //lower part of red, minus halfangle
//            inRange(H,0,halfAngle,th_temp2);    //higher part of red, plus halfangle
//            H_th[i] = (th_temp1 | th_temp2) & S_th & V_th;
//        }
//        else
//        {
//            lRange = i*angle - halfAngle;
//            hRange = i*angle + halfAngle;
//            inRange(H,lRange,hRange,th_temp1);
//            H_th[i] = th_temp1 & S_th & V_th;
//        }
//        H_drawValues[i] = i*angle;
//        H_th_total = H_th_total | H_th[i];
//    }
//
//    vector<Mat> ch_hsv;
//    ch_hsv.push_back(H & S_th & V_th);
//    ch_hsv.push_back(S & V_th);
//    ch_hsv.push_back(V & V_th);
//
//    Mat finaal_hsv = Mat::zeros(r1.height,r1.width,CV_8UC3);
//    Mat finaal_bgr = Mat::zeros(r1.height,r1.width,CV_8UC3);
//
//    merge(ch_hsv, finaal_hsv);
//
//    cvtColor(finaal_hsv,finaal_bgr,CV_HSV2BGR);
//
//    imshow(window_name,finaal_bgr);
//           //use a key entry to stop the programm
//        char key = (char) waitKey(30);
//        if (key == 'q' || key == 27)
//        {
//            break;
//        }
//    }

    return 0;
}

