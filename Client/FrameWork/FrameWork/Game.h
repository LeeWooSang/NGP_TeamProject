#pragma once
#include "Scene.h"
#include "Sprite.h"
#include "MyInclude.h"
#include "Hero.h"

#define TYPE_INIT 100
#define TYPE_RUN 101
#define TYPE_SKILL 102
#define TYPE_END 103

#define KEY_IDLE    0X00
#define KEY_RIGHT 	0X01
#define KEY_LEFT  	0X02
#define KEY_UP    	0X03
#define KEY_SPACE	0X04

struct SC_INIT
{
	byte        type;
	bool        isStart;
	byte        player;
};

struct SC_RUN
{
	byte        type;
	COORD		pos[2];
	USHORT		hp[2];
	bool        onSkill;
};

struct CS_RUN
{
	byte        type;
	byte        key;
	byte        player;
};

struct SC_SKILL
{
	byte       type;
	byte       skillIndex;
	COORD    skillPos;
	bool       isCrush;
	byte       player;
	bool       isRight;
};

struct SC_END
{
	byte type;
	byte winner;
};

class Game : public Scene
{
private:
	Sprite * background;
	class Hero * pHero;
	class Hero * eHero;
	struct SC_INIT pscInit;
	struct SC_RUN pscRun;
	struct SC_SKILL pscSkill;
	struct SC_END pscEnd;
	struct CS_RUN pcsRun;
	//bool PlayerRightMove;
public:
	Game();
	~Game();
public:
	void Render(HDC* cDC);
	void MouseInput(int iMessage, int x, int y);
	void KeyboardInput(int iMessage, int wParam);
	void KeyboardCharInput(int wParam);
	void Enter();
	void Destroy();
	void Update();
public:
	static DWORD WINAPI ClientThread(LPVOID socket);
	static DWORD WINAPI RecvThread(LPVOID socket);
};