#pragma once
#include "Defines.h"
#include "Collision.h"

#define PLAYER_WIDTH 75
#define PLAYER_HEIGHT 70
#define FIREBALL_WIDTH 173
#define FIREBALL_HEIGHT 46
//#define MAXSKILL 10


class SkillManager
{
public:
	SkillManager();
	~SkillManager();

	//현재 발동한 스킬 정보 추가
	void addSkill(byte& player,COORD& skillPos,bool isRight);
	void checkCollision(COORD& player,SKILL_INFO* skill,Client_Info& client);
	void update(Client_Info& client1,Client_Info& client2);


public:
	static int m_Player1_SkillCount;
	static int m_Player2_SkillCount;
	SKILL_INFO	**player1_Skill = NULL;
	SKILL_INFO  **player2_Skill = NULL;

};