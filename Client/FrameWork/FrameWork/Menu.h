#pragma once
#include "Scene.h"
#include "MyInclude.h"
#include "Sprite.h"
#include "Collision.h"
#include <string>

class Menu : public Scene
{
private:
	Sprite * pSprite;			// 사용할 이미지 또는 애니메이션
	float alpha;
	bool fadeIn;
	LPCTSTR startButton{ "Start!" };
	LPCTSTR exitButton{ "Exit" };
	Collision* ColFunc;
public:
	Menu();
	~Menu();
public:
	void Render(HDC* cDC);
	void MouseInput(int iMessage, int x, int y);
	void KeyboardInput(int iMessage, int wParam);
	void KeyboardCharInput(int wParam);
	void Enter();
	void Destroy();
	void Update();
};