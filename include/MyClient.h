#ifndef MY_CLIENT_H
#define MY_CLIENT_H


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>
#include <Manager.h>
#include <tuple>
#include <time.h>
namespace za{

class MyClient
{
private:

	int socketConnectToClient_;
	unsigned short serverPortNumber_;
	int numberOfByteOfBuffer_;

	struct sockaddr_in serverAddress_;
	struct hostent *serverIP_;
	za::Manager logManager;
	za::my_logger::logger_type log = za::my_logger::get();
	char buffer[1024];

public:


	MyClient(char *_serverIP_, unsigned short _serverPort_);
	int createClientForConnexion();
	int connectClientToServer();
	int sendRequestToServer();
	int receiveResponseFromServer();
	int closeConnexion();
	int setLog(std::string& _logName_);
	int setLog(std::string&& _logName_);
	std::tuple <std::string, std::string> getCurrentTime();
	int receiveFileFromServer(const std::string& savePath);
	int receiveFileFromServer(const char* savePath);
	

	auto socketConnectToClient() const& -> const int& { return socketConnectToClient_; }
	auto socketConnectToClient() &      -> int&       { return socketConnectToClient_; }
	auto socketConnectToClient() &&     -> int&&      { return std::move(socketConnectToClient_); }


	auto serverPortNumber() const& -> const unsigned short& { return serverPortNumber_; }
	auto serverPortNumber() &      -> unsigned short&       { return serverPortNumber_; }
	auto serverPortNumber() &&     -> unsigned short&&      { return std::move(serverPortNumber_); }

	

	auto numberOfByteOfBuffer() const& -> const int& { return numberOfByteOfBuffer_; }
	auto numberOfByteOfBuffer() &      -> int&       { return numberOfByteOfBuffer_; }
	auto numberOfByteOfBuffer() &&     -> int&&      { return std::move(numberOfByteOfBuffer_); }



	auto serverAddress() const& -> const sockaddr_in& { return serverAddress_; }
	auto serverAddress() &      -> sockaddr_in&       { return serverAddress_; }
	auto serverAddress() &&     -> sockaddr_in&&      { return std::move(serverAddress_); }

};
}

#endif