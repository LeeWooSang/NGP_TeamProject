#include "Game.h"
#include <vector>

#define MAXSKILL 10

byte gameState;
Hero * pHero;
Hero * eHero;
SC_INIT pSCInit;
CS_INIT pCSInit;
SC_RUN pSCRun;
SC_SKILL pSCSkill;
CS_SKILL pCSSkill;
SC_END pSCEnd;
CS_RUN pCSRun;
CRITICAL_SECTION cs;
SOCKET sock;
HANDLE hThread, hThread2;
int elapsedNum = 0;
byte createSkillIdx = 0;
std::vector<Fireball> pFireball;
std::vector<Fireball> eFireball;
bool initSent = false;

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
	
	//CreateThread(NULL, 0, ClientThread, NULL, 0, NULL);			//필요없을거 같아서 일단 주석 -명진
	//BulletIndex = pSprite->getIndex() + 1;

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
}


void Game::Update()
{
	
	hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)sock, 0, NULL);
	if (hThread == NULL)
		closesocket(sock);
	else
		CloseHandle(hThread);

	if (elapsedNum > 0)
	{
		elapsedNum = 0;
		hThread2 = CreateThread(NULL, 0, RecvThread, (LPVOID)sock, 0, NULL);
		if (hThread2 == NULL)
			closesocket(sock);
		else
			CloseHandle(hThread2);
	}
	elapsedNum++;
}

void Game::Destroy()
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
	DeleteCriticalSection(&cs);
	WSACleanup();
}

DWORD WINAPI Game::ClientThread(LPVOID sock)
{
	int retval = 0;
	SOCKET client_socket = (SOCKET)sock;
	bool sent = false;

	switch (gameState)
	{
	case TYPE_START:
		if (pCSInit.isReady && !sent)
		{
			retval = send((SOCKET)client_socket, (char*)&pCSInit, sizeof(CS_INIT), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send( )");
				return 0;
			}
			pCSRun.player = pSCInit.player;
			gameState = TYPE_RUN;
			sent = true;
		}
		break;
	case TYPE_RUN:
		EnterCriticalSection(&cs);
		if (pCSRun.key != KEY_IDLE)
		{
			retval = send((SOCKET)client_socket, (char*)&pCSRun, sizeof(CS_RUN), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send( )");
				return 0;
			}
			pCSRun.onSkill = false;
			// TYPE_RUN일때, 데이터를 보내고 키 값을 바로 IDLE로 바꿈
			// 키가 다운이 되고, 업이 될때까지 그 사이에 이상한 값이 들어가는 것을 막기위해
			pCSRun.key = KEY_IDLE;
		}
		LeaveCriticalSection(&cs);
		if (pCSRun.key == KEY_SPACE)
		{
			retval = send((SOCKET)client_socket, (char*)&pCSSkill, sizeof(CS_SKILL), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send( )");
				return 0;
			}
		}
		break;
	}
	return 0;
}

DWORD WINAPI Game::RecvThread(LPVOID sock)
{
	int retval = 0;
	SOCKET client_socket = (SOCKET)sock;
	int optval = 1;

	switch (gameState)
	{
	case TYPE_INIT:
		
		// 고정길이 : 패킷 받기
		optval = 1000;			//대기 시간 0.1초 -by 명진
		retval = setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&optval, sizeof(optval));
		// SO_RCVTIMEO 0.1초로 타임아웃 지정 ,0.1초안에 데이터가 도착하지 않으면 오류 리턴

		retval = recvn(client_socket, (char*)&pSCInit, sizeof(pSCInit), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recvn( )");
			return 0;
		}

		cout << (int)pSCInit.player << "\n";
		if (initSent == false)
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
		
		if (pSCInit.type == TYPE_INIT)
		{
			if (pSCInit.isStart)
			{
				initSent = true;
				cout << "상대방과 연결 성공!\n";
				
				pCSInit.player = pHero->player;
				//std::cout <<(int)pCSInit.player << std::endl;
				pCSInit.isReady = true;
				gameState = TYPE_START;
				break;
			}
			else
			{
				cout << "상대 기다리는 중!\n";
				break;
			}
		}
		
		break;
	case TYPE_RUN:
		int x, y;
		optval = INFINITE;		//대기 소켓으로 변경	-by 명진
		retval = setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&optval, sizeof(optval));
		if (retval == SOCKET_ERROR)
		{
			err_quit("setsockopt()");
		}

		retval = recvn((SOCKET)client_socket, (char*)&pSCRun, sizeof(SC_RUN), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		pHero->getLocation(&x, &y);
		
		/*if (pSCRun.onSkill)
		{
			retval = recvn((SOCKET)client_socket, (char*)&pSCSkill, sizeof(SC_SKILL), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}
			pSCRun.onSkill = false;
			EnterCriticalSection(&cs);
			if (pSCSkill.player == pHero->player)
			{
				pFireball[pSCSkill.skillIndex].setLocation(pSCSkill.skillPos.X, pSCSkill.skillPos.Y);
				if (pSCSkill.isCrush)
					pFireball[pSCSkill.skillIndex].isCrush = true;
			}
			else
			{
				eFireball[pSCSkill.skillIndex].setLocation(pSCSkill.skillPos.X, pSCSkill.skillPos.Y);
				if (pSCSkill.isCrush)
					eFireball[pSCSkill.skillIndex].isCrush = true;
			}
			LeaveCriticalSection(&cs);

		}*/
		if (sqrt(pow(pSCRun.pos[pHero->player].X - x, 2)) < 100 || sqrt(pow(pSCRun.pos[pHero->player].Y - y, 2)) < 100)
		{
			EnterCriticalSection(&cs);
			if (pHero->player == PLAYER1)
			{
				pHero->setLocation(pSCRun.pos[PLAYER1].X, pSCRun.pos[PLAYER1].Y);
				pHero->setHP(pSCRun.hp[PLAYER1]);
				eHero->setMode(pSCRun.eMode[pHero->player]);
				eHero->setLocation(pSCRun.pos[PLAYER2].X, pSCRun.pos[PLAYER2].Y);
				eHero->setHP(pSCRun.hp[PLAYER2]);
			}
			else
			{
				pHero->setLocation(pSCRun.pos[PLAYER2].X, pSCRun.pos[PLAYER2].Y);
				pHero->setHP(pSCRun.hp[PLAYER2]);
				eHero->setMode(pSCRun.eMode[pHero->player]);
				eHero->setLocation(pSCRun.pos[PLAYER1].X, pSCRun.pos[PLAYER1].Y);
				eHero->setHP(pSCRun.hp[PLAYER1]);
			}
			LeaveCriticalSection(&cs);
		}
		
		break;
	}
	
	return 0;
}

void Game::Render(HDC* cDC)
{
	background->Render(cDC, 0);
	if (elapsedNum>0)
	{
		if (pHero)
			pHero->Render(cDC);
		if (eHero)
			eHero->Render(cDC);
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
			pCSRun.key = KEY_RIGHT;
			pHero->setMode(WALK);
			pHero->isBack = false;
			break;
		case VK_LEFT:
			pCSRun.key = KEY_LEFT;
			pHero->setMode(WALK_B);
			pHero->isBack = true;
			break;
		case VK_UP:
			pCSRun.key = KEY_UP;
			if (pHero->isBack)
				pHero->setMode(JUMP_B);
			else if (!pHero->isBack)
				pHero->setMode(JUMP);
			cout << "UP key down\n";
			break;
		case VK_SPACE:
			pCSRun.key = KEY_SPACE;
			if (pHero->isBack)
				pHero->setMode(ATTACK_B);
			else if (!pHero->isBack)
				pHero->setMode(ATTACK);

			for (int i = 0; i < MAXSKILL; ++i)
			{
				if (!pFireball[i].isDraw)
				{
					pCSSkill.skillIndex = i;
					pHero->getLocation(&x, &y);
					pFireball[i].setLocation(x, y);
					pFireball[i].isDraw = true;
					if (pHero->isBack)
						pFireball[i].isRight = false;
					else
						pFireball[i].isRight = true;

					pCSRun.onSkill = true;
					break;
				}
			}
			
			cout << "SPACE key down\n";
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

				pCSRun.key = KEY_IDLE;
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
