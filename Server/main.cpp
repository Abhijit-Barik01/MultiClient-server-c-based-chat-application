#include<iostream>
#include <winsock2.h>
#include<WS2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>

#pragma comment(lib,"ws2_32.lib")


//intialize winsock libary
bool Initialize()
{
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void InteractWithClient(SOCKET clientSocket, std::vector<SOCKET>& clientsVec)
{
	//send / recv 
	std::cout << "client connected" << std::endl;
	char buffer[4096];

	while (1)
	{
		int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesrecvd <= 0)
		{
			std::cout << "client disconnected" << std::endl;
			break;
		}

		std::string messge(buffer, bytesrecvd);

		std::cout << "message from client :" << messge << std::endl;

		for (auto client : clientsVec)
		{
			if (client != clientSocket)
			{
				send(client, messge.c_str(), messge.length(), 0);
			}
		}
	}
	auto it = std::find(clientsVec.begin(),clientsVec.end(), clientSocket);

	if (it != clientsVec.end())
	{
		clientsVec.erase(it);
	}


	closesocket(clientSocket);
 }


int main()
{
	if (!Initialize())
	{
		std::cout << "Winshock initialization is failed !!" <<std::endl;
		return 1;
	}
	std::cout << " Server Program " <<std::endl;

	//create socket

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSocket ==INVALID_SOCKET)
	{
		std::cout << "socket creation failed" << std::endl;
		return 1;
	}
	//create address structure
	int port = 12345;
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	//covert ip 0.0.0.0.put it inside sin_family in binary format

	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1)
	{
		std::cout << "Setting  adress structure failed" <<std::endl;
		closesocket(listenSocket);
		WSACleanup;
		return 1;
	}


    //bind the ip/port with the socket
	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
	{
		std::cout << "bind filled" << std::endl;
	}
	

	//listen on the socket 

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "listen failed" << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;

	 }


	std::cout << "server has started listen on port : " << port << std::endl;
	std::vector<SOCKET>clientsVec;
	//accept
	while (1)
	{
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET)
		{
			std::cout << "invalid client socket " << std::endl;
		}
		clientsVec.push_back(clientSocket);


		std::thread t1(InteractWithClient, clientSocket,std::ref(clientsVec));
		t1.detach();
	}

	 closesocket(listenSocket);

	 WSACleanup();

	 return 0;


}
