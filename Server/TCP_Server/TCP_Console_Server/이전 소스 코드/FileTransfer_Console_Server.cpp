#pragma comment(lib, "ws2_32")
#pragma pack(1)
#include <iostream>
#include <string>
#include <fstream>
#include <WinSock2.h>
using namespace std;

void err_quit(const char*);
void err_display(const char*);
int recvn(SOCKET, char*, int, int);
void File_Transfer();
const u_short ServerPort{ 9000 };
const u_int BufferSize{ 5000 };
int main()
{
	File_Transfer();
}

void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);
	cout << msg << (char*)lpMsgBuf << endl;

	LocalFree(lpMsgBuf);
}

int recvn(SOCKET socket, char* buf, int len, int flag)
{
	int recev;
	char* data = buf;
	int left = len;

	while (left > 0)
	{
		recev = recv(socket, data, left, flag);
		if (recev == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (recev == 0)
			break;
		left -= recev;
		data += recev;
	}
	return (len - left);
}

void File_Transfer()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	// Socket( )
	// 대기 소켓 생성
	SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET)
		err_quit("socket( )");

	cout << "대기 소켓 생성 완료" << endl;

	int data = 0;

	// bind( )
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(ServerPort);
	data = bind(listen_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (data == SOCKET_ERROR)
		err_quit("bind( )");

	// listen( )
	data = listen(listen_socket, SOMAXCONN);
	if (data == SOCKET_ERROR)
		err_quit("listen( )");

	// 데이터 통신에 사용할 클라이언트 전용 소켓 생성
	SOCKET client_socket;
	SOCKADDR_IN client_addr;
	int addrlen;

	while (true)
	{
		// accept( )
		addrlen = sizeof(client_addr);
		client_socket = accept(listen_socket, (SOCKADDR*)&client_addr, &addrlen);
		if (client_socket == INVALID_SOCKET)
		{
			err_display("accept( )");
			break;
		}

		// 접속한 클라이언트 정보 출력
		cout << endl << "[ TCP 서버 ] 클라이언트 접속 - IP : " << inet_ntoa(client_addr.sin_addr)
			<< ", 포트 번호 : " << ntohs(client_addr.sin_port) << endl;

		int file_name_size = 0;

		data = recvn(client_socket, (char*)&file_name_size, sizeof(int), 0);
		if (data == SOCKET_ERROR)
		{
			err_display("recvn( )");
			break;
		}
		else if (data == 0)
			break;

		// 널문자가 들어가기 위해 이름길이을 해줌
		char* file_data = new char[file_name_size + 1];
		if (file_data == nullptr)
			return;

		data = recvn(client_socket, file_data, file_name_size, 0);
		if (data == SOCKET_ERROR)
		{
			err_display("recvn( )");
			break;
		}
		else if (data == 0)
			break;

		file_data[data] = '\0';
		cout << file_data << endl;

		// 전송 받은 파일이름으로 파일 생성
		ofstream out(file_data);

		delete[] file_data;

		u_int file_size = 0;

		// 데이터 받기( recvn( ) ) => 고정길이(받을 데이터 사이즈)
		data = recvn(client_socket, (char*)&file_size, sizeof(int), 0);
		if (data == SOCKET_ERROR)
		{
			err_display("recvn( )");
			break;
		}
		else if (data == 0)
			break;

		file_data = new char[BufferSize];
		if (file_data == nullptr)
			return;

		u_int recv_data_size = 0;

		// 총 파일 크기 / 버퍼 사이즈 을 하면 몇번 recvn( )를 호출 해야하는지 알 수 있다.
		int left_data_count = file_size / BufferSize;
		// BufferSize만큼 데이터를 받자.
		
		while(left_data_count > 0)
		{
			// 실제 데이터 받기( recvn( ) ) => 가변 길이
			data = recvn(client_socket, file_data, BufferSize, 0);
			if (data == SOCKET_ERROR)
			{
				err_display("recvn( )");
				break;
			}
			else if (data == 0)
				break;

			// 받은 데이터 출력
			recv_data_size += data;

			cout << "[ TCP / " << inet_ntoa(client_addr.sin_addr) << ", " << ntohs(client_addr.sin_port)
				<< " ] - 전송률 : " << ((double)recv_data_size / file_size) * 100 << "%, 받은 데이터 크기 : " << recv_data_size << "Byte" << endl;

			for (int i = 0; i < data; ++i)
				out << file_data[i];

			--left_data_count;
		} 

		// 남은 데이터
		data = recvn(client_socket, file_data, file_size - recv_data_size, 0);
		if (data == SOCKET_ERROR)
		{
			err_display("recvn( )");
			break;
		}
		else if (data == 0)
			break;

		file_data[data] = '\0';

		for (int i = 0; i < data; ++i)
			out << file_data[i];
		out.close();

		recv_data_size += data;

		cout << "[ TCP / " << inet_ntoa(client_addr.sin_addr) << ", " << ntohs(client_addr.sin_port)
			<< " ] - 전송률 : " << ((double)recv_data_size / file_size) * 100 << "%, 받은 데이터 크기 : " << recv_data_size << "Byte" << endl;
		cout << "파일 전송이 끝났습니다." << endl;

		delete[] file_data;
		
		// closesocket( ) - 클라 전용 소켓 닫기
		closesocket(client_socket);
		cout << endl << "[ TCP 서버 ] 클라이언트 접속 - IP : " << inet_ntoa(client_addr.sin_addr)
			<< ", 포트 번호 : " << ntohs(client_addr.sin_port) << endl;
	}

	// closesocket( ) - 대기 소켓 닫기
	closesocket(listen_socket);

	// 윈속 종료
	WSACleanup();
}