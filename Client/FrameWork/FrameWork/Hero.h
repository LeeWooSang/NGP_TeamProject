#pragma once
#include "Sprite.h"
#include "FWMain.h"

#define ANIMNUM 11

#define WALK 0
#define WALK_B 1
#define JUMP 2
#define JUMP_B 3
#define IDLE 4
#define IDLE_B 5
#define ATTACK 6
#define ATTACK_B 7
#define FIREBALL 8
#define FIREBALL_B 9
#define DEATH 10
#define CRUSH 11

class Hero
{
private:
	int x;
	int y;
	int mode;
#pragma region Sprite변수선언
	Sprite * sMode[10];
	//Sprite * sWalk;
	//Sprite * sWalk_b;
	//Sprite * sJump;
	//Sprite * sJump_b;
	//Sprite * sIdle;
	//Sprite * sIdle_b;
	//Sprite * sAttack;
	//Sprite * sAttack_b;
	//Sprite * sDeath;

	//Sprite * sFireball;
	//Sprite * sFireball_b;
	//Sprite * sCrushEffect;
#pragma endregion
	int animCount[ANIMNUM];
	byte player;

public:
	Hero();
	Hero(int x, int y, int mode, int player);
	~Hero();

	void Render(HDC* cDC);
	void Enter();
	void Destroy();
	void Update();

	void setLocation(int x, int y);

	int getX();
	int getY();
};

