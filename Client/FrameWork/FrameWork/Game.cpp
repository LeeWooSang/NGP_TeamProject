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
Sprite * winTEXT;
Sprite * loseTEXT;
bool isWin;
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

	if (pFireball.size() > 0)
	{
		for (int i = 0; i < MAXSKILL; ++i)
			pFireball[i].Destroy();
	}
	if (eFireball.size() > 0)
	{
		for (int i = 0; i < MAXSKILL; ++i)
			eFireball[i].Destroy();
	}
	SAFE_DELETE(background);
	SAFE_DELETE(winTEXT);
	SAFE_DELETE(loseTEXT);
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

	if (winTEXT == NULL)
	{
		winTEXT = new Sprite;
		winTEXT->Entry(0, "image/winText.bmp", 0, 0);
		winTEXT->setLocation(150, 200);
	}

	if (loseTEXT == NULL)
	{
		loseTEXT = new Sprite;
		loseTEXT->Entry(0, "image/loseText.bmp", 0, 0);
		loseTEXT->setLocation(150, 200);
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

	// 스킬 vector 오브젝트풀
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
	memset(&pSCEnd, 0, sizeof(SC_END));
	memset(&pCSRun, 0, sizeof(CS_RUN));
	gameState = TYPE_INIT;

	//윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "윈속 초기화 실패\n";
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
	if (pFireball.size() > 0)
	{
		for (int i = 0; i < MAXSKILL; ++i)
			pFireball[i].Destroy();
	}
	if (eFireball.size() > 0)
	{
		for (int i = 0; i < MAXSKILL; ++i)
			eFireball[i].Destroy();
	}
	SAFE_DELETE(background);
	SAFE_DELETE(winTEXT);
	SAFE_DELETE(loseTEXT);
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
				// TYPE_RUN일때, 데이터를 보내고 키 값을 바로 IDLE로 바꿈
				// 키가 다운이 되고, 업이 될때까지 그 사이에 이상한 값이 들어가는 것을 막기위해

			}
			LeaveCriticalSection(&cs);
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
			retval = recvn(client_socket, (char*)&pSCInit, sizeof(pSCInit), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recvn( )");
				return 0;
			}
			cout << "INIT 받음\n";
			cout << (int)pSCInit.player << "\n";

			if (pSCInit.isStart)
			{
				EnterCriticalSection(&cs);
				cout << "상대방과 연결 성공!\n";
				pHero->player = pSCInit.player;				//플레이어 자신의 정보를 갖고있는다.

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
				gameState = TYPE_RUN;
				LeaveCriticalSection(&cs);
				break;
			}
			else
			{
				cout << "상대 기다리는 중!\n";
				break;
			}

			break;
		case TYPE_RUN:
			retval = recvn((SOCKET)client_socket, (char*)&pSCRun, sizeof(SC_RUN), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}

			EnterCriticalSection(&cs);
			if (pHero->player == PLAYER1)
			{
				pHero->setLocation(pSCRun.pos[PLAYER1].X, pSCRun.pos[PLAYER1].Y);
				//cout << "플레0 : " << pSCRun.pos2[PLAYER1].X << ", " << pSCRun.pos2[PLAYER1].Y << std::endl;
				pHero->setHP(pSCRun.hp[PLAYER1]);
				eHero->setMode(pSCRun.eMode[pHero->player]);
				eHero->setLocation(pSCRun.pos[PLAYER2].X, pSCRun.pos[PLAYER2].Y);
				eHero->setHP(pSCRun.hp[PLAYER2]);

				for (int i = 0; i < MAXSKILL; ++i)
				{
					// isCrush를 스킬이 적중할 때 까지 체크
					if (pFireball[i].isDraw)
						pFireball[i].isCrush = pSCRun.skillInfo.player1_skill[i].isCrush;
					// 스킬 발사 직후의 초기 정보를 한 번만 받도록 하기 위함
					if (pFireball[i].isDraw == false && pFireball[i].isCrush == false)
					{
						pFireball[i].isDraw = pSCRun.skillInfo.player1_skill[i].isEnable;
						pFireball[i].player = pSCRun.skillInfo.player1_skill[i].player;
						pFireball[i].isRight = pSCRun.skillInfo.player1_skill[i].isSkillRight;
					}
					// skill 적중 후 isCrush에 대한 처리를 클라에서 하기 위함
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
				//cout << "플레0 : " << pSCRun.pos2[PLAYER2].X << ", " << pSCRun.pos2[PLAYER2].Y << std::endl;
				pHero->setHP(pSCRun.hp[PLAYER2]);
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
				std::cout << "끝내기 과정 돌입" << std::endl;
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
				eHero->setMode(DEATH);
				isWin = true;
			}
			else {
				pHero->setMode(DEATH);
				isWin = false;
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

	if (gameState == TYPE_END)
	{
		if (isWin)
			winTEXT->Render(cDC, 0, (UINT)RGB(255, 0, 255));
		else
			loseTEXT->Render(cDC, 0, (UINT)RGB(255, 0, 255));
	}
}
void Game::KeyboardInput(int iMessage, int wParam)
{
	EnterCriticalSection(&cs);
	if (iMessage == WM_KEYDOWN && gameState == TYPE_RUN)
	{
		switch (wParam)
		{
		case VK_RIGHT:
			pCSRun.key = KEY_RIGHT;
			pHero->setMode(WALK);
			pHero->isRight = true;
			break;
		case VK_LEFT:
			pCSRun.key = KEY_LEFT;
			pHero->setMode(WALK_B);
			pHero->isRight = false;
			break;
		case VK_UP:
			pCSRun.key = KEY_UP;
			if (!pHero->isRight)
				pHero->setMode(JUMP_B);
			else if (pHero->isRight)
				pHero->setMode(JUMP);
			break;
		case VK_SPACE:
			pCSRun.key = KEY_SPACE;
			if (!pHero->isRight)
				pHero->setMode(ATTACK_B);
			else if (pHero->isRight)
				pHero->setMode(ATTACK);
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
				pCSRun.key = KEY_IDLE;
				if(!pHero->isRight)
					pHero->setMode(IDLE_B);
				else
					pHero->setMode(IDLE);
			}
			break;
		}
	}
	LeaveCriticalSection(&cs);
}
void Game::KeyboardCharInput(int wParam)
{
	switch (wParam)
	{
	case VK_ESCAPE:
		Destroy();
		PostQuitMessage(0);
		break;
	}
}

void Game::MouseInput(int iMessage, int x, int y)
{
	switch (iMessage)
	{
	case WM_LBUTTONDOWN:
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
