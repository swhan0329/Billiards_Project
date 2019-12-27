#pragma once
#include <Eigen/Dense>
#include "Variable.h"
#include "Coordinate.h"


using namespace Eigen;

class Coordinate;
class Ball : public Coordinate
{
public:

	float diameter = ball_diameter;	
	int Color;


	Ball();
	~Ball();
};

