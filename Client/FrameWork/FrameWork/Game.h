#pragma once
#include "Scene.h"
#include "Sprite.h"
#include "MyInclude.h"
#include "Hero.h"

// PACKET_TYPE
#define TYPE_INIT 100
#define TYPE_RUN 101
#define TYPE_SKILL 102
#define TYPE_END 103

// 플레이어 구분용
#define PLAYER1 0
#define PLAYER2 1

// 키보드 아스키 번호 정의
#define RIGHT 77
#define LEFT 75
#define UP 72
#define SPACE 32

#define KEY_IDLE    0X00
#define KEY_RIGHT 	0X01
#define KEY_LEFT  	0X02
#define KEY_UP    	0X03
#define KEY_SPACE	0X04

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000

#pragma pack(1)
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
#pragma pack()

class Game : public Scene
{
private:
	Sprite * background;
	static size_t packetSize;
	static byte gameState;
	static class Hero * pHero;
	static class Hero * eHero;
	static struct SC_INIT pSCInit;
	static struct SC_RUN pSCRun;
	static struct SC_SKILL pSCSkill;
	static struct SC_END pSCEnd;
	static struct CS_RUN pCSRun;

	static CRITICAL_SECTION cs;
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

	static DWORD WINAPI ClientThread(LPVOID sock);
	static DWORD WINAPI RecvThread(LPVOID sock);

public:
	static int recvn(SOCKET s, char* buf, int len, int flags);
	
	static void err_quit(const char *msg);
	static void err_display(const char *msg);
};