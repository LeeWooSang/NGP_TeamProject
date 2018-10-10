#include "FileTransfer_Multithread_Console_Server.h"

int main()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 0;

	CFileTransfer_Multithread_Console_Server* pFileTransfer = nullptr;
	pFileTransfer = new CFileTransfer_Multithread_Console_Server;
	if (pFileTransfer == nullptr)
	{
		WSACleanup();
		return 0;
	}

	if (pFileTransfer->Socket_Init() == true)
		cout << "서버를 닫습니다." << endl;

	delete pFileTransfer;
	pFileTransfer = nullptr;

	WSACleanup();
}