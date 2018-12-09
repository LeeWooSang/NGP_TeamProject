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
enum PACKET_TYPE { TYPE_INIT, TYPE_RUN, TYPE_END };
enum PLAYER { PLAYER_1, PLAYER_2 };
// 키보드 아스키 번호 정의
enum ASCII_KEY { RIGHT = 77, LEFT = 75, UP = 72, SPACE = 32 };
// 패킷 구조체
#pragma pack(1)
struct SC_INIT
{
	bool			isStart;
	byte			player;
};

struct CS_RUN
{
	CS_RUN() {}
	CS_RUN(byte t, byte k,bool p) : key(t), player(k) { }
	byte        key;
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
	SKILL_INFO      player1_skill[MAXSKILL];
	SKILL_INFO		player2_skill[MAXSKILL];
};

struct SC_RUN
{
	byte		eMode[2];
	COORD		pos[2];
	USHORT		hp[2];
	SKILL		skillInfo;
};

struct SC_END
{
	byte			winner;
};
#pragma pack()

struct Client_Info
{
	Client_Info(SOCKET socket, byte num, COORD p, byte mode, bool bRight,bool sRight,USHORT hp) : client_socket(socket), player(num), pos(p), eMode(mode), isRight(bRight),isSkillRight(sRight),hp(hp) {}

	SOCKET			client_socket;
	byte			player;
	COORD			pos;
	COORD			skillPos;
	byte			eMode;				// 상대방의 애니메이션 상태
	bool			isRight;				// false = Right	
	bool			isSkillRight; // right == true		
	USHORT			hp;
};

