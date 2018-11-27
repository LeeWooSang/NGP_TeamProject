#include "SkillManager.h"



SkillManager::SkillManager()
{

}

SkillManager::~SkillManager()
{}


void SkillManager::addSkill(byte& player,COORD& skillPos)
{
	
	SC_SKILL fireballInfo;
	fireballInfo.type = TYPE_SKILL;
	fireballInfo.player = player;
	if (player == PLAYER_1)
	{
		fireballInfo.isRight = true;
	}
	else {
		fireballInfo.isRight = false;
	}
	fireballInfo.skillPos = skillPos;
	fireballInfo.isCrush = false;
	fireballInfo.skillIndex = skillVector.size();

	//cout << "���� ��ų �߻� ����" <<skillVector.size()<< endl;
	skillVector.emplace_back(fireballInfo);


}



void SkillManager::update(float fTimeElapsed,COORD& player1,COORD& player2)
{
	if (skillVector.size() >= 1) {
		checkCollision(player1, player2); //������Ʈ ���� �浹�˻縦 �̸� �ǽ�
		for (iter = skillVector.begin(); iter != skillVector.end(); )
		{

			if (iter->isCrush == true)
			{
				iter = skillVector.erase(iter);		//�� �κп��� ���� ����
												//������ ���Ҹ� ����� ������
				cout << "���� " <<(int)iter->skillIndex<<"��° �浹 , �� ��ų ����"<< skillVector.size() << endl;
			}

			if (iter->isCrush == false)
			{
				//���̾ ��ġ�� ����
				if (iter->isRight == true) {
					iter->skillPos.X += 10;
				}
				else {
					iter->skillPos.X -= 10;
				}
				cout << "��ų [" << iter->skillIndex << "] ��ġ" << iter->skillPos.X << endl;
				++iter;
			}

		}
	}
	
}
void SkillManager::checkCollision(COORD& player1,COORD& player2)
{
	float player1MinX = player1.X;
	float player1MaxX = player1.X + PLAYER_WIDTH;
	float player1MinY = player1.Y;
	float player1MaxY = player1.Y + PLAYER_HEIGHT;



	for (iter = skillVector.begin(); iter != skillVector.end(); ++iter)
	{

		if (Collision::RRCollision(player1MinX, player1MinY, player1MaxX, player1MaxY, iter->skillPos.X, iter->skillPos.Y,
			iter->skillPos.X + FIREBALL_WIDTH, iter->skillPos.Y + FIREBALL_HEIGHT)) 
		{
			//�浹�Ұ�� isCrush�� true�� ������
			iter->isCrush = true;
			cout << "�浹" << endl;
			
		}
	}
}