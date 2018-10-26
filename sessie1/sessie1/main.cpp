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
        "{image_bimodal ib    |    | (required) path to image}"
        "{image_color ic    |    | (required) path to image}"
        "{image_color_adapted ica    |    | (required) path to image}"
    );

    ///Print help message
    if(parser.has("help")){
        parser.printMessage();
        cerr <<"Use absolute paths for images."<<endl;
        return -1;
    }

    ///Empty string warning
    string image_bimodal(parser.get<string>("image_bimodal"));
    string image_color(parser.get<string>("image_color"));
    string image_color_adapted(parser.get<string>("image_color"));
    if(image_bimodal.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for bimodal image!"<<endl;
        return -1;
    }
    else if(image_color.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for color image!"<<endl;
        return -1;
    }
    else if(image_color_adapted.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for adapted color image!"<<endl;
        return -1;
    }
    else{
        cerr <<"Image parameters correct."<<endl;
    }


    ///Read images and check if read is succsfull
    Mat bimodal;
    Mat color;
    Mat color_adapted;
    bimodal = imread(image_bimodal);
    color = imread(image_color);
    color_adapted = imread(image_color_adapted);
    if(bimodal.empty()){
        cerr<< image_bimodal + " not found"<<endl;
        return -1;
    }
    else if(color.empty()){
        cerr<< image_color + " not found"<<endl;
        return -1;
    }
    else if(color_adapted.empty()){
        cerr<< image_color_adapted + " not found"<<endl;
        return -1;
    }
    else{
        imshow(image_bimodal+" original",bimodal);
        imshow(image_color+" original",color);
        imshow(image_color_adapted+" original",color_adapted);
        waitKey(0);
        cerr<<"images read correctly"<<endl;
    }

    ///Split the color images in the 3 colorchannels and apply a filter for skin color
    vector<Mat> channels;
    Mat skinPixels_mask;
    split(color, channels); //channel[0]=blue, channel[1]=green, channel[2]=red

    skinPixels_mask = (channels[2]>95) & (channels[1]>40) & (channels[0]>20) & ((max(channels[2],max(channels[1],channels[0])) - min(channels[2],min(channels[1],channels[0])))>15) & (abs(channels[2]-channels[1])>15) & (channels[2]>channels[1]) & (channels[2]>channels[0]);
    imshow("Skin pixels", skinPixels_mask);
    cerr<<"skin pixel mask shown"<<endl;
    waitKey(0);

    //put mask on image
    Mat finaal = Mat::zeros(Size(color.rows,color.cols),CV_8UC3);
    cerr << channels[0].size() << " " << channels[0].type() << " " << skinPixels_mask.size() << " " << skinPixels_mask.type() << endl;
    Mat pixels_blue = channels[0] & skinPixels_mask;
    Mat pixels_green = channels[1] & skinPixels_mask;
    Mat pixels_red = channels[2] & skinPixels_mask;

    vector<Mat> ch;
    ch.push_back(pixels_blue);
    ch.push_back(pixels_green);
    ch.push_back(pixels_red);

    cerr<<"hier zijn we"<<endl;

    merge(ch, finaal);

    imshow("Skin mask on image",finaal);
    waitKey(0);

    ///OTSU thresholding
    //rgb->grijswaarde
    //threshold
    //histogram equalisatie equalizeHist(src.clone(), , , )
    //clahe

//    Mat result_CLAHE;
//    Ptr<CLAHE> clahe_pointer = createCLAHE();
//    clahe_pointer->setTilesGridSize(Size(15,15));
//    clahe_pointer->setClipLimit(1);
//    clahe_pointer->apply(gray_ticket.clone(), result_CLAHE);
//    imshow("CLAHE ticket grayscale", result_CLAHE); waitKey(0);
//
//    Mat mask;
//    threshold(result_CLAHE, mask, 0, 255, THRESH_OTSU | THRESH_BINARY);
//
//    findcontours RETR_EXTERNAL

    return 0;
}

