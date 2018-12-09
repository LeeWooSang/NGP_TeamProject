#include "Hero.h"


/////////////////////////////////////////////////////
//////////			HERO ÄÚµå		/////////////////
/////////////////////////////////////////////////////

Hero::Hero()
{
	for (int i = 0; i < ANIMNUM; ++i)
	{
		sPMode[i] = NULL;
		sEMode[i] = NULL;
	}
	sHpGaze = NULL;

	x = 0;
	y = 0;
	mode = IDLE;
	player = 1;
	isRight = false;
}

Hero::Hero(int x1, int y1, int m)
{
	for (int i = 0; i < ANIMNUM; ++i)
	{
		sPMode[i] = NULL;
		sEMode[i] = NULL;
	}
	sHpGaze = NULL;

	x = x1;
	y = y1;
	mode = m;
	isRight = false;
}

Hero::~Hero()
{
	for (int i = 0; i < ANIMNUM; ++i)
	{
		SAFE_DELETE(sPMode[i]);
		SAFE_DELETE(sEMode[i]);
	}
	SAFE_DELETE(sHpGaze);
}

void Hero::Enter()
{
	for (int i = 0; i < ANIMNUM; ++i)
		animCount[i] = 0;

	sHpGaze = new Sprite;
	sHpGaze->Entry(0, "image/hpgaze.bmp", 0, 0);
	sHpGaze->setLocation(0, x, y);


	if (sPMode[WALK] == NULL && sEMode[WALK] == NULL)
	{
		sPMode[WALK] = new Sprite;
		sEMode[WALK] = new Sprite;

		sPMode[WALK]->Entry(0, "image/walk-1-1.bmp", 0, 0);
		sPMode[WALK]->Entry(1, "image/walk-2-1.bmp", 0, 0);
		sPMode[WALK]->Entry(2, "image/walk-3-1.bmp", 0, 0);
		sPMode[WALK]->Entry(3, "image/walk-4-1.bmp", 0, 0);

		sPMode[WALK]->setLocation(0, x, y);
		sPMode[WALK]->setLocation(1, x, y);
		sPMode[WALK]->setLocation(2, x, y);
		sPMode[WALK]->setLocation(3, x, y);

		sEMode[WALK]->Entry(0, "image/e_walk-1-1.bmp", 0, 0);
		sEMode[WALK]->Entry(1, "image/e_walk-2-1.bmp", 0, 0);
		sEMode[WALK]->Entry(2, "image/e_walk-3-1.bmp", 0, 0);
		sEMode[WALK]->Entry(3, "image/e_walk-4-1.bmp", 0, 0);
		
		sEMode[WALK]->setLocation(0, x, y);
		sEMode[WALK]->setLocation(1, x, y);
		sEMode[WALK]->setLocation(2, x, y);
		sEMode[WALK]->setLocation(3, x, y);
	}

	if (sPMode[WALK_B] == NULL && sEMode[WALK_B] == NULL)
	{
		sPMode[WALK_B] = new Sprite;
		sEMode[WALK_B] = new Sprite;
		
		sPMode[WALK_B]->Entry(0, "image/walk-1-b.bmp", 0, 0);
		sPMode[WALK_B]->Entry(1, "image/walk-2-b.bmp", 0, 0);
		sPMode[WALK_B]->Entry(2, "image/walk-3-b.bmp", 0, 0);
		sPMode[WALK_B]->Entry(3, "image/walk-4-b.bmp", 0, 0);

		sPMode[WALK_B]->setLocation(0, x, y);
		sPMode[WALK_B]->setLocation(1, x, y);
		sPMode[WALK_B]->setLocation(2, x, y);
		sPMode[WALK_B]->setLocation(3, x, y);
		
		sEMode[WALK_B]->Entry(0, "image/e_walk-1-b.bmp", 0, 0);
		sEMode[WALK_B]->Entry(1, "image/e_walk-2-b.bmp", 0, 0);
		sEMode[WALK_B]->Entry(2, "image/e_walk-3-b.bmp", 0, 0);
		sEMode[WALK_B]->Entry(3, "image/e_walk-4-b.bmp", 0, 0);
		
		sEMode[WALK_B]->setLocation(0, x, y);
		sEMode[WALK_B]->setLocation(1, x, y);
		sEMode[WALK_B]->setLocation(2, x, y);
		sEMode[WALK_B]->setLocation(3, x, y);
	}

	if (sPMode[JUMP] == NULL && sEMode[JUMP] == NULL)
	{
		sPMode[JUMP] = new Sprite;
		sEMode[JUMP] = new Sprite;

		sPMode[JUMP]->Entry(0, "image/jump-1-1.bmp", 0, 0);
		sPMode[JUMP]->setLocation(0, x, y);

		sEMode[JUMP]->Entry(0, "image/e_jump-1-1.bmp", 0, 0);
		sEMode[JUMP]->setLocation(0, x, y);
	}

	if (sPMode[JUMP_B] == NULL && sEMode[JUMP_B] == NULL)
	{
		sPMode[JUMP_B] = new Sprite;
		sEMode[JUMP_B] = new Sprite;

		sPMode[JUMP_B]->Entry(0, "image/jump-1-b.bmp", 0, 0);
		sPMode[JUMP_B]->setLocation(0, x, y);

		sEMode[JUMP_B]->Entry(0, "image/e_jump-1-b.bmp", 0, 0);
		sEMode[JUMP_B]->setLocation(0, x, y);
	}

	if (sPMode[IDLE] == NULL && sEMode[IDLE] == NULL)
	{
		sPMode[IDLE] = new Sprite;
		sEMode[IDLE] = new Sprite;

		sPMode[IDLE]->Entry(0, "image/alert-1.bmp", 0, 0);
		sPMode[IDLE]->Entry(1, "image/alert-2.bmp", 0, 0);
		sPMode[IDLE]->Entry(2, "image/alert-3.bmp", 0, 0);

		sPMode[IDLE]->setLocation(0, x, y);
		sPMode[IDLE]->setLocation(1, x, y);
		sPMode[IDLE]->setLocation(2, x, y);
		
		sEMode[IDLE]->Entry(0, "image/e_alert-1.bmp", 0, 0);
		sEMode[IDLE]->Entry(1, "image/e_alert-2.bmp", 0, 0);
		sEMode[IDLE]->Entry(2, "image/e_alert-3.bmp", 0, 0);
		
		sEMode[IDLE]->setLocation(0, x, y);
		sEMode[IDLE]->setLocation(1, x, y);
		sEMode[IDLE]->setLocation(2, x, y);
	}

	if (sPMode[IDLE_B] == NULL && sEMode[IDLE_B] == NULL)
	{
		sPMode[IDLE_B] = new Sprite;
		sEMode[IDLE_B] = new Sprite;

		sPMode[IDLE_B]->Entry(0, "image/alert-1-b.bmp", 0, 0);
		sPMode[IDLE_B]->Entry(1, "image/alert-2-b.bmp", 0, 0);
		sPMode[IDLE_B]->Entry(2, "image/alert-3-b.bmp", 0, 0);

		sPMode[IDLE_B]->setLocation(0, x, y);
		sPMode[IDLE_B]->setLocation(1, x, y);
		sPMode[IDLE_B]->setLocation(2, x, y);
	
		sEMode[IDLE_B]->Entry(0, "image/e_alert-1-b.bmp", 0, 0);
		sEMode[IDLE_B]->Entry(1, "image/e_alert-2-b.bmp", 0, 0);
		sEMode[IDLE_B]->Entry(2, "image/e_alert-3-b.bmp", 0, 0);
		
		sEMode[IDLE_B]->setLocation(0, x, y);
		sEMode[IDLE_B]->setLocation(1, x, y);
		sEMode[IDLE_B]->setLocation(2, x, y);
	}

	if (sPMode[ATTACK] == NULL && sEMode[ATTACK] == NULL)
	{
		sPMode[ATTACK] = new Sprite;
		sEMode[ATTACK] = new Sprite;
		
		sPMode[ATTACK]->Entry(0, "image/attack-1.bmp", 0, 0);
		sPMode[ATTACK]->Entry(1, "image/attack-2.bmp", 0, 0);
		sPMode[ATTACK]->Entry(2, "image/attack-3.bmp", 0, 0);

		sPMode[ATTACK]->setLocation(0, x, y);
		sPMode[ATTACK]->setLocation(1, x, y);
		sPMode[ATTACK]->setLocation(2, x, y);
		
		sEMode[ATTACK]->Entry(0, "image/e_attack-1.bmp", 0, 0);
		sEMode[ATTACK]->Entry(1, "image/e_attack-2.bmp", 0, 0);
		sEMode[ATTACK]->Entry(2, "image/e_attack-3.bmp", 0, 0);
		
		sEMode[ATTACK]->setLocation(0, x, y);
		sEMode[ATTACK]->setLocation(1, x, y);
		sEMode[ATTACK]->setLocation(2, x, y);
	}

	if (sPMode[ATTACK_B] == NULL && sEMode[ATTACK_B] == NULL)
	{
		sPMode[ATTACK_B] = new Sprite;
		sEMode[ATTACK_B] = new Sprite;

		sPMode[ATTACK_B]->Entry(0, "image/attack-1-b.bmp", 0, 0);
		sPMode[ATTACK_B]->Entry(1, "image/attack-2-b.bmp", 0, 0);
		sPMode[ATTACK_B]->Entry(2, "image/attack-3-b.bmp", 0, 0);

		sPMode[ATTACK_B]->setLocation(0, x, y);
		sPMode[ATTACK_B]->setLocation(1, x, y);
		sPMode[ATTACK_B]->setLocation(2, x, y);
		
		sEMode[ATTACK_B]->Entry(0, "image/e_attack-1-b.bmp", 0, 0);
		sEMode[ATTACK_B]->Entry(1, "image/e_attack-2-b.bmp", 0, 0);
		sEMode[ATTACK_B]->Entry(2, "image/e_attack-3-b.bmp", 0, 0);
		
		sEMode[ATTACK_B]->setLocation(0, x, y);
		sEMode[ATTACK_B]->setLocation(1, x, y);
		sEMode[ATTACK_B]->setLocation(2, x, y);
	}

	if (sPMode[DEATH] == NULL)
	{
		sPMode[DEATH] = new Sprite;
		sEMode[DEATH] = new Sprite;

		sPMode[DEATH]->Entry(0, "image/death-1.bmp", 0, 0);
		sPMode[DEATH]->Entry(1, "image/death-2.bmp", 0, 0);
		sPMode[DEATH]->Entry(2, "image/death-3.bmp", 0, 0);
		sPMode[DEATH]->Entry(3, "image/death-4.bmp", 0, 0);
		sPMode[DEATH]->Entry(4, "image/death-5.bmp", 0, 0);
		sPMode[DEATH]->Entry(5, "image/death-6.bmp", 0, 0);
		sPMode[DEATH]->Entry(6, "image/death-7.bmp", 0, 0);
		sPMode[DEATH]->Entry(7, "image/death-8.bmp", 0, 0);
		sPMode[DEATH]->Entry(8, "image/death-9.bmp", 0, 0);
		sPMode[DEATH]->Entry(9, "image/death-10.bmp", 0, 0);
		sPMode[DEATH]->Entry(10, "image/death-11.bmp", 0, 0);
		sPMode[DEATH]->setLocation(0, x, y);
		sPMode[DEATH]->setLocation(1, x, y);
		sPMode[DEATH]->setLocation(2, x, y);
		sPMode[DEATH]->setLocation(3, x, y);
		sPMode[DEATH]->setLocation(4, x, y);
		sPMode[DEATH]->setLocation(5, x, y);
		sPMode[DEATH]->setLocation(6, x, y);
		sPMode[DEATH]->setLocation(7, x, y);
		sPMode[DEATH]->setLocation(8, x, y);
		sPMode[DEATH]->setLocation(9, x, y);
		sPMode[DEATH]->setLocation(10, x, y);

		sEMode[DEATH]->Entry(0, "image/death-1.bmp", 0, 0);
		sEMode[DEATH]->Entry(1, "image/death-2.bmp", 0, 0);
		sEMode[DEATH]->Entry(2, "image/death-3.bmp", 0, 0);
		sEMode[DEATH]->Entry(3, "image/death-4.bmp", 0, 0);
		sEMode[DEATH]->Entry(4, "image/death-5.bmp", 0, 0);
		sEMode[DEATH]->Entry(5, "image/death-6.bmp", 0, 0);
		sEMode[DEATH]->Entry(6, "image/death-7.bmp", 0, 0);
		sEMode[DEATH]->Entry(7, "image/death-8.bmp", 0, 0);
		sEMode[DEATH]->Entry(8, "image/death-9.bmp", 0, 0);
		sEMode[DEATH]->Entry(9, "image/death-10.bmp", 0, 0);
		sEMode[DEATH]->Entry(10, "image/death-11.bmp", 0, 0);
		sEMode[DEATH]->setLocation(0, x, y);
		sEMode[DEATH]->setLocation(1, x, y);
		sEMode[DEATH]->setLocation(2, x, y);
		sEMode[DEATH]->setLocation(3, x, y);
		sEMode[DEATH]->setLocation(4, x, y);
		sEMode[DEATH]->setLocation(5, x, y);
		sEMode[DEATH]->setLocation(6, x, y);
		sEMode[DEATH]->setLocation(7, x, y);
		sEMode[DEATH]->setLocation(8, x, y);
		sEMode[DEATH]->setLocation(9, x, y);
		sEMode[DEATH]->setLocation(10, x, y);
	}
}

void Hero::Render(HDC* cDC, int elapsedNum)
{
	sHpGaze->setLocation(x - sHpGaze->getWidth(0) / 4, y - 40);
	sHpGaze->Render(cDC, 0, (float)hp, 100.f);
	if (player == PLAYER1)
	{
		if (animCount[mode] >= sPMode[mode]->getIndex())
			animCount[mode] = 0;
		sPMode[mode]->setLocation(x, y);
		sPMode[mode]->Render(cDC, animCount[mode], (UINT)RGB(255, 0, 255));
		if (elapsedNum % 3 == 0)
			animCount[mode]++;
	}
	else
	{
		if (animCount[mode] >= sEMode[mode]->getIndex())
			animCount[mode] = 0;
		sEMode[mode]->setLocation(x, y);
		sEMode[mode]->Render(cDC, animCount[mode], (UINT)RGB(255, 0, 255));
		if (elapsedNum % 3 == 0)
			animCount[mode]++;
	}
}

void Hero::Destroy()
{
	for (int i = 0; i < ANIMNUM; ++i)
	{
		SAFE_DELETE(sPMode[i]);
		SAFE_DELETE(sEMode[i]);
	}
	SAFE_DELETE(sHpGaze);
}

void Hero::setLocation(int x1, int y1)
{
	x = x1;
	y = y1;
}

void Hero::setMode(int m)
{
	mode = m;
}

void Hero::setHP(USHORT h)
{
	hp = h;
}

void Hero::getLocation(int *x1, int *y1)
{
	*x1 = x;
	*y1 = y;
}

int Hero::getMode()
{
	return mode;
}