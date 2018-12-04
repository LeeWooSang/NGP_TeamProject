#include "SkillManager.h"

int SkillManager::m_Player1_SkillCount = 0;
int SkillManager::m_Player2_SkillCount = 0;

SkillManager::SkillManager()
{
	

	
	player1 = new SKILL();
	player2 = new SKILL();


	//INITALIZE
	for (int i = 0; i < MAXSKILL; i++)
	{
		player1->player1_skill[i].isEnable = false;
		player1->player1_skill[i].isCrush = false;
		player1->player1_skill[i].isRight = true;
		player1->player1_skill[i].player = PLAYER_1;
		player1->player1_skill[i].skillPos = { -100,-100 };
		player1->player1_skillIndex = i;


		player2->player2_skill[i].isEnable = false;
		player2->player2_skill[i].isCrush = false;
		player2->player2_skill[i].isRight = false;
		player2->player2_skill[i].player = PLAYER_2;
		player2->player2_skill[i].skillPos = { -100 , -100 };
		player2->player2_skillIndex = i;
		/*player1_SkillArray->isEnable = false;
		player1_SkillArray->isCrush = false;
		player1_SkillArray->isRight = true;
		player1_SkillArray->player1_skillIndex = i;
		player1_SkillArray->player = PLAYER_1;
		player1_SkillArray->player1_skillPos[i] = { -100,-100 };
		

		player2_SkillArray->isEnable = false;
		player2_SkillArray->isCrush = false;
		player2_SkillArray->isRight = false;
		player2_SkillArray->player2_skillIndex = i;
		player2_SkillArray->player = PLAYER_2;
		player2_SkillArray->player2_skillPos[i] = { -100,-100 };*/


	}
}

SkillManager::~SkillManager()
{
	if (player1) {
		player1 = nullptr;
		delete player1;
	}
	if (player2) {
		
		player2 = nullptr;
		delete player2;
	}

}


void SkillManager::addSkill(byte& player,COORD& skillPos)
{
	
	int index = 0;
	if (player == PLAYER_1)
	{	
		for (int i = 0; i < MAXSKILL; i++)
		{
			if (player1->player1_skill[i].isEnable==false)
			{
				index = i;
				break;
			}
			else {
				if (index == MAXSKILL - 1)
				{
					index = MAXSKILL - 1;
				}
				continue;
			}
		}
		

		player1->player1_skill[index].isEnable = true;
		player1->player1_skill[index].isCrush = false;
		player1->player1_skill[index].skillPos = skillPos;
		player1->player1_skill[index].isRight = true;
		player1->player1_skill[index].player = player;
		player1->player1_skillIndex = index;
		SkillManager::m_Player1_SkillCount++;

	}
	else if (player == PLAYER_2)
	{
		for (int i = 0; i < MAXSKILL; i++)
		{
			if (player1->player1_skill[i].isEnable == false)
			{
				index = i;
				break;
			}
			else {
				if (index == MAXSKILL - 1)
				{
					index = MAXSKILL - 1;
				}
				continue;
			}
		}
		player2->player2_skill[index].isEnable = true;
		player2->player2_skill[index].isCrush = false;
		player2->player2_skill[index].skillPos = skillPos;
		player2->player2_skill[index].isRight = false;
		player2->player2_skill[index].player = player;
		player2->player2_skillIndex = index;
		SkillManager::m_Player2_SkillCount++;
	}

}



void SkillManager::update(float fTimeElapsed,COORD& Player1,COORD& Player2)
{
	//if (skillVector.size() >= 1) {
	//	checkCollision(player1, player2); //업데이트 전에 충돌검사를 미리 실시
	//	for (auto iter = skillVector.begin(); iter != skillVector.end(); )
	//	{

	//		if (iter->isCrush == true)
	//		{
	//			iter = skillVector.erase(iter);		//이 부분에서 현재 에러
	//											//벡터의 원소를 지우면 에러남
	//			cout << "현재 " <<(int)iter->skillIndex<<"번째 충돌 , 총 스킬 개수"<< skillVector.size() << endl;
	//		}

	//		if (iter->isCrush == false)
	//		{
	//			//파이어볼 위치값 갱신
	//			if (iter->isRight == true) {
	//				iter->skillPos.X += 10;
	//			}
	//			else {
	//				iter->skillPos.X -= 10;
	//			}
	//			cout << "스킬 [" << iter->skillIndex << "] 위치" << iter->skillPos.X << endl;
	//			++iter;
	//		}

	//	}
	//}


	//////////////////////////////////////////////////////////////////////////////////

	if (SkillManager::m_Player1_SkillCount > 0)
	{
				

		for (int i = 0; i < MAXSKILL; i++)
		{
			
			if (player1->player1_skill[i].isEnable == true)
			{
				checkCollision(Player2, player1->player1_skill[i]);
				if (player1->player1_skill[i].isCrush == false)
				{
					player1->player1_skill[i].skillPos.X += 10;
					cout << "플레이어: " << (int)player1->player1_skill[i].player << "->" << player1->player1_skill[i].skillPos.X << "," << player1->player1_skill[i].skillPos.Y << std::endl;
				}

				//if(player1->player1_skill[i].skillPos)
			}
		}


	}

	if (SkillManager::m_Player2_SkillCount > 0)
	{
		for (int i = 0; i < MAXSKILL; i++)
		{
			if (player2->player2_skill[i].isEnable == true)
			{
				checkCollision(Player1, player2->player2_skill[i]);
				if (player2->player2_skill[i].isCrush == false)
				{
					player2->player2_skill[i].skillPos.X -= 10;
					cout << "플레이어: " << (int)player2->player2_skill[i].player << "->" << player2->player2_skill[i].skillPos.X << "," << player2->player2_skill[i].skillPos.Y << std::endl;
				}
			}
		}
	}

	
}
void SkillManager::checkCollision(COORD& player,SKILL_INFO& skill)
{
	float playerMinX = player.X;
	float playerMaxX = player.X + PLAYER_WIDTH;
	float playerMinY = player.Y;
	float playerMaxY = player.Y + PLAYER_HEIGHT;

	
	if (Collision::RRCollision(playerMinX, playerMinY, playerMaxX, playerMaxY, skill.skillPos.X, skill.skillPos.Y,
		skill.skillPos.X + FIREBALL_WIDTH, skill.skillPos.Y + FIREBALL_HEIGHT))
	{
		skill.isCrush = true;
		skill.skillPos.X = -100;
		skill.skillPos.Y = -100;
		skill.isEnable = false;
		if (skill.player == PLAYER_1)
		{
			if (SkillManager::m_Player1_SkillCount > 0) {
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

		cout << "충돌" << endl;
	}
	


	//std::cout << skill->skillPos.X << "," << skill->skillPos.Y << std::endl;

	//for (auto iter = skillVector.begin(); iter != skillVector.end(); ++iter)
	//{

	//	if (Collision::RRCollision(playerMinX, playerMinY, playerMaxX, playerMaxY, iter->skillPos.X, iter->skillPos.Y,
	//		iter->skillPos.X + FIREBALL_WIDTH, iter->skillPos.Y + FIREBALL_HEIGHT)) 
	//	{
	//		//충돌할경우 isCrush를 true로 만들어라
	//		iter->isCrush = true;
	//		cout << "충돌" << endl;
	//		
	//	}
	//}
}