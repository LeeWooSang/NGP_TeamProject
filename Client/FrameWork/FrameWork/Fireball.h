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
	// Game���� �� �����Ӹ��� �˻��ϴٰ� true�ΰ� �߰��ϸ� ��ü �����ʿ�
	int animCount[S_ANIMNUM];
	Sprite * sPFireball[S_ANIMNUM];
	Sprite * sEFireball[S_ANIMNUM];
public:
	bool isDraw;
	bool isRight;
	bool isCrush;
	int player;
public:
	Fireball();
	Fireball(int x, int y, bool isRight);
	~Fireball();

	void Render(HDC* cDC);
	void initSprite();

	void setLocation(int x, int y);
	void getLocation(int *x, int *y);
};
