#pragma once

#pragma comment(lib, "ws2_32")
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <map>
#include <ctime>

using namespace std;

// ����-Ŭ�󰣿� ��ӵ� ��ũ��
#define KEY_IDLE    	0X00
#define KEY_RIGHT 		0X01
#define KEY_LEFT		0X02
#define KEY_UP    		0X03
#define KEY_SPACE		0X04

#define WALK 0
#define WALK_B 1
#define JUMP 2
#define JUMP_B 3
#define IDLE 4
#define IDLE_B 5
#define ATTACK 6
#define ATTACK_B 7
#define DEATH 8

#define MAXSKILL 10

//HANDLE hEvent;

// PACKET_TYPE
enum PACKET_TYPE { TYPE_INIT, TYPE_START, TYPE_RUN, TYPE_SKILL, TYPE_END };
enum PLAYER { PLAYER_1, PLAYER_2 };
// Ű���� �ƽ�Ű ��ȣ ����
enum ASCII_KEY { RIGHT = 77, LEFT = 75, UP = 72, SPACE = 32 };
// ��Ŷ ����ü
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

struct CS_SKILL
{
	byte       skillIndex;
	byte       player;
};


struct SC_END
{
	byte			winner;
};
#pragma pack()

struct Client_Info
{
	Client_Info(SOCKET socket, byte num, COORD p, bool skill, byte mode, bool bRight,bool sRight,USHORT hp) : client_socket(socket), player(num), pos(p), onSkill(skill), eMode(mode), isRight(bRight),isSkillRight(sRight),hp(hp) {}

	SOCKET			client_socket;
	byte			player;
	COORD			pos;
	bool			onSkill;
	COORD			skillPos;
	byte			eMode;				// ������ �ִϸ��̼� ����
	bool			isRight;				// false = Right	
	bool			isSkillRight; // right == true		
	USHORT			hp;
};

