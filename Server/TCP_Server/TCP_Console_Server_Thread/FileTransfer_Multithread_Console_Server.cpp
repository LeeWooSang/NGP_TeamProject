#include "FileTransfer_Multithread_Console_Server.h"

CFileTransfer_Multithread_Console_Server::CFileTransfer_Multithread_Console_Server()
{
}

CFileTransfer_Multithread_Console_Server::CFileTransfer_Multithread_Console_Server(SOCKET client_socket, SOCKADDR_IN client_addr, int addrlen)
	: m_client_socket(client_socket), m_client_addr(client_addr), m_addrlen(addrlen)
{
}

CFileTransfer_Multithread_Console_Server::~CFileTransfer_Multithread_Console_Server()
{
	Release();

	if (vec_client.size() != 0)
		vec_client.clear();

	closesocket(m_listen_socket);
}

void CFileTransfer_Multithread_Console_Server::err_quit(const char* msg)
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

void CFileTransfer_Multithread_Console_Server::err_display(const char* msg)
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

bool CFileTransfer_Multithread_Console_Server::Socket_Init()
{
	vec_client.reserve(Max_Client_Count);

	// Socket( )
	// 대기 소켓 생성
	m_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listen_socket == INVALID_SOCKET)
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
	// namespace std에 있는 bind 함수와 Winsock2에 있는 bind 함수와 겹쳐서 구분
	data = ::bind(m_listen_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (data == SOCKET_ERROR)
	{
		err_quit("bind( )");
		return false;
	}

	// listen( )
	data = listen(m_listen_socket, SOMAXCONN);
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

	//thread* thread_client[3];
	while (true)
	{
		// accept( )
		addrlen = sizeof(client_addr);
		client_socket = accept(m_listen_socket, (SOCKADDR*)&client_addr, &addrlen);
		if (client_socket == INVALID_SOCKET)
		{
			err_display("accept( )");
			break;
		}

		// 클라이언트를 담을 벡터
		vec_client.emplace_back(client_socket, client_addr, addrlen);
		// 벡터 인덱스
		static u_int index = 0;
		if (thread_client[index] == nullptr)
		{
			thread_client[index] = new thread([=]() { Thread_ProcessClient(vec_client, index); });
			thread_client[index]->join();

			++index;
		}

		//hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_socket, 0, NULL);
		//thread_client = new thread(Thread_ProcessClient, this, client_socket, client_addr, addrlen);
		//thread_client[index++] = new thread( [=]() { Thread_ProcessClient(client_socket, client_addr, addrlen); } );
		//thread_client = new thread( [=]() { Thread_ProcessClient(vec_client, index++); } );

		if (index == 2)
			break;
	}

	//for (int i = 0; i < Max_Client_Count; ++i)
	//{
	//	if (thread_client[i] != nullptr)
	//	{
	//		//thread_client[i]->join();
	//		delete thread_client[i];
	//		thread_client[i] = nullptr;
	//	}
	//}

	return true;
}

DWORD WINAPI CFileTransfer_Multithread_Console_Server::ProcessClient(LPVOID client_sock)
{
	SOCKET client_socket = (SOCKET)client_sock;
	SOCKADDR_IN client_addr;
	int addrlen = sizeof(client_addr);

	getpeername(client_socket, (SOCKADDR*)&client_addr, &addrlen);
 
	CFileTransfer_Multithread_Console_Server* client = new CFileTransfer_Multithread_Console_Server();
	client->Packet_Recv(client_socket, client_addr, addrlen);

	delete client;
	client = nullptr;

	return 0;
}

void CFileTransfer_Multithread_Console_Server::Thread_ProcessClient(SOCKET sock, SOCKADDR_IN addr, int len)
{
	SOCKET client_socket = sock;
	SOCKADDR_IN client_addr;
	int addrlen = sizeof(client_addr);

	getpeername(client_socket, (SOCKADDR*)&client_addr, &addrlen);

	CFileTransfer_Multithread_Console_Server* client = new CFileTransfer_Multithread_Console_Server;
	client->Packet_Recv(client_socket, client_addr, addrlen);

	delete client;
	client = nullptr;
}

void CFileTransfer_Multithread_Console_Server::Thread_ProcessClient(vector<CFileTransfer_Multithread_Console_Server>& v, int index)
{
	v[index].Packet_Recv(v[index].m_client_socket, v[index].m_client_addr, v[index].m_addrlen);
}

int CFileTransfer_Multithread_Console_Server::recvn(SOCKET socket, char* buf, int len, int flag)
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

void CFileTransfer_Multithread_Console_Server::Packet_Recv(SOCKET& client_socket, SOCKADDR_IN& client_addr, int& addrlen)
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
	m_packet.file_data = new char[file_name_size + 1];
	if (m_packet.file_data == nullptr)
		return;

	data = recvn(client_socket, m_packet.file_data, file_name_size, 0);
	if (data == SOCKET_ERROR)
	{
		err_display("recvn( )");
		return;
	}
	else if (data == 0)
		return;

	// 이름 끝에 널문자 추가
	m_packet.file_data[file_name_size] = '\0';

	cout << "파일이름 : " << m_packet.file_data << endl;

	// 전송 받은 파일이름으로 파일 생성
	ofstream out(m_packet.file_data, ios::binary);

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

	m_packet.file_data = new char[BufferSize];
	if (m_packet.file_data == nullptr)
		return;

	// 받은 데이터의 크기
	size_t recv_data_size = 0;
	// 총 파일 크기 / 버퍼 사이즈 을 하면 몇번 recvn( )를 호출 해야하는지 알 수 있다.
	size_t left_data_count = file_size / BufferSize;

	while (left_data_count > 0)
	{
		// 실제 데이터 받기( recvn( ) ) => 가변 길이
		data = recvn(client_socket, m_packet.file_data, BufferSize, 0);
		if (data == SOCKET_ERROR)
		{
			err_display("recvn( )");
			break;
		}
		else if (data == 0)
			break;

		// 받은 데이터 출력
		recv_data_size += data;

		system("cls");
		cout << "[ TCP / " << inet_ntoa(client_addr.sin_addr) << ", " << ntohs(client_addr.sin_port)
			<< " ] - 전송률 : " << ((double)recv_data_size / file_size) * 100 << "%, 받은 데이터 크기 : " << recv_data_size << "Byte" << endl;

		out.write((const char*)m_packet.file_data, data * sizeof(char));

		--left_data_count;
	}
	Release();

	// 남은 데이터 받기
	m_packet.file_data = new char[file_size - recv_data_size ];
	data = recvn(client_socket, m_packet.file_data, file_size - recv_data_size, 0);
	if (data == SOCKET_ERROR)
	{
		err_display("recvn( )");
		return;
	}
	else if (data == 0)
		return;

	out.write((const char*)m_packet.file_data, (data) * sizeof(char));
	out.close();

	recv_data_size += data;

	system("cls");
	cout << "[ TCP / " << inet_ntoa(client_addr.sin_addr) << ", " << ntohs(client_addr.sin_port)
		<< " ] - 전송률 : " << ((double)recv_data_size / file_size) * 100 << "%, 받은 데이터 크기 : " << recv_data_size << "Byte" << endl;
	cout << "파일 다운이 끝났습니다." << endl;

	Release();

	// closesocket( ) - 클라 전용 소켓 닫기
	closesocket(client_socket);
	//cout << endl << "[ TCP 서버 ] 클라이언트 접속 - IP : " << inet_ntoa(client_addr.sin_addr)
	//	<< ", 포트 번호 : " << ntohs(client_addr.sin_port) << endl;
}

void CFileTransfer_Multithread_Console_Server::Release()
{
	if (m_packet.file_data != nullptr)
	{
		delete[] m_packet.file_data;
		m_packet.file_data = nullptr;
	}
}