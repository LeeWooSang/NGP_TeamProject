#include "SkillManager.h"


int SkillManager::m_Player1_SkillCount = 0;
int SkillManager::m_Player2_SkillCount = 0;

SkillManager::SkillManager()
{
	player1_SkillArray = new SC_SKILL*[MAXSKILL];
	player2_SkillArray = new SC_SKILL*[MAXSKILL];

	for (int i = 0; i < MAXSKILL; i++)
	{
		player1_SkillArray[i] = new SC_SKILL();
		player2_SkillArray[i] = new SC_SKILL();
	}

	//INITALIZE
	for (int i = 0; i < MAXSKILL; i++)
	{
		player1_SkillArray[i]->isEnable = false;
		player1_SkillArray[i]->isCrush = false;
		player1_SkillArray[i]->isRight = true;
		player1_SkillArray[i]->skillIndex = i;
		player1_SkillArray[i]->player = PLAYER_1;
		player1_SkillArray[i]->skillPos = { -100,-100 };
		

		player2_SkillArray[i]->isEnable = false;
		player2_SkillArray[i]->isCrush = false;
		player2_SkillArray[i]->isRight = false;
		player2_SkillArray[i]->skillIndex = i;
		player2_SkillArray[i]->player = PLAYER_2;
		player2_SkillArray[i]->skillPos = { -100,-100 };

	}
}

SkillManager::~SkillManager()
{
	if (player1_SkillArray) {
		for (int i = 0; i < MAXSKILL; i++) {
			delete player1_SkillArray[i];
		}
		delete[] player1_SkillArray;
	}
	if (player2_SkillArray) {
		for (int i = 0; i < MAXSKILL; i++) {
			delete player2_SkillArray[i];
		}
		delete[] player2_SkillArray;
	}

}


void SkillManager::addSkill(byte& player,COORD& skillPos)
{
	
	//SC_SKILL fireballInfo;
	////fireballInfo.type = TYPE_SKILL;
	//fireballInfo.player = player;
	//if (player == PLAYER_1)
	//{
	//	fireballInfo.isRight = true;
	//	fireballInfo.skillIndex = skillVector.size();
	//}
	//else {
	//	fireballInfo.isRight = false;
	//}
	//fireballInfo.skillPos = skillPos;
	//fireballInfo.isCrush = false;
	//

	////cout << "���� ��ų �߻� ����" <<skillVector.size()<< endl;
	//skillVector.emplace_back(fireballInfo);
	int index = 0;
	if (player == PLAYER_1)
	{	
		for (int i = 0; i < MAXSKILL; i++)
		{
			if (player1_SkillArray[i]->isEnable == false)
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
		

		player1_SkillArray[index]->isEnable = true;
		player1_SkillArray[index]->isCrush = false;
		player1_SkillArray[index]->skillPos = skillPos;
		player1_SkillArray[index]->isRight = true;
		player1_SkillArray[index]->skillIndex = index;
		player1_SkillArray[index]->player = player;
		SkillManager::m_Player1_SkillCount++;

	}
	else if (player == PLAYER_2)
	{
		for (int i = 0; i < MAXSKILL; i++)
		{
			if (player2_SkillArray[i]->isEnable == false)
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


		player2_SkillArray[index]->isEnable = true;
		player2_SkillArray[index]->isCrush = false;
		player2_SkillArray[index]->skillPos = skillPos;
		player2_SkillArray[index]->isRight = false;
		player2_SkillArray[index]->skillIndex = index;
		player2_SkillArray[index]->player = player;
		SkillManager::m_Player2_SkillCount++;
	}

}



void SkillManager::update(float fTimeElapsed,COORD& player1,COORD& player2)
{
	//if (skillVector.size() >= 1) {
	//	checkCollision(player1, player2); //������Ʈ ���� �浹�˻縦 �̸� �ǽ�
	//	for (auto iter = skillVector.begin(); iter != skillVector.end(); )
	//	{

	//		if (iter->isCrush == true)
	//		{
	//			iter = skillVector.erase(iter);		//�� �κп��� ���� ����
	//											//������ ���Ҹ� ����� ������
	//			cout << "���� " <<(int)iter->skillIndex<<"��° �浹 , �� ��ų ����"<< skillVector.size() << endl;
	//		}

	//		if (iter->isCrush == false)
	//		{
	//			//���̾ ��ġ�� ����
	//			if (iter->isRight == true) {
	//				iter->skillPos.X += 10;
	//			}
	//			else {
	//				iter->skillPos.X -= 10;
	//			}
	//			cout << "��ų [" << iter->skillIndex << "] ��ġ" << iter->skillPos.X << endl;
	//			++iter;
	//		}

	//	}
	//}
	if (SkillManager::m_Player1_SkillCount > 0)
	{
		

		for (int i = 0; i < MAXSKILL; i++)
		{
			checkCollision(player2,player1_SkillArray[i]);
			if (player1_SkillArray[i]->isEnable == true)
			{
				if (player1_SkillArray[i]->isCrush == false)
				{
					player1_SkillArray[i]->skillPos.X += 10;
					cout << "�÷��̾�: " << (int)player1_SkillArray[i]->player << "->" << player1_SkillArray[i]->skillPos.X << "," << player1_SkillArray[i]->skillPos.Y << std::endl;
				}
			}
		}


	}

	if (SkillManager::m_Player2_SkillCount > 0)
	{
		for (int i = 0; i < MAXSKILL; i++)
		{
			checkCollision(player1, player2_SkillArray[i]);
			if (player2_SkillArray[i]->isEnable == true)
			{
				if (player2_SkillArray[i]->isCrush == false)
				{
					player2_SkillArray[i]->skillPos.X -= 10;
					cout << "�÷��̾�: " << (int)player2_SkillArray[i]->player << "->" << player2_SkillArray[i]->skillPos.X << "," << player2_SkillArray[i]->skillPos.Y << std::endl;
				}
			}
		}
	}

	
}
void SkillManager::checkCollision(COORD& player,SC_SKILL* skill)
{
	float playerMinX = player.X;
	float playerMaxX = player.X + PLAYER_WIDTH;
	float playerMinY = player.Y;
	float playerMaxY = player.Y + PLAYER_HEIGHT;

	for (int i = 0; i < MAXSKILL; i++)
	{
		if (Collision::RRCollision(playerMinX, playerMinY, playerMaxX, playerMaxY, skill->skillPos.X, skill->skillPos.Y,
			skill->skillPos.X + FIREBALL_WIDTH, skill->skillPos.Y + FIREBALL_HEIGHT))
		{
			skill->isCrush = true;
			skill->skillPos.X = -100;
			skill->skillPos.Y = -100;
			skill->isEnable = false;
			if (skill->player == PLAYER_1)
			{
				if (SkillManager::m_Player1_SkillCount > 0) {
					SkillManager::m_Player1_SkillCount--;
				}
			}
			else {
				if (SkillManager::m_Player2_SkillCount > 0)
				{
					SkillManager::m_Player2_SkillCount--;
				}
			}

			cout << "�浹" << endl;
		}
	}
	//std::cout << skill->skillPos.X << "," << skill->skillPos.Y << std::endl;

	//for (auto iter = skillVector.begin(); iter != skillVector.end(); ++iter)
	//{

	//	if (Collision::RRCollision(playerMinX, playerMinY, playerMaxX, playerMaxY, iter->skillPos.X, iter->skillPos.Y,
	//		iter->skillPos.X + FIREBALL_WIDTH, iter->skillPos.Y + FIREBALL_HEIGHT)) 
	//	{
	//		//�浹�Ұ�� isCrush�� true�� ������
	//		iter->isCrush = true;
	//		cout << "�浹" << endl;
	//		
	//	}
	//}
}