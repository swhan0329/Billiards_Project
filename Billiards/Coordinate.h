#pragma once
#include <Eigen/Dense>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <memory.h>
#include "Variable.h"

using namespace Eigen;

class Coordinate {
public:
	
	MatrixXd Top2D = MatrixXd(P_NUM * 3, 1);	//P_NUM points in Billiards Coordinate
	MatrixXd Pers2D	= MatrixXd(P_NUM * 3, 1); 	//P_NUM points in Pixel Coordinate
	MatrixXd Top2D_input = MatrixXd(3, 1);
	MatrixXd Top2D_output = MatrixXd(3, 1);
	MatrixXd Pers2D_input = MatrixXd(3, 1);
	MatrixXd Pers2D_output = MatrixXd(3, 1);

	//m Matrix
	MatrixXd m = MatrixXd(3, 3);
					
	Coordinate();
	~Coordinate();
	void Top2Pers(MatrixXd Top2D_input, MatrixXd Pers2D_ouput, MatrixXd m);
	void Pers2Top(MatrixXd Top2D_output, MatrixXd Pers2D_input, MatrixXd m);

};


