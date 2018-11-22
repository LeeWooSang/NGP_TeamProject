#include "Game.h"
byte gameState;
Hero * pHero;
Hero * eHero;
SC_INIT pSCInit;
CS_INIT pCSInit;
SC_RUN pSCRun;
SC_SKILL pSCSkill;
SC_END pSCEnd;
CS_RUN pCSRun;
CRITICAL_SECTION cs;
SOCKET sock;
HANDLE hThread, hThread2;

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
	//static FWMain* main = &(FWMain::getMain());
	if (background == NULL)
	{
		background = new Sprite;
		background->Entry(0, "image/start.bmp", 0, 0);
		//background->setLocation(0, background->getWidth(0)/2, background->getHeight(0)/2);
	}
	if (pHero == NULL)
	{
		pHero = new Hero(50, 50, IDLE, 1);
		pHero->Enter();
	}
	if (eHero == NULL)
	{
		eHero = new Hero(250, 50, IDLE, 2);
		eHero->Enter();
	}
	CreateThread(NULL, 0, ClientThread, NULL, 0, NULL);
	//BulletIndex = pSprite->getIndex() + 1;

	memset(&pSCInit, 0, sizeof(SC_INIT));
	memset(&pSCRun, 0, sizeof(SC_RUN));
	memset(&pCSRun, 0, sizeof(CS_RUN));
	pHero->player = -1;
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

	hThread2 = CreateThread(NULL, 0, RecvThread, (LPVOID)sock, 0, NULL);
	if (hThread2 == NULL)
		closesocket(sock);
	else
		CloseHandle(hThread2);
	/*if (pHero)
	pHero->setLocation(pSCRun.pos[0].X, pSCRun.pos[0].Y);
	if (eHero)
	eHero->setLocation(pSCRun.pos[1].X, pSCRun.pos[1].Y);*/

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
		if (pCSInit.isReady || !sent)
		{
			retval = send((SOCKET)client_socket, (char*)&pCSInit, sizeof(CS_INIT), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send( )");
				return 0;
			}
			EnterCriticalSection(&cs);
			cout << "player" << (int)pCSInit.player << " is Ready\n";
			LeaveCriticalSection(&cs);
			pCSRun.player = pSCInit.player;
			gameState = TYPE_RUN;
		}
		break;
	case TYPE_RUN:
		if (pCSRun.key != KEY_IDLE)
		{
			retval = send((SOCKET)client_socket, (char*)&pCSRun, sizeof(CS_RUN), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send( )");
				return 0;
			}
		}
		break;
	}
}

DWORD WINAPI Game::RecvThread(LPVOID sock)
{
	int retval = 0;
	SOCKET client_socket = (SOCKET)sock;
	
	switch (gameState)
	{
	case TYPE_INIT:
		// 고정길이 : 패킷 받기
		retval = recvn(client_socket, (char*)&pSCInit, sizeof(pSCInit), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recvn( )");
			return 0;
		}
		//cout << "isStart : " << pSCInit.isStart << "\nplayer : " << (int)pSCInit.player << "\ntype : " << (int)pSCInit.type << "\n";
		pHero->player = pSCInit.player;				//플레이어 자신의 정보를 갖고있는다. 
		if (pHero->player == PLAYER1)
			eHero->player = PLAYER2;
		else
			eHero->player = PLAYER1;
		if (pSCInit.type == TYPE_INIT)
		{
			if (pSCInit.isStart)
			{
				cout << "상대방과 연결 성공!\n";

				pCSInit.player = pHero->player;
				pCSInit.type = 0;
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
		retval = recvn((SOCKET)client_socket, (char*)&pSCRun, sizeof(SC_RUN), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}

		EnterCriticalSection(&cs);
		cout << "PLAYER1 : " << pSCRun.pos[PLAYER1].X << ", " << pSCRun.pos[PLAYER1].Y << std::endl;
		cout << "PLAYER2 : " << pSCRun.pos[PLAYER2].X << ", " << pSCRun.pos[PLAYER2].Y << std::endl;
		if (pHero->player == PLAYER1)
		{
			pHero->setLocation(pSCRun.pos[PLAYER1].X, pSCRun.pos[PLAYER1].Y);
			pHero->setHP(pSCRun.hp[PLAYER1]);
			eHero->setLocation(pSCRun.pos[PLAYER2].X, pSCRun.pos[PLAYER2].Y);
			eHero->setHP(pSCRun.hp[PLAYER2]);
		}
		else
		{
			pHero->setLocation(pSCRun.pos[PLAYER2].X, pSCRun.pos[PLAYER2].Y);
			pHero->setHP(pSCRun.hp[PLAYER2]);
			eHero->setLocation(pSCRun.pos[PLAYER1].X, pSCRun.pos[PLAYER1].Y);
			eHero->setHP(pSCRun.hp[PLAYER1]);
		}
		LeaveCriticalSection(&cs);
		break;
	}
	
}

void Game::Render(HDC* cDC)
{
	background->Render(cDC, 0);

	if (pHero)
		pHero->Render(cDC);
	if (eHero)
		eHero->Render(cDC);
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
	//pCSRun.type = TYPE_RUN;

	if (iMessage == WM_KEYDOWN && pCSRun.type == TYPE_RUN)
	{
		switch (wParam)
		{
		case VK_RIGHT:
			pCSRun.key = KEY_RIGHT;
			break;
		case VK_LEFT:
			pCSRun.key = KEY_LEFT;
			break;
		case VK_UP:
			pCSRun.key = KEY_UP;
			break;
		case VK_SPACE:
			pCSRun.key = KEY_SPACE;
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
			if (pCSRun.type == TYPE_RUN || pCSRun.type == TYPE_SKILL)
				pCSRun.key = KEY_IDLE;
			break;
		}
	}
}
void Game::KeyboardCharInput(int wParam)
{
	switch (wParam)
	{
	case VK_SPACE:
		printf("%d, %d\n", pHero->getX(), pHero->getY());
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
