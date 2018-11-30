
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

vector<Point> PosPunten;
vector<Point> NegPunten;

void CallBackFuncPosPoints(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN )
    {
        cout << "Add position (" << x << ", " << y << ")" << endl;
        //push_back: element bijvoegen
        PosPunten.push_back(Point(x,y));
    }
    else if  ( event == EVENT_RBUTTONDOWN )
    {
        cout << "Delete position (" << x << ", " << y << ")" << endl;
        //pop_back: element verwijderen
        PosPunten.pop_back();
    }
    else if  ( event == EVENT_MBUTTONDOWN )
    {
        //TODO catch empty vector
        for (int i = 0; i < PosPunten.size(); ++i)
        {
            cout << "List of clicked points: " <<endl;
            cout << PosPunten.at(i) << endl;
        }
    }
}

void CallBackFuncNegPoints(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN )
    {
        cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
        //push_back: element bijvoegen
        NegPunten.push_back(Point(x,y));
    }
    else if  ( event == EVENT_RBUTTONDOWN )
    {
        cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
        //pop_back: element verwijderen
        NegPunten.pop_back();
    }
    else if  ( event == EVENT_MBUTTONDOWN )
    {
        //TODO catch empty vector
        for (int i = 0; i < PosPunten.size(); ++i)
        {
            cout << "List of clicked points: " <<endl;
            cout << NegPunten.at(i) << endl;
        }
    }
}

int main(int argc, const char** argv)
{
    ///Adding a little help option and command line parser input
    CommandLineParser parser(argc, argv,
        "{help h usage ?    |    | show this message}"
        "{strawberry1 s1    |    | (required) path to image}"
        "{strawberry2 s2    |    | (required) path to image}"
    );

    ///Print help message
    if(parser.has("help")){
        parser.printMessage();
        cerr <<"Use absolute paths for images."<<endl;
        return -1;
    }

    ///Empty string warning
    string s1_name(parser.get<string>("strawberry1"));
    string s2_name(parser.get<string>("strawberry2"));

    if(s1_name.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for strawberry1!"<<endl;
        return -1;
    }
    else if(s2_name.empty())
    {
        parser.printMessage();
        cerr <<"Empty parameter for strawberry2!"<<endl;
        return -1;
    }
    else{
        cerr <<"Image parameters correct."<<endl;
    }

        //Create a window

    string pos_points_window("Select the positive training points");
    string neg_points_window("Select the negative training points");
    namedWindow(pos_points_window, 1);
    namedWindow(neg_points_window, 1);

    //set the callback function for any mouse event
    setMouseCallback(pos_points_window, CallBackFuncPosPoints, NULL);
    setMouseCallback(neg_points_window, CallBackFuncNegPoints, NULL);

     ///Read image and check if read is succsfull
    Mat s1 = imread(s1_name);
    GaussianBlur(s1,s1,Size(5,5), 0, 0);

    Mat s2 = imread(s2_name);
    if(s1.empty()){
        cerr<< s1_name + " not found"<<endl;
        return -1;
    }
    else if(s2.empty())
    {
        cerr<< s2_name + " not found"<<endl;
        return -1;
    }
    else{
//        imshow(s1_name + " original",s1);
//        waitKey(0);
//        imshow(s2_name+" original",s2);
//        waitKey(0);
    }

    Mat s1_pos_points;
    s1.copyTo(s1_pos_points);
    Mat s1_neg_points = s1;
    s1.copyTo(s1_neg_points);

    putText(s1_pos_points,
            "Click on the good strawberries.",
            Point(5,20), // Coordinates
            FONT_HERSHEY_COMPLEX_SMALL, // Font
            0.9, // Scale. 2.0 = 2x bigger
            Scalar(255,255,255), // BGR Color
            1, // Line Thickness (Optional)
            CV_AA);

    putText(s1_neg_points,
            "Click on the background and bad strawberries.",
            Point(5,20), // Coordinates
            FONT_HERSHEY_COMPLEX_SMALL, // Font
            0.9, // Scale. 2.0 = 2x bigger
            Scalar(255,255,255), // BGR Color
            1, // Line Thickness (Optional)
            CV_AA);

    imshow(pos_points_window, s1_pos_points);
    waitKey(0);
    imshow(neg_points_window, s1_neg_points);
    waitKey(0);

    Mat s1_hsv;
    cvtColor(s1, s1_hsv, COLOR_BGR2HSV);

    Mat trDataPosPunten(PosPunten.size(), 3, CV_32FC1);
    Mat labels_pos = Mat::ones(PosPunten.size(),CV_32FC1);

    //in for loop extract all descriptors


    Mat trDataPosPunten(PosPunten.size(), 3, CV_32FC1);
    Mat labels_pos = Mat::zeros(PosPunten.size(),CV_32FC1);

    //svm settermcriteria 1e-06

    return 0;

}
