#pragma once
#include "Defines.h"

void err_quit(const char *msg);

void err_display(const char *msg);

int recvn(SOCKET s, char* buf, int len, int flags);

void Input_Keyboard(CS_RUN&);
