#pragma once
#include "Sprite.h"
#include "FWMain.h"

#define ANIMNUM 9
#define S_ANIMNUM 3

#define WALK 0
#define WALK_B 1
#define JUMP 2
#define JUMP_B 3
#define IDLE 4
#define IDLE_B 5
#define ATTACK 6
#define ATTACK_B 7
#define DEATH 8

#define FIREBALL 0
#define FIREBALL_B 1
#define CRUSH 2

class Fireball
{
private:
	COORD pos;
	bool isRight;
	bool isCrush;
	// Game에서 매 프레임마다 검사하다가 true인걸 발견하면 객체 삭제필요
	int animCount[S_ANIMNUM];
	Sprite * sFireball[S_ANIMNUM];
public:
	bool willDelete;

public:
	Fireball(int x, int y, bool isRight, byte player);
	~Fireball();

	void Render(HDC* cDC);
	void Update();
	void initSprite(byte player);

	void setLocation(int x, int y);
	void getLocation(int *x, int *y);
};

class Hero
{
private:
	int x;
	int y;
	int mode;
	USHORT hp;
	int animCount[ANIMNUM];
	Sprite * sMode[ANIMNUM];
public:
	byte player;
	bool isBack;

public:
	Hero();
	Hero(int x, int y, int mode, int player);
	~Hero();

	void Render(HDC* cDC);
	void Enter(int p);
	void Destroy();
	void Update();

	void setLocation(int x, int y);
	void setHP(USHORT h);
	void setMode(int mode);

	void getLocation(int *x, int *y);

	int getMode();
};

