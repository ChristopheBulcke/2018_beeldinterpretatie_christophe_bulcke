#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char** argv)
{
    ///Adding a little help option and command line parser input
    CommandLineParser parser(argc, argv,
        "{help h usage ?    |    | show this message}"
        "{image_gray ig    |    | (required) path to image}"
        "{image_color ic    |    | (required) path to image}"
    );

    ///Print help message
    if(parser.has("help")){
        parser.printMessage();
        cerr <<"Use absolute paths for images."<<endl;
        return -1;
    }

    ///Empty string warning
    string image_gray(parser.get<string>("image_gray"));
    string image_color(parser.get<string>("image_color"));
    if(image_gray.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for gray image!"<<endl;
        return -1;
    }
    else if(image_color.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for color image!"<<endl;
        return -1;
    }
    else{
        cerr <<"Images read correctly."<<endl;
    }


    ///Read images and check if read is succsfull
    Mat gray;
    Mat color;
    gray = imread(image_gray);
    color = imread(image_color);
    if(gray.empty()){
        cerr<< image_gray + " not found"<<endl;
        return -1;
    }
    else if(color.empty()){
        cerr<< image_color + " not found"<<endl;
        return -1;
    }
    else{
        imshow(image_gray+" original",gray);
        waitKey(0);
        imshow(image_color+" original",color);
        waitKey(0);
    }

    ///Split the color images in the 3 colorchannels and visualize them
    vector<Mat> channels;
    split(color, channels);
    imshow("Blue", channels[0]);
    imshow("Green", channels[1]);
    imshow("Red", channels[2]);
    waitKey(0);

    ///Convert color image to gray
    Mat color2gray;
    cvtColor(color, color2gray, CV_RGB2GRAY);
    imshow("Color image to gray converted",color2gray);
    waitKey(0);

    ///Print every pixel value to terminal
    cout<<color2gray<<endl;
    waitKey(0);

    ///Draw a green rectangle on an empty image
    Point rect1(25,25);
    Point rect2(125,125);
    Point circ_center(125,125);
    Point ellipse_center(125,175);
    int radius = 50;
    Mat canvas = Mat::zeros(Size(250, 250),CV_8UC3); ///create a black image 250x250
    circle(canvas, circ_center, radius, Scalar(255, 0, 0)); ///draw a blue circle
    rectangle(canvas, rect1, rect2, Scalar(0, 255, 0)); ///draw a green rectangle
    ellipse(canvas, ellipse_center, Size(100,25), 0, 0, 360, Scalar(0, 0, 255)); ///draw a red ellipse
    imshow("Canvas", canvas);
    waitKey(0);

    return 0;
}
