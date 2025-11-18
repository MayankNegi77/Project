#include "MyServer.h"
#include "FileAccessManager.h"
namespace za{
MyServer::MyServer():MyServer(0, 5){}
MyServer::MyServer(unsigned short _portNumber_, unsigned short _max_connexion_):
serverPortNumber(_portNumber_), MAX_CONNEXION(_max_connexion_)
{
	this->serverAddressLength = sizeof(this->serverAddrress);
	memset(&serverAddrress, 0, sizeof(serverAddrress));
	memset(&clientAddress, 0, sizeof(clientAddress));
    this->serverAddrress.sin_family = AF_INET;
    this->serverAddrress.sin_addr.s_addr = INADDR_ANY;
    this->serverAddrress.sin_port = htons(this->serverPortNumber);
	clientAddressLength = sizeof(clientAddress);
	this->isConnexionActive = false;
}
int MyServer::createServerForBindAndListen()
{
	this->showMaxConnexion();
	int returnValue = 0;
	BOOST_LOG_SEV(log, report) << "Creating a server socket ...\n";
	socketListenToConnexion = socket(AF_INET, SOCK_STREAM, 0);
	if (socketListenToConnexion < 0)
	{
		returnValue = -100000;
		BOOST_LOG_SEV(log, error)<<"Error while creating a server socket"
			<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
	}
	else
	{
		BOOST_LOG_SEV(log, report) << "A client socket created\n";
	}
	return returnValue;
}
int MyServer::bindServerToPortNumber()
{
	int returnValue = 0;
	BOOST_LOG_SEV(log, report) << "Binding to a local port ...\n";
	if (bind(this->socketListenToConnexion,
			(struct sockaddr *) &this->serverAddrress, 
					sizeof(this->serverAddrress)) < 0)
	{
		returnValue = -100001;
		BOOST_LOG_SEV(log, error)<<"Error while binding a socket"
			<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
	}
	else
	{
		BOOST_LOG_SEV(log, report) << "Bind success\n";
	}
	return returnValue;
}
int MyServer::listenToNewClientConnexion()
{
	int returnValue = 0;
	BOOST_LOG_SEV(log, report) << "Listenning to a new connexion...\n";
	if (listen(socketListenToConnexion, this->MAX_CONNEXION) < 0)
	{
		returnValue = -100002;
		BOOST_LOG_SEV(log, error)<<"Error while Listenning to a new connexion"
			<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
	}
	else
	{
		BOOST_LOG_SEV(log, report) << "A client connected to a server\n";
	}
	return returnValue;
}
int MyServer::acceptToMakeConnexionWithClient()
{
	int returnValue = 0;
	BOOST_LOG_SEV(log, report)<< "Accepting a connexion...\n";
	socketProcessAcceptedConnexion = accept(socketListenToConnexion,
										(struct sockaddr *)&clientAddress,
										&clientAddressLength);
	if (socketProcessAcceptedConnexion < 0)
	{
		returnValue = -100003;
		BOOST_LOG_SEV(log, error)<<"Error while accepting a connexion"
			<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
	}
	else
	{
		BOOST_LOG_SEV(log, report)<<"Connexion accepted\n";
		this->getClientAddress();
		this->showClientIP();
		this->showClientPort();
		this->isConnexionActive = true;
	}
	return returnValue;
}
int MyServer::readIncomingMessageFromClient()
{
	int returnValue = 0;
	if(this->isConnexionActive)
	{
		memset(buffer, 0, sizeof(buffer));
		numberOfByteReadFromBuffer = read(socketProcessAcceptedConnexion, buffer, 
											sizeof(buffer));
		if (numberOfByteReadFromBuffer < 0)
		{
			returnValue = -100004;
			BOOST_LOG_SEV(log, error)<<"Error while reading client message"
				<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
		}
		else if(numberOfByteReadFromBuffer == 0)
		{
			BOOST_LOG_SEV(log, report) << "Closing connexion with client: "
									<<socketProcessAcceptedConnexion<<"\n";
			this->closeAcceptedConnexion();
		}
		else
		{
			BOOST_LOG_SEV(log, report)<< "Time received from the client "
				<< socketProcessAcceptedConnexion <<": "<< buffer <<"\n";
		}
	}
	return returnValue;
}
int MyServer::sendResponseToClient(std::string &message, int status)
{
	int returnValue = 0;
	if(this->isConnexionActive)
	{
		switch (status)
		{
		case 200:
			numberOfByteReadFromBuffer = write(socketProcessAcceptedConnexion,
										&message[0],18);
			if (numberOfByteReadFromBuffer < 0)
			{
				returnValue = -100006;
				BOOST_LOG_SEV(log, error)<<"Fail to send back message to the client"
					<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
			}
			break;
		case 400:
			BOOST_LOG_SEV(log, error)<<"Connexion with client closed.\n";
		default:
			BOOST_LOG_SEV(log, error)<<"Unknown sendResponseToClient ERROR.\n";
			break;
		}
	}
	return returnValue;
}
int MyServer::sendResponseToClient(std::string &&message, int status)
{
	int returnValue = 0;
	if(this->isConnexionActive)
	{
		switch (status)
		{
		case 200:
			numberOfByteReadFromBuffer = write(socketProcessAcceptedConnexion,
										&message[0],18);
			if (numberOfByteReadFromBuffer < 0)
			{
				returnValue = -100006;
				BOOST_LOG_SEV(log, error)<<"Fail to send back message to the client"
					<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
			}
			break;
		case 400:
			BOOST_LOG_SEV(log, error)<<"Connexion with client closed.\n";
		default:
			BOOST_LOG_SEV(log, error)<<"Unknown sendResponseToClient ERROR.\n";
			break;
		}
	}
	return returnValue;
}
int MyServer::getPort()
{
	int returnValue = 0;
	if( getsockname(socketListenToConnexion, (struct sockaddr *)&serverAddrress, 
		&serverAddressLength) < 0)
	{
		returnValue = -100007;
		BOOST_LOG_SEV(log, error)<<"Fail to get server port number from getsockname"
		<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
	}
	else
	{
		returnValue = ntohs(serverAddrress.sin_port);
		if(returnValue < 0)
		{
			returnValue = -100008;
			BOOST_LOG_SEV(log, error)<<"ntos error with sin_port"
			<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
		}
		else
		{
			this->serverPortNumber = (unsigned short) returnValue;
		}
	}
	return returnValue;
}
int MyServer::getClientAddress()
{
	int returnValue = 0;
	if(inet_ntop(AF_INET, &this->clientAddress.sin_addr, this->clientIPv4, 
				INET_ADDRSTRLEN) == NULL)
	{
		returnValue = -100009;
		BOOST_LOG_SEV(log, error) << "unable to convert IP from binary to text "
					<<"dotted-decimal format"
			<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
	}
	return returnValue;
}
int MyServer::showClientPort()
{
	int returnValue = 0;
	this->clientPortNumber = ntohs(this->clientAddress.sin_port);
	if(this->clientPortNumber < 0)
	{
		returnValue = -100010;
		BOOST_LOG_SEV(log, error) << "unable to convert Port Number from network to host order "
			<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
	}
	else
	{
		BOOST_LOG_SEV(log, report) << "Client Port Number: ["<< this->clientPortNumber<<"]\n";
	}
	return returnValue;
}
int MyServer::showClientIP()
{
	int returnValue = 0;
	BOOST_LOG_SEV(log, report) << "Client IP: ["<< this->clientIPv4<<"]\n";
	return returnValue;
}
int MyServer::showServerPort()
{
	int returnValue = 0;
	this->getPort();
	BOOST_LOG_SEV(log, report) << "Server Port Number: ["<< this->serverPortNumber<<"]\n";
	return returnValue;
}
int MyServer::showServerIP()
{
	int returnValue = 0;
	if( za::getLocalAddress(this->serverIPv4) < 0)
	{
		returnValue = -100011;
		BOOST_LOG_SEV(log, error) << "Fail to get server address from getLocalAddress"
			<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
	}
	else
	{
		BOOST_LOG_SEV(log, report) << "Server IP: ["<< this->serverIPv4<<"]\n";
	}
	return returnValue;
}
int MyServer::showMaxConnexion()
{
	int returnValue = 0;
	BOOST_LOG_SEV(log, report) << "Server max connexion: ["<< this->MAX_CONNEXION<<"]\n";
	return returnValue;
}
int MyServer::closeAcceptedConnexion()
{
	int returnValue = 0;
	BOOST_LOG_SEV(log, report)<< "A Server closing an accpeted connexion ...\n";
	if(close(socketProcessAcceptedConnexion) < 0)
	{
		returnValue = -100012;
		BOOST_LOG_SEV(log, error)<<"Fail to close a connexion with a server"
			<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
	}
	else
	{
		BOOST_LOG_SEV(log, report) <<"Accepted connexion closed\n";
		this->isConnexionActive = false;
	}
	return returnValue;
}
int MyServer::closeListenningSocket()
{
	int returnValue = 0;
	BOOST_LOG_SEV(log, report) << "A Server stopping listen to a new connexion ...\n";
	if(close(socketListenToConnexion) < 0)
	{
		returnValue = -100013;
		BOOST_LOG_SEV(log, error)<<"Fail to close a listenning socket"
			<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
	}
	else
	{
		BOOST_LOG_SEV(log, report)<<"Listenning socket closed\n";
		BOOST_LOG_SEV(log, report) << "A server stopped listen to a new connexion\n";
	}
	return returnValue;
}
int MyServer::setLog(std::string& _logName_)
{
	int returnValue = 0;
	this->logManager.set_log_file( &_logName_[0]);
	return returnValue;
}
int MyServer::setLog(std::string&& _logName_)
{
	int returnValue = 0;
	this->logManager.set_log_file( &_logName_[0]);
	return returnValue;
}

std::string MyServer::getCurrentTime()
{
	std::time_t now = std::time(NULL);
	std::tm * ptm = std::localtime(&now);
	char buffer[32];
	std::strftime(buffer, 32, "%d_%m_%Y_%H_%M_%S_", ptm);
	return std::string(buffer);
}
int MyServer::sendFileToClient(const std::string& filePath)
{
	return sendFileToClient(filePath.c_str());
}

int MyServer::sendFileToClient(const char* filePath)
{
	int returnValue = 0;
	
	if(!this->isConnexionActive)
	{
		returnValue = -100014;
		BOOST_LOG_SEV(log, error) << "No active connection to send file"
			<< "[ERROR][" << returnValue << "]\n";
		return returnValue;
	}
	
	BOOST_LOG_SEV(log, report) << "Sending file to client: " << filePath << "\n";
	
	FileAccessManager* fileManager = FileAccessManager::getInstance();
	fileManager->acquireReadLock(filePath);
	
	// Open file
	FILE* file = fopen(filePath, "rb");
	if(file == NULL)
	{
		returnValue = -100015;
		BOOST_LOG_SEV(log, error) << "Failed to open file: " << filePath
			<< "[ERROR][" << strerror(errno) << "][" << returnValue << "]\n";
		
		// Send error signal to client
		long fileSize = -1;
		write(socketProcessAcceptedConnexion, &fileSize, sizeof(fileSize));
		fileManager->releaseReadLock(filePath);
		return returnValue;
	}
	
	// Get file size
	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	BOOST_LOG_SEV(log, report) << "File size: " << fileSize << " bytes\n";
	
	// Send file size first
	if(write(socketProcessAcceptedConnexion, &fileSize, sizeof(fileSize)) < 0)
	{
		returnValue = -100016;
		BOOST_LOG_SEV(log, error) << "Failed to send file size"
			<< "[ERROR][" << strerror(errno) << "][" << returnValue << "]\n";
		fclose(file);
		fileManager->releaseReadLock(filePath);
		return returnValue;
	}
	
	// Send file data in chunks
	char fileBuffer[4096];
	size_t bytesRead;
	long totalBytesSent = 0;
	
	while((bytesRead = fread(fileBuffer, 1, sizeof(fileBuffer), file)) > 0)
	{
		ssize_t bytesSent = write(socketProcessAcceptedConnexion, fileBuffer, bytesRead);
		if(bytesSent < 0)
		{
			returnValue = -100017;
			BOOST_LOG_SEV(log, error) << "Failed to send file data"
				<< "[ERROR][" << strerror(errno) << "][" << returnValue << "]\n";
			fclose(file);
			fileManager->releaseReadLock(filePath);
			return returnValue;
		}
		totalBytesSent += bytesSent;
	}
	
	fclose(file);
	fileManager->releaseReadLock(filePath);
	
	BOOST_LOG_SEV(log, report) << "File sent successfully. Total bytes: " 
		<< totalBytesSent << "\n";
	
	return returnValue;
}
}