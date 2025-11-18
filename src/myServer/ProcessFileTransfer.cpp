#include "ProcessFileTransfer.h"
#include "ServerStats.h"
#include <iostream>
#include <sys/stat.h>

namespace za{

ProcessFileTransfer::ProcessFileTransfer() : filePath_("./test_file.txt"), clientIP_("unknown"), clientPort_(0)
{
}

ProcessFileTransfer::ProcessFileTransfer(const std::string& filePath) : filePath_(filePath), clientIP_("unknown"), clientPort_(0)
{
}

ProcessFileTransfer::ProcessFileTransfer(const std::string& filePath, const std::string& clientIP, unsigned short clientPort) 
	: filePath_(filePath), clientIP_(clientIP), clientPort_(clientPort)
{
}

void ProcessFileTransfer::process(za::MyServer _serverProcessingAcceptedConnexion_)
{
	bool flag = true;
	int transferCount = 0;
	ServerStats* stats = ServerStats::getInstance();
	
	stats->incrementConnections();
	stats->addClientConnection(clientIP_);
	stats->addLogEntry("Client " + clientIP_ + ":" + std::to_string(clientPort_) + " connected");

	while(flag)
	{
		if(_serverProcessingAcceptedConnexion_.readIncomingMessageFromClient() >= 0)
		{
			transferCount++;
			
			struct stat fileStat;
			long fileSize = 0;
			if (stat(filePath_.c_str(), &fileStat) == 0) {
				fileSize = fileStat.st_size;
			}
			
			if(_serverProcessingAcceptedConnexion_.sendFileToClient(filePath_) >= 0)
			{
				_serverProcessingAcceptedConnexion_.sendResponseToClient("file_sent_ok      ", 200);
				
				stats->addBytesTransferred(fileSize);
				stats->incrementFilesTransferred();
				stats->addLogEntry("File sent to " + clientIP_ + " (" + std::to_string(fileSize) + " bytes)");
				
				std::cout << "[" << clientIP_ << ":" << clientPort_ << "] Transfer #" << transferCount 
				          << " - File sent successfully (" << fileSize << " bytes)\n";
				
				stats->printStats();
			}
			else
			{
				_serverProcessingAcceptedConnexion_.sendResponseToClient("file_error        ", 400);
				stats->addLogEntry("Failed to send file to " + clientIP_);
				
				std::cout << "[" << clientIP_ << ":" << clientPort_ << "] Transfer #" << transferCount 
				          << " - Failed to send file\n";
			}
			
			usleep(100000);
		} 
		else 
		{
			_serverProcessingAcceptedConnexion_.sendResponseToClient("connection_closed ", 400);		
			flag = false;
			
			stats->decrementConnections();
			stats->removeClientConnection(clientIP_);
			stats->addLogEntry("Client " + clientIP_ + " disconnected after " + std::to_string(transferCount) + " transfers");
			
			std::cout << "[" << clientIP_ << ":" << clientPort_ << "] Disconnected - Total transfers: " 
			          << transferCount << "\n";
		}
	}
}
}
