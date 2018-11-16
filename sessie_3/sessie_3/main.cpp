#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

bool use_mask;
Mat img; Mat templ; Mat mask; Mat result;
const char* image_window = "Source Image";
const char* result_window = "Result window";
int match_method;
int max_Trackbar = 5;
void MatchingMethod( int, void* );

int main(int argc, const char** argv)
{
    ///Adding a little help option and command line parser input
    CommandLineParser parser(argc, argv,
        "{help h usage ?    |    | show this message}"
        "{recht re    |    | (required) path to image}"
        "{rot ro    |    | (required) path to image}"
        "{temp te    |    | (required) path to image}"
    );

    ///Print help message
    if(parser.has("help")){
        parser.printMessage();
        cerr <<"Use absolute paths for images."<<endl;
        return -1;
    }

    ///Empty string warning
    string recht(parser.get<string>("recht"));
    string rot(parser.get<string>("rot"));
    string temp(parser.get<string>("temp"));
    if(recht.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for recht image!"<<endl;
        return -1;
    }
    else if(rot.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for rot image!"<<endl;
        return -1;
    }
    else if(temp.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for temp image!"<<endl;
        return -1;
    }
    else{
        cerr <<"Image parameters correct."<<endl;
    }


    ///Read images and check if read is succsfull
    img = imread(recht,IMREAD_GRAYSCALE);
    Mat rot_img = imread(rot);
    templ = imread(temp,IMREAD_GRAYSCALE);

    if(img.empty()){
        cerr<< recht + " not found"<<endl;
        return -1;
    }
    else if(rot_img.empty()){
        cerr<< rot + " not found"<<endl;
        return -1;
    }
    else if(templ.empty()){
        cerr<< temp + " not found"<<endl;
        return -1;
    }
    else{
//        imshow(recht+" original",img);
//        waitKey(0);
//        imshow(rot+" original",rot_img);
//        waitKey(0);
//        imshow(temp+" original",templ);
//        waitKey(0);
    }

    namedWindow( image_window, WINDOW_AUTOSIZE );
    namedWindow( result_window, WINDOW_AUTOSIZE );
    const char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
    createTrackbar( trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod );
    MatchingMethod( 0, 0 );
    waitKey(0);

    return 0;
}

void MatchingMethod( int, void* )
{
    Mat img_display;
    img.copyTo( img_display );
    int result_cols =  img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;
    result.create( result_rows, result_cols, CV_32FC1 );

    bool method_accepts_mask = (TM_SQDIFF == match_method || match_method == TM_CCORR_NORMED);
    if (use_mask && method_accepts_mask)
    {
        matchTemplate( img, templ, result, match_method, mask);
    }
    else
    {
        matchTemplate( img, templ, result, match_method);
    }


    normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
    double minVal; double maxVal; Point minLoc; Point maxLoc; Point matchLoc;
    inRange(result,Scalar(0.9),Scalar(1),result);

    vector<vector<Point> > contours;
    findContours(result, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    for (int i = 0; i < contours.size(); i++)
    {
        Rect region = boundingRect(contours[i]);
        Mat temp = result(region);
        Point maxLoc;
        minMaxLoc(temp,NULL,NULL,NULL,&maxLoc);
        rectangle(img_display, Point(region.x + maxLoc.x, region.y + maxLoc.y),
            Point(maxLoc.x + region.x + templ.cols, region.y + maxLoc.y + templ.rows),Scalar::all(0), 2, 8, 0);
    }

    imshow( image_window, img_display );
    imshow( result_window, result );
    return;
}
