#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

RNG rng(12345);

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
    string image_color_adapted(parser.get<string>("image_color_adapted"));
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
        waitKey(0);
        imshow(image_color+" original",color);
        waitKey(0);
        imshow(image_color_adapted+" original",color_adapted);
        waitKey(0);
        cerr<<"images read correctly"<<endl;
        cerr<<image_color <<" "<< image_color_adapted <<endl;
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
    //cerr << channels[0].size() << " " << channels[0].type() << " " << skinPixels_mask.size() << " " << skinPixels_mask.type() << endl;
    Mat pixels_blue = channels[0] & skinPixels_mask;
    Mat pixels_green = channels[1] & skinPixels_mask;
    Mat pixels_red = channels[2] & skinPixels_mask;

    vector<Mat> ch;
    ch.push_back(pixels_blue);
    ch.push_back(pixels_green);
    ch.push_back(pixels_red);

    merge(ch, finaal);

    imshow("Skin mask on image",finaal);
    waitKey(0);

    ///OTSU thresholding
    //RGB 2 GRAY
    Mat bim_gray;
    cvtColor(bimodal,bim_gray,CV_RGB2GRAY);

    //OTSU threshold
    Mat bim_bw;
    threshold(bim_gray,bim_bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    imshow("Bimodal OTSU threshold", bim_bw);
    waitKey(0);

    //Histogram equalization then OTSU threshold
    Mat bim_eq;
    equalizeHist(bim_gray.clone(),bim_eq);
    imshow("Bimodal hist. eq.",bim_eq);
    threshold(bim_eq,bim_bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    imshow("Bimodal hist. eq. OTSU threshold", bim_bw);
    waitKey(0);

    //CLAHE then OTSU threshold
    Mat result_CLAHE;
    Ptr<CLAHE> clahe_pointer = createCLAHE();
    clahe_pointer->setTilesGridSize(Size(15,15));
    clahe_pointer->setClipLimit(1);
    clahe_pointer->apply(bim_gray.clone(), result_CLAHE);
    imshow("Bimodal CLAHE", result_CLAHE); waitKey(0);
    Mat mask;
    threshold(result_CLAHE, mask, 0, 255, THRESH_OTSU | THRESH_BINARY);
    imshow("Bimodal CLAHE OTSU threshold", mask); waitKey(0);

    ///Erode and dilate
    split(color_adapted, channels); //channel[0]=blue, channel[1]=green, channel[2]=red
    skinPixels_mask = (channels[2]>95) & (channels[1]>40) & (channels[0]>20) & ((max(channels[2],max(channels[1],channels[0])) - min(channels[2],min(channels[1],channels[0])))>15) & (abs(channels[2]-channels[1])>15) & (channels[2]>channels[1]) & (channels[2]>channels[0]);
    skinPixels_mask = skinPixels_mask * 255;
    imshow("Skin pixels", skinPixels_mask);
    waitKey(0);

    //erode and dilate mask
    Mat skinPixels_mask_adapted;
    Point anchor = Point(-1,-1);

    erode(skinPixels_mask,skinPixels_mask_adapted,Mat(),anchor,1);
    dilate(skinPixels_mask_adapted,skinPixels_mask_adapted,Mat(),anchor,5);

    //Connect all the blobs
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours( skinPixels_mask_adapted, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0) );

    vector<vector<Point> >hull( contours.size() );
    for( size_t i = 0; i < contours.size(); i++ ){
        convexHull( Mat(contours[i]), hull[i], false );
    }
    Mat connected_blobs = Mat::zeros( skinPixels_mask_adapted.size(), CV_8UC1 );
    for( size_t i = 0; i< contours.size(); i++ ){
       drawContours( connected_blobs, hull, (int)i, Scalar(255,255,255), -1, 8, vector<Vec4i>(), 0, Point() );
    }

    imshow("Skin mask with erosian and dilation",connected_blobs);
    waitKey(0);

    //put mask on image
    pixels_blue = channels[0] & connected_blobs;
    pixels_green = channels[1] & connected_blobs;
    pixels_red = channels[2] & connected_blobs;

    vector<Mat> ch_adapted;
    ch_adapted.push_back(pixels_blue);
    ch_adapted.push_back(pixels_green);
    ch_adapted.push_back(pixels_red);

    Mat finaal_adapted = Mat::zeros(Size(color_adapted.rows,color_adapted.cols),CV_8UC3);

    merge(ch_adapted, finaal_adapted);

    imshow("Skin mask on image wtih erosian and dilation",finaal_adapted);

    waitKey(0);
//
//    findcontours RETR_EXTERNAL

    return 0;
}

