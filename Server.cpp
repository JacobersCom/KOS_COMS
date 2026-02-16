#include "Server.h"

int Server::init(uint16_t port)
{

	//Init WinSock
	if (WSAStartup(WINSOCK_VERSION, &wsadata) < 0)
	{
		printf("Failed to start WSA");
		return 1;
	}

	//Create listening socket

	listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_socket == INVALID_SOCKET)
	{
		printf("socket() failed");
		WSACleanup();
		return 1;
	}

	//Bind socket
	
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);

	if (bind(listen_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		printf("bind() failed\n");
		closesocket(listen_socket);
		WSACleanup();

		return 1;
	}

	//start listening

	if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("Listen() failed");
		closesocket(listen_socket);
		WSACleanup();
		return 1;
	}

	printf("Server listening on port %d, ", port);

	//Setting up sets

	//Clears the bitset
	FD_ZERO(&master_set);
	//adds the listening socket to the master_set. Select will now watch this socket
	FD_SET(listen_socket, &master_set);
	//Starts tracking the highest socket id
	max_fd = listen_socket;

	while (true)
	{
		//Copy master
		ready_set = master_set;

		//Wait for activity
		int activity = select(max_fd + 1, &ready_set, NULL, NULL, NULL);

		//loop through all possible sockets, because fd_set is just bits
		for (int s = 0; s <= max_fd; s++)
		{
			//is the socket ready
			if (FD_ISSET(s, &ready_set))
			{
				//is a client is trying to connect
				if (s == listen_socket)
				{
					//accept the connection
					client_socket = accept(listen_socket, NULL, NULL);
					//add client socket to the master set
					FD_SET(client_socket, &master_set);

					if (client_socket > max_fd)
						max_fd = client_socket;
					
				}
				//Incoming data
				else
				{
					//Reads the data and not blocked by select because select said its ready
					char buffer[512];
					int bytes = recv(s, buffer, sizeof(buffer), 0);

					if (bytes <= 0)
					{
						//close socket
						closesocket(s);
						//remove from master set
						FD_CLR(s, &master_set);
					}
					else
					{
						// handle data
					}
				}

			}
		}

	}

}
