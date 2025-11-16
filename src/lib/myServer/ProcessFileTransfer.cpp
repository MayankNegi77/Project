#include "ProcessFileTransfer.h"
#include <iostream>

namespace za{

// Default constructor
ProcessFileTransfer::ProcessFileTransfer() : filePath_("./test_file.txt")
{
}

// Constructor with file path
ProcessFileTransfer::ProcessFileTransfer(const std::string& filePath) : filePath_(filePath)
{
}

void ProcessFileTransfer::process(za::MyServer _serverProcessingAcceptedConnexion_)
{
	bool flag = true;
	int transferCount = 0;

	while(flag)
	{
		// Read client request (could be a file request command)
		if(_serverProcessingAcceptedConnexion_.readIncomingMessageFromClient() >= 0)
		{
			transferCount++;
			// Send the file specified in constructor
			if(_serverProcessingAcceptedConnexion_.sendFileToClient(filePath_) >= 0)
			{
				// Message must be exactly 18 bytes (protocol requirement)
				_serverProcessingAcceptedConnexion_.sendResponseToClient("file_sent_ok      ", 200);
				std::cout << "[Transfer #" << transferCount << "] File sent successfully to client\n";
			}
			else
			{
				// Message must be exactly 18 bytes (protocol requirement)
				_serverProcessingAcceptedConnexion_.sendResponseToClient("file_error        ", 400);
				std::cout << "[Transfer #" << transferCount << "] Failed to send file to client\n";
			}
			
			// Note: Client can request multiple files in same connection
			// Remove or adjust sleep if you want faster consecutive transfers
			usleep(100000); // Wait 0.1 seconds before ready for next transfer
		} 
		else 
		{
			// Message must be exactly 18 bytes (protocol requirement)
			_serverProcessingAcceptedConnexion_.sendResponseToClient("connection_closed ", 400);		
			flag = false;
			std::cout << "[Client Disconnected] Total transfers: " << transferCount << "\n";
		}
	}
}
}
