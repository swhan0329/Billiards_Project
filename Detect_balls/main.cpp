#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <stdio.h>
#include <iomanip>
#include <cmath>
#include <Eigen/Dense>
#include <Windows.h> 

using namespace cv;
using namespace std;
using namespace Eigen;

#define P_NUM 6
#define IMSHOW(IMG)	imshow(#IMG,IMG)

string input_path1 = "perspective.mp4";
string input_path2 = "topview.mp4";

MatrixXd get_per_matrix()
{
	MatrixXd R = MatrixXd(2 * P_NUM, 9);
	//가변적 p,q 행렬 선언
	MatrixXd p = MatrixXd(P_NUM * 2, 1);	//P_NUM points in Billiards Coordinate
	MatrixXd q = MatrixXd(P_NUM * 2, 1); 	//P_NUM points in Pixel Coordinate
	
	ifstream BilliardFile;
	double sump = 0;
	BilliardFile.open("Billiard_coordinate.txt");
	if (!BilliardFile.eof())
	{
		for (int i = 0; i < 2 * P_NUM; i++)
		{
			BilliardFile >> p(i, 0);
		}
	}
	BilliardFile.close();

	ifstream PixelFile;
	double sumq = 0;
	PixelFile.open("pixel_coordinate.txt");
	if (!PixelFile.eof())
	{
		for (int i = 0; i < 2 * P_NUM; i++)
		{
			PixelFile >> q(i, 0);
		}
	}
	PixelFile.close();

	for (int i = 0; i < P_NUM; i++)
	{
		R(2 * i, 0) = R(2 * i + 1, 3) = p(2 * i, 0);
		R(2 * i, 1) = R(2 * i + 1, 4) = p(2 * i + 1, 0);
		R(2 * i, 2) = R(2 * i + 1, 5) = 1;
		R(2 * i, 3) = R(2 * i, 4) = R(2 * i, 5) = 0;
		R(2 * i + 1, 0) = R(2 * i + 1, 1) = R(2 * i + 1, 2) = 0;
		R(2 * i, 6) = (-1)*q(2 * i, 0)*p(2 * i, 0);
		R(2 * i, 7) = (-1)*q(2 * i, 0)*p(2 * i + 1, 0);
		R(2 * i, 8) = (-1)*q(2 * i, 0);
		R(2 * i + 1, 6) = (-1)*q(2 * i + 1, 0)*p(2 * i, 0);
		R(2 * i + 1, 7) = (-1)*q(2 * i + 1, 0)*p(2 * i + 1, 0);
		R(2 * i + 1, 8) = (-1)*q(2 * i + 1, 0);
	}
	
	/*
	//R 입력값 확인
	cout << "R " << endl;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			cout << R(i, j) << " ";
		}
		cout << endl;
	}
	cout << endl;
	*/

	MatrixXd RT = R.transpose(); //R 행렬을 tranpose 시킨 행렬

	/*
	//RT 입력값 확인
	cout << "R^T " << endl;
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			cout << RT(i, j) << " ";
		}
		cout << endl;
	}
	*/

	MatrixXd A = RT*R; //R^T와 R의 곱행렬

	/*
	//A 입력값 확인
	cout << endl;
	cout << "R^T*R " << endl;
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			cout << A(i, j) << " ";
		}
		cout << endl;
	}
	cout << endl;
	*/

	EigenSolver<MatrixXd> es(A);   //A 행렬을 es로 두고
	//cout << "The eigenvalues of A are:" << endl << es.eigenvalues() << endl << endl; //es의 고윳값을 구한다
	//cout << "The matrix of eigenvectors, V, is:" << endl << es.eigenvectors() << endl << endl; //es의 고윳값을 이용하여 고유벡터를 구한다.

	float min = es.eigenvalues().coeff(0, 0).real();
	int mincoeff = 0;
	for (int i = 0; i < 9; i++)
	{
		if (es.eigenvalues().coeff(i, 0).real() < min)
		{
			min = es.eigenvalues().coeff(i, 0).real();
			mincoeff = i;
		}
	}
	//cout << endl;
	//cout << "mincoeff" << mincoeff << endl; //mincoeff 값 확인
	double lambda = min; //고윳값 중 가작 작은(0에 가까운 값)을 lambda라 한다
	//cout << "Consider the smallest eigenvalue, lambda = " << lambda << endl; //lambda 출력

	MatrixXd m = MatrixXd(3, 3);
	//M벡터에 고유 벡터 coefficient 입력
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m(i, j) = es.eigenvectors().coeff(3 * i + j, mincoeff).real();
		}
	}

	//cout << "Here is the matrix 계산 M:" << endl << m << endl << endl;
	return m;
}

vector<Point> getMaxArea(Mat img) {
	Mat img_labels, stats, centroids;
	int numOfLables = connectedComponentsWithStats(img, img_labels, stats, centroids, 8, CV_32S); // labeling
	int max_area = 0, left = 0, top = 0, width = 0, height = 0;
	// get ltwh(left, top width, height) of maximum area
	for (int j = 1; j < numOfLables; j++) {
		int area = stats.at<int>(j, CC_STAT_AREA);
		if (area > max_area) {
			max_area = area;
			left = stats.at<int>(j, CC_STAT_LEFT);
			top = stats.at<int>(j, CC_STAT_TOP);
			width = stats.at<int>(j, CC_STAT_WIDTH);
			height = stats.at<int>(j, CC_STAT_HEIGHT);
		}
	}
	vector<Point> returnVal;
	returnVal.push_back(Point(left, top));
	returnVal.push_back(Point(left + width, top + height));
	return returnVal;
}

vector<Point> getBlueSheet(Mat in_hsv, Mat &bin) {
	// make binary image using threshold (blue).
	// we don't care it's saturation and value.
	inRange(in_hsv, Scalar(15, 94, 30), Scalar(179, 255, 255), bin);

	// erode and dilate operation
	erode(bin, bin, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));
	dilate(bin, bin, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));
	dilate(bin, bin, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));
	erode(bin, bin, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));

	for (int i = 0; i < 10; i++)
		dilate(bin, bin, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));

	for (int i = 0; i < 10; i++)
		erode(bin, bin, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));


	return getMaxArea(bin);
}

vector<Vec3f> getCircles(Mat in_img, Mat blueSheet, vector<Point> cBox) {
	Mat gray, fgray;
	cvtColor(in_img, gray, COLOR_BGR2GRAY);
	gray = gray&blueSheet;
	Rect RoI(cBox[0], cBox[1]);
	gray = gray(RoI);

	bilateralFilter(gray, fgray, 10, 80, 80);

	vector<Vec3f> circles;
	HoughCircles(fgray, circles, HOUGH_GRADIENT, 1,
		15,  // 30, change this value to detect circles with different distances to each other
		100, 15, 10, 16 // change the last two parameters 100, 30, 1, 80
						// (min_radius & max_radius) to detect larger circles
	);
	for (int j = 0; j < circles.size(); j++) {
		circles[j][0] += cBox[0].x;
		circles[j][1] += cBox[0].y;
	}

	return circles;
}

//hsvRange - [min Hue, min Sat, min Val, max Hue, max Sat, max Val]
Vector3d getCircle(Mat in_hsv, int* hsvRange, vector<Point> cBox, Mat blueSheet, string wname = "ball") {

	Mat gray, new_hsv, gray2, fgray;
	Rect RoI(cBox[0], cBox[1]);
	cvtColor(blueSheet, blueSheet, COLOR_GRAY2BGR);
	in_hsv = in_hsv&blueSheet;
	new_hsv = in_hsv(RoI);

	// make binary image using threshold.
	inRange(new_hsv, Scalar(hsvRange[0], hsvRange[1], hsvRange[2]), Scalar(hsvRange[3], hsvRange[4], hsvRange[5]), gray);
	// when you use 2 range.
	if (sizeof(hsvRange) / sizeof(*hsvRange) > 6) {
		inRange(new_hsv, Scalar(hsvRange[6], hsvRange[7], hsvRange[8]), Scalar(hsvRange[9], hsvRange[10], hsvRange[11]), gray2);
		gray = gray | gray2;
	}

	bilateralFilter(gray, fgray, 10, 80, 80);
	//medianBlur(gray, gray, 5);

	// dilate operation 3 times
	for (int j = 0; j < 3; j++)
		dilate(fgray, fgray, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	cvtColor(new_hsv, gray2, COLOR_HSV2BGR);
	cvtColor(gray2, gray2, COLOR_BGR2GRAY);
	fgray = fgray&gray2;

	//namedWindow(wname, 1);
	//imshow(wname, fgray);

	vector<Vec3f> circles;
	HoughCircles(fgray, circles, HOUGH_GRADIENT, 1,
		15,  // change this value to detect circles with different distances to each other
		100, 15, 10, 16 // change the last two parameters 100, 30, 1, 80
						// (min_radius & max_radius) to detect larger circles
	);

	// check is the circle is included in constrain box
	Vector3d returnVal(0, 0, 0);
	for (size_t i = 0; i < circles.size(); i++)
	{
		Vec3d c = circles[i];
		if (c[0] < (cBox[1].x - cBox[0].x) && c[1] < (cBox[1].y - cBox[0].y) && returnVal[2] < c[2]) {
			returnVal[0] = c[0] + cBox[0].x;
			returnVal[1] = c[1] + cBox[0].y;
			returnVal[2] = c[2];
		}
	}
	return returnVal;
}

void drawCircle(Mat frame, Vector3d c) {
	Point center = Point(c[0], c[1]);
	int radius = c[2];
	circle(frame, center, 1, Scalar(0, 100, 100), 2, LINE_AA);
	circle(frame, center, radius, Scalar(0, 100, 255), 2, LINE_AA);
}

void drawCircle(Mat frame, Vector3f c) {
	Point center = Point(c[0], c[1]);
	int radius = c[2];
	circle(frame, center, 1, Scalar(0, 100, 100), 2, LINE_AA);
	circle(frame, center, radius, Scalar(0, 100, 255), 2, LINE_AA); 
}

Mat ImageRotateInner(const Mat src, double degree, Point2d base) {
	Mat dst = src.clone();
	Mat rot = getRotationMatrix2D(base, degree, 1.0);
	warpAffine(src, dst, rot, src.size());
	return std::move(dst);
}

int main() 
{
	//for recording process time
	LARGE_INTEGER Frequency;
	LARGE_INTEGER BeginTime;
	LARGE_INTEGER Endtime;
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&BeginTime);
	
	ofstream result("balls'_coordinate.txt");
	ofstream top("Top_view_coordinate.txt");

	VideoCapture cap1(input_path1); // open input file
	if (!cap1.isOpened()) // check if we succeeded
		return -1;

	VideoCapture cap2(input_path2); // open input file
	if (!cap2.isOpened()) // check if we succeeded
		return -1;

	vector<Point> rect;
	vector<Vec3f> circleVec;
	Mat blueSheet;

	// this parameter is used for seowoo's method.
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// hsvRange - [min Hue, min Sat, min Val, max Hue, max Sat, max Val],[min Hue, min Sat, min Val, max Hue, max Sat, max Val]
	int rRange[6] = { 133,38,9,179,255,255 };//{ 130,40,0,179,255,255 };//{ 112, 0, 0, 179, 255, 255, }; // { 155, 200, 0, 180, 255, 150 };
	int yRange[6] = { 0,21,80,26,194,255 };//{ 0,40,100,102,255,255 };//{ 0, 150, 93, 104, 255, 255, };// { 0, 0, 0, 30, 255, 255 };
	int wRange[6] = { 0,0,61,134,66,255 };//{ 0,0,75,152,110,255 };//{ 27, 9, 21, 103, 59, 255, }; // { 30, 0, 100, 120, 100, 255 };
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//결과 output.avi로 저장하기 위한 코드
	VideoWriter outputVideo;
	Size size = Size((int)cap1.get(CAP_PROP_FRAME_WIDTH), (int)cap1.get(CAP_PROP_FRAME_HEIGHT));
	cap1.set(CAP_PROP_FPS, 29.97);
	outputVideo.open("ouput.avi", VideoWriter::fourcc('X', 'V', 'I', 'D'), 29.97, size, true);
	if (!outputVideo.isOpened())
	{
		cout << "동영상을 저장하기 위한 초기화 작업 중 에러 발생" << endl;
	}
	
	for (int i = 0;; i++) {
		Mat frame1, frame2, hsv;
		cap1 >> frame1; // get a new frame from video
		cap2 >> frame2;
		cvtColor(frame1, hsv, COLOR_BGR2HSV); // change color space

		if (i == 0)
			rect = getBlueSheet(hsv, blueSheet);
		else
			getBlueSheet(hsv, blueSheet);


		rectangle(frame1, rect[0], rect[1], Scalar(0, 0, 255),3);

		// just get circles inside of RoI
		circleVec = getCircles(frame1, blueSheet, rect);


		// for seowoo's method.
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// get red ball and draw circle
		result << "frame num : " << i + 1 << endl;
		Vector3d rBall = getCircle(hsv, rRange, rect, blueSheet, "rBall");
		drawCircle(frame1, rBall);
		result << "Red Ball" << "( " << rBall[0] << ", " << rBall[1] << " )  /  radius : " <<rBall[2] << endl;
		// get yellow ball and draw circle
		Vector3d yBall = getCircle(hsv, yRange, rect, blueSheet, "yBall");
		drawCircle(frame1, yBall);
		result << "Yellow Ball" << "( " << yBall[0] << ", " << yBall[1] << " ) /  radius : " << yBall[2] << endl;
		// get white ball and draw circle
		Vector3d wBall = getCircle(hsv, wRange, rect, blueSheet, "wBall");
		drawCircle(frame1, wBall);
		result << "White Ball" << "( " << wBall[0] << ", " << wBall[1] << " )  /  radius : " << wBall[2] << endl;
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
		//get perspective matrix
		MatrixXd per_matrix = get_per_matrix();
		MatrixXd per_matrix_inverse = per_matrix.inverse();
		rBall[2] = yBall[2] = wBall[2] = 1;
		MatrixXd rb = per_matrix_inverse*rBall; MatrixXd yb = per_matrix_inverse*yBall; MatrixXd wb = per_matrix_inverse*wBall;
		rb(0, 0) = rb(0, 0) / rb(2, 0); rb(1, 0) = rb(1, 0) / rb(2, 0);
		yb(0, 0) = yb(0, 0) / yb(2, 0); yb(1, 0) = yb(1, 0) / yb(2, 0);
		wb(0, 0) = wb(0, 0) / wb(2, 0); wb(1, 0) = wb(1, 0) / wb(2, 0);
		
		top << "frame num : " << i + 1 << endl;
		top << "Red Ball" << "( " << rb(0,0) << ", " << rb(1, 0) << " )  "<< endl;
		top << "Yellow Ball" << "( " << yb(0, 0)  << ", " << yb(1, 0)  << " ) " << endl;
		top << "White Ball" << "( " << wb(0, 0) << ", " << wb(1, 0) << " )  "  << endl;
		
		Vector3d rball, yball, wball;
		for (int i = 0; i < 3; i++)
		{
			rball[i] = rb(i, 0);
			yball[i] = yb(i, 0);
			wball[i] = wb(i, 0);
		}
		rball[2] = yball[2] = wball[2] = 7;

		/*/////////////////////////////////////////////////////////////
		당구대 좌표 -> pixel 값으로 변환하는 부분이 필요
		pixel값으로 바꾼 뒤 전체 영상에서 당구대만 따로 딸 필요가 있음.
		/////////////////////////////////////////////////////////////*/
		drawCircle(frame2, rball);
		drawCircle(frame2, yball);
		drawCircle(frame2, wball);
		cap2.set(CAP_PROP_FRAME_WIDTH, 1280); 	cap2.set(CAP_PROP_FRAME_HEIGHT, 720);

		/////////////////////////////////영상 회전 출력/////////////////////////////////////////
		double angle = 180;
		Point2d mid(frame2.cols / 2.0, frame2.rows / 2.0);
		Mat inner = ImageRotateInner(frame2, angle, mid);
		//Mat outer = ImageRotateOuter(img, angle, &adding);
		//IMSHOW(inner);
		//////////////////////////////////////////////////////////////////////////////////////
		
		//동영상 파일에 한프레임을 저장함.  
		outputVideo << frame1;

		QueryPerformanceCounter(&Endtime);
		int64 elapsed = Endtime.QuadPart - BeginTime.QuadPart;
		double duringtime = (double)elapsed / (double)Frequency.QuadPart;
		//cout << duringtime <<endl;
		if (waitKey(30) >= 0) break;
	}

	return 0;
}