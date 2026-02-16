#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include "stdint.h"
#include "stdio.h"

class Server
{
public:

	int init(uint16_t port);

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