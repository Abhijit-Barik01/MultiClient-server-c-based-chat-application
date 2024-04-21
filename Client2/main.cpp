#include<iostream>
#include <winsock2.h>
#include<WS2tcpip.h>
#include<tchar.h>
#include<thread>

#pragma comment(lib,"ws2_32.lib")


/* initialize winshock 
   create socket
   connect to server
   send/recv
   cose the socket
 */


bool Initialize()
{
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void SendMsg(SOCKET s)
{
	std::cout << "Enter your chat name :" << std::endl;
	std::string name;
	getline(std::cin, name);
	std::string message;

	while (1)
	{
		getline(std::cin, message);
		std::string msg = name + ":" + message;
		int bytesent = send(s, msg.c_str(), msg.length(),0);

		if (bytesent == SOCKET_ERROR)
		{
			std::cout << "ERROR SENDING MESSGE" << std::endl;
			break;
		}
		if (message == "quit")
		{
			std::cout << " STOPPING THE APPLICATION" << std::endl;
			break;
		}
	}
	closesocket(s);
	WSACleanup();
}

void ReceiveMessage(SOCKET s)
{
	char buffer[4096];
	int recvlength;

	while (1)
	{
		recvlength = recv(s, buffer, sizeof(buffer), 0);
		std::string msg = "";
		if (recvlength <= 0)
		{
			std::cout << "disconnect from the server" << std::endl;
			break;
		}
		else
		{
			msg = std::string(buffer, recvlength);

			std::cout << msg << std::endl;
		}
	}
	closesocket(s);
	WSACleanup();
}

int main()
{
	std::cout << "client program started" << std::endl;

	if (!Initialize())
	{
		std::cout << "Winshock Initialization " <<std::endl;
		return 1;
	}
	// create socket 
	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s == INVALID_SOCKET)
	{
		std::cout << "INVALID SOCKET CREATED" << std::endl;
		return 1;
	}


	int port = 12345;
	std::string serveraddress = "127.0.0.1";
	sockaddr_in  serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));

	if (connect(s,reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		std::cout << "NOT ABLE TO CONNECT TO SERVER" << std::endl;
		std::cout << ":" << WSAGetLastError() << std::endl;
		closesocket(s);
		WSACleanup();
		return 1;
	}

	std::cout << " Sucesssfully connected to server" << std::endl;
	//send // recive in thread 
	std::thread sendthread(SendMsg, s);
	std::thread reciverthread(ReceiveMessage, s);


	// running thread detach mode 


	sendthread.join();
	reciverthread.join();
	
	
	return  0;
}
