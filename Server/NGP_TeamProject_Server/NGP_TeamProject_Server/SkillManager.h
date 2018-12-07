#pragma once
#include "Defines.h"
#include "Collision.h"

#define PLAYER_WIDTH 75
#define PLAYER_HEIGHT 70
#define FIREBALL_WIDTH 173
#define FIREBALL_HEIGHT 46
#define MAXSKILL 10


class SkillManager
{
public:
	SkillManager();
	~SkillManager();

	//현재 발동한 스킬 정보 추가
	void addSkill(byte& player,COORD& skillPos);

	void checkCollision(COORD& player,SC_SKILL* skill);
	void update(float fTimeElapsed,COORD& player1,COORD& player2);


public:
	static int m_Player1_SkillCount;
	static int m_Player2_SkillCount;
private:
	//map<pair<bool,int>,SC_SKILL> skillMap;
	//vector<SC_SKILL> skillVector;

	SC_SKILL **player1_SkillArray =NULL;
	//int		m_Player1_SkillCount=0;

	SC_SKILL **player2_SkillArray =NULL;
	//int     m_Player2_SkillCount=0;
	//vector<SC_SKILL> player1_SkillVector;
	//vector<SC_SKILL> player2_SkillVector;

	//vector<SC_SKILL> ::iterator iter;
};