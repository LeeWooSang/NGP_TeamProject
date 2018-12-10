#include "SkillManager.h"



int SkillManager::m_Player1_SkillCount = 0;
int SkillManager::m_Player2_SkillCount = 0;

SkillManager::SkillManager()
{

	player1_Skill = new SKILL_INFO*[MAXSKILL];
	player2_Skill = new SKILL_INFO*[MAXSKILL];

	for (int i = 0; i < MAXSKILL; i++)
	{
		player1_Skill[i] = new SKILL_INFO();
		player2_Skill[i] = new SKILL_INFO();
	}

	for (int i = 0; i < MAXSKILL; i++)
	{
		player1_Skill[i]->isEnable = false;
		player1_Skill[i]->isCrush = false;
		player1_Skill[i]->isSkillRight = true;
		player1_Skill[i]->player = PLAYER_1;
		player1_Skill[i]->skillPos = { -100,-100 };
		

		player2_Skill[i]->isEnable = false;
		player2_Skill[i]->isCrush = false;
		player2_Skill[i]->isSkillRight = false;
		player2_Skill[i]->player = PLAYER_2;
		player2_Skill[i]->skillPos = { -100,-100 };
	}

}

SkillManager::~SkillManager()
{
	
	if (player1_Skill)
	{
		for (int i = 0; i < MAXSKILL; i++)
		{
			delete player1_Skill[i];
		}
		delete[] player1_Skill;
	}

	if (player2_Skill)
	{
		for (int i = 0; i < MAXSKILL; i++)
		{
			delete player2_Skill[i];
		}
		delete[] player2_Skill;
	}

}


void SkillManager::addSkill(byte& player,COORD& skillPos,bool isRight)
{

	if (player == PLAYER_1) {
		for (int i = 0; i < MAXSKILL; i++)
		{
			if (player1_Skill[i]->isEnable == false)
			{
				
				player1_Skill[i]->isEnable = true;
				player1_Skill[i]->isCrush = false;
				player1_Skill[i]->skillPos = skillPos;
				player1_Skill[i]->isSkillRight = isRight;
				player1_Skill[i]->player = player;
				SkillManager::m_Player1_SkillCount++;
				break;
			}
		}
	}

	else if (player == PLAYER_2)
	{
		for (int i = 0; i < MAXSKILL; i++)
		{
			if (player2_Skill[i]->isEnable == false)
			{
				
				player2_Skill[i]->isEnable = true;
				player2_Skill[i]->isCrush = false;
				player2_Skill[i]->skillPos = skillPos;
				player2_Skill[i]->isSkillRight = isRight;
				player2_Skill[i]->player = player;
				SkillManager::m_Player2_SkillCount++;
				break;
			}
		}
	}

}


void SkillManager::update(Client_Info& client1,Client_Info &client2)
{

	if (SkillManager::m_Player1_SkillCount > 0)
	{
		for (int i = 0; i < MAXSKILL; i++)
		{
			if (player1_Skill[i]->isEnable == true)
			{
				checkCollision(client2.pos, player1_Skill[i],client2);
				if (player1_Skill[i]->isCrush == false)
				{
					if (player1_Skill[i]->isSkillRight == true) {
						player1_Skill[i]->skillPos.X += 10;
					}
					else {
						player1_Skill[i]->skillPos.X -= 10;
					}
				}
			}
		}


	}

	if (SkillManager::m_Player2_SkillCount > 0)
	{
		for (int i = 0; i < MAXSKILL; i++)
		{
			if (player2_Skill[i]->isEnable == true)
			{
				checkCollision(client1.pos, player2_Skill[i],client1);
				if (player2_Skill[i]->isCrush == false)
				{
					if (player2_Skill[i]->isSkillRight == true)
					{
						player2_Skill[i]->skillPos.X += 10;
					}
					else {
						player2_Skill[i]->skillPos.X -= 10;
					}
				}
			}
		}
	}

	
}



void SkillManager::checkCollision(COORD& player,SKILL_INFO* skill,Client_Info& client)
{
	float playerMinX = player.X;
	float playerMaxX = player.X + PLAYER_WIDTH;
	float playerMinY = player.Y;
	float playerMaxY = player.Y + PLAYER_HEIGHT;


		if (Collision::RRCollision(playerMinX, playerMinY, playerMaxX, playerMaxY, skill->skillPos.X, skill->skillPos.Y,
			skill->skillPos.X + FIREBALL_WIDTH, skill->skillPos.Y + FIREBALL_HEIGHT))
		{
			skill->isCrush = true;
			skill->skillPos.X = -100;
			skill->skillPos.Y = -100;
			skill->isEnable = false;
			if (skill->player == PLAYER_1)
			{
				if (SkillManager::m_Player1_SkillCount > 0) 
				{
					SkillManager::m_Player1_SkillCount--;
				}
			}
			else 
			{
				if (SkillManager::m_Player2_SkillCount > 0)
				{
					SkillManager::m_Player2_SkillCount--;
				}
			}
			if (client.hp > 0) {
				client.hp -= 10;
			}
		}

		if (skill->skillPos.X < 0.0f && skill->skillPos.X != -100)
		{
			skill->isCrush = true;
			skill->skillPos.X = -100;
			skill->skillPos.Y = -100;
			skill->isEnable = false;
			if (skill->player == PLAYER_1)
			{
				if (SkillManager::m_Player1_SkillCount > 0)
				{
					SkillManager::m_Player1_SkillCount--;
				}
			}
			else
			{
				if (SkillManager::m_Player2_SkillCount > 0)
				{
					SkillManager::m_Player2_SkillCount--;
				}
			}

			
		}

		if (skill->skillPos.X > 1000.0f)
		{
			skill->isCrush = true;
			skill->skillPos.X = -100;
			skill->skillPos.Y = -100;
			skill->isEnable = false;
			if (skill->player == PLAYER_1)
			{
				if (SkillManager::m_Player1_SkillCount > 0)
				{
					SkillManager::m_Player1_SkillCount--;
				}
			}
			else
			{
				if (SkillManager::m_Player2_SkillCount > 0)
				{
					SkillManager::m_Player2_SkillCount--;
				}
			}
		}
	

}