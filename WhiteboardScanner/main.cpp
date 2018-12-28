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
    else{
        imshow(img_name + " original",img);
        waitKey(0);
    }

    ///Define whiteboard area

    ///Transform perspective

    ///Convert to HSV to segment different colors
    Mat img_hsv;
    cvtColor(img,img_hsv,COLOR_BGR2HSV);


    return 0;
}

