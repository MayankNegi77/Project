#include "MyClient.h"
namespace za{
MyClient::MyClient(char *_serverIP_, unsigned short _serverPort_)
{
	BOOST_LOG_SEV(log, report)<<"Client initializing ...\n";
	serverPortNumber() = _serverPort_;
	this->createClientForConnexion();
	serverIP_ = gethostbyname(_serverIP_);
	if (serverIP_ == NULL)
	{
		BOOST_LOG_SEV(log, report)<<"Error host not found"
					<<strerror(h_errno)<<"\n";
	}
	else
	{
		BOOST_LOG_SEV(log, report)<<"Client initialized\n";
	}
	this->connectClientToServer();
}
int MyClient::createClientForConnexion()
{
	int returnValue = 0;
	BOOST_LOG_SEV(log, report) << "Creating a client socket ...\n";
	socketConnectToClient_ = socket(AF_INET, SOCK_STREAM, 0);
	if (socketConnectToClient_ < 0)
	{
		returnValue = -200000;
		BOOST_LOG_SEV(log, error) <<"Error while creating a client socket"
	<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
	}
	else
	{
		BOOST_LOG_SEV(log, report) << "A client socket created\n";
	}
	return returnValue;
}
int MyClient::connectClientToServer()
{
	int returnValue = 0;
	BOOST_LOG_SEV(log, report) << "Connecting to a server ...\n";
	bzero((char *) &serverAddress(), sizeof(serverAddress()));
	serverAddress().sin_family = AF_INET;
	bcopy((char *)serverIP_->h_addr, (char *)&serverAddress().sin_addr.s_addr,
	serverIP_->h_length);
	serverAddress().sin_port = htons(serverPortNumber());
	if (connect(socketConnectToClient_,(struct sockaddr *) &serverAddress(), 
	sizeof(serverAddress())) < 0)
	{
		returnValue = -200001;
		BOOST_LOG_SEV(log, error)<<"Error while connecting to a server"
			<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
	}
	else
	{
		BOOST_LOG_SEV(log, report) << "A client connected to a server\n";
	}
	return returnValue;
}
int MyClient::sendRequestToServer()
{
	int returnValue = 0;
	BOOST_LOG_SEV(log, report) << "A Client sending a request ...\n";
	std::string data = std::get<1>(getCurrentTime());
	numberOfByteOfBuffer() = write(socketConnectToClient_,data.c_str(),18);
	if (numberOfByteOfBuffer() < 0 )
	{
		returnValue = -200002;
		BOOST_LOG_SEV(log, error)<<"Error of sending a request to a server"
			<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
	}
	else
	{
		BOOST_LOG_SEV(log, report) << "Request sent to a server\n";
	}
	return returnValue;
}

int MyClient::receiveResponseFromServer()
{
	int returnValue = 0;
	BOOST_LOG_SEV(log, report) << "A Client receiving a response ...\n";
	bzero(buffer,1024);
	numberOfByteOfBuffer() = read(socketConnectToClient_,buffer,1023);
	if (numberOfByteOfBuffer() < 0)
	{
		returnValue = -200003;
		BOOST_LOG_SEV(log, error)<<"Fail to receive response from a server"
			<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
	}
	else
	{
		BOOST_LOG_SEV(log, report) << "Response from a server\n";
		BOOST_LOG_SEV(log, report) << buffer << "\n";
	}
	return returnValue;
}

int MyClient::closeConnexion()
{
	int returnValue = 0;
	BOOST_LOG_SEV(log, report) << "A Client closing a connexion ...\n";
	if(close(socketConnectToClient_) < 0)
	{
		returnValue = -200004;
		BOOST_LOG_SEV(log, error)<<"Fail to close a connexion with a server"
			<<"[ERROR]["<<strerror(errno)<<"]["<<returnValue<<"]\n";
	}
	else
	{
		BOOST_LOG_SEV(log, report)<<"Connexion with a server closed\n";
	}
	return returnValue;
}
int MyClient::setLog(std::string& _logName_)
{
	int returnValue = 0;
	this->logManager.set_log_file( &_logName_[0]);
	return returnValue;
}
int MyClient::setLog(std::string&& _logName_)
{
	int returnValue = 0;
	this->logManager.set_log_file( &_logName_[0]);
	return returnValue;
}

std::tuple <std::string, std::string> MyClient::getCurrentTime()
{
	std::time_t now = std::time(NULL);
	std::tm * ptm = std::localtime(&now);
	char buffer1[32], buffer2[32];
	std::strftime(buffer1, 32, "%d_%m_%Y_%H_%M_%S_", ptm);
	std::strftime(buffer2, 32, "%H:%M:%S", ptm);
	return std::make_tuple(std::string(buffer1), std::string(buffer2));
}
int MyClient::receiveFileFromServer(const std::string& savePath)
{
	return receiveFileFromServer(savePath.c_str());
}

int MyClient::receiveFileFromServer(const char* savePath)
{
	int returnValue = 0;
	BOOST_LOG_SEV(log, report) << "Receiving file from server...\n";
	long fileSize;
	if(read(socketConnectToClient_, &fileSize, sizeof(fileSize)) < 0)
	{
		returnValue = -200005;
		BOOST_LOG_SEV(log, error) << "Failed to receive file size"
			<< "[ERROR][" << strerror(errno) << "][" << returnValue << "]\n";
		return returnValue;
	}
	if(fileSize < 0)
	{
		returnValue = -200006;
		BOOST_LOG_SEV(log, error) << "Server failed to open file"
			<< "[ERROR][" << returnValue << "]\n";
		return returnValue;
	}
	BOOST_LOG_SEV(log, report) << "File size: " << fileSize << " bytes\n";
	FILE* file = fopen(savePath, "wb");
	if(file == NULL)
	{
		returnValue = -200007;
		BOOST_LOG_SEV(log, error) << "Failed to create file: " << savePath
			<< "[ERROR][" << strerror(errno) << "][" << returnValue << "]\n";
		return returnValue;
	}
	char fileBuffer[4096];
	long totalBytesReceived = 0;
	while(totalBytesReceived < fileSize)
	{
		long bytesToRead = (fileSize - totalBytesReceived < sizeof(fileBuffer)) 
			? (fileSize - totalBytesReceived) : sizeof(fileBuffer);
		ssize_t bytesRead = read(socketConnectToClient_, fileBuffer, bytesToRead);
		if(bytesRead < 0)
		{
			returnValue = -200008;
			BOOST_LOG_SEV(log, error) << "Failed to receive file data"
				<< "[ERROR][" << strerror(errno) << "][" << returnValue << "]\n";
			fclose(file);
			return returnValue;
		}
		if(bytesRead == 0)
		{
			returnValue = -200009;
			BOOST_LOG_SEV(log, error) << "Connection closed before file transfer completed"
				<< "[ERROR][" << returnValue << "]\n";
			fclose(file);
			return returnValue;
		}
		
		fwrite(fileBuffer, 1, bytesRead, file);
		totalBytesReceived += bytesRead;
	}
	
	fclose(file);
	
	BOOST_LOG_SEV(log, report) << "File received successfully. Total bytes: " 
		<< totalBytesReceived << "\n";
	BOOST_LOG_SEV(log, report) << "File saved to: " << savePath << "\n";
	
	return returnValue;
}
}

