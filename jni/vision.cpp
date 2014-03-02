#include <jni.h>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <vector>
#include <android/log.h>
#include <sstream>
#include <cstring>
#include <string>
#include <errno.h>

#define APPNAME "charminarAR"
#define PATH "/sdcard/"
using namespace cv;
using namespace std;
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, APPNAME, __VA_ARGS__))

extern "C" {

void mergeImgs(Mat& orig, Mat snap) {
    for (int i = 0; i < snap.rows; i++) {
        for (int j = 0; j < snap.cols; j++) {
            if (norm(snap.at<Vec3b>(i,j)) > 10) {
                orig.at<Vec3b>(i,j) = snap.at<Vec3b>(i,j);
            }
        }
    }

}

void tx(Mat img_object, Mat img_scene_col, Mat overlay) {

	string root = string(PATH);

    Mat scene_col = img_scene_col;
    Mat img_scene;
    if (!img_scene_col.data) {
    	LOGI("UNABLE TO READ IMAGE");
    	return;
    }
    cvtColor(img_scene_col, img_scene, CV_BGR2GRAY);
	SIFT detector(4000);

	std::vector<KeyPoint> keypoints_object, keypoints_scene;

	FileStorage fs(root + "charminarAR/obj-kpts-desc.yml", FileStorage::READ);
	if (!fs.isOpened()) {
		LOGI("Unable to open the object keypoints file!!!");
		return;
	}
	FileNode kpts = fs["kpts"];
	read(kpts, keypoints_object);

	detector.detect( img_scene, keypoints_scene );

	//-- Step 2: Calculate descriptors (feature vectors)
	SiftDescriptorExtractor extractor;

	Mat descriptors_object, descriptors_scene;

	FileNode desc = fs["desc"];
	read(desc, descriptors_object);
	extractor.compute( img_scene, keypoints_scene, descriptors_scene );

	fs.release();

	//-- Step 3: Matching descriptor vectors using FLANN matcher
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match( descriptors_object, descriptors_scene, matches );

	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for( int i = 0; i < descriptors_object.rows; i++ )
	{ double dist = matches[i].distance;
	if( dist < min_dist ) min_dist = dist;
	if( dist > max_dist ) max_dist = dist;
	}

	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< DMatch > good_matches;

	for( int i = 0; i < descriptors_object.rows; i++ )
	{ if( matches[i].distance <= 3*min_dist )
	{ good_matches.push_back( matches[i]); }
	}

	good_matches.clear();
	vector<uchar> inliers;
	vector<Point2f> pts1, pts2;
	for (int i = 0; i < matches.size(); i++) {
		pts1.push_back(keypoints_object[matches[i].queryIdx].pt);
		pts2.push_back(keypoints_scene[matches[i].trainIdx].pt);
	}
	Mat F = findFundamentalMat(Mat(pts1), Mat(pts2),
			FM_RANSAC, 3, 0.99, inliers);
	for (int i = 0; i < inliers.size(); i++) {
		if ( (int)inliers[i] ) {
			good_matches.push_back(matches[i]);
		}
	}

	Mat img_matches;

	drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
			good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
			vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
	imwrite(root + "charminarAR/matches.jpg", img_matches);

	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for( size_t i = 0; i < good_matches.size(); i++ )
	{
		//-- Get the keypoints from the good matches
		obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
		scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
	}

	Mat H = findHomography( obj, scene, RANSAC );

	warpPerspective(overlay, overlay, H, overlay.size());
	mergeImgs(scene_col, overlay);
	imwrite(root + "charminarAR/res.jpg", scene_col);
}

JNIEXPORT jint JNICALL Java_ar_charminar_MainActivity_transform(
		JNIEnv*, jobject) {
	LOGI("started native");
	string root = string(PATH);
//	FILE *f = fopen((root + "charminar/obj.jpg").c_str(), "rb");
//	if (!f) {
//		LOGI("cant open normally also!!");
//		LOGI("fopen() failed: %s", strerror(errno));
//	}
	//LOGI((root + "charminarAR/obj.jpg").c_str());
	Mat img_obj = imread(root + "charminarAR/obj.jpg");
	Mat img_overlay = imread(root + "charminarAR/overlay.jpg");
	Mat img_scn = imread(root + "charminarAR/temp.jpg");
	resize(img_scn, img_scn, Size(1024, 768));
	if (!img_scn.data) {
		LOGI("UNABLE TO READ QUERY IMG");
	}
	tx(img_obj, img_scn, img_overlay);
}
}
