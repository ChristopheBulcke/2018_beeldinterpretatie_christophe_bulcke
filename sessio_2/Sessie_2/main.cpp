#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

//const int alpha_slider_max = 180;
//int alpha_slider = 180;
//int h1;
//
//static void on_trackbar(int , void*){
//    h1 = alpha_slider;
//}

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
        imshow(sign1+" original",sign1_img);
        waitKey(0);
        imshow(sign2+" original",sign2_img);
        waitKey(0);
        imshow(sign3+" original",sign3_img);
        waitKey(0);
        imshow(sign4+" original",sign4_img);
        waitKey(0);
    }




    ///Threshold image in BGR and HSV color space
    Mat img = sign3_img;

    vector<Mat> channels;
    split(img,channels);

    Mat finaal = Mat::zeros(Size(img.rows,img.cols),CV_8UC3);

    Mat rood = channels[2];
    Mat blauw = channels[0];
    Mat groen = channels[1];
    Mat rood_th, blauw_th, groen_th;
    inRange(rood,150,255,rood_th);
    inRange(blauw,0,50,blauw_th);
    inRange(groen,0,50,groen_th);

    vector<Mat> ch;
    ch.push_back(blauw_th);
    ch.push_back(groen_th);
    ch.push_back(rood_th);

    merge(ch, finaal);

    imshow("BGR color spqce",finaal);
    waitKey(0);

    Mat img_hsv;
    cvtColor(img,img_hsv,CV_BGR2HSV);

    vector<Mat> channels_hsv;
    split(img_hsv,channels_hsv);

    Mat finaal_hsv = Mat::zeros(Size(img_hsv.rows,img_hsv.cols),CV_8UC3);

    Mat H = channels_hsv[0];
    Mat S = channels_hsv[1];
    Mat V = channels_hsv[2];
    Mat H_th1, S_th, H_th2, H_th, V_th;
    inRange(H,168,180,H_th1);
    inRange(H,0,10,H_th2);
    inRange(S,150,255,S_th);
    inRange(V,0,255,V_th);

    H_th = (H_th1 | H_th2) & S_th;

    Mat H_eroded = Mat::zeros(Size(img_hsv.rows,img_hsv.cols),CV_8UC3);;
    Mat H_eroded_dilated = Mat::zeros(Size(img_hsv.rows,img_hsv.cols),CV_8UC3);
    Point anchor = Point(-1,-1);

    erode(H_th,H_eroded,Mat(),anchor,5);
    dilate(H_eroded,H_eroded_dilated,Mat(),anchor,2);

    imshow("H thresholded eroded and dilated",H_eroded_dilated);
    waitKey(0);

    vector<Mat> ch_hsv;
    ch_hsv.push_back(H_eroded_dilated & H);
    ch_hsv.push_back(S_th & S);
    ch_hsv.push_back(V_th & V);

    merge(ch_hsv, finaal_hsv);

    cvtColor(finaal_hsv.clone(),finaal_hsv,CV_HSV2BGR);

    imshow("HSV color space",finaal_hsv);
    waitKey(0);

//    vector<Point> grootste_blob = contours[0];
//    for

    return 0;
}

