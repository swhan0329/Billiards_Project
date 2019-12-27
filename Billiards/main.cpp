#include <iostream>
#include <Eigen/Dense>
#include <fstream>
#include <stdio.h>
#include <iomanip>
#include <cmath>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "Ball.h"
#include "Coordinate.h"
#include "Variable.h"
#include "Detect_Balls.h"

using namespace Eigen;
using namespace std;
using namespace cv;

void main()
{

	//가변적 p,q 행렬 선언
	MatrixXd R = MatrixXd(2 * P_NUM, 9);

	Coordinate Billiards;
	
	ifstream BilliardFile;
	double sump = 0;
	BilliardFile.open("Billiard_coordinate.txt");
	if (!BilliardFile.eof())
	{
		for (int i = 0; i < 2 * P_NUM; i++)
		{
			BilliardFile >> Billiards.Top2D(i, 0);
		}
	}
	BilliardFile.close();

	/*
	//p 입력값 확인
	cout << "p " << endl;
	for (int i = 0; i < P_NUM; i++)
	{
		cout << Billards.Top2D(i, 0) << " ";
	}
	cout << endl;
	*/

	ifstream PixelFile;
	double sumq = 0;
	PixelFile.open("pixel_coordinate.txt");
	if (!PixelFile.eof())
	{
		for (int i = 0; i < 2 * P_NUM; i++)
		{
			PixelFile >> Billiards.Pers2D(i, 0);
		}
	}
	PixelFile.close();

	/*
	//q 입력값 확인
	cout << "q " << endl;
	for (int i = 0; i < P_NUM; i++)
	{
		cout << Billards.Pers2D(i, 0) << " ";
	}
	cout << endl;
	*/

	//R 행렬에 데이터 집어 넣기
	for (int i = 0; i < P_NUM; i++)
	{
		R(2 * i, 0) = R(2 * i + 1, 3) = Billiards.Top2D(2 * i, 0);
		R(2 * i, 1) = R(2 * i + 1, 4) = Billiards.Top2D(2 * i + 1, 0);
		R(2 * i, 2) = R(2 * i + 1, 5) = 1;
		R(2 * i, 3) = R(2 * i, 4) = R(2 * i, 5) = 0;
		R(2 * i + 1, 0) = R(2 * i + 1, 1) = R(2 * i + 1, 2) = 0;
		R(2 * i, 6) = (-1)*Billiards.Pers2D(2*i, 0)*Billiards.Top2D(2 * i, 0);
		R(2 * i, 7) = (-1)*Billiards.Pers2D(2 * i, 0)*Billiards.Top2D(2 * i + 1, 0);
		R(2 * i, 8) = (-1)*Billiards.Pers2D(2 * i, 0);
		R(2 * i + 1, 6) = (-1)*Billiards.Pers2D(2 * i + 1, 0)*Billiards.Top2D(2 * i, 0);
		R(2 * i + 1, 7) = (-1)*Billiards.Pers2D(2 * i + 1, 0)*Billiards.Top2D(2 * i + 1, 0);
		R(2 * i + 1, 8) = (-1)*Billiards.Pers2D(2 * i + 1, 0);
	}
	
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

		MatrixXd RT = R.transpose(); //R 행렬을 tranpose 시킨 행렬
	
		
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
	

		MatrixXd A = RT*R; //R^T와 R의 곱행렬
	

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
	

		EigenSolver<MatrixXd> es(A);   //A 행렬을 es로 두고
		cout << "The eigenvalues of A are:" << endl << es.eigenvalues() << endl << endl; //es의 고윳값을 구한다
		cout << "The matrix of eigenvectors, V, is:" << endl << es.eigenvectors() << endl << endl; //es의 고윳값을 이용하여 고유벡터를 구한다.

		float min = es.eigenvalues().coeff(0, 0).real();
		int mincoeff=0;
		for (int i = 0; i < 9; i++)
		{
			if (es.eigenvalues().coeff(i, 0).real() < min)
			{
				min = es.eigenvalues().coeff(i, 0).real();
				mincoeff = i;
			}
		}
		cout << endl;
		cout << "mincoeff" << mincoeff << endl; //mincoeff 값 확인
		double lambda = min; //고윳값 중 가작 작은(0에 가까운 값)을 lambda라 한다
		cout << "Consider the smallest eigenvalue, lambda = " << lambda << endl; //lambda 출력

		//M벡터에 고유 벡터 coefficient 입력
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				Billiards.m(i, j) = es.eigenvectors().coeff(3 * i + j, mincoeff).real();
			}
		}

		cout << "Here is the matrix 계산 M:" << endl << Billiards.m << endl << endl;
		Ball* Red = new Ball; 	Ball* Yellow = new Ball; 	Ball* White = new Ball;
		Detect_Balls Detect;
		Detect.Find_PC_Balls(Red, Yellow, White);
		Red->Pers2Top(Red->Top2D_output, Red->Pers2D_input, Billiards.m);
		Yellow->Pers2Top(Yellow->Top2D_output, Yellow->Pers2D_input, Billiards.m);
		White->Pers2Top(White->Top2D_output, White->Pers2D_input, Billiards.m);
		cout << "빨간공 중심: " << Red->Top2D_output(0, 0) / Red->Top2D_output(2, 0) << ",  " << Red->Top2D_output(1, 0) / Red->Top2D_output(2, 0) <<endl;
		cout << "노란공 중심: " << Yellow->Top2D_output(0, 0) / Yellow->Top2D_output(2, 0) << ",  " << Yellow->Top2D_output(1, 0) / Yellow->Top2D_output(2, 0) << endl;
		cout << "흰공 중심: " << White->Top2D_output(0, 0) / White->Top2D_output(2, 0) << ",  " << White->Top2D_output(1, 0) / White->Top2D_output(2, 0) << endl;
		
		/*
		MatrixXd RB_BR = MatrixXd(3, 1);
		MatrixXd RB_BY = MatrixXd(3, 1);
		MatrixXd RB_BW = MatrixXd(3, 1);
		RB_BR(0, 0) = 1.7775; RB_BR(1, 0) = 0.711; RB_BR(2, 0) = 1;
		RB_BY(0, 0) = 0.711; RB_BY(1, 0) = 0.711; RB_BY(2, 0) = 1;
		RB_BW(0, 0) = 0.711; RB_BW(1, 0) = 0.53325; RB_BW(2, 0) = 1;
		MatrixXd Result_BR = Billards.m*RB_BR;
		MatrixXd Result_BY = Billards.m*RB_BY;
		MatrixXd Result_BW = Billards.m*RB_BW;
		cout << Result_BR(0, 0) / Result_BR(2, 0) << ",  " << Result_BR(1, 0) / Result_BR(2, 0) << endl;
		cout << Result_BY(0, 0) / Result_BY(2, 0) << ",  " << Result_BY(1, 0) / Result_BY(2, 0) << endl;
		cout << Result_BW(0, 0) / Result_BW(2, 0) << ",  " << Result_BW(1, 0) / Result_BW(2, 0) << endl;
		*/

		system("pause");

}
