#pragma once
#pragma comment(lib, "ws2_32")
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <map>
#include <ctime>

using namespace std;

// 서버-클라간에 약속된 매크로
#define KEY_IDLE    	0X00
#define KEY_RIGHT 	0X01
#define KEY_LEFT  	0X02
#define KEY_UP    	0X03
#define KEY_SPACE	0X04

// PACKET_TYPE
enum PACKET_TYPE { TYPE_INIT, TYPE_START, TYPE_RUN, TYPE_SKILL, TYPE_END };
enum PLAYER { PLAYER_1, PLAYER_2 };
// 키보드 아스키 번호 정의
enum ASCII_KEY { RIGHT = 77, LEFT = 75, UP = 72, SPACE = 32 };
// 패킷 구조체
#pragma pack(1)
struct SC_INIT
{
	byte			type;
	bool			isStart;
	byte			player;
};

struct CS_INIT
{
	//byte			type;
	bool			isReady;
	byte			player;
};

struct CS_RUN
{
	CS_RUN() {}
	CS_RUN(byte t, byte k,bool p) : key(t), player(k),onSkill(p) { }

	//byte        type;
	byte        key;
	byte        player;
	bool		onSkill;
};

#define MAXSKILL 10
struct SKILL_INFO
{
	bool			isEnable;
	COORD			skillPos;
	bool			isCrush;
	byte			player;
	bool			isRight;
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
struct SC_SKILL
{
	//byte			type;
	bool			isEnable;
	byte			skillIndex;
	COORD	skillPos;
	bool			isCrush;
	byte			player;
	bool			isRight;
};

struct CS_SKILL
{
	byte       skillIndex;
	byte       player;
};


struct SC_END
{
	byte			type;
	byte			winner;
};
#pragma pack()


