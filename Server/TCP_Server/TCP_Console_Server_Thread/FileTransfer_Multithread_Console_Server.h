#pragma once
#pragma comment(lib, "ws2_32")
#pragma pack(1)
#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include <WinSock2.h>
#include <vector>
using namespace std;

struct SPacket
{
	string file_name;
	char* file_data;
};

class CFileTransfer_Multithread_Console_Server
{
public:
	CFileTransfer_Multithread_Console_Server();
	CFileTransfer_Multithread_Console_Server(SOCKET, SOCKADDR_IN, int);
	~CFileTransfer_Multithread_Console_Server();

	void err_quit(const char*);
	void err_display(const char*);

	bool Socket_Init();

	// 클래스의 scope 때문에 static 함수로 정의한다.
	static DWORD WINAPI ProcessClient(LPVOID arg);
	static void Thread_ProcessClient(SOCKET, SOCKADDR_IN, int);
	void Thread_ProcessClient(vector<CFileTransfer_Multithread_Console_Server>&, int);

	int recvn(SOCKET, char*, int, int);
	void Packet_Recv(SOCKET&, SOCKADDR_IN&, int&);

	void Release();

private:
	const u_short ServerPort{ 9000 };
	const u_short BufferSize{ 5000 };

	// 대기 소켓
	SOCKET			m_listen_socket;

	int data{ 0 };
	SPacket			m_packet;

	SOCKET			m_client_socket;
	SOCKADDR_IN	m_client_addr;
	int						m_addrlen;

	#define Max_Client_Count 10

	thread* thread_client[Max_Client_Count]{ nullptr };

	vector<CFileTransfer_Multithread_Console_Server> vec_client;
};

