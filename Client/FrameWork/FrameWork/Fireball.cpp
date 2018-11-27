#include "Fireball.h"

Fireball::Fireball()
{
	pos.X = 0;
	pos.Y = 0;
	isRight = true;
	isCrush = false;
	isDraw = false;
}

Fireball::Fireball(int x, int y, bool r, byte p)
{
	pos.X = x;
	pos.Y = y;
	isRight = r;
	isCrush = false;
	isDraw = false;

	initSprite(p);
}

void Fireball::initSprite(byte p)
{
	if (sFireball[FIREBALL] == NULL)
	{
		sFireball[FIREBALL] = new Sprite;
		if (p == PLAYER1)
		{
			sFireball[FIREBALL]->Entry(0, "image/attack-01-1.bmp", 0, 0);
			sFireball[FIREBALL]->Entry(1, "image/attack-02-1.bmp", 0, 0);
			sFireball[FIREBALL]->Entry(2, "image/attack-03-1.bmp", 0, 0);
			sFireball[FIREBALL]->Entry(3, "image/attack-04-1.bmp", 0, 0);
		}
		else
		{
			sFireball[FIREBALL]->Entry(0, "image/e_attack-01-1.bmp", 0, 0);
			sFireball[FIREBALL]->Entry(1, "image/e_attack-02-1.bmp", 0, 0);
			sFireball[FIREBALL]->Entry(2, "image/e_attack-03-1.bmp", 0, 0);
			sFireball[FIREBALL]->Entry(3, "image/e_attack-04-1.bmp", 0, 0);
		}
		sFireball[FIREBALL]->setLocation(0, pos.X, pos.Y);
		sFireball[FIREBALL]->setLocation(1, pos.X, pos.Y);
		sFireball[FIREBALL]->setLocation(2, pos.X, pos.Y);
		sFireball[FIREBALL]->setLocation(3, pos.X, pos.Y);
	}

	if (sFireball[FIREBALL_B] == NULL)
	{
		sFireball[FIREBALL_B] = new Sprite;
		if (p == PLAYER1)
		{
			sFireball[FIREBALL_B]->Entry(0, "image/attack-01-b.bmp", 0, 0);
			sFireball[FIREBALL_B]->Entry(1, "image/attack-02-b.bmp", 0, 0);
			sFireball[FIREBALL_B]->Entry(2, "image/attack-03-b.bmp", 0, 0);
			sFireball[FIREBALL_B]->Entry(3, "image/attack-04-b.bmp", 0, 0);
		}
		else
		{
			sFireball[FIREBALL_B]->Entry(0, "image/e_attack-01-b.bmp", 0, 0);
			sFireball[FIREBALL_B]->Entry(1, "image/e_attack-02-b.bmp", 0, 0);
			sFireball[FIREBALL_B]->Entry(2, "image/e_attack-03-b.bmp", 0, 0);
			sFireball[FIREBALL_B]->Entry(3, "image/e_attack-04-b.bmp", 0, 0);
		}
		sFireball[FIREBALL_B]->setLocation(0, pos.X, pos.Y);
		sFireball[FIREBALL_B]->setLocation(1, pos.X, pos.Y);
		sFireball[FIREBALL_B]->setLocation(2, pos.X, pos.Y);
		sFireball[FIREBALL_B]->setLocation(3, pos.X, pos.Y);
	}

	if (sFireball[CRUSH] == NULL)
	{
		sFireball[CRUSH] = new Sprite;
		sFireball[CRUSH]->Entry(0, "image/boom_01.bmp", 0, 0);
		sFireball[CRUSH]->Entry(1, "image/boom_02.bmp", 0, 0);
		sFireball[CRUSH]->Entry(2, "image/boom_03.bmp", 0, 0);
		sFireball[CRUSH]->Entry(3, "image/boom_04.bmp", 0, 0);
		sFireball[CRUSH]->Entry(4, "image/boom_05.bmp", 0, 0);
		sFireball[CRUSH]->Entry(5, "image/boom_06.bmp", 0, 0);
		sFireball[CRUSH]->setLocation(0, pos.X, pos.Y);
		sFireball[CRUSH]->setLocation(1, pos.X, pos.Y);
		sFireball[CRUSH]->setLocation(2, pos.X, pos.Y);
		sFireball[CRUSH]->setLocation(3, pos.X, pos.Y);
		sFireball[CRUSH]->setLocation(4, pos.X, pos.Y);
		sFireball[CRUSH]->setLocation(5, pos.X, pos.Y);
	}
}

Fireball::~Fireball()
{
	for (int i = 0; i<S_ANIMNUM; ++i)
		SAFE_DELETE_ARRAY(sFireball[i]);
}

void Fireball::Render(HDC* cDC)
{
	if (isCrush)
	{
		if (animCount[CRUSH] >= sFireball[CRUSH]->getIndex())
			isDraw = false;
		else
		{
			sFireball[CRUSH]->setLocation(pos.X, pos.Y);
			sFireball[CRUSH]->Render(cDC, animCount[CRUSH], (UINT)RGB(255, 0, 255));
			animCount[CRUSH]++;
		}
	}
	else if (!isCrush)
	{
		if (isRight)
		{
			if (animCount[FIREBALL] >= sFireball[FIREBALL]->getIndex())
				animCount[FIREBALL] = 0;
			sFireball[FIREBALL]->setLocation(pos.X, pos.Y);
			sFireball[FIREBALL]->Render(cDC, animCount[FIREBALL], (UINT)RGB(255, 0, 255));
			animCount[FIREBALL]++;
		}
		else if (!isRight)
		{
			if (animCount[FIREBALL_B] >= sFireball[FIREBALL_B]->getIndex())
				animCount[FIREBALL_B] = 0;
			sFireball[FIREBALL_B]->setLocation(pos.X, pos.Y);
			sFireball[FIREBALL_B]->Render(cDC, animCount[FIREBALL_B], (UINT)RGB(255, 0, 255));
			animCount[FIREBALL_B]++;
		}
	}
}

void Fireball::setLocation(int x, int y)
{
	pos.X = x;
	pos.Y = y;
}

void Fireball::getLocation(int *x, int *y)
{
	*x = pos.X;
	*y = pos.Y;
}
