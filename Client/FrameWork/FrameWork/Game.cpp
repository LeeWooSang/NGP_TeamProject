#include "Game.h"


Game::Game()
{
	background = NULL;
	//PlayerRightMove = false;
	pHero = NULL;
	eHero = NULL;
}
Game::~Game()
{
	if (pHero)
	{
		pHero->Destroy();
		SAFE_DELETE(pHero);
	}
	if (eHero)
	{
		eHero->Destroy();
		SAFE_DELETE(eHero);
	}
	SAFE_DELETE(background);
}
void Game::Enter()
{
	//static FWMain* main = &(FWMain::getMain());
	if (background == NULL)
	{
		background = new Sprite;
		background->Entry(0, "image/start.bmp", 0, 0);
		//background->setLocation(0, background->getWidth(0)/2, background->getHeight(0)/2);
	}
	if (pHero == NULL)
	{
		pHero = new Hero(50, 50, IDLE, 1);
		pHero->Enter();
	}
	if (eHero == NULL)
	{
		eHero = new Hero(250, 50, IDLE, 2);
		eHero->Enter();
	}
	CreateThread(NULL, 0, ClientThread, NULL, 0, NULL);
	//BulletIndex = pSprite->getIndex() + 1;
}
void Game::Destroy()
{
	if (pHero)
	{
		pHero->Destroy();
		SAFE_DELETE(pHero);
	}
	if (eHero)
	{
		eHero->Destroy();
		SAFE_DELETE(eHero);
	}
	SAFE_DELETE(background);
}
void Game::Render(HDC* cDC)
{
	background->Render(cDC, 0);

	if (pHero)
		pHero->Render(cDC);
	if (eHero)
		eHero->Render(cDC);
}
void Game::MouseInput(int iMessage, int x, int y)
{
	switch (iMessage)
	{
	case WM_LBUTTONDOWN:
		printf("%d %d\n", x, y);
		break;
	}
}
void Game::KeyboardInput(int iMessage, int wParam)
{
	if (iMessage == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case VK_RIGHT:
			//PlayerRightMove = true;
			break;
		case VK_LEFT:
			break;
		}
	}
	if (iMessage == WM_KEYUP)
	{
		switch (wParam)
		{
		case VK_RIGHT:
			break;
		case VK_LEFT:
			break;
		}
	}
}
void Game::KeyboardCharInput(int wParam)
{
	switch (wParam)
	{
	case VK_SPACE:
		printf("%d, %d\n", pHero->getX(), pHero->getY());
		break;
	case VK_ESCAPE:
		PostQuitMessage(0);
		break;
	}
}

void Game::Update()
{
	if (pHero)
		pHero->setLocation(pscRun.pos[0].X, pscRun.pos[0].Y);
	if (eHero)
		eHero->setLocation(pscRun.pos[1].X, pscRun.pos[1].Y);
	/*if (PlayerRightMove)
	{
		pWalk->Move(10, 0);
	}
	else if (PlayerLeftMove)
	{
		pSprite[PLAYER1].setLocation(PLAYER1, pSprite[PLAYER1].getX(PLAYER1) - 10, pSprite[PLAYER1].getY(PLAYER1));
	}*/
}

DWORD WINAPI Game::ClientThread(LPVOID socket)
{

}

DWORD WINAPI Game::RecvThread(LPVOID socket)
{

}
