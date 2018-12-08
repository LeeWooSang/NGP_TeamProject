#include "Fireball.h"

Fireball::Fireball()
{
	for (int i = 0; i < S_ANIMNUM; ++i)
	{
		sPFireball[i] = NULL;
		sEFireball[i] = NULL;
	}
	pos.X = 0;
	pos.Y = 0;
	isRight = true;
	isCrush = false;
	isDraw = false;
}

Fireball::Fireball(int x, int y, bool r)
{
	for (int i = 0; i < S_ANIMNUM; ++i)
	{
		sPFireball[i] = NULL;
		sEFireball[i] = NULL;
	}
	pos.X = x;
	pos.Y = y;
	isRight = r;
	isCrush = false;
	isDraw = false;

	initSprite();
}

void Fireball::Destroy()
{
	for (int i = 0; i < S_ANIMNUM; ++i)
	{
		SAFE_DELETE(sPFireball[i]);
		SAFE_DELETE(sEFireball[i]);
	}
}

void Fireball::initSprite()
{
	for (int i = 0; i < S_ANIMNUM; ++i)
		animCount[i] = 0;

	if (sPFireball[FIREBALL] == NULL && sEFireball[FIREBALL] == NULL)
	{
		sPFireball[FIREBALL] = new Sprite;
		sEFireball[FIREBALL] = new Sprite;
		
		sPFireball[FIREBALL]->Entry(0, "image/fireball_01-1.bmp", 0, 0);
		sPFireball[FIREBALL]->Entry(1, "image/fireball_02-1.bmp", 0, 0);
		sPFireball[FIREBALL]->Entry(2, "image/fireball_03-1.bmp", 0, 0);
		sPFireball[FIREBALL]->Entry(3, "image/fireball_04-1.bmp", 0, 0);

		sPFireball[FIREBALL]->setLocation(0, pos.X, pos.Y);
		sPFireball[FIREBALL]->setLocation(1, pos.X, pos.Y);
		sPFireball[FIREBALL]->setLocation(2, pos.X, pos.Y);
		sPFireball[FIREBALL]->setLocation(3, pos.X, pos.Y);
		
		sEFireball[FIREBALL]->Entry(0, "image/e_fireball_01-1.bmp", 0, 0);
		sEFireball[FIREBALL]->Entry(1, "image/e_fireball_02-1.bmp", 0, 0);
		sEFireball[FIREBALL]->Entry(2, "image/e_fireball_03-1.bmp", 0, 0);
		sEFireball[FIREBALL]->Entry(3, "image/e_fireball_04-1.bmp", 0, 0);
		
		sEFireball[FIREBALL]->setLocation(0, pos.X, pos.Y);
		sEFireball[FIREBALL]->setLocation(1, pos.X, pos.Y);
		sEFireball[FIREBALL]->setLocation(2, pos.X, pos.Y);
		sEFireball[FIREBALL]->setLocation(3, pos.X, pos.Y);
	}

	if (sPFireball[FIREBALL_B] == NULL && sEFireball[FIREBALL_B] == NULL)
	{
		sPFireball[FIREBALL_B] = new Sprite;
		sEFireball[FIREBALL_B] = new Sprite;
		
		sPFireball[FIREBALL_B]->Entry(0, "image/fireball_01-b.bmp", 0, 0);
		sPFireball[FIREBALL_B]->Entry(1, "image/fireball_02-b.bmp", 0, 0);
		sPFireball[FIREBALL_B]->Entry(2, "image/fireball_03-b.bmp", 0, 0);
		sPFireball[FIREBALL_B]->Entry(3, "image/fireball_04-b.bmp", 0, 0);

		sPFireball[FIREBALL_B]->setLocation(0, pos.X, pos.Y);
		sPFireball[FIREBALL_B]->setLocation(1, pos.X, pos.Y);
		sPFireball[FIREBALL_B]->setLocation(2, pos.X, pos.Y);
		sPFireball[FIREBALL_B]->setLocation(3, pos.X, pos.Y);
		
		sEFireball[FIREBALL_B]->Entry(0, "image/e_fireball_01-b.bmp", 0, 0);
		sEFireball[FIREBALL_B]->Entry(1, "image/e_fireball_02-b.bmp", 0, 0);
		sEFireball[FIREBALL_B]->Entry(2, "image/e_fireball_03-b.bmp", 0, 0);
		sEFireball[FIREBALL_B]->Entry(3, "image/e_fireball_04-b.bmp", 0, 0);
		
		sEFireball[FIREBALL_B]->setLocation(0, pos.X, pos.Y);
		sEFireball[FIREBALL_B]->setLocation(1, pos.X, pos.Y);
		sEFireball[FIREBALL_B]->setLocation(2, pos.X, pos.Y);
		sEFireball[FIREBALL_B]->setLocation(3, pos.X, pos.Y);
	}

	if (sPFireball[CRUSH] == NULL && sEFireball[CRUSH] == NULL)
	{
		sPFireball[CRUSH] = new Sprite;
		sEFireball[CRUSH] = new Sprite;

		sPFireball[CRUSH]->Entry(0, "image/boom_01.bmp", 0, 0);
		sPFireball[CRUSH]->Entry(1, "image/boom_02.bmp", 0, 0);
		sPFireball[CRUSH]->Entry(2, "image/boom_03.bmp", 0, 0);
		sPFireball[CRUSH]->Entry(3, "image/boom_04.bmp", 0, 0);
		sPFireball[CRUSH]->Entry(4, "image/boom_05.bmp", 0, 0);
		sPFireball[CRUSH]->Entry(5, "image/boom_06.bmp", 0, 0);

		sPFireball[CRUSH]->setLocation(0, pos.X, pos.Y);
		sPFireball[CRUSH]->setLocation(1, pos.X, pos.Y);
		sPFireball[CRUSH]->setLocation(2, pos.X, pos.Y);
		sPFireball[CRUSH]->setLocation(3, pos.X, pos.Y);
		sPFireball[CRUSH]->setLocation(4, pos.X, pos.Y);
		sPFireball[CRUSH]->setLocation(5, pos.X, pos.Y);

		sEFireball[CRUSH]->Entry(0, "image/boom_01.bmp", 0, 0);
		sEFireball[CRUSH]->Entry(1, "image/boom_02.bmp", 0, 0);
		sEFireball[CRUSH]->Entry(2, "image/boom_03.bmp", 0, 0);
		sEFireball[CRUSH]->Entry(3, "image/boom_04.bmp", 0, 0);
		sEFireball[CRUSH]->Entry(4, "image/boom_05.bmp", 0, 0);
		sEFireball[CRUSH]->Entry(5, "image/boom_06.bmp", 0, 0);
		 
		sEFireball[CRUSH]->setLocation(0, pos.X, pos.Y);
		sEFireball[CRUSH]->setLocation(1, pos.X, pos.Y);
		sEFireball[CRUSH]->setLocation(2, pos.X, pos.Y);
		sEFireball[CRUSH]->setLocation(3, pos.X, pos.Y);
		sEFireball[CRUSH]->setLocation(4, pos.X, pos.Y);
		sEFireball[CRUSH]->setLocation(5, pos.X, pos.Y);
	}
}

Fireball::~Fireball()
{
	for (int i = 0; i < S_ANIMNUM; ++i)
	{
		SAFE_DELETE_ARRAY(sPFireball[i]);
		SAFE_DELETE_ARRAY(sEFireball[i]);
	}
}

void Fireball::Render(HDC* cDC)
{
	if (player == PLAYER1)
	{
		if (isCrush)
		{
			if (animCount[CRUSH] >= sPFireball[CRUSH]->getIndex())
			{
				animCount[CRUSH] = 0;
				isDraw = false;
				isCrush = false;
			}
			sPFireball[CRUSH]->setLocation(pos.X, pos.Y);
			sPFireball[CRUSH]->Render(cDC, animCount[CRUSH], (UINT)RGB(255, 0, 255));
			animCount[CRUSH]++;
		}
		else if (!isCrush)
		{
			if (isRight)
			{
				if (animCount[FIREBALL] >= sPFireball[FIREBALL]->getIndex())
					animCount[FIREBALL] = 0;
				sPFireball[FIREBALL]->setLocation(pos.X, pos.Y);
				sPFireball[FIREBALL]->Render(cDC, animCount[FIREBALL], (UINT)RGB(255, 0, 255));
				animCount[FIREBALL]++;
			}
			else if (!isRight)
			{
				if (animCount[FIREBALL_B] >= sPFireball[FIREBALL_B]->getIndex())
					animCount[FIREBALL_B] = 0;
				sPFireball[FIREBALL_B]->setLocation(pos.X, pos.Y);
				sPFireball[FIREBALL_B]->Render(cDC, animCount[FIREBALL_B], (UINT)RGB(255, 0, 255));
				animCount[FIREBALL_B]++;
			}
		}
	}
	else
	{
		if (isCrush)
		{
			if (animCount[CRUSH] >= sEFireball[CRUSH]->getIndex())
			{
				animCount[CRUSH] = 0;
				isDraw = false;
				isCrush = false;
			}
			sEFireball[CRUSH]->setLocation(pos.X, pos.Y);
			sEFireball[CRUSH]->Render(cDC, animCount[CRUSH], (UINT)RGB(255, 0, 255));
			animCount[CRUSH]++;
		}
		else if (!isCrush)
		{
			if (isRight)
			{
				if (animCount[FIREBALL] >= sEFireball[FIREBALL]->getIndex())
					animCount[FIREBALL] = 0;
				sEFireball[FIREBALL]->setLocation(pos.X, pos.Y);
				sEFireball[FIREBALL]->Render(cDC, animCount[FIREBALL], (UINT)RGB(255, 0, 255));
				animCount[FIREBALL]++;
			}
			else if (!isRight)
			{
				if (animCount[FIREBALL_B] >= sEFireball[FIREBALL_B]->getIndex())
					animCount[FIREBALL_B] = 0;
				sEFireball[FIREBALL_B]->setLocation(pos.X, pos.Y);
				sEFireball[FIREBALL_B]->Render(cDC, animCount[FIREBALL_B], (UINT)RGB(255, 0, 255));
				animCount[FIREBALL_B]++;
			}
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
