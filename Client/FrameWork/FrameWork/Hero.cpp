#include "Hero.h"

Hero::Hero()
{
	for (int i = 0; i < ANIMNUM; ++i)
		sMode[i] = NULL;
	/*sWalk = sWalk_b = sJump = sJump_b = sIdle = sIdle_b = sAttack = sAttack_b = NULL;
	sFireball = sFireball_b = NULL;
	sDeath = sCrushEffect = NULL;*/

	x = 0;
	y = 0;
	mode = IDLE;
	player = 1;
}

Hero::Hero(int x1, int y1, int m, int p)
{
	for (int i = 0; i < ANIMNUM; ++i)
		sMode[i] = NULL;
	/*sWalk = sWalk_b = sJump = sJump_b = sIdle = sIdle_b = sAttack = sAttack_b = NULL;
	sFireball = sFireball_b = NULL;
	sDeath = sCrushEffect = NULL;*/

	x = x1;
	y = y1;
	mode = m;
	player = p;
}

Hero::~Hero()
{
#pragma region SpriteDelete
	for(int i=0;i<ANIMNUM;++i)
		SAFE_DELETE_ARRAY(sMode[i]);
	/*SAFE_DELETE(sWalk);
	SAFE_DELETE(sWalk_b);
	SAFE_DELETE(sJump);
	SAFE_DELETE(sJump_b);
	SAFE_DELETE(sIdle);
	SAFE_DELETE(sIdle_b);
	SAFE_DELETE(sAttack);
	SAFE_DELETE(sAttack_b);
	SAFE_DELETE(sDeath);
	SAFE_DELETE(sFireball);
	SAFE_DELETE(sFireball_b);
	SAFE_DELETE(sCrushEffect);
	*/
#pragma endregion
}

void Hero::Enter()
{
	for (int i = 0; i < ANIMNUM; ++i)
		animCount[i] = 0;
	
	if (sMode[WALK] == NULL)
	{
		sMode[WALK] = new Sprite;

		if (player == 1)
		{
			sMode[WALK]->Entry(0, "image/walk-1-1.bmp", 0, 0);
			sMode[WALK]->Entry(1, "image/walk-2-1.bmp", 0, 0);
			sMode[WALK]->Entry(2, "image/walk-3-1.bmp", 0, 0);
			sMode[WALK]->Entry(3, "image/walk-4-1.bmp", 0, 0);
		}
		else
		{
			sMode[WALK]->Entry(0, "image/e_walk-1-1.bmp", 0, 0);
			sMode[WALK]->Entry(1, "image/e_walk-2-1.bmp", 0, 0);
			sMode[WALK]->Entry(2, "image/e_walk-3-1.bmp", 0, 0);
			sMode[WALK]->Entry(3, "image/e_walk-4-1.bmp", 0, 0);
		}
		sMode[WALK]->setLocation(0, x, y);
		sMode[WALK]->setLocation(1, x, y);
		sMode[WALK]->setLocation(2, x, y);
		sMode[WALK]->setLocation(3, x, y);
	}

	if (sMode[WALK_B] == NULL)
	{
		sMode[WALK_B] = new Sprite;
		if (player == 1)
		{
			sMode[WALK_B]->Entry(0, "image/walk-1-b.bmp", 0, 0);
			sMode[WALK_B]->Entry(1, "image/walk-2-b.bmp", 0, 0);
			sMode[WALK_B]->Entry(2, "image/walk-3-b.bmp", 0, 0);
			sMode[WALK_B]->Entry(3, "image/walk-4-b.bmp", 0, 0);
		}
		else
		{
			sMode[WALK_B]->Entry(0, "image/e_walk-1-b.bmp", 0, 0);
			sMode[WALK_B]->Entry(1, "image/e_walk-2-b.bmp", 0, 0);
			sMode[WALK_B]->Entry(2, "image/e_walk-3-b.bmp", 0, 0);
			sMode[WALK_B]->Entry(3, "image/e_walk-4-b.bmp", 0, 0);
		}
		sMode[WALK_B]->setLocation(0, x, y);
		sMode[WALK_B]->setLocation(1, x, y);
		sMode[WALK_B]->setLocation(2, x, y);
		sMode[WALK_B]->setLocation(3, x, y);
	}

	if (sMode[JUMP] == NULL)
	{
		sMode[JUMP] = new Sprite;

		if (player == 1)
			sMode[JUMP]->Entry(0, "image/jump-1-1.bmp", 0, 0);
		else
			sMode[JUMP]->Entry(0, "image/e_jump-1-1.bmp", 0, 0);

		sMode[JUMP]->setLocation(0, x, y);
	}

	if (sMode[JUMP_B] == NULL)
	{
		sMode[JUMP_B] = new Sprite;

		if (player == 1)
			sMode[JUMP_B]->Entry(0, "image/jump-1-b.bmp", 0, 0);
		else
			sMode[JUMP_B]->Entry(0, "image/e_jump-1-b.bmp", 0, 0);

		sMode[JUMP_B]->setLocation(0, x, y);
	}

	if (sMode[IDLE] == NULL)
	{
		sMode[IDLE] = new Sprite;

		if (player == 1)
		{
			sMode[IDLE]->Entry(0, "image/alert-1.bmp", 0, 0);
			sMode[IDLE]->Entry(1, "image/alert-2.bmp", 0, 0);
			sMode[IDLE]->Entry(2, "image/alert-3.bmp", 0, 0);
		}
		else
		{
			sMode[IDLE]->Entry(0, "image/e_alert-1.bmp", 0, 0);
			sMode[IDLE]->Entry(1, "image/e_alert-2.bmp", 0, 0);
			sMode[IDLE]->Entry(2, "image/e_alert-3.bmp", 0, 0);
		}
		sMode[IDLE]->setLocation(0, x, y);
		sMode[IDLE]->setLocation(1, x, y);
		sMode[IDLE]->setLocation(2, x, y);
	}

	if (sMode[IDLE_B] == NULL)
	{
		sMode[IDLE_B] = new Sprite;

		if (player == 1)
		{
			sMode[IDLE_B]->Entry(0, "image/alert-1-b.bmp", 0, 0);
			sMode[IDLE_B]->Entry(1, "image/alert-2-b.bmp", 0, 0);
			sMode[IDLE_B]->Entry(2, "image/alert-3-b.bmp", 0, 0);
		}
		else
		{
			sMode[IDLE_B]->Entry(0, "image/e_alert-1-b.bmp", 0, 0);
			sMode[IDLE_B]->Entry(1, "image/e_alert-2-b.bmp", 0, 0);
			sMode[IDLE_B]->Entry(2, "image/e_alert-3-b.bmp", 0, 0);
		}
		sMode[IDLE_B]->setLocation(0, x, y);
		sMode[IDLE_B]->setLocation(1, x, y);
		sMode[IDLE_B]->setLocation(2, x, y);
	}

	if (sMode[ATTACK] == NULL)
	{
		sMode[ATTACK] = new Sprite;
		
		if (player == 1)
		{
			sMode[ATTACK]->Entry(0, "image/attack-1.bmp", 0, 0);
			sMode[ATTACK]->Entry(1, "image/attack-2.bmp", 0, 0);
			sMode[ATTACK]->Entry(2, "image/attack-3.bmp", 0, 0);
		}
		else
		{
			sMode[ATTACK]->Entry(0, "image/e_attack-1.bmp", 0, 0);
			sMode[ATTACK]->Entry(1, "image/e_attack-2.bmp", 0, 0);
			sMode[ATTACK]->Entry(2, "image/e_attack-3.bmp", 0, 0);
		}
		sMode[ATTACK]->setLocation(0, x, y);
		sMode[ATTACK]->setLocation(1, x, y);
		sMode[ATTACK]->setLocation(2, x, y);
	}

	if (sMode[ATTACK_B] == NULL)
	{
		sMode[ATTACK_B] = new Sprite;
		if (player == 1)
		{
			sMode[ATTACK_B]->Entry(0, "image/attack-1-b.bmp", 0, 0);
			sMode[ATTACK_B]->Entry(1, "image/attack-2-b.bmp", 0, 0);
			sMode[ATTACK_B]->Entry(2, "image/attack-3-b.bmp", 0, 0);
		}
		else
		{
			sMode[ATTACK_B]->Entry(0, "image/e_attack-1-b.bmp", 0, 0);
			sMode[ATTACK_B]->Entry(1, "image/e_attack-2-b.bmp", 0, 0);
			sMode[ATTACK_B]->Entry(2, "image/e_attack-3-b.bmp", 0, 0);
		}
		sMode[ATTACK_B]->setLocation(0, x, y);
		sMode[ATTACK_B]->setLocation(1, x, y);
		sMode[ATTACK_B]->setLocation(2, x, y);
	}

	if (sMode[DEATH] == NULL)
	{
		sMode[DEATH] = new Sprite;
		sMode[DEATH]->Entry(0, "image/death-1.bmp", 0, 0);
		sMode[DEATH]->Entry(1, "image/death-2.bmp", 0, 0);
		sMode[DEATH]->Entry(2, "image/death-3.bmp", 0, 0);
		sMode[DEATH]->Entry(3, "image/death-4.bmp", 0, 0);
		sMode[DEATH]->Entry(4, "image/death-5.bmp", 0, 0);
		sMode[DEATH]->Entry(5, "image/death-6.bmp", 0, 0);
		sMode[DEATH]->Entry(6, "image/death-7.bmp", 0, 0);
		sMode[DEATH]->Entry(7, "image/death-8.bmp", 0, 0);
		sMode[DEATH]->Entry(8, "image/death-9.bmp", 0, 0);
		sMode[DEATH]->Entry(9, "image/death-10.bmp", 0, 0);
		sMode[DEATH]->Entry(10, "image/death-11.bmp", 0, 0);
		sMode[DEATH]->setLocation(0, x, y);
		sMode[DEATH]->setLocation(1, x, y);
		sMode[DEATH]->setLocation(2, x, y);
		sMode[DEATH]->setLocation(3, x, y);
		sMode[DEATH]->setLocation(4, x, y);
		sMode[DEATH]->setLocation(5, x, y);
		sMode[DEATH]->setLocation(6, x, y);
		sMode[DEATH]->setLocation(7, x, y);
		sMode[DEATH]->setLocation(8, x, y);
		sMode[DEATH]->setLocation(9, x, y);
		sMode[DEATH]->setLocation(10, x, y);
	}

	if (sMode[FIREBALL] == NULL)
	{
		sMode[FIREBALL] = new Sprite;
		if (player == 1)
		{
			sMode[FIREBALL]->Entry(0, "image/attack-01-1.bmp", 0, 0);
			sMode[FIREBALL]->Entry(1, "image/attack-02-1.bmp", 0, 0);
			sMode[FIREBALL]->Entry(2, "image/attack-03-1.bmp", 0, 0);
			sMode[FIREBALL]->Entry(3, "image/attack-04-1.bmp", 0, 0);
		}
		else
		{
			sMode[FIREBALL]->Entry(0, "image/e_attack-01-1.bmp", 0, 0);
			sMode[FIREBALL]->Entry(1, "image/e_attack-02-1.bmp", 0, 0);
			sMode[FIREBALL]->Entry(2, "image/e_attack-03-1.bmp", 0, 0);
			sMode[FIREBALL]->Entry(3, "image/e_attack-04-1.bmp", 0, 0);
		}
		sMode[FIREBALL]->setLocation(0, x, y);
		sMode[FIREBALL]->setLocation(1, x, y);
		sMode[FIREBALL]->setLocation(2, x, y);
		sMode[FIREBALL]->setLocation(3, x, y);
	}

	if (sMode[FIREBALL_B] == NULL)
	{
		sMode[FIREBALL_B] = new Sprite;
		if (player == 1)
		{
			sMode[FIREBALL_B]->Entry(0, "image/attack-01-b.bmp", 0, 0);
			sMode[FIREBALL_B]->Entry(1, "image/attack-02-b.bmp", 0, 0);
			sMode[FIREBALL_B]->Entry(2, "image/attack-03-b.bmp", 0, 0);
			sMode[FIREBALL_B]->Entry(3, "image/attack-04-b.bmp", 0, 0);
		}
		else
		{
			sMode[FIREBALL_B]->Entry(0, "image/e_attack-01-b.bmp", 0, 0);
			sMode[FIREBALL_B]->Entry(1, "image/e_attack-02-b.bmp", 0, 0);
			sMode[FIREBALL_B]->Entry(2, "image/e_attack-03-b.bmp", 0, 0);
			sMode[FIREBALL_B]->Entry(3, "image/e_attack-04-b.bmp", 0, 0);
		}
		sMode[FIREBALL_B]->setLocation(0, x, y);
		sMode[FIREBALL_B]->setLocation(1, x, y);
		sMode[FIREBALL_B]->setLocation(2, x, y);
		sMode[FIREBALL_B]->setLocation(3, x, y);
	}

	if (sMode[CRUSH] == NULL)
	{
		sMode[CRUSH] = new Sprite;
		sMode[CRUSH]->Entry(0, "image/boom_01.bmp", 0, 0);
		sMode[CRUSH]->Entry(1, "image/boom_02.bmp", 0, 0);
		sMode[CRUSH]->Entry(2, "image/boom_03.bmp", 0, 0);
		sMode[CRUSH]->Entry(3, "image/boom_04.bmp", 0, 0);
		sMode[CRUSH]->Entry(4, "image/boom_05.bmp", 0, 0);
		sMode[CRUSH]->Entry(5, "image/boom_06.bmp", 0, 0);
		sMode[CRUSH]->setLocation(0, x, y);
		sMode[CRUSH]->setLocation(1, x, y);
		sMode[CRUSH]->setLocation(2, x, y);
		sMode[CRUSH]->setLocation(3, x, y);
		sMode[CRUSH]->setLocation(4, x, y);
		sMode[CRUSH]->setLocation(5, x, y);
	}
}

void Hero::Update()
{

}

void Hero::Render(HDC* cDC)
{
	if (animCount[mode] >= sMode[mode]->getIndex())
		animCount[mode] = 0;
	sMode[mode]->Render(cDC, animCount[mode], (UINT)RGB(255, 0, 255));
	animCount[mode]++;
	/*
	if (animCount[WALK_B] >= sWalk_b->getIndex())
		animCount[WALK_B] = 0;
	sWalk_b->Render(cDC, animCount[WALK_B], (UINT)RGB(255, 0, 255));
	animCount[WALK_B]++;

	if (animCount[JUMP] >= sJump->getIndex())
		animCount[JUMP] = 0;
	sJump->Render(cDC, animCount[JUMP], (UINT)RGB(255, 0, 255));
	animCount[JUMP]++;

	if (animCount[JUMP_B] >= sJump_b->getIndex())
		animCount[JUMP_B] = 0;
	sJump_b->Render(cDC, animCount[JUMP_B], (UINT)RGB(255, 0, 255));
	animCount[JUMP_B]++;

	if (animCount[IDLE] >= sIdle->getIndex())
		animCount[IDLE] = 0;
	sIdle->Render(cDC, animCount[IDLE], (UINT)RGB(255, 0, 255));
	animCount[IDLE]++;

	if (animCount[IDLE_B] >= sIdle_b->getIndex())
		animCount[IDLE_B] = 0;
	sIdle_b->Render(cDC, animCount[IDLE_B], (UINT)RGB(255, 0, 255));
	animCount[IDLE_B]++;

	if (animCount[ATTACK] >= sAttack->getIndex())
		animCount[ATTACK] = 0;
	sAttack->Render(cDC, animCount[ATTACK], (UINT)RGB(255, 0, 255));
	animCount[ATTACK]++;

	if (animCount[ATTACK_B] >= sAttack_b->getIndex())
		animCount[ATTACK_B] = 0;
	sAttack_b->Render(cDC, animCount[ATTACK_B], (UINT)RGB(255, 0, 255));
	animCount[ATTACK_B]++;
*/
}

void Hero::Destroy()
{
#pragma region SpriteDelete
	for (int i = 0; i<ANIMNUM; ++i)
		SAFE_DELETE(sMode[i]);
	/*SAFE_DELETE(sWalk);
	SAFE_DELETE(sWalk_b);
	SAFE_DELETE(sJump);
	SAFE_DELETE(sJump_b);
	SAFE_DELETE(sIdle);
	SAFE_DELETE(sIdle_b);
	SAFE_DELETE(sAttack);
	SAFE_DELETE(sAttack_b);
	SAFE_DELETE(sDeath);
	SAFE_DELETE(sFireball);
	SAFE_DELETE(sFireball_b);
	SAFE_DELETE(sCrushEffect);*/
#pragma endregion
}

void Hero::setLocation(int x1, int y1)
{
	x = x1;
	y = y1;
}

int Hero::getX()
{
	return x;
}

int Hero::getY()
{
	return y;
}