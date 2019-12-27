#pragma once
#include "Ball.h"

class Ball;
class Detect_Balls : public Ball
{
public:
	Detect_Balls();
	void Find_PC_Balls(Ball* Red, Ball* Yellow, Ball* White);
	~Detect_Balls();
};
