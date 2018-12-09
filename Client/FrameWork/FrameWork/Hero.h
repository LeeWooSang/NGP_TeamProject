#pragma once
#include "Sprite.h"
#include "FWMain.h"

#define ANIMNUM 9

#define WALK 0
#define WALK_B 1
#define JUMP 2
#define JUMP_B 3
#define IDLE 4
#define IDLE_B 5
#define ATTACK 6
#define ATTACK_B 7
#define DEATH 8

class Hero
{
private:
	int x;
	int y;
	int mode;
	USHORT hp;
	int animCount[ANIMNUM];
	Sprite * sPMode[ANIMNUM];
	Sprite * sEMode[ANIMNUM];
	Sprite * sHpGaze;
public:
	byte player;
	bool isRight;

public:
	Hero();
	Hero(int x, int y, int mode);
	~Hero();

	void Render(HDC* cDC, int elapsedNum);
	void Enter();
	void Destroy();

	void setLocation(int x, int y);
	void setHP(USHORT h);
	void setMode(int mode);

	void getLocation(int *x, int *y);

	int getMode();
};

