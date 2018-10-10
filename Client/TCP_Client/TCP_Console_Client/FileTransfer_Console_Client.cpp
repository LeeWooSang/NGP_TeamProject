#include "FileTransfer_Console_Client.h"

CFileTransfer_Console_Client::CFileTransfer_Console_Client()
{
}

CFileTransfer_Console_Client::~CFileTransfer_Console_Client()
{
	Release();
	closesocket(client_socket);
}

void CFileTransfer_Console_Client::err_quit(const char* msg)
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

	return;
}

void CFileTransfer_Console_Client::err_display(const char* msg)
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

bool CFileTransfer_Console_Client::Socket_Init()
{
	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (client_socket == INVALID_SOCKET)
	{
		err_quit("socket( )");
		return false;
	}

	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(ServerIP);
	server_addr.sin_port = htons(ServerPort);
	data = connect(client_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));

	if (data == SOCKET_ERROR)
	{
		err_quit("connect( )");
		return false;
	}

	cout << ServerIP << " - ������ ���� ��" << endl;
	return true;
}

void CFileTransfer_Console_Client::File_Read()
{
	cout << "������ ���� �� : ";
	cin >> packet.file_name;
	packet.file_name;
	// ���� ���ſ�
	getchar();

	ifstream in(packet.file_name, ios::binary);
	if (!in)
	{
		cout << packet.file_name << " ������ �����ϴ�." << endl;
		return;
	}

	Packet_Send(in);
}

void CFileTransfer_Console_Client::Packet_Send(ifstream& in)
{
	// ���� �̸� ������
	// file_name.size() ���� const�� int ������ ��Ƽ� ������.
	int file_name_size = packet.file_name.size();

	data = send(client_socket, (char*)&file_name_size, sizeof(int), 0);
	if (data == SOCKET_ERROR)
	{
		err_display("send( )");
		return;
	}
	cout << "���� �̸�(��������) ���� Byte �� : " << data << "Byte" << endl;

	cout << "���� ������(�����̸�) ����(��������)" << endl;
	data = send(client_socket, packet.file_name.c_str(), file_name_size, 0);
	if (data == SOCKET_ERROR)
	{
		err_display("send( )");
		return;
	}

	cout << "[ TCP Ŭ���̾�Ʈ ] - " << data << "Byte�� ���½��ϴ�." << endl;

	// ���� �����͸� ������
	in.seekg(0, ios::end);
	// tellg( ) : ���� ũ�⸦ �˷���
	// ���� ũ�Ⱑ �󸶳� Ŭ�� �𸣴�
	size_t file_size = in.tellg();
	// ���� �����͸� ó������
	in.seekg(0, ios::beg);

	data = send(client_socket, (char*)&file_size, sizeof(int), 0);
	if (data == SOCKET_ERROR)
	{
		err_display("send( )");
		return;
	}
	cout << "(��������)���� Byte �� : " << data << endl;

	cout << "���� ������(����) ����(��������)" << endl;

	size_t send_data_size = 0;
	size_t left_data_count = file_size / BufferSize;

	// ���� ���� ������
	
	if (file_size > BufferSize)
	{
		packet.file_data = new char[BufferSize];
		if (packet.file_data == nullptr)
			return;

		while (left_data_count > 0)
		{
			in.read(packet.file_data, BufferSize * sizeof(char));

			data = send(client_socket, packet.file_data, BufferSize, 0);
			if (data == SOCKET_ERROR)
			{
				err_display("send( )");
				return;
			}

			// ���� ������ ���
			send_data_size += data;
			cout << "[ TCP Ŭ���̾�Ʈ ] - ���۷� : " << ((double)send_data_size / file_size) * 100 << "%, ���� ������ ũ�� : "
				<< send_data_size << "Byte" << endl;

			--left_data_count;
		}
		Release();
	}
	packet.file_data = new char[file_size - send_data_size];
	in.read(packet.file_data, (file_size - send_data_size) * sizeof(char));

	// ���� ������ ����
	data = send(client_socket, packet.file_data, file_size - send_data_size, 0);
	if (data == SOCKET_ERROR)
	{
		err_display("send( )");
		return;
	}

	send_data_size += data;
	cout << "[ TCP Ŭ���̾�Ʈ ] - ���۷� : " << ((double)send_data_size / file_size) * 100 << "%, ���� ������ ũ�� : "
		<< send_data_size << "Byte" << endl;

	Release();

	closesocket(client_socket);
}

void CFileTransfer_Console_Client::Release()
{
	if (packet.file_data != nullptr)
	{
		delete[] packet.file_data;
		packet.file_data = nullptr;
	}
}