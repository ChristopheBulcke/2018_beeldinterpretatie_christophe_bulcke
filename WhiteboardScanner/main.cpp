#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

const int max_value_H = 360/2;
const int max_value = 255;

const String window_name = "Image with contoured mask";

int low_H = 10, low_S = 100, low_V = 0;
int high_H = 168, high_S = max_value, high_V = max_value;
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

int main(int argc, const char** argv)
{
    ///Adding a little help option and command line parser input
    CommandLineParser parser(argc, argv,
        "{help h usage ?    |    | show this message}"
        "{sign1 s1    |    | (required) path to image}"
        "{sign2 s2    |    | (required) path to image}"
        "{sign3 s3    |    | (required) path to image}"
        "{sign4 s4    |    | (required) path to image}"
    );

    ///Print help message
    if(parser.has("help")){
        parser.printMessage();
        cerr <<"Use absolute paths for images."<<endl;
        cerr <<"When in loop use hit q to stop"<<endl;
        return -1;
    }

    ///Empty string warning
    string sign1(parser.get<string>("sign1"));
    string sign2(parser.get<string>("sign2"));
    string sign3(parser.get<string>("sign3"));
    string sign4(parser.get<string>("sign4"));
    if(sign1.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for sign1 image!"<<endl;
        return -1;
    }
    else if(sign2.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for sign2 image!"<<endl;
        return -1;
    }
    else if(sign3.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for sign3 image!"<<endl;
        return -1;
    }
    else if(sign4.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for sign4 image!"<<endl;
        return -1;
    }
    else{
        cerr <<"Image parameters correct."<<endl;
    }


    ///Read images and check if read is succsfull
    Mat sign1_img = imread(sign1);
    Mat sign2_img = imread(sign2);
    Mat sign3_img = imread(sign3);
    Mat sign4_img = imread(sign4);
    if(sign1_img.empty()){
        cerr<< sign1 + " not found"<<endl;
        return -1;
    }
    else if(sign2_img.empty()){
        cerr<< sign2 + " not found"<<endl;
        return -1;
    }
    else if(sign3_img.empty()){
        cerr<< sign3 + " not found"<<endl;
        return -1;
    }
    else if(sign4_img.empty()){
        cerr<< sign4 + " not found"<<endl;
        return -1;
    }
    else{
//        imshow(sign1+" original",sign1_img);
//        waitKey(0);
//        imshow(sign2+" original",sign2_img);
//        waitKey(0);
//        imshow(sign3+" original",sign3_img);
//        waitKey(0);
//        imshow(sign4+" original",sign4_img);
//        waitKey(0);
    }

    ///Threshold image in BGR color space
    Mat img = sign1_img; //choose the image

    vector<Mat> channels;
    split(img,channels);    //split in bgr channels

    Mat finaal = Mat::zeros(Size(img.rows,img.cols),CV_8UC3); //make an empty image

    Mat rood = channels[2];
    Mat blauw = channels[0];
    Mat groen = channels[1];
    Mat rood_th, blauw_th, groen_th;
    inRange(rood,150,255,rood_th);  //threshold r channel
    inRange(blauw,0,50,blauw_th);   //threshold b channel
    inRange(groen,0,50,groen_th);   //threshold g channel

    vector<Mat> ch;
    ch.push_back(blauw_th); //make a vector of Mat (make a 3D matrix)
    ch.push_back(groen_th);
    ch.push_back(rood_th);

    merge(ch, finaal);

//    imshow("BGR color space",finaal);
//    waitKey(0);


    ///Threshold image in HSV color space

    namedWindow(window_name);

    //create trackbars
    createTrackbar("Low H", window_name, &low_H, max_value_H, on_low_H_thresh_trackbar);
    createTrackbar("High H", window_name, &high_H, max_value_H, on_high_H_thresh_trackbar);
    createTrackbar("Low S", window_name, &low_S, max_value, on_low_S_thresh_trackbar);
    createTrackbar("High S", window_name, &high_S, max_value, on_high_S_thresh_trackbar);
    createTrackbar("Low V", window_name, &low_V, max_value, on_low_V_thresh_trackbar);
    createTrackbar("High V", window_name, &high_V, max_value, on_high_V_thresh_trackbar);

    Mat img_hsv;
    cvtColor(img,img_hsv,CV_BGR2HSV);   //convert to hsv colorspace

    vector<Mat> channels_hsv;
    split(img_hsv,channels_hsv);

    //define everything that is used in infinite loop
    Mat finaal_hsv = Mat::zeros(Size(img_hsv.rows,img_hsv.cols),CV_8UC3);
    Mat finaal_BGR = Mat::zeros(Size(img_hsv.rows,img_hsv.cols),CV_8UC3);

    Mat H = channels_hsv[0];
    Mat S = channels_hsv[1];
    Mat V = channels_hsv[2];
    Mat H_th1, S_th, H_th2, H_th, V_th;

    Mat H_eroded = Mat::zeros(Size(img_hsv.rows,img_hsv.cols),CV_8UC3);;
    Mat H_eroded_dilated = Mat::zeros(Size(img_hsv.rows,img_hsv.cols),CV_8UC3);
    Point anchor = Point(-1,-1);
    vector<Mat> ch_hsv;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    vector<vector<Point> >hull( contours.size() );
    Mat finaal_connected = Mat::zeros(Size(img_hsv.rows,img_hsv.cols),CV_8UC3);
    Mat finaal_BGR_connected = Mat::zeros(Size(img_hsv.rows,img_hsv.cols),CV_8UC3);
    Mat connected_blobs = Mat::zeros( H_eroded_dilated.size(), CV_8UC1 );
    vector<Mat> ch_connected;

    while(true) {

        inRange(H,high_H,180,H_th1);
        inRange(H,0,low_H,H_th2);
        inRange(S,low_S,high_S,S_th);
        inRange(V,low_V,high_V,V_th);

        finaal_hsv = Mat::zeros(Size(img_hsv.rows,img_hsv.cols),CV_8UC3);
        finaal_BGR = Mat::zeros(Size(img_hsv.rows,img_hsv.cols),CV_8UC3);

        H_th = (H_th1 | H_th2) & S_th;

        erode(H_th,H_eroded,Mat(),anchor,5);
        dilate(H_eroded,H_eroded_dilated,Mat(),anchor,5);

        //imshow("H thresholded eroded and dilated",H_eroded_dilated);
        //waitKey(0);

        vector<Mat> ch_hsv;
        ch_hsv.push_back(H_eroded_dilated & H);
        ch_hsv.push_back(S & H_eroded_dilated);
        ch_hsv.push_back(V & H_eroded_dilated);

        merge(ch_hsv, finaal_hsv);

        cvtColor(finaal_hsv,finaal_BGR,CV_HSV2BGR);

        //imshow("masked image",finaal_BGR);

        ///Contours
        findContours( H_eroded_dilated, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0) );

        vector<vector<Point> >hull( contours.size() );
        for( size_t i = 0; i < contours.size(); i++ ){
            convexHull( Mat(contours[i]), hull[i], false );
        }

        connected_blobs = Mat::zeros( H_eroded_dilated.size(), CV_8UC1 );

        //for( size_t i = 0; i< contours.size(); i++ ){
        drawContours( connected_blobs, contours, 0, Scalar(255,255,255), -1, 8, vector<Vec4i>(), 0, Point() );
        //}
        //imshow("connected mask",connected_blobs);
        //waitKey(0);

        //clear Mat
        finaal_connected = Mat::zeros(Size(img_hsv.rows,img_hsv.cols),CV_8UC3);
        finaal_BGR_connected = Mat::zeros(Size(img_hsv.rows,img_hsv.cols),CV_8UC3);

        vector<Mat> ch_connected;
        ch_connected.push_back(connected_blobs & H);
        ch_connected.push_back(S & connected_blobs);
        ch_connected.push_back(V & connected_blobs);

        merge(ch_connected, finaal_connected);

        cvtColor(finaal_connected,finaal_BGR_connected,CV_HSV2BGR);

        //place some informative text on the image
        putText(finaal_BGR_connected, "Hit 'q' to exit.", cvPoint(30,30), FONT_HERSHEY_COMPLEX_SMALL, 1, cvScalar(255,255,255), 1, CV_AA);

        imshow(window_name,finaal_BGR_connected);

        //use a key entry to stop the programm
        char key = (char) waitKey(30);
        if (key == 'q' || key == 27)
        {
            break;
        }

    }

    return 0;
}

