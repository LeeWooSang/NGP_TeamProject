#include "Game.h"
#include <vector>

#define MAXSKILL 10

byte gameState;
Hero * pHero;
Hero * eHero;
SC_INIT pSCInit;
SC_RUN pSCRun;
SC_END pSCEnd;
CS_RUN pCSRun;
CRITICAL_SECTION cs;
SOCKET sock;
HANDLE hThread, hThread2;
int elapsedNum = 0;
byte createSkillIdx = 0;
std::vector<Fireball> pFireball;
std::vector<Fireball> eFireball;

Game::Game()
{
	background = NULL;
}
Game::~Game()
{
	if (pHero)
	{
		pHero->Destroy();
		SAFE_DELETE(pHero);
	}
	if (eHero)
	{
		eHero->Destroy();
		SAFE_DELETE(eHero);
	}
	SAFE_DELETE(background);
}

int Game::recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;

	char *ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		else if (received == 0)
		{
			break;
		}
		left -= received;
		ptr += received;
	}

	return (len - left);
}

void Game::Enter()
{
	InitializeCriticalSection(&cs);
	int retval = 0;
	int x = 0;
	int y = 0;

	//static FWMain* main = &(FWMain::getMain());
	if (background == NULL)
	{
		background = new Sprite;
		background->Entry(0, "image/start.bmp", 0, 0);
	}
	
	if (pHero == NULL)
	{
		pHero = new Hero(500, 550, IDLE);
		pHero->Enter();
	}
	if (eHero == NULL)
	{
		eHero = new Hero(500, 550, IDLE);
		eHero->Enter();
	}
	
	//CreateThread(NULL, 0, ClientThread, NULL, 0, NULL);			//�ʿ������ ���Ƽ� �ϴ� �ּ� -����
	//BulletIndex = pSprite->getIndex() + 1;

	// ��ų vector ������ƮǮ
	pFireball.reserve(MAXSKILL);
	eFireball.reserve(MAXSKILL);
	pHero->getLocation(&x, &y);
	for (int i = 0; i < MAXSKILL; ++i)
		pFireball.emplace_back(x, y, true);
	eHero->getLocation(&x, &y);
	for (int i = 0; i < MAXSKILL; ++i)
		eFireball.emplace_back(x, y, true);

	memset(&pSCInit, 0, sizeof(SC_INIT));
	memset(&pSCRun, 0, sizeof(SC_RUN));
	memset(&pCSRun, 0, sizeof(CS_RUN));
	gameState = TYPE_INIT;
	pSCInit.type = -1;
	//���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "���� �ʱ�ȭ ����\n";
	}
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit("socket( )");

	//connect
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = ntohs(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("connect( )");

	hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)sock, 0, NULL);
	hThread2 = CreateThread(NULL, 0, RecvThread, (LPVOID)sock, 0, NULL);
}


void Game::Update()
{
	elapsedNum++;
	if (elapsedNum > 333)
		elapsedNum = 0;
}

void Game::Destroy()
{
	if (hThread == NULL)
		closesocket(sock);
	else
		CloseHandle(hThread);

	if (hThread2 == NULL)
		closesocket(sock);
	else
		CloseHandle(hThread2);
	if (pHero)
	{
		pHero->Destroy();
		SAFE_DELETE(pHero);
	}
	if (eHero)
	{
		eHero->Destroy();
		SAFE_DELETE(eHero);
	}
	SAFE_DELETE(background);
	DeleteCriticalSection(&cs);
	WSACleanup();
}

DWORD WINAPI Game::ClientThread(LPVOID sock)
{
	int retval = 0;
	SOCKET client_socket = (SOCKET)sock;
	bool sent = false;

	while (true)
	{
		switch (gameState)
		{
		case TYPE_RUN:
			static int num = 0;
			EnterCriticalSection(&cs);
			if (pCSRun.key != KEY_IDLE)
			{
				if (num < 1)
				{
					retval = send((SOCKET)client_socket, (char*)&pCSRun, sizeof(CS_RUN), 0);
					if (retval == SOCKET_ERROR)
					{
						err_display("send( )");
						return 0;
					}
					++num;
				}
				else
				{
					num = 0;
					pCSRun.key = KEY_IDLE;
				}
				pCSRun.onSkill = false;
				// TYPE_RUN�϶�, �����͸� ������ Ű ���� �ٷ� IDLE�� �ٲ�
				// Ű�� �ٿ��� �ǰ�, ���� �ɶ����� �� ���̿� �̻��� ���� ���� ���� ��������

			}
			LeaveCriticalSection(&cs);
			/*	if (pCSRun.key == KEY_SPACE)
				{
					retval = send((SOCKET)client_socket, (char*)&pCSSkill, sizeof(CS_SKILL), 0);
					if (retval == SOCKET_ERROR)
					{
						err_display("send( )");
						return 0;
					}
				}*/
			break;
		}
	}
	return 0;
}

DWORD WINAPI Game::RecvThread(LPVOID sock)
{
	int retval = 0;
	SOCKET client_socket = (SOCKET)sock;
	int optval = 1;

	while (true)
	{
		switch (gameState)
		{
		case TYPE_INIT:

			// �������� : ��Ŷ �ޱ�
			//optval = 1000;			//��� �ð� 1�� -by ����
			//retval = setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&optval, sizeof(optval));
			// SO_RCVTIMEO 0.1�ʷ� Ÿ�Ӿƿ� ���� ,0.1�ʾȿ� �����Ͱ� �������� ������ ���� ����

			retval = recvn(client_socket, (char*)&pSCInit, sizeof(pSCInit), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recvn( )");
				return 0;
			}
			cout << "INIT ����\n";
			cout << (int)pSCInit.player << "\n";

			if (pSCInit.type == TYPE_INIT)
			{
				if (pSCInit.isStart)
				{
					EnterCriticalSection(&cs);
					cout << "����� ���� ����!\n";
					pHero->player = pSCInit.player;				//�÷��̾� �ڽ��� ������ �����ִ´�.

					if (pHero->player == PLAYER1)
						eHero->player = PLAYER2;
					else
						eHero->player = PLAYER1;
					for (int i = 0; i < MAXSKILL; ++i)
					{
						pFireball[i].player = pHero->player;
						eFireball[i].player = eHero->player;
					}

					pCSRun.player = pSCInit.player;
					//std::cout <<(int)pCSInit.player << std::endl;
					gameState = TYPE_RUN;
					LeaveCriticalSection(&cs);
					break;
				}
				else
				{
					cout << "��� ��ٸ��� ��!\n";
					break;
				}
			}

			break;
		case TYPE_RUN:
			int x, y;
			retval = recvn((SOCKET)client_socket, (char*)&pSCRun, sizeof(SC_RUN), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}
			pHero->getLocation(&x, &y);


			EnterCriticalSection(&cs);
			
			if (pHero->player == PLAYER1)
			{
				pHero->setLocation(pSCRun.pos[PLAYER1].X, pSCRun.pos[PLAYER1].Y);
				//cout << "�÷�0 : " << pSCRun.pos2[PLAYER1].X << ", " << pSCRun.pos2[PLAYER1].Y << std::endl;
				pHero->setHP(pSCRun.hp[PLAYER1]);
				//std::cout << pSCRun.hp[PLAYER1] << std::endl;
				eHero->setMode(pSCRun.eMode[pHero->player]);
				eHero->setLocation(pSCRun.pos[PLAYER2].X, pSCRun.pos[PLAYER2].Y);
				eHero->setHP(pSCRun.hp[PLAYER2]);

				
				for (int i = 0; i < MAXSKILL; ++i)
				{
					if (pFireball[i].isDraw)
						pFireball[i].isCrush = pSCRun.skillInfo.player1_skill[i].isCrush;
					if (pFireball[i].isDraw == false && pFireball[i].isCrush == false)
					{
						pFireball[i].isDraw = pSCRun.skillInfo.player1_skill[i].isEnable;
						pFireball[i].player = pSCRun.skillInfo.player1_skill[i].player;
						pFireball[i].isRight = pSCRun.skillInfo.player1_skill[i].isSkillRight;
					}
					if (pFireball[i].isCrush == false)
						pFireball[i].setLocation(pSCRun.skillInfo.player1_skill[i].skillPos.X, pSCRun.skillInfo.player1_skill[i].skillPos.Y);

					if (eFireball[i].isDraw)
						eFireball[i].isCrush = pSCRun.skillInfo.player2_skill[i].isCrush;
					if (eFireball[i].isDraw == false && eFireball[i].isCrush == false)
					{
						eFireball[i].player = pSCRun.skillInfo.player2_skill[i].player;
						eFireball[i].isDraw = pSCRun.skillInfo.player2_skill[i].isEnable;
						eFireball[i].isRight = pSCRun.skillInfo.player2_skill[i].isSkillRight;
					}
					if (eFireball[i].isCrush == false)
						eFireball[i].setLocation(pSCRun.skillInfo.player2_skill[i].skillPos.X, pSCRun.skillInfo.player2_skill[i].skillPos.Y);
				}
				

			}
			else
			{
				pHero->setLocation(pSCRun.pos[PLAYER2].X, pSCRun.pos[PLAYER2].Y);
				//cout << "�÷�0 : " << pSCRun.pos2[PLAYER2].X << ", " << pSCRun.pos2[PLAYER2].Y << std::endl;
				pHero->setHP(pSCRun.hp[PLAYER2]);
				//std::cout << pSCRun.hp[PLAYER2] << std::endl;
				eHero->setMode(pSCRun.eMode[pHero->player]);
				eHero->setLocation(pSCRun.pos[PLAYER1].X, pSCRun.pos[PLAYER1].Y);
				eHero->setHP(pSCRun.hp[PLAYER1]);

				for (int i = 0; i < MAXSKILL; ++i)
				{
					if (eFireball[i].isDraw)
						eFireball[i].isCrush = pSCRun.skillInfo.player1_skill[i].isCrush;
					if (eFireball[i].isDraw == false && eFireball[i].isCrush == false)
					{
						eFireball[i].player = pSCRun.skillInfo.player1_skill[i].player;
						eFireball[i].isDraw = pSCRun.skillInfo.player1_skill[i].isEnable;
						eFireball[i].isRight = pSCRun.skillInfo.player1_skill[i].isSkillRight;
					}
					if(eFireball[i].isCrush == false)
						eFireball[i].setLocation(pSCRun.skillInfo.player1_skill[i].skillPos.X, pSCRun.skillInfo.player1_skill[i].skillPos.Y);

					if (pFireball[i].isDraw)
						pFireball[i].isCrush = pSCRun.skillInfo.player2_skill[i].isCrush;
					if (pFireball[i].isDraw == false && pFireball[i].isCrush == false)
					{
						pFireball[i].player = pSCRun.skillInfo.player2_skill[i].player;
						pFireball[i].isDraw = pSCRun.skillInfo.player2_skill[i].isEnable;
						pFireball[i].isRight = pSCRun.skillInfo.player2_skill[i].isSkillRight;
					}
					if (pFireball[i].isCrush == false)
						pFireball[i].setLocation(pSCRun.skillInfo.player2_skill[i].skillPos.X, pSCRun.skillInfo.player2_skill[i].skillPos.Y);
				}
			}
			LeaveCriticalSection(&cs);

			if (pSCRun.hp[PLAYER1] == 0 || pSCRun.hp[PLAYER2] == 0)
			{

				gameState = TYPE_END;
				std::cout << "������ ���� ����" << std::endl;

			}
			break;
		case TYPE_END:

			retval = recvn((SOCKET)client_socket, (char*)&pSCEnd, sizeof(SC_END), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}

			if (pSCEnd.winner == pHero->player)
			{
				std::cout << "�̰��!!     ESC�� ���� ������ �ּ���" << std::endl;
				
			}
			else {
				std::cout << "����!!       ESC�� ���� ������ �ּ���" << std::endl;
			}
			

			break;
		}
	}
	
	return 0;
}

void Game::Render(HDC* cDC)
{
	background->Render(cDC, 0);
	if (pHero)
		pHero->Render(cDC, elapsedNum);
	if (eHero)
		eHero->Render(cDC, elapsedNum);

	if (pFireball.size() > 0 && eFireball.size() > 0)
	{
		for (int i = 0; i < MAXSKILL; ++i)
		{
			if (pFireball[i].isDraw)
				pFireball[i].Render(cDC);
			if (eFireball[i].isDraw)
				eFireball[i].Render(cDC);
		}
	}
	
}
void Game::MouseInput(int iMessage, int x, int y)
{
	switch (iMessage)
	{
	case WM_LBUTTONDOWN:
		printf("%d %d\n", x, y);
		break;
	}
}
void Game::KeyboardInput(int iMessage, int wParam)
{
	int x, y;
	if (iMessage == WM_KEYDOWN && gameState == TYPE_RUN)
	{
		switch (wParam)
		{
		case VK_RIGHT:
			EnterCriticalSection(&cs);
			pCSRun.key = KEY_RIGHT;
			LeaveCriticalSection(&cs);
			pHero->setMode(WALK);
			pHero->isBack = false;
			break;
		case VK_LEFT:
			EnterCriticalSection(&cs);
			pCSRun.key = KEY_LEFT;
			LeaveCriticalSection(&cs);
			pHero->setMode(WALK_B);
			pHero->isBack = true;
			break;
		case VK_UP:
			EnterCriticalSection(&cs);
			pCSRun.key = KEY_UP;
			LeaveCriticalSection(&cs);
			if (pHero->isBack)
				pHero->setMode(JUMP_B);
			else if (!pHero->isBack)
				pHero->setMode(JUMP);
			cout << "UP key down\n";
			break;
		case VK_SPACE:
			EnterCriticalSection(&cs);
			pCSRun.key = KEY_SPACE;
			LeaveCriticalSection(&cs);
			if (pHero->isBack)
				pHero->setMode(ATTACK_B);
			else if (!pHero->isBack)
				pHero->setMode(ATTACK);

			//for (int i = 0; i < MAXSKILL; ++i)
			//{
			//	if (!pFireball[i].isDraw)
			//	{
			//		pCSSkill.skillIndex = i;
			//		pHero->getLocation(&x, &y);
			//		pFireball[i].setLocation(x, y);
			//		pFireball[i].isDraw = true;
			//		if (pHero->isBack)
			//			pFireball[i].isRight = false;
			//		else
			//			pFireball[i].isRight = true;

			//		pCSRun.onSkill = true;
			//		break;
			//	}
			//}
			
			//cout << "SPACE key down\n";
			break;
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
	}
	if (iMessage == WM_KEYUP)
	{
		switch (wParam)
		{
		case VK_RIGHT:
		case VK_LEFT:
		case VK_UP:
		case VK_SPACE:
			if (gameState == TYPE_RUN)
			{
				if(pHero->isBack)
					pHero->setMode(IDLE_B);
				else
					pHero->setMode(IDLE);

				EnterCriticalSection(&cs);
				pCSRun.key = KEY_IDLE;
				LeaveCriticalSection(&cs);
				cout << "KEY UP\n";
			}
			break;
		}
	}
}
void Game::KeyboardCharInput(int wParam)
{
	switch (wParam)
	{
	case VK_SPACE:
		//printf("%d, %d\n", pHero->getX(), pHero->getY());
		break;
	case VK_ESCAPE:
		PostQuitMessage(0);
		break;
	}
}

void Game::err_quit(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void Game::err_display(const char *msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
