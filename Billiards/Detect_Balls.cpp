#include "Detect_Balls.h"
#include "Ball.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;


Detect_Balls::Detect_Balls()
{
			
}

void Detect_Balls::Find_PC_Balls(Ball* Red, Ball* Yellow, Ball* White)
{

	Mat src, gray;
	src = imread("6_1.png", 1);

	Mat bgr_image =  imread("6_1.png", 1);
	Mat orig_image = bgr_image.clone();
	medianBlur(bgr_image, bgr_image, 3);

	// Convert input image to HSV
	Mat hsv_image;
	cvtColor(bgr_image, hsv_image,  COLOR_BGR2HSV);

	// Threshold the HSV image, keep only the red pixels
	Mat lower_red_hue_range, lower_yellow_hue_range, lower_white_hue_range;
	Mat upper_red_hue_range, upper_yellow_hue_range, upper_white_hue_range;
	inRange(hsv_image, Scalar(0, 100, 100), Scalar(10, 255, 255), lower_red_hue_range);
	inRange(hsv_image, Scalar(160, 100, 100), Scalar(179, 255, 255), upper_red_hue_range);
	inRange(hsv_image, Scalar(15, 100, 100), Scalar(50, 1.00 * 256, 1.00 * 256), lower_yellow_hue_range);
	inRange(hsv_image, Scalar(60, 100, 100), Scalar(80, 255, 255), upper_yellow_hue_range);
	inRange(hsv_image, Scalar(0, 0, 200), Scalar(255, 20, 255), lower_white_hue_range);
	inRange(hsv_image, Scalar(180, 0, 200), Scalar(360, 0, 255), upper_white_hue_range);

	// Combine the above two images
	Mat red_hue_image, yellow_hue_image, white_hue_image;
	addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);
	addWeighted(lower_yellow_hue_range, 1.0, upper_yellow_hue_range, 0.0, 0.0, yellow_hue_image);
	addWeighted(lower_white_hue_range, 1.0, upper_white_hue_range, 0.0, 0.0, white_hue_image);

	GaussianBlur(red_hue_image, red_hue_image,  Size(9, 9), 2, 2);
	GaussianBlur(yellow_hue_image, yellow_hue_image,  Size(9, 9), 2, 2);
	GaussianBlur(white_hue_image, white_hue_image,  Size(9, 9), 2, 2);

	// Use the Hough transform to detect circles in the combined threshold image
	std::vector< Vec3f> circles_r, circles_y, circles_w;
	HoughCircles(red_hue_image, circles_r, CV_HOUGH_GRADIENT, 1, red_hue_image.rows / 8, 100, 20, 0, 45);
	HoughCircles(red_hue_image, circles_y, CV_HOUGH_GRADIENT, 1, yellow_hue_image.rows / 8, 100, 20, 0, 45);
	HoughCircles(red_hue_image, circles_w, CV_HOUGH_GRADIENT, 1, white_hue_image.rows / 8, 100, 20, 0, 45);

	// Loop over all detected circles and outline them on the original image
	//Set Red Ball's Pixel Coordinates
	//if (circles_r.size() == 0) std::exit(-1);
	for (size_t current_circle = 0; current_circle < circles_r.size(); ++current_circle) {
		 Point center(std::round(circles_r[current_circle][0]), std::round(circles_r[current_circle][1]));
		int radius = std::round(circles_r[current_circle][2]);

		 circle(orig_image, center, radius,  Scalar(0, 255, 0), 5);
	}
	Red->Pers2D_input(0, 0) = cvRound(circles_r[0][0]); Red->Pers2D_input(1, 0) = cvRound(circles_r[0][1]); Red->Pers2D_input(2, 0) = 1;
	
    //Set Yellow Ball's Pixel Coordinates
	//if (circles_y.size() == 0) std::exit(-1);
	for (size_t current_circle = 0; current_circle < circles_y.size(); ++current_circle) {
		 Point center(std::round(circles_y[current_circle][0]), std::round(circles_y[current_circle][1]));
		int radius = std::round(circles_y[current_circle][2]);

		 circle(orig_image, center, radius,  Scalar(0, 255, 0), 5);
	}
	Yellow->Pers2D_input(0,0) = cvRound(circles_y[0][0]); Yellow->Pers2D_input(1, 0) = cvRound(circles_y[0][1]); Yellow->Pers2D_input(2, 0) = 1;
	
	//Set White Ball's Pixel Coordinates
	//if (circles_w.size() == 0) std::exit(-1);
	for (size_t current_circle = 0; current_circle < circles_w.size(); ++current_circle) {
		 Point center(std::round(circles_w[current_circle][0]), std::round(circles_w[current_circle][1]));
		int radius = std::round(circles_w[current_circle][2]);

		 circle(orig_image, center, radius,  Scalar(0, 255, 0), 5);
	}
	White->Pers2D_input(0, 0) = cvRound(circles_w[0][0]); White->Pers2D_input(1, 0) = cvRound(circles_w[0][1]); White->Pers2D_input(2, 0) = 1;

	// Draw the circles detected
	for (size_t i = 0; i < circles_r.size(); i++)
	{
		Point center_r(cvRound(circles_r[i][0]), cvRound(circles_r[i][1]));
		int radius_r = cvRound(circles_r[i][2]);
		circle(src, center_r, 3, Scalar(0, 255, 0), -1, 8, 0);// circle center
		circle(src, center_r, radius_r, Scalar(0, 0, 255), 1, 8, 0);// circle outline
		//cout << "center : " << center_r << "\nradius : " << radius_r << endl;
	}
	for (size_t i = 0; i < circles_y.size(); i++)
	{
		Point center_y(cvRound(circles_y[i][0]), cvRound(circles_y[i][1]));
		int radius_y = cvRound(circles_y[i][2]);
		circle(src, center_y, 3, Scalar(0, 255, 0), -1, 8, 0);// circle center
		circle(src, center_y, radius_y, Scalar(0, 0, 255), 1, 8, 0);// circle outline
		//cout << "center : " << center_y << "\nradius : " << radius_y << endl;
	}
	for (size_t i = 0; i < circles_w.size(); i++)
	{
		Point center_w(cvRound(circles_w[i][0]), cvRound(circles_w[i][1]));
		int radius_w = cvRound(circles_w[i][2]);
		circle(src, center_w, 3, Scalar(0, 255, 0), -1, 8, 0);// circle center
		circle(src, center_w, radius_w, Scalar(0, 0, 255), 1, 8, 0);// circle outline
		//cout << "center : " << center_w << "\nradius : " << radius_w << endl;
	}
	 namedWindow("Detected red circles on the input image",  WINDOW_AUTOSIZE);
	 imshow("Detected red circles on the input image", orig_image);

	 waitKey(0);

}

Detect_Balls::~Detect_Balls()
{

}