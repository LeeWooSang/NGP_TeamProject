#pragma once
#include "Defines.h"
#include "Collision.h"

#define PLAYER_WIDTH 75
#define PLAYER_HEIGHT 70
#define FIREBALL_WIDTH 173
#define FIREBALL_HEIGHT 46

class SkillManager
{
public:
	SkillManager();
	~SkillManager();

	//현재 발동한 스킬 정보 추가
	void addSkill(byte& player,COORD& skillPos);

	void checkCollision(COORD& player1,COORD& player2);
	void update(float fTimeElapsed,COORD& player1,COORD& player2);
private:
	//map<pair<bool,int>,SC_SKILL> skillMap;
	vector<SC_SKILL> skillVector;

	//vector<SC_SKILL> ::iterator iter;
};