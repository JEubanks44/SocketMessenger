// NetworkingServer.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <string>
#pragma comment(lib, "ws2_32.lib")
SOCKET Connection[100]; //An array of sockets that stores up to 100 connections
int ConnectionCounter = 0;
char buffer[256];
void ClientHandlerThread(int index)//Thread that recieves data from clients and sends the data to all other clients
{
	 //Creates a character buffer of 256 chars
	while (true)
	{
		recv(Connection[index], buffer, sizeof(buffer), NULL); //recieve message from the client that is passed to the function and places it in the buffer
		for (int i = 0; i < ConnectionCounter; i++)
		{
			if (i == index)
			{
				continue;
			}
			send(Connection[i], buffer, sizeof(buffer), NULL); //sends the message to all other connected clients on the server
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	//Winsock StartUp
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	int connectionCounter = 0;
	if (WSAStartup(DllVersion, &wsaData) != 0) //If WSAStartup returns anything other than 0 there is an error
	{
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	SOCKADDR_IN addr; //Address that is binded to the listening socket
	int addrlen = sizeof(addr); //length of address (required for accept call)
	addr.sin_addr.s_addr = inet_addr("10.36.148.107"); //Broadcast Locally (local host IP)
	addr.sin_port = htons(1111); //Port
	addr.sin_family = AF_INET; //IPv4 Socket

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); //Creates socket that listens(waits and recieves new connections)
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr)); //binds server address to socket(Socket, address, length of address)
	listen(sListen, SOMAXCONN); // Places socket in a state where it is listening for an incoming connection SOMAX = max number of connections at once

	SOCKET newConnection; //Socket that holds the client connection
	for (int i = 0; i < 100; i++) //Loops through all connections in the Connection array
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //Accepts the new connection
		if (newConnection == 0) //if the client connection is not accepted
		{
			std::cout << "Failed to accept the client's connection." << std::endl;
		}
		else //IF the client connects do this stuff
		{
			char MOTD[256] = "This is a test message!"; //Creates a buffer with a message to be sent
			send(newConnection, MOTD, sizeof(MOTD), NULL); //Send MOTD to buffer (Socket with client connection, Char pointer to data, size of data in bytes, flag)
			Connection[i] = newConnection; //sets socket at i to hold the new connection to the server
			newConnection;
			connectionCounter += 1;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(i), NULL, NULL); //Creates thread that will allow the client at index i to send messages to the server and other clients
			recv(Connection[i], buffer, sizeof(buffer), NULL);
			std::string s = buffer;
			std::string clientConnectedString = "Client " + s + " Connected";
			std::cout << clientConnectedString << std::endl;
		}
		
	}
	system("pause");
	ConnectionCounter = 0;
	return 0;
}

