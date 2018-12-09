#pragma once
#include "Defines.h"

class Collision 
{
public:
	static bool DotSquare(int LeftTopX,int LeftTopY, int len, int x, int y);
	static bool DotRectangle(int LeftTopX, int LeftTopY, int Width, int Height, int x, int y);
	static bool DotCircle(int CenterX, int CenterY, int rad, int x, int y);

	static bool RecRectangle(int R1RightTopX, int R1RightTopY, int R1LeftTopX, int R1LeftTopY, int R1LeftBottomX, int R1LeftBottomY, int R1RightBottomX, int R1RightBottomY,
		int R2RightTopX, int R2RightTopY, int R2LeftTopX, int R2LeftTopY, int R2LeftBottomX, int R2LeftBottomY, int R2RightBottomX, int R2RightBottomY);
	static bool CirCircle(int C1CenterX, int C1CenterY, int C1rad, int C2CenterX, int C2CenterY, int C2rad);
	static bool RRCollision(float minX, float minY, float maxX, float maxY, float minX1, float minY1, float maxX1, float maxY1);
};