#include "ProcessSingleClient.h"
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>
namespace za{
void ProcessSingleClient::process(int clientSocket)
{
	bool flag = true;
	char buffer[1024];
	while(flag)
	{
		memset(buffer, 0, sizeof(buffer));
		ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer));
		if (bytesRead < 0)
		{
			std::cerr << "Error reading from socket\n";
			flag = false;
		}
		else if(bytesRead == 0)
		{
			std::cout << "Client disconnected\n";
			flag = false;
		}
		else
		{
			std::cout << "Time received from client: " << buffer << "\n";
			std::string response = "sucess            ";
			write(clientSocket, response.c_str(), 18);
			usleep(6000000);
		}
	}
	close(clientSocket);
}
}