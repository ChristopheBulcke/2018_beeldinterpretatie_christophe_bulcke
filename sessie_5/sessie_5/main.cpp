
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace ml;

vector<Point> PosPunten;
vector<Point> NegPunten;

void drawPoints(Mat);

void CallBackFuncPosPoints(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN )
    {
        cout << "Add foreground position (" << x << ", " << y << ")" << endl;
        //push_back: element bijvoegen
        PosPunten.push_back(Point(x,y));
    }
    else if  ( event == EVENT_RBUTTONDOWN )
    {
        if(PosPunten.size() > 0)
        {
            cout << "Deleted last added position. " << PosPunten.size()-1 << " entries left." << endl;
            //pop_back: element verwijderen
            PosPunten.pop_back();
        }
        else
        {
            cout << "There are no points in the list." << endl;
        }

    }
    else if  ( event == EVENT_MBUTTONDOWN )
    {
        if(PosPunten.size() == 0) cout << "There are no points in the list" << endl;
        else
        {
            for (int i = 0; i < PosPunten.size(); ++i)
            {
                cout << "List of clicked points: " <<endl;
                cout << PosPunten.at(i) << endl;
            }
        }
    }
}

void CallBackFuncNegPoints(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN )
    {
        cout << "Add background position (" << x << ", " << y << ")" << endl;
        //push_back: element bijvoegen
        NegPunten.push_back(Point(x,y));
    }
    else if  ( event == EVENT_RBUTTONDOWN )
    {
        if(NegPunten.size() > 0)
        {
            cout << "Deleted last added position. " << NegPunten.size()-1 << " entries left." << endl;
            //pop_back: element verwijderen
            NegPunten.pop_back();
        }
        else
        {
            cout << "There are no points in the list." << endl;
        }
    }
    else if  ( event == EVENT_MBUTTONDOWN )
    {
        if(NegPunten.size() == 0) cout << "There are no points in the list." << endl;
        else
        {
            for (int i = 0; i < NegPunten.size(); ++i)
            {
                cout << "List of clicked points: " <<endl;
                cout << NegPunten.at(i) << endl;
            }
        }
    }
}

int main(int argc, const char** argv)
{
    ///Adding a little help option and command line parser input
    CommandLineParser parser(argc, argv,
        "{help h usage ?    |    | show this message}"
        "{strawberry1 s1    |    | (required) path to image}"
    );

    ///Print help message
    if(parser.has("help")){
        parser.printMessage();
        cerr <<"Use absolute paths for images."<<endl;
        return -1;
    }

    ///Empty string warning
    string s1_name(parser.get<string>("strawberry1"));

    if(s1_name.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for strawberry1!"<<endl;
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

    ///We're using mouseclicks to select keypoints in the image
    //set the callback function for any mouse event
    setMouseCallback(pos_points_window, CallBackFuncPosPoints, NULL);
    setMouseCallback(neg_points_window, CallBackFuncNegPoints, NULL);

    ///Read image and check if read is succsfull
    Mat s1 = imread(s1_name);

    if(s1.empty()){
        cerr<< s1_name + " not found"<<endl;
        return -1;
    }
    else{
//        imshow(s1_name + " original",s1);
//        waitKey(0);
    }

    Mat s1_pos_points;
    s1.copyTo(s1_pos_points);
    Mat s1_neg_points;
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
    destroyWindow(pos_points_window);
    imshow(neg_points_window, s1_neg_points);
    waitKey(0);
    destroyWindow(neg_points_window);

    ///extract all trainingsdata
    drawPoints(s1); //show all points

    ///De groendienst komt langs
    Mat bgr_channels[3];
    split(s1, bgr_channels);
    bgr_channels[1] = Mat::zeros(s1.rows, s1.cols, CV_8UC1);
    merge(bgr_channels, 3, s1);

    ///Put image in hsv to create descriptors

    Mat s1_hsv;
    cvtColor(s1, s1_hsv, COLOR_BGR2HSV);

    GaussianBlur(s1_hsv,s1_hsv,Size(5,5), 0, 0);

    Mat trDataPosPunten(PosPunten.size(), 3, CV_32FC1);
    Mat labels_pos = Mat::ones(PosPunten.size(), 1, CV_32SC1);

    //descriptors are the hsv values of each point in PosPunten
    for (int i = 0; i < PosPunten.size(); i++)
    {
        Vec3b descriptor1 = s1_hsv.at<Vec3b>(PosPunten[i]);

        trDataPosPunten.at<float>(i,0) = descriptor1[0];
        trDataPosPunten.at<float>(i,1) = descriptor1[1];
        trDataPosPunten.at<float>(i,2) = descriptor1[2];
    }


    Mat trDataNegPunten(NegPunten.size(), 3, CV_32FC1);
    Mat labels_neg = Mat::zeros(NegPunten.size(), 1, CV_32SC1);

    for (int j = 0; j < NegPunten.size(); j++)
    {
        Vec3b descriptor2 = s1_hsv.at<Vec3b>(NegPunten[j]);

        trDataNegPunten.at<float>(j,0) = descriptor2[0];
        trDataNegPunten.at<float>(j,1) = descriptor2[1];
        trDataNegPunten.at<float>(j,2) = descriptor2[2];
    }

    Mat trainData, labels;
    //combine all the training points and labels so we can later scan them row by row
    vconcat(trDataPosPunten, trDataNegPunten, trainData);   //these are the trainingsamples
    vconcat(labels_pos, labels_neg, labels);

    ///Training
    // knn
    Ptr<TrainData> trainingData;
    Ptr<KNearest> kclassifier = KNearest::create(); //create a knearest classifie

    trainingData = TrainData::create(trainData, ROW_SAMPLE, labels);
    kclassifier->setIsClassifier(true);
    kclassifier->setAlgorithmType(KNearest::BRUTE_FORCE);
    kclassifier->setDefaultK(3);

    kclassifier->train(trainingData);

    //Bayes

    Ptr<NormalBayesClassifier> bayes = NormalBayesClassifier::create();
    bayes->train(trainData, ROW_SAMPLE, labels);

    //svm

    Ptr<SVM> svm = SVM::create();
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::LINEAR);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
    svm->train(trainData, ROW_SAMPLE, labels);


    /// Classification

    Mat labels_knn, labels_bay, labels_svm;
    Mat result_knn = Mat::zeros(s1.rows, s1.cols, CV_8UC1);
    Mat result_bay = Mat::zeros(s1.rows, s1.cols, CV_8UC1);
    Mat result_svm = Mat::zeros(s1.rows, s1.cols, CV_8UC1);

    for(int i=0; i < s1.rows; i++)
    {
        for(int j=0; j < s1.cols; j++)
        {

            Vec3b pixel = s1_hsv.at<Vec3b>(i, j);
            Mat data(1,3, CV_32FC1);
            data.at<float>(0,0) = pixel[0];
            data.at<float>(0,1) = pixel[1];
            data.at<float>(0,2) = pixel[2];

            // pixel per pixel nearest zoeken
            kclassifier->findNearest(data, kclassifier->getDefaultK(), labels_knn);
            bayes->predict( data, labels_bay);
            svm->predict(data, labels_svm);

            result_knn.at<uchar>(i,j) = (uchar)labels_knn.at<float>(0,0);
            result_bay.at<uchar>(i,j) = (uchar)labels_bay.at<int>(0,0);
            result_svm.at<uchar>(i,j) = (uchar)labels_svm.at<float>(0,0);
        }
    }


    imshow("result knn", result_knn*255);
    imshow("result bay", result_bay*255);
    imshow("result svm", result_svm*255);

    waitKey(0);

    //svm settermcriteria 1e-06

    return 0;

}

void drawPoints(Mat x)
{
    Mat img;
    x.copyTo(img);

    for (int i = 0; i < PosPunten.size(); i++)
    {
        circle(img, PosPunten[i], 2, Scalar(0,255,0));
    }
    for (int j = 0; j < NegPunten.size(); j++)
    {
        circle(img, NegPunten[j], 2, Scalar(0,0,255));
    }

    imshow("Red = background, Green = strawberry",img);
    waitKey(0);
}
