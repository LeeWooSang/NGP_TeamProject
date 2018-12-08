#include "Collision.h"
#define PIE 3.14

bool Collision::DotSquare(int mx, int my, int len, int x, int y)
{
	return mx > x && mx < x + len && my> y && my < y + len;
}
bool Collision::DotRectangle(int mx, int my, int w, int h, int x, int y)
{
	return mx > x && mx < x + w && my> y && my < y + h;
}
bool Collision::DotCircle(int mx, int my, int rad, int x, int y)
{
	return (x - mx)*(x - mx) + (y - my)*(y - my) <= rad*rad;
}
bool Collision::RecRectangle(int R1RightTopX, int R1RightTopY, int R1LeftTopX, int R1LeftTopY, int R1LeftBottomX, int R1LeftBottomY, int R1RightBottomX, int R1RightBottomY,
	int R2RightTopX, int R2RightTopY, int R2LeftTopX, int R2LeftTopY, int R2LeftBottomX, int R2LeftBottomY, int R2RightBottomX, int R2RightBottomY)
{
	if ((R2RightTopX<R1RightTopX&&R2RightTopX>R1LeftTopX&&R2RightTopY<R1RightTopY&&R2RightTopY>R1RightBottomY)
		|| (R1RightTopX<R2RightTopX&&R1RightTopX>R2LeftTopX&&R1RightTopY<R2RightTopY&&R1RightTopY>R2RightBottomY))
		return true;
	else if ((R2LeftTopX<R1RightTopX&&R2LeftTopX>R1LeftTopX&&R2LeftTopY<R1RightTopY&&R2LeftTopY>R1RightBottomY)
		|| (R1LeftTopX<R2RightTopX&&R1LeftTopX>R2LeftTopX&&R1LeftTopY<R2RightTopY&&R1LeftTopY>R2RightBottomY))
		return true;
	else if ((R2LeftBottomX<R1RightTopX&&R2LeftBottomX>R1LeftTopX&&R2LeftBottomY<R1RightTopY&&R2LeftBottomY>R1RightBottomY)
		|| (R1LeftBottomX<R2RightTopX&&R1LeftBottomX>R2LeftTopX&&R1LeftBottomY<R2RightTopY&&R1LeftBottomY>R2RightBottomY))
		return true;
	else if ((R2RightBottomX<R1RightTopX&&R2RightBottomX>R1LeftTopX&&R2RightBottomY<R1RightTopY&&R2RightBottomY>R1RightBottomY)
		|| (R1RightBottomX<R2RightTopX&&R1RightBottomX>R2LeftTopX&&R1RightBottomY<R2RightTopY&&R1RightBottomY>R2RightBottomY))
		return true;
	else
		return false;
}
bool Collision::CirCircle(int C1CenterX, int C1CenterY, int C1rad, int C2CenterX, int C2CenterY, int C2rad)
{
	return (sqrt((C1CenterX - C2CenterX)*(C1CenterX - C2CenterX) + (C1CenterY - C2CenterY)*(C1CenterY - C2CenterY)) < C1rad + C2rad);
}

bool Collision::RRCollision(float minX, float minY, float maxX, float maxY, float minX1, float minY1, float maxX1, float maxY1)
{
	if (minX > maxX1)
		return false;
	if (maxX < minX1)
		return false;
	if (minY > maxY1)
		return false;
	if (maxY < minY1)
		return false;

	return true;
}