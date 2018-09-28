#include "FileTransfer_Console_Server.h"

CFileTransfer_Console_Server::CFileTransfer_Console_Server()
{
}

CFileTransfer_Console_Server::~CFileTransfer_Console_Server()
{
	Release();
	closesocket(listen_socket);
}

void CFileTransfer_Console_Server::err_quit(const char* msg)
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

void CFileTransfer_Console_Server::err_display(const char* msg)
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

bool CFileTransfer_Console_Server::Socket_Init()
{
	// Socket( )
	// 대기 소켓 생성
	listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET)
	{
		err_quit("socket( )");
		return false;
	}

	// bind( )
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(ServerPort);
	data = bind(listen_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (data == SOCKET_ERROR)
	{
		err_quit("bind( )");
		return false;
	}

	// listen( )
	data = listen(listen_socket, SOMAXCONN);
	if (data == SOCKET_ERROR)
	{
		err_quit("listen( )");
		return false;
	}

	cout << "대기 소켓 생성 완료" << endl;

	// 클라전용 소켓
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

		Packet_Recv(client_socket, client_addr, addrlen);
	}

	return true;
}

int CFileTransfer_Console_Server::recvn(SOCKET socket, char* buf, int len, int flag)
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

void CFileTransfer_Console_Server::Packet_Recv(SOCKET& client_socket, SOCKADDR_IN& client_addr, int& addrlen)
{

	// 접속한 클라이언트 정보 출력
	cout << endl << "[ TCP 서버 ] 클라이언트 접속 - IP : " << inet_ntoa(client_addr.sin_addr)
		<< ", 포트 번호 : " << ntohs(client_addr.sin_port) << endl;

	int file_name_size = 0;

	data = recvn(client_socket, (char*)&file_name_size, sizeof(int), 0);
	if (data == SOCKET_ERROR)
	{
		err_display("recvn( )");
		return;
	}
	else if (data == 0)
		return;

	// 널문자가 들어가기 위해 이름길이 +1을 해줌
	packet.file_data = new char[file_name_size + 1];
	if (packet.file_data == nullptr)
		return;

	data = recvn(client_socket, packet.file_data, file_name_size, 0);
	if (data == SOCKET_ERROR)
	{
		err_display("recvn( )");
		return;
	}
	else if (data == 0)
		return;

	// 이름 끝에 널문자 추가
	packet.file_data[file_name_size] = '\0';

	cout << "파일이름 : " << packet.file_data << endl;

	// 전송 받은 파일이름으로 파일 생성
	ofstream out(packet.file_data, ios::binary);

	Release();

	size_t file_size = 0;

	// 데이터 받기( recvn( ) ) => 고정길이(받을 데이터 사이즈)
	data = recvn(client_socket, (char*)&file_size, sizeof(int), 0);
	if (data == SOCKET_ERROR)
	{
		err_display("recvn( )");
		return;
	}
	else if (data == 0)
		return;

	packet.file_data = new char[BufferSize];
	if (packet.file_data == nullptr)
		return;

	// 받은 데이터의 크기
	size_t recv_data_size = 0;
	// 총 파일 크기 / 버퍼 사이즈 을 하면 몇번 recvn( )를 호출 해야하는지 알 수 있다.
	size_t left_data_count = file_size / BufferSize;

	while (left_data_count > 0)
	{
		// 실제 데이터 받기( recvn( ) ) => 가변 길이
		data = recvn(client_socket, packet.file_data, BufferSize, 0);
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

		out.write((const char*)packet.file_data, data * sizeof(char));

		--left_data_count;
	}
	Release();

	// 남은 데이터 받기
	packet.file_data = new char[file_size - recv_data_size ];
	data = recvn(client_socket, packet.file_data, file_size - recv_data_size, 0);
	if (data == SOCKET_ERROR)
	{
		err_display("recvn( )");
		return;
	}
	else if (data == 0)
		return;

	out.write((const char*)packet.file_data, (data) * sizeof(char));
	out.close();

	recv_data_size += data;

	cout << "[ TCP / " << inet_ntoa(client_addr.sin_addr) << ", " << ntohs(client_addr.sin_port)
		<< " ] - 전송률 : " << ((double)recv_data_size / file_size) * 100 << "%, 받은 데이터 크기 : " << recv_data_size << "Byte" << endl;
	cout << "파일 다운이 끝났습니다." << endl;

	Release();

	// closesocket( ) - 클라 전용 소켓 닫기
	closesocket(client_socket);
	cout << endl << "[ TCP 서버 ] 클라이언트 접속 - IP : " << inet_ntoa(client_addr.sin_addr)
		<< ", 포트 번호 : " << ntohs(client_addr.sin_port) << endl;
}

void CFileTransfer_Console_Server::Release()
{
	if (packet.file_data != nullptr)
	{
		delete[] packet.file_data;
		packet.file_data = nullptr;
	}
}