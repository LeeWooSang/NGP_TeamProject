#pragma once
#include "Sprite.h"
#include "FWMain.h"

#define FIREBALL 0
#define FIREBALL_B 1
#define CRUSH 2
#define S_ANIMNUM 3

class Fireball
{
private:
	COORD pos;
	// Game에서 매 프레임마다 검사하다가 true인걸 발견하면 객체 삭제필요
	int animCount[S_ANIMNUM];
	Sprite * sFireball[S_ANIMNUM];
public:
	bool isDraw;
	bool isRight;
	bool isCrush;
public:
	Fireball();
	Fireball(int x, int y, bool isRight, byte player);
	~Fireball();

	void Render(HDC* cDC);
	void initSprite(byte player);

	void setLocation(int x, int y);
	void getLocation(int *x, int *y);
};
