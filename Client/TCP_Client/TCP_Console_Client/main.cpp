#include "FileTransfer_Console_Client.h"

int main()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 0;

	CFileTransfer_Console_Client* pFileTransfer = new CFileTransfer_Console_Client;
	if (pFileTransfer == nullptr)
	{
		WSACleanup();
		return 0;
	}
	
	if (pFileTransfer->Socket_Init() == true)
		pFileTransfer->File_Read();
	else
		cout << "파일 읽기 실패" << endl;

	delete pFileTransfer;
	pFileTransfer = nullptr;

	WSACleanup();
}