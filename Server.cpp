#include "Server.h"

int Server::init(uint16_t port)
{
	InitHashTable();

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

	printf("Server listening on port %d \n", port);

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
					client_socket = accept(s, NULL, NULL);

					if (client_socket < 0)
					{
						printf("NULL connection\n");
						closesocket(client_socket);
						WSACleanup();
						return 1;
					}

					//add client socket to the master set
					FD_SET(client_socket, &master_set);

					if (client_socket > max_fd)
						max_fd = client_socket;
					
					send_packet(client_socket, "Welcome to KOS COMS\n Each command starts with ~ try ~help to get started!\n"); 

				}
				//Incoming data
				else
				{
					//Reads the data and not blocked by select because select said its ready
					char buffer[512];
					int payload_size = recv_packet(client_socket, buffer);

					if (payload_size <= 0)
					{
						//close socket
						closesocket(s);
						//remove from master set
						FD_CLR(s, &master_set);
					}
					else
					{
						printf("Client Says: %s\n", buffer);
						server_commands(client_socket,buffer);
					}
				}

			}
		}

	}

}

int Server::send_all(SOCKET s, const char* msg, int len)
{
	int bytesSent = 0;

	//While the full msg hasnt been sent
	while (bytesSent < len)
	{
		//Send one byte pre iteration
		int result = send(s, msg + bytesSent, len - bytesSent, 0);

		//Error
		if (result < 0)
		{
			return result;
		}

		//Advance to the next byte
		bytesSent += result;
	}
	//return the lenght of the msg
	return bytesSent;
}

int Server::send_packet(SOCKET s, const char* msg)
{
	//If there is no msg return error
	if (!msg) return SOCKET_ERROR;

	//Get the len of the message
	size_t msg_len = strlen(msg);
	
	//If msg lenght is bigger than 255
	if (msg_len > 255)
	{
		printf("Msg is bigger than 255 bytes");
	}

	unsigned char len_bytes = (unsigned char)msg_len;

	//Send the lenght of the message over
	if (send_all(s, (const char*)&len_bytes, 1) == SOCKET_ERROR)
		return SOCKET_ERROR;

	//If there is a payload send that one byte at a time
	if (msg_len > 0)
	{
		if (send_all(s, msg, (int)msg_len) == SOCKET_ERROR)
			return SOCKET_ERROR;
	}

	return (int)(1 + msg_len);
}

int Server::revc_exact(SOCKET s, char* incomingMsg, int len)
{
	int incomingBytes = 0;

	//While the full msg hasnt been sent
	while (incomingBytes < len)
	{
		//Send one byte pre iteration
		int result = recv(s, incomingMsg + incomingBytes, len - incomingBytes, 0);

		//client disconnect
		if (result == 0)
		{
			return 0;
		}

		//Error
		if (result == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}

		//Advance to the next byte
		incomingBytes += result;
	}
	//return the lenght of the msg 
	return incomingBytes;
}

int Server::recv_packet(SOCKET s, char incomingMsg[256])
{
	unsigned char len_byte = 0;
	
	//Get the message lenght
	int result = revc_exact(s, (char*)&len_byte, 1);
	if (result == 0) return 0; //disconnected
	if (result == SOCKET_ERROR) return SOCKET_ERROR;
	
	int payload_len = (int)len_byte;

	//Read in the message
	if (payload_len)
	{
		result = revc_exact(s, incomingMsg, payload_len);
		if (result == 0) return 0;
		if (result == SOCKET_ERROR) return SOCKET_ERROR;
	}

	//Adds a null terminate to the end of the string
	incomingMsg[payload_len] = '\0';
	return payload_len;
}

int Server::server_commands(SOCKET s, const char* buffer)
{
	switch (CommandFromToken(buffer))
	{
		case ServerCommands::help:
		{
			send_packet(s,
				"List of available commands:\n ~help\n ~register\n ~login\n");

			return ServerReturns::SUCCESS;
			
			break;
		}
		case ServerCommands::signup:
		{
			send_packet(s,
				"Please enter a username and password for your user EX: Jacobers 1234.");

			recv_packet(s, (char*)buffer);
			
			char username[256];
			char password[256];
			int index = 0;

			for (int i = 0; i < strnlen(buffer, 512); i++)
			{
				index = i;
				if (buffer[i] == ' ') break;
				username[i] = buffer[i];
			}

			username[index] = '\0';

			for (int i = 0; index < strnlen(buffer, 256); index++)
			{
				if (buffer[index] == ' ') index++;
				password[i] = buffer[index];
				i++;
			}

			password[index] = '\0';

			strcat(userInfo.username, username);
			strcat(userInfo.password, password);

			HashTableInsert(&userInfo);
		}
	}
	return ServerReturns::Unknown;
}

