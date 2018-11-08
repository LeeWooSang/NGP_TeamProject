#pragma once
#pragma comment(lib,"ws2_32")

#include <iostream>
#include <string>
#include <WinSock2.h>
#include <Windows.h>
#include <conio.h>

using namespace std;

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000

// ����-Ŭ�󰣿� ��ӵ� ��ũ��
#define KEY_IDLE    	0X00
#define KEY_RIGHT 	0X01
#define KEY_LEFT  	0X02
#define KEY_UP    	0X03
#define KEY_SPACE	0X04

// PACKET_TYPE
enum PACKET_TYPE { TYPE_INIT, TYPE_RUN, TYPE_SKILL, TYPE_END };
// �÷��̾� ���п�
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

struct CS_RUN
{
	byte        type;
	byte        key;
	byte        player;
};

struct SC_RUN
{
	byte			type;
	COORD			pos[2];
	USHORT			hp[2];
	bool			onSkill;
};

struct SC_SKILL
{
	byte			type;
	byte			skillIndex;
	COORD	skillPos;
	bool			isCrush;
	byte			player;
	bool			isRight;
};

struct SC_END
{
	byte			type;
	byte			winner;
};
#pragma pack()
