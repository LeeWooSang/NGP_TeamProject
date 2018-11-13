#pragma once
#include "Scene.h"
#include "MyInclude.h"
#include "Sprite.h"
#include "Collision.h"
#include <string>

class Menu : public Scene
{
private:
	Sprite * pSprite;			// ����� �̹��� �Ǵ� �ִϸ��̼�
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