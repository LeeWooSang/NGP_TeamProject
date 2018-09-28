#pragma once
#pragma comment(lib, "ws2_32")
#pragma pack(1)
#include <iostream>
#include <string>
#include <fstream>
#include <WinSock2.h>
using namespace std;

struct SPacket
{
	string file_name;
	char* file_data;
};

class CFileTransfer_Console_Server
{
public:
	CFileTransfer_Console_Server();
	~CFileTransfer_Console_Server();

	void err_quit(const char*);
	void err_display(const char*);

	bool Socket_Init();

	int recvn(SOCKET, char*, int, int);
	void Packet_Recv(SOCKET&, SOCKADDR_IN&, int&);

	void Release();

private:
	const u_short ServerPort{ 9000 };
	const u_short BufferSize{ 5000 };

	// 대기 소켓
	SOCKET listen_socket;

	int data{ 0 };
	SPacket packet;
};

