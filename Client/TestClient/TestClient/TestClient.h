#pragma once
#include "Defines.h"

void err_quit(const char *msg);

void err_display(const char *msg);

int recvn(SOCKET s, char* buf, int len, int flags);

bool Input_Keyboard(CS_RUN& runPacket);

void Release_Key(CS_RUN& runPacket);

void show_Position();