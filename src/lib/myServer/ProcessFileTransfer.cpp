#include "ProcessFileTransfer.h"
#include <iostream>

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

	while(flag)
	{
		if(_serverProcessingAcceptedConnexion_.readIncomingMessageFromClient() >= 0)
		{
			transferCount++;
			if(_serverProcessingAcceptedConnexion_.sendFileToClient(filePath_) >= 0)
			{
				_serverProcessingAcceptedConnexion_.sendResponseToClient("file_sent_ok      ", 200);
				std::cout << "[" << clientIP_ << ":" << clientPort_ << "] Transfer #" << transferCount 
				          << " - File sent successfully\n";
			}
			else
			{
				_serverProcessingAcceptedConnexion_.sendResponseToClient("file_error        ", 400);
				std::cout << "[" << clientIP_ << ":" << clientPort_ << "] Transfer #" << transferCount 
				          << " - Failed to send file\n";
			}
			
			usleep(100000);
		} 
		else 
		{
			_serverProcessingAcceptedConnexion_.sendResponseToClient("connection_closed ", 400);		
			flag = false;
			std::cout << "[" << clientIP_ << ":" << clientPort_ << "] Disconnected - Total transfers: " 
			          << transferCount << "\n";
		}
	}
}
}
