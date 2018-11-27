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

	//cout << "현재 스킬 발생 개수" <<skillVector.size()<< endl;
	skillVector.emplace_back(fireballInfo);


}



void SkillManager::update(float fTimeElapsed,COORD& player1,COORD& player2)
{
	if (skillVector.size() >= 1) {
		checkCollision(player1, player2); //업데이트 전에 충돌검사를 미리 실시
		for (iter = skillVector.begin(); iter != skillVector.end(); )
		{

			if (iter->isCrush == true)
			{
				iter = skillVector.erase(iter);		//이 부분에서 현재 에러
												//벡터의 원소를 지우면 에러남
				cout << "현재 " <<(int)iter->skillIndex<<"번째 충돌 , 총 스킬 개수"<< skillVector.size() << endl;
			}

			if (iter->isCrush == false)
			{
				//파이어볼 위치값 갱신
				if (iter->isRight == true) {
					iter->skillPos.X += 10;
				}
				else {
					iter->skillPos.X -= 10;
				}
				cout << "스킬 [" << iter->skillIndex << "] 위치" << iter->skillPos.X << endl;
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
			//충돌할경우 isCrush를 true로 만들어라
			iter->isCrush = true;
			cout << "충돌" << endl;
			
		}
	}
}