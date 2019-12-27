#include "Coordinate.h"

Coordinate::Coordinate()
{

}

void Coordinate::Top2Pers(MatrixXd Top2D_input, MatrixXd Pers2D_output, MatrixXd m)
{
	Pers2D_output = m*Top2D_input;
}

void Coordinate::Pers2Top(MatrixXd Top2D_output, MatrixXd Pers2D_input, MatrixXd m)
{
	MatrixXd m_inverse = m.inverse();
	Top2D_output = m_inverse*Pers2D_input;
}

Coordinate::~Coordinate()
{

}