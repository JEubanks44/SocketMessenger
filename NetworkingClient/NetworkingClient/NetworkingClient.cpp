// NetworkingClient.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <string>
#pragma comment(lib, "ws2_32.lib")

SOCKET Connection;
std::string username;
void ClientThread()
{
	char buffer[256];
	while (true)
	{
		recv(Connection, buffer, sizeof(buffer), NULL); // Recieve message from server(message sent by another user that is stored in servers buffer
		std::cout << buffer << std::endl; //Print the message from the server to the screen.
	}
}

std::string getUsername()
{
	return username;
}

//First send integer of string length
//Then send the string
int main(int argc, _TCHAR* argv[])
{
	//Winsock StartUp
	std::string ip;
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) //If WSAStartup returns anything other than 0 there is an error
	{
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		std::cout << WSAGetLastError();
		exit(1);
	}

	SOCKADDR_IN addr; //Address that is binded to the listening socket
	char buffer[256];
	int addrlen = sizeof(addr); //length of address (required for accept call)
	std::cout << "Enter IP Address: ";
	std::cin >> ip;
	std::cout << std::endl;
	std::cout << "Enter a username for this session: ";
	std::cin >> username;
	strncpy_s(buffer, username.c_str(), sizeof(buffer));
	std::cout << std::endl;
	addr.sin_addr.s_addr = inet_addr(ip.c_str()); //Address of of the server
	addr.sin_port = htons(1111); //Port = 1111
	addr.sin_family = AF_INET; //IPv4 Socket
	Connection = socket(AF_INET, SOCK_STREAM, NULL); //Set Connection Socket
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr))) //IF Unable to connect...
	{
		MessageBoxA(NULL, "Failed to Connect", "Error", MB_OK | MB_ICONERROR); //Creates a message box wiht an error icon and an ok button
		return 0; //Failed to Connect
	}

	std::cout << "Connected!" << std::endl;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL); // launches the client thread.
	send(Connection, buffer, sizeof(buffer), NULL);
	
	while (true) // While loop that allow user to contiunously send message
	{
		std::cin.getline(buffer, sizeof(buffer)); //Gets the user input and stores it in the buffer	
		send(Connection, buffer, sizeof(buffer), NULL); //Sends user message to the server buffer
		Sleep(10);
	}
	system("pause");
	
	return 0;
}

