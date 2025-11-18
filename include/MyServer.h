#ifndef MY_SERVER_H
#define MY_SERVER_H
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <stdlib.h>
#include <cstddef>
#include <Manager.h>
#include "GetLocalAddress.h"
#include <tuple>
namespace src = boost::log::sources;
namespace za{
class MyServer
{
private:
	int socketListenToConnexion;
	int socketProcessAcceptedConnexion;
	bool isConnexionActive;
	unsigned short MAX_CONNEXION;
	unsigned short serverPortNumber;
	unsigned short  clientPortNumber;
	
	ssize_t numberOfByteReadFromBuffer;
	char clientIPv4[INET_ADDRSTRLEN +1];
	socklen_t clientAddressLength;
	socklen_t serverAddressLength;
	char buffer[1024];
	std::string serverIPv4;
	struct sockaddr_in serverAddrress;
	struct sockaddr_in clientAddress;
	const char* logName;
	za::Manager logManager;
	za::my_logger::logger_type log = za::my_logger::get();

public:
	MyServer();
	MyServer(unsigned short _portNumber_, unsigned short _max_connexion_);
	int createServerForBindAndListen();
	int bindServerToPortNumber();
	int listenToNewClientConnexion();
	int acceptToMakeConnexionWithClient();
	int sendResponseToClient(std::string &message, int status);
	int sendResponseToClient(std::string &&message, int status);
	int readIncomingMessageFromClient();
	int getPort();
	int showMaxConnexion();
	int showServerPort();
	int showServerIP();
	int getClientAddress();
	int showClientPort();
	int showClientIP();
	int closeAcceptedConnexion();
	int closeListenningSocket();
	int setLog(std::string& _logName_);
	int setLog(std::string&& _logName_);
	std::string getCurrentTime();
	int sendFileToClient(const std::string& filePath);
	int sendFileToClient(const char* filePath);
	
	const char* getClientIPAddress() const { return clientIPv4; }
	unsigned short getClientPortNumber() const { return clientPortNumber; }
	int getAcceptedSocket() const { return socketProcessAcceptedConnexion; }
};
}
#endif 
