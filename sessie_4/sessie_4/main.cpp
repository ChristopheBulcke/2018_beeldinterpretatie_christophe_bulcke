#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

const int MAX_FEATURES = 500;
const float GOOD_MATCH_PERCENT = 0.15f;

int main(int argc, const char** argv)
{
    ///Adding a little help option and command line parser input
    CommandLineParser parser(argc, argv,
        "{help h usage ?    |    | show this message}"
        "{image im    |    | (required) path to image}"
        "{object ob    |    | (required) path to image}"
    );

    ///Print help message
    if(parser.has("help")){
        parser.printMessage();
        cerr <<"Use absolute paths for images."<<endl;
        return -1;
    }

    ///Empty string warning
    string img_name(parser.get<string>("image"));
    string obj_name(parser.get<string>("object"));

    if(img_name.empty()){
        parser.printMessage();
        cerr <<"Empty parameter for image!"<<endl;
        return -1;
    }
    else if(obj_name.empty())
    {
        parser.printMessage();
        cerr <<"Empty parameter for object!"<<endl;
        return -1;
    }
    else{
        cerr <<"Image parameters correct."<<endl;
    }

     ///Read image and check if read is succsfull
    Mat img = imread(img_name);
    Mat obj = imread(obj_name);
    if(img.empty()){
        cerr<< img_name + " not found"<<endl;
        return -1;
    }
    else if(obj.empty())
    {
        cerr<< obj_name + " not found"<<endl;
        return -1;
    }
    else{
//        imshow(img_name+" original",img);
//        waitKey(0);
//        imshow(obj_name+" original",obj);
//        waiSWtKey(0);
    }

    ///ORB
    // Variables to store keypoints and descriptors
    std::vector<KeyPoint> orb_keypoints1, orb_keypoints2;
    Mat orb_descriptors1, orb_descriptors2;

    // Detect ORB features and compute descriptors.
    Ptr<Feature2D> orb = ORB::create();
    orb->detectAndCompute(img, Mat(), orb_keypoints1, orb_descriptors1);
    orb->detectAndCompute(obj, Mat(), orb_keypoints2, orb_descriptors2);

    Mat orb_img_keypoints, orb_obj_keypoints;

    drawKeypoints(img.clone(),orb_keypoints1,orb_img_keypoints,Scalar(0,250,0));
    drawKeypoints(obj.clone(),orb_keypoints2,orb_obj_keypoints,Scalar(0,250,0));

    ///AKAZE
    // Variables to store keypoints and descriptors
    std::vector<KeyPoint> akaze_keypoints1, akaze_keypoints2;
    Mat akaze_descriptors1, akaze_descriptors2;

    // Detect akaze features and compute descriptors.
    Ptr<Feature2D> akaze = AKAZE::create();
    akaze->detectAndCompute(img, Mat(), akaze_keypoints1, akaze_descriptors1);
    akaze->detectAndCompute(obj, Mat(), akaze_keypoints2, akaze_descriptors2);

    Mat akaze_img_keypoints, akaze_obj_keypoints;

    drawKeypoints(img.clone(),akaze_keypoints1,akaze_img_keypoints,Scalar(0,250,0));
    drawKeypoints(obj.clone(),akaze_keypoints2,akaze_obj_keypoints,Scalar(0,250,0));

    ///BRISK
    // Variables to store keypoints and descriptors
    std::vector<KeyPoint> brisk_keypoints1, brisk_keypoints2;
    Mat brisk_descriptors1, brisk_descriptors2;

    // Detect brisk features and compute descriptors.
    Ptr<Feature2D> brisk = BRISK::create();
    brisk->detectAndCompute(img, Mat(), brisk_keypoints1, brisk_descriptors1);
    brisk->detectAndCompute(obj, Mat(), brisk_keypoints2, brisk_descriptors2);

    Mat brisk_img_keypoints, brisk_obj_keypoints;

    drawKeypoints(img.clone(),brisk_keypoints1,brisk_img_keypoints,Scalar(0,250,0));
    drawKeypoints(obj.clone(),brisk_keypoints2,brisk_obj_keypoints,Scalar(0,250,0));

    //imshow("",);

    ///Search matches ORB
    BFMatcher orb_matcher(NORM_L2);
    std::vector< DMatch > orb_matches;
    orb_matcher.match(orb_descriptors1, orb_descriptors2, orb_matches);

    ///Draw matches ORB
    Mat orb_img_matches;
    drawMatches(img, orb_keypoints1, obj, orb_keypoints2, orb_matches, orb_img_matches);

    ///Search matches AKAZE
    BFMatcher akaze_matcher(NORM_L2);
    std::vector< DMatch > akaze_matches;
    akaze_matcher.match(akaze_descriptors1, akaze_descriptors2, akaze_matches);

    ///Draw matches AKAZE
    Mat akaze_img_matches;
    drawMatches(img, akaze_keypoints1, obj, akaze_keypoints2, akaze_matches, akaze_img_matches);

    ///Search matches AKAZE
    BFMatcher brisk_matcher(NORM_L2);
    std::vector< DMatch > brisk_matches;
    brisk_matcher.match(brisk_descriptors1, brisk_descriptors2, brisk_matches);

    ///Draw matches AKAZE
    Mat brisk_img_matches;
    drawMatches(img, brisk_keypoints1, obj, brisk_keypoints2, brisk_matches, brisk_img_matches);

    imshow("Matches orb",orb_img_matches);
    waitKey(0);
    imshow("Matches akaze",akaze_img_matches);
    waitKey(0);
    imshow("Matches brisk",brisk_img_matches);
    waitKey(0);



    // Sort matches by score
//    std::sort(orb_matches.begin(), orb_matches.end());
//
//    // Remove not so good matches
//    const int orb_numGoodMatches = orb_matches.size() * GOOD_MATCH_PERCENT;
//    orb_matches.erase(orb_matches.begin()+orb_numGoodMatches, orb_matches.end());

    const float ratio_thresh = 0.75f;
    std::vector<DMatch> good_matches;
    for (size_t i = 0; i < orb_matches.size(); i++)
    {
        if (orb_matches[i][0].distance < ratio_thresh * orb_matches[i][1].distance)
        {
            good_matches.push_back(orb_matches[i][0]);
        }
    }

    // Draw top matches
    Mat orb_imMatches;
    drawMatches(img, orb_keypoints1, obj, orb_keypoints2, good_matches, orb_imMatches);
    imshow("beste matches", orb_imMatches);
    waitKey(0);


    // Extract location of good matches
    std::vector<Point2f> object, scene;

    for( size_t i = 0; i < good_matches.size(); i++ )
    {
        object.push_back( orb_keypoints1[ good_matches[i].queryIdx ].pt );
        scene.push_back( orb_keypoints2[ good_matches[i].trainIdx ].pt );
    }

    Mat imReg, h;

    // Find homography
    h = findHomography( object, scene, CV_RANSAC );


    std::vector<Point2f> obj_corners(4);
    obj_corners[0] = Point2f(0, 0);
    obj_corners[1] = Point2f( (float)obj.cols, 0 );
    obj_corners[2] = Point2f( (float)obj.cols, (float)obj.rows );
    obj_corners[3] = Point2f( 0, (float)obj.rows );
    std::vector<Point2f> scene_corners(4);

    perspectiveTransform( obj_corners, scene_corners, h);

    line( orb_imMatches, scene_corners[0] + Point2f((float)obj.cols, 0),
          scene_corners[1] + Point2f((float)obj.cols, 0), Scalar(0, 255, 0), 4 );
    line( orb_imMatches, scene_corners[1] + Point2f((float)obj.cols, 0),
          scene_corners[2] + Point2f((float)obj.cols, 0), Scalar( 0, 255, 0), 4 );
    line( orb_imMatches, scene_corners[2] + Point2f((float)obj.cols, 0),
          scene_corners[3] + Point2f((float)obj.cols, 0), Scalar( 0, 255, 0), 4 );
    line( orb_imMatches, scene_corners[3] + Point2f((float)obj.cols, 0),
          scene_corners[0] + Point2f((float)obj.cols, 0), Scalar( 0, 255, 0), 4 );


    imshow("Good Matches & Object detection", orb_imMatches );
    waitKey(0);

    return 0;

}
