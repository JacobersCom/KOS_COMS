#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include "stdint.h"
#include "stdio.h"
#include <cstdlib>
#include "ServerCommands.h"
#include "ServerReturns.h"
#include "UserInfo.h"

static UserInfo userInfo;

class Server
{
public:

	int init(uint16_t port);

	static int send_all(SOCKET s, const char* msg, int len);
	static int send_packet(SOCKET s, const char* msg);
	static int revc_exact(SOCKET s, char* incomingMsg, int len);
	static int recv_packet(SOCKET s, char incomingMsg[256]);
	static int server_commands(SOCKET s, const char* buffer);
	static void splitString(char dst, const char* src);

private:

	WSADATA wsadata;

	//record of all connected sockets
	fd_set master_set;
	//copy that gets passed into select
	fd_set ready_set;
	//accepts new connections
	SOCKET listen_socket;
	//newly accepted clients
	SOCKET client_socket;
	//the highest numbered socket
	int max_fd;

	
};