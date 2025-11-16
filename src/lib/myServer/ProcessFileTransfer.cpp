#include "ProcessFileTransfer.h"

namespace za{
void ProcessFileTransfer::process(za::MyServer _serverProcessingAcceptedConnexion_)
{
	bool flag = true;

	while(flag)
	{
		// Read client request (could be a file request command)
		if(_serverProcessingAcceptedConnexion_.readIncomingMessageFromClient() >= 0)
		{
			// Example: Send a file to the client
			// In a real scenario, you would parse the client request to determine which file to send
			const char* fileToSend = "./test_file.txt"; // Example file path
			
			if(_serverProcessingAcceptedConnexion_.sendFileToClient(fileToSend) >= 0)
			{
				_serverProcessingAcceptedConnexion_.sendResponseToClient("file_sent", 200);
			}
			else
			{
				_serverProcessingAcceptedConnexion_.sendResponseToClient("file_error", 400);
			}
			
			usleep(2000000); // Wait 2 seconds before next transfer
		} 
		else 
		{
			_serverProcessingAcceptedConnexion_.sendResponseToClient("fail", 400);		
			flag = false;
		}
	}
}
}
