#pragma once
#include "Scene.h"
#include "Sprite.h"
#include "Hero.h"
#include "Fireball.h"

#define MAXSKILL 10

// PACKET_TYPE
#define TYPE_INIT 0
#define TYPE_START 1
#define TYPE_RUN 2
#define TYPE_SKILL 3
#define TYPE_END 4

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
//#define SERVERIP "192.168.121.212"
#define SERVERPORT 9000

#pragma pack(1)
struct SC_INIT
{
	byte        type;
	bool        isStart;
	byte        player;
};

struct CS_INIT
{
	bool        isReady;
	byte        player;
};

struct SKILL_INFO
{
	bool			isEnable;
	COORD			skillPos;
	bool			isCrush;
	byte			player;
	bool			isSkillRight;
};


struct SKILL
{
	byte			player1_skillIndex;
	byte			player2_skillIndex;
	SKILL_INFO      player1_skill[MAXSKILL];
	SKILL_INFO		player2_skill[MAXSKILL];
};

struct SC_RUN
{
	byte		type;
	byte		eMode[2];
	COORD		pos[2];
	USHORT		hp[2];
	bool        onSkill;
	SKILL		skillInfo;
};

struct CS_RUN
{
	byte        key;
	byte        player;
	bool        onSkill;
};



struct SC_END
{
	byte winner;
};
#pragma pack()

class Game : public Scene
{
private:
	Sprite * background;
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
	static DWORD WINAPI ClientThread(LPVOID sock);
	static DWORD WINAPI RecvThread(LPVOID sock);

public:
	static int recvn(SOCKET s, char* buf, int len, int flags);
	
	static void err_quit(const char *msg);
	static void err_display(const char *msg);
};



