#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char** argv)
{
    ///Adding a little help option and command line parser input
    CommandLineParser parser(argc, argv,
        "{help h usage ?    |    | show this message}"
        "{image_gray ig    |    | (required) path to image}"
        "{image_color ic    |    |(required) path to image)}"
    );

    if(parser.has("help")){
        parser.printMessage();
        cerr <<"Use absolute paths!"<<endl;
        return 1;
    }

    string image_gray(parser.get<string>("image_gray"));
    if(image_gray.empty()){
        parser.printMessage();
        cerr <<"Empty parameter! Use help for more info"<<endl;
        return -1;
    }

//    Mat image;
//    image = imread(image_gray_location);
//    if(image.empty()){
//        cerr<< "image not found"<<endl;
//        return -1;
//    }
//
//    imshow("titel",image);
//    waitKey(0);

//    vector<Mat> channels;
//    split(image, channels);
//    imshow("blue", channels[0]);
//    waitKey(0);
//
//    Mat canvas = Mat::zeros(Size(250, 250)),CV_8UC3);
//    rectangle();
//    imshow("Canvas", canvas);
//    waitKey(0);

    return 0;
}
