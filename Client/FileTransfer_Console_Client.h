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

class CFileTransfer_Console_Client
{
public:
	CFileTransfer_Console_Client();
	~CFileTransfer_Console_Client();

	void err_quit(const char*);
	void err_display(const char*);

	bool Socket_Init();

	void File_Read();
	void Packet_Send(ifstream&);

	void Release();

private:
	const char* ServerIP{ "127.0.0.1" };
	//const char* ServerIP{ "192.168.101.218" };
	const u_short ServerPort{ 9000 };
	const u_short BufferSize{ 5000 };

	SOCKET client_socket;

	int data{ 0 };
	SPacket packet;
};