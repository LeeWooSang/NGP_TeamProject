#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32")


#include <WinSock2.h>
#include <iostream>
#include <string>


#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000

using namespace std;

void err_quit(char *msg);

void err_display(char *msg);

int recvn(SOCKET s, char* buf, int len, int flags);
