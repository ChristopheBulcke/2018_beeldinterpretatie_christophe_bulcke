#include <iostream>
#include <opencv2/opencv.hpp>



using namespace std;
using namespace cv;

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
    vector<KeyPoint> orb_keypoints_scene, orb_keypoints_obj;
    Mat orb_descriptors_scene, orb_descriptors_obj;

    // Detect ORB features and compute descriptors.
    Ptr<Feature2D> orb = ORB::create();
    orb->detectAndCompute(obj, Mat(), orb_keypoints_obj, orb_descriptors_obj);
    orb->detectAndCompute(img, Mat(), orb_keypoints_scene, orb_descriptors_scene);

    //Search matches ORB
    BFMatcher orb_matcher(NORM_L2);
    vector<DMatch> orb_matches;
    orb_matcher.match(orb_descriptors_obj, orb_descriptors_scene, orb_matches);

    //Draw matches ORB
    Mat orb_img_matches;
    drawMatches(obj, orb_keypoints_obj, img, orb_keypoints_scene, orb_matches, orb_img_matches);

    imshow("Matches orb",orb_img_matches);
    waitKey(0);

    ///AKAZE
    // Variables to store keypoints and descriptors
//    std::vector<KeyPoint> akaze_keypoints1, akaze_keypoints2;
//    Mat akaze_descriptors1, akaze_descriptors2;
//
//    // Detect akaze features and compute descriptors.
//    Ptr<Feature2D> akaze = AKAZE::create();
//    akaze->detectAndCompute(img, Mat(), akaze_keypoints1, akaze_descriptors1);
//    akaze->detectAndCompute(obj, Mat(), akaze_keypoints2, akaze_descriptors2);
//
//    Mat akaze_img_keypoints, akaze_obj_keypoints;
//
//    drawKeypoints(img,akaze_keypoints1,akaze_img_keypoints,Scalar(0,250,0));
//    drawKeypoints(obj,akaze_keypoints2,akaze_obj_keypoints,Scalar(0,250,0));
//
//    //Search matches AKAZE
//    BFMatcher akaze_matcher(NORM_L2); //not compatible
//    vector< DMatch > akaze_matches;
//    akaze_matcher.match(akaze_descriptors1, akaze_descriptors2, akaze_matches);
//
//    //Draw matches AKAZE
//    Mat akaze_img_matches;
//    drawMatches(img, akaze_keypoints1, obj, akaze_keypoints2, akaze_matches, akaze_img_matches);
//
//    imshow("Matches akaze",akaze_img_matches);
//    waitKey(0);

    ///BRISK
    // Variables to store keypoints and descriptors
//    std::vector<KeyPoint> brisk_keypoints1, brisk_keypoints2;
//    Mat brisk_descriptors1, brisk_descriptors2;
//
//    // Detect brisk features and compute descriptors.
//    Ptr<Feature2D> brisk = BRISK::create();
//    brisk->detectAndCompute(img, Mat(), brisk_keypoints1, brisk_descriptors1);
//    brisk->detectAndCompute(obj, Mat(), brisk_keypoints2, brisk_descriptors2);
//
//    Mat brisk_img_keypoints, brisk_obj_keypoints;
//
//    drawKeypoints(img.clone(),brisk_keypoints1,brisk_img_keypoints,Scalar(0,250,0));
//    drawKeypoints(obj.clone(),brisk_keypoints2,brisk_obj_keypoints,Scalar(0,250,0));
//
//    //Search matches BRISK
//    BFMatcher brisk_matcher(NORM_L2);   //not compatible
//    std::vector< DMatch > brisk_matches;
//    brisk_matcher.match(brisk_descriptors1, brisk_descriptors2, brisk_matches);
//
//    //Draw matches BRISK
//    Mat brisk_img_matches;
//    drawMatches(img, brisk_keypoints1, obj, brisk_keypoints2, brisk_matches, brisk_img_matches);
//
//    imshow("Matches brisk",brisk_img_matches);
//    waitKey(0);



    ///RANSAC & best matches

    ///matches filter optie 1
    //Sort matches by score
//    std::sort(orb_matches.begin(), orb_matches.end());
//
//    // Remove not so good matches
//    std::vector<DMatch> good_matches = orb_matches;
//    const int orb_numGoodMatches = good_matches.size() * GOOD_MATCH_PERCENT;
//    good_matches.erase(good_matches.begin()+orb_numGoodMatches, good_matches.end());
    ///matches filter optie 2
//    const float ratio_thresh = 0.75f;
//    std::vector<DMatch> good_matches;
//    for (size_t i = 0; i < orb_matches.size(); i++)
//    {
//        if (orb_matches[i][0].distance < ratio_thresh * orb_matches[i][1].distance)
//        {
//            good_matches.push_back(orb_matches[i][0]);
//        }
//    }

    ///matches filter optie 3
    //double max_dist = 0; double min_dist = 100;

    //-- Quick calculation of max and min distances between keypoints
//    for( int i = 0; i < orb_descriptors_obj.rows; i++ )
//    {
//        double dist = orb_matches[i].distance;
//        if( dist < min_dist ) min_dist = dist;
//        if( dist > max_dist ) max_dist = dist;
//    }
//
//    printf("-- Max dist : %f \n", max_dist );
//    printf("-- Min dist : %f \n", min_dist );
//
//    //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
//    std::vector< DMatch > good_matches;
//
//    for( int i = 0; i < orb_descriptors_obj.rows; i++ )
//    { if( orb_matches[i].distance < 3*min_dist )
//        { good_matches.push_back( orb_matches[i]); }
//    }

    ///matches filter optie 4
    double min_dist = 1000, max_dist = 0;
    vector<DMatch> good_matches;

    // Find min and max distance
    for(int i=0; i < orb_descriptors_obj.rows; i++)
    {
        double distance = orb_matches[i].distance;

        // Valid distance, update
        if(distance < min_dist) {
            min_dist = distance;
        }
        if(distance > max_dist) {
            max_dist = distance;
        }
    }

    // Filter on distance
//    for(int i=0; i < orb_descriptors_obj.rows; i++ )
//    {
//        if(orb_matches.at(i).distance <= DISTANCE_MULTIPLIER * (maximum-minimum) + minimum) // min + DISTANCE_MULTIPLIER * difference
//        {
//            good_matches.push_back(orb_matches.at(i));
//        }
//    }

    for(int i=0; i < orb_descriptors_obj.rows; i++ )
    {
        if(orb_matches[i].distance <= 3 * min_dist)
        {
            good_matches.push_back(orb_matches[i]);
        }
    }

    // Draw top matches
    Mat result;
    drawMatches(obj, orb_keypoints_obj, img, orb_keypoints_scene, good_matches, result);
    imshow("beste matches", result);
    waitKey(0);


    // Extract location of good matches
    vector<Point2f> object, scene;
    for( int i = 0; i < good_matches.size(); i++ )
    {
        //-- Get the keypoints from the good matches
        object.push_back( orb_keypoints_obj[ good_matches[i].queryIdx ].pt );
        scene.push_back( orb_keypoints_scene[ good_matches[i].trainIdx ].pt );
    }

    // Find homography
    Mat h = findHomography( object, scene, RANSAC );

    vector<Point2f> obj_corners(4);
    obj_corners[0] = Point2f(0, 0);
    obj_corners[1] = Point2f( (float)obj.cols, 0 );
    obj_corners[2] = Point2f( (float)obj.cols, (float)obj.rows );
    obj_corners[3] = Point2f( 0, (float)obj.rows );
    vector<Point2f> scene_corners(4);

    perspectiveTransform( obj_corners, scene_corners, h);

    line( result, scene_corners[0] + Point2f((float)obj.cols, 0),
          scene_corners[1] + Point2f((float)obj.cols, 0), Scalar(0, 255, 0), 4 );

    line( result, scene_corners[1] + Point2f((float)obj.cols, 0),
          scene_corners[2] + Point2f((float)obj.cols, 0), Scalar( 0, 255, 0), 4 );

    line( result, scene_corners[2] + Point2f((float)obj.cols, 0),
          scene_corners[3] + Point2f((float)obj.cols, 0), Scalar( 0, 255, 0), 4 );

    line( result, scene_corners[3] + Point2f((float)obj.cols, 0),
          scene_corners[0] + Point2f((float)obj.cols, 0), Scalar( 0, 255, 0), 4 );


    imshow("Good Matches & Object detection", result );
    waitKey(0);

    return 0;

}
