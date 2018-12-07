#include "Menu.h"
#include "FWMain.h"

#define BACKGROUND 0
#define LOGO 1
#define STARTBUTTON 2
#define EXITBUTTON 3

Menu::Menu()
{
	pSprite = NULL;
	alpha = 0.0f;
	fadeIn = true;
	ColFunc = NULL;
}
Menu::~Menu()
{
	SAFE_DELETE(pSprite);
}

void Menu::Enter()
{
	if (pSprite == NULL)
	{
		pSprite = new Sprite;
		pSprite->Entry(BACKGROUND, "image/background.bmp",0,0);			// 애니메이션에 필요한 이미지 로드
		pSprite->Entry(LOGO, "image/Galagalogo.bmp",200,100);
		pSprite->Entry(STARTBUTTON, "image/startbutton.bmp",500,550);
		pSprite->Entry(EXITBUTTON, "image/exitbutton.bmp",500,700);
	}
}
void Menu::Destroy()
{
	SAFE_DELETE(pSprite);
}
void Menu::Render(HDC* cDC)
{
#pragma region LogoFadeIn
	pSprite->Render(cDC, BACKGROUND);
	pSprite->Render(cDC, LOGO, RGB(34, 31, 31), alpha);
#pragma endregion

#pragma region Button
	pSprite->Render(cDC, STARTBUTTON, (UINT)RGB(0, 255, 255));
	pSprite->Render(cDC, EXITBUTTON, (UINT)RGB(0, 255, 255));
#pragma endregion

#pragma region Text
	/*HFONT hFont = CreateFont(50, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("Algerian"));
	HFONT oldFont = (HFONT)SelectObject(*cDC, hFont);
	SetTextColor(*cDC, RGB(255,255,255));
	SetBkColor(*cDC, WHITE_BRUSH);
	SetBkMode(*cDC, TRANSPARENT);
	TextOut(*cDC, 600, 600, startButton, strlen(startButton));
	TextOut(*cDC, 600, 700, exitButton, strlen(exitButton));
	SelectObject(*cDC, oldFont);
	DeleteObject(hFont);*/
#pragma endregion


	
}
void Menu::Update()
{
	if (fadeIn)
	{
		alpha += 0.01;
		if (alpha >= 1)
		{
			alpha = 1;
			fadeIn = false;
		}
	}
	else
	{
		alpha -= 0.01;
		if (alpha <= 0)
		{
			alpha = 0;
			fadeIn = true;
		}
	}
}
void Menu::MouseInput(int iMessage, int x, int y)
{
	switch (iMessage)
	{
	case WM_LBUTTONDOWN:
		printf("%d %d\n", x, y);
		if (ColFunc->DotRectangle(x, y, pSprite->getWidth(STARTBUTTON), pSprite->getHeight(STARTBUTTON), pSprite->getX(STARTBUTTON), pSprite->getY(STARTBUTTON)))
			FWMain::getMain().Warp(1);
		else if (ColFunc->DotRectangle(x, y, pSprite->getWidth(EXITBUTTON), pSprite->getHeight(EXITBUTTON), pSprite->getX(EXITBUTTON), pSprite->getY(EXITBUTTON)))
			PostQuitMessage(0);
		break;
	}
}
void Menu::KeyboardInput(int iMessage, int wParam)
{
	if (iMessage == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case VK_RIGHT:

			printf("업다운 %d\n", wParam);
			break;
		case VK_LEFT:

			FWMain::getMain().Warp(1);

			break;
		//case VK_SPACE:
		//	//printf("파이어볼");
		//	break;
		}
		

	}
	
}
void Menu::KeyboardCharInput(int wParam)
{
	switch (wParam)
	{
	case VK_RETURN:
		printf("CHAR %d\n", wParam);
		break;
	case VK_ESCAPE:
		PostQuitMessage(0);
		break;
	}
}
