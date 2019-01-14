#include <iostream>
#include <opencv2/opencv.hpp>

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

    //calculate bounding rectangle of the whiteboard so we have a reference for the perspective transform
    Rect r1 = boundingRect(boardCorners);

    //there is a fixed order in wich the corners need to be clicked on, so we are able to match the clicked corners against the bounding rectangle corners
    vector<Point> boundingRectanglePoints;
    boundingRectanglePoints.push_back(Point(r1.x , r1.y));    //top left
    boundingRectanglePoints.push_back(Point(r1.x+r1.width , r1.y));    //top right
    boundingRectanglePoints.push_back(Point(r1.x+r1.width , r1.y+r1.height));    //bottom right
    boundingRectanglePoints.push_back(Point(r1.x , r1.y+r1.height));    //bottom left

    Point2f temp1[4];
    Point2f temp2[4];

    //put the corners of the whiteboard in an array
    temp1[0] = boardCorners.at(0);
    temp1[1] = boardCorners.at(1);
    temp1[2] = boardCorners.at(2);
    temp1[3] = boardCorners.at(3);

    //put the corners of the boundingrectangle in an array
    temp2[0] = boundingRectanglePoints.at(0);
    temp2[1] = boundingRectanglePoints.at(1);
    temp2[2] = boundingRectanglePoints.at(2);
    temp2[3] = boundingRectanglePoints.at(3);

    M = getPerspectiveTransform(temp1,temp2);
    warpPerspective(board,transformed,M,transformed.size());
    imshow("transformed mask",transformed);
    waitKey(0);

    Mat transformedCropped = Mat::zeros(r1.height,r1.width,img.type());

    Mat tmp = transformed(Rect(r1.x+1,r1.y+1,r1.width-1,r1.height-1));
    tmp.copyTo(transformedCropped);
    imshow("transformedCropped",transformedCropped);
    waitKey(0);

    ///Preprocessing
    //noise reduction

    Mat gray = Mat::zeros(r1.height,r1.width,CV_8UC1);
    cvtColor(transformedCropped,gray,CV_BGR2GRAY);
    imshow("gray",gray);
    waitKey(0);

    Mat grayCLAHE;
    Ptr<CLAHE> clahe_pointer = createCLAHE();
    clahe_pointer->setTilesGridSize(Size(15,15));
    clahe_pointer->setClipLimit(1);
    clahe_pointer->apply(gray.clone(), grayCLAHE);
    imshow("gray CLAHE", grayCLAHE); waitKey(0);

//    Mat smoothedBilateral;
//    bilateralFilter ( gray, smoothedBilateral, 15, 80, 80 );
//    imshow("smoothedBilateral",smoothedBilateral);
//    waitKey(0);

    Mat smoothedGaussian;
    GaussianBlur(grayCLAHE,smoothedGaussian,Size(5,5), 0, 0);
    imshow("smoothedGaussian",smoothedGaussian);
    waitKey(0);

//    Mat smoothedMedian;
//    medianBlur(gray,smoothedMedian,3);
//    imshow("smoothedMedian",smoothedMedian);
//    waitKey(0);

//    Mat sharpenedBilateral,sharpenedGaussian,sharpenedMedian;
    float kdata[] = {-1, -1, -1, -1, 9, -1, -1, -1, -1};
    Mat kernel(3,3,CV_32F,kdata);  //laplace kernel for sharpening the image

//    filter2D(smoothedBilateral,sharpenedBilateral,-1,kernel);
//    imshow("sharpenedBilateral",sharpenedBilateral);
//    waitKey(0);

//    filter2D(smoothedGaussian,sharpenedGaussian,-1,kernel);
//    imshow("sharpenedGaussian",sharpenedGaussian);
//    waitKey(0);

//    filter2D(smoothedMedian,sharpenedMedian,-1,kernel);
//    imshow("sharpenedMedian",sharpenedMedian);
//    waitKey(0);

//    Mat thresh;
//    inRange(sharpenedGaussian,0,150,thresh);
//    imshow("gaussian, thresholded",thresh);
//    waitKey(0);

//    inRange(sharpenedBilateral,0,150,thresh);
//    imshow("bilateral, thresholded",thresh);
//    waitKey(0);
//
//    inRange(sharpenedMedian,0,150,thresh);
//    imshow("median, thresholded",thresh);
//    waitKey(0);

    ///Mask
    Mat maskThresh = Mat::zeros(r1.height,r1.width,CV_8UC1);
    adaptiveThreshold(smoothedGaussian, maskThresh, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 5, 3);
    imshow("gray CLAHE adaptive gaussian threshold", maskThresh);
    waitKey(0);

    ///Convert to HSV to quantise different colors
    Mat hsv,hsvMasked,result;
    vector<Mat> channels;
    //Mat bgrQuantised = Mat::zeros(hsv.rows,hsv.cols,hsv.type());

    cvtColor(transformedCropped,hsv,CV_BGR2HSV);
    Mat hsvQuantised = Mat::zeros(hsv.rows,hsv.cols,hsv.type());
    hsv.copyTo(hsvMasked,maskThresh);

    //all the interval boundaries for quantising the hues
    int reL = 170;
    int reU = 12;
    int orL = 13;
    int orU = 22;
    int yeL = 23;
    int yeU = 32;
    int grL = 33;
    int grU = 85;
    int blL = 86;
    int blU = 127;
    int puL = 128;
    int puU = 145;
    int piL = 146;
    int piU = 169;

    int colorIntervals[7][2] = {{reL,reU},
                                {orL,orU},
                                {yeL,yeU},
                                {grL,grU},
                                {blL,blU},
                                {puL,puU},
                                {piL,piU}}; //all the intervals for quantising the hsv hues

    int drawValue[7] = {0,
                        15,
                        25,
                        60,
                        115,
                        140,
                        154};   //all the quantised hsv hues

    int colorIntervalsSize = sizeof(colorIntervals)/sizeof(*colorIntervals); //calculate the size of
    Vec3b pixelValue;
    Vec3b pixelValueBGR;

    int maskPixel = 0;

    for (int r = 0; r < hsvMasked.rows; r++)
    {
        for (int c = 0; c < hsvMasked.cols; c++)
        {
            pixelValue = hsv.at<Vec3b>(r,c);    //pixel value in hsv
            pixelValueBGR = transformedCropped.at<Vec3b>(r,c);  //pixelvalue in bgr
            maskPixel = maskThresh.at<uint8_t>(r,c);    //mask pixel value (black or white)
            if(maskPixel == 255) //if pixel is an element of the mask (white pixel)
            {
                for (int i = 0; i < colorIntervalsSize; i++) //run over all the color intervals
                {
                    ///Uncomment if you want the colors to be quantised to the above values
                    //if( (pixelValue[0] >= colorIntervals[i][0]) && (pixelValue[0] <= colorIntervals[i][1]))
                    //{
                        //hsvQuantised.at<Vec3b>(r,c)[0] = drawValue[i];
//                        hsvQuantised.at<Vec3b>(r,c)[0] = pixelValue[0];
//                        hsvQuantised.at<Vec3b>(r,c)[1] = 255;
//                        hsvQuantised.at<Vec3b>(r,c)[2] = 255;
                        //break;  //quit when the right interval has been found
                    //}

                    if(pixelValueBGR[2] <= 25)
                    {
                        //cerr <<"H" << (int)pixelValue[0] << " S" << (int)pixelValue[1] << " V" << (int)pixelValue[2] << endl;
                        hsvQuantised.at<Vec3b>(r,c)[0] = 0;
                        hsvQuantised.at<Vec3b>(r,c)[1] = 0;
                        hsvQuantised.at<Vec3b>(r,c)[2] = 0;
                        break; //quit if the pixel was black to avoid unnecessary calculations
                    }
                    else{
                        hsvQuantised.at<Vec3b>(r,c)[0] = pixelValue[0];
                        hsvQuantised.at<Vec3b>(r,c)[1] = 255;
                        hsvQuantised.at<Vec3b>(r,c)[2] = 255;
                    }
                }
            }
            else    //if pixel isnt an element of the mask put it on white ("whiteboard")
            {
                hsvQuantised.at<Vec3b>(r,c)[0] = 0;
                hsvQuantised.at<Vec3b>(r,c)[1] = 0;
                hsvQuantised.at<Vec3b>(r,c)[2] = 240;
            }
        }
    }

    cvtColor(hsvQuantised,result,CV_HSV2BGR);
    imshow("result", result);
    waitKey(0);

    return 0;
}

