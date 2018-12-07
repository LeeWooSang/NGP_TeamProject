#pragma once
#include <Windows.h>
class Scene {
public:
	Scene() {}
	virtual ~Scene() {}
public:
	virtual void Render(HDC* cDC) = 0;
	virtual void MouseInput(int iMessage,int x, int y) = 0;
	virtual void KeyboardInput(int iMessage, int wParam) = 0;
	virtual void KeyboardCharInput(int wParam) = 0;
	virtual void Enter() = 0;
	virtual void Destroy() = 0;
	virtual void Update() = 0;
};