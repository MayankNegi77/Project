#include "ProcessFileTransfer.h"
#include "ServerStats.h"
#include "FileAccessManager.h"
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
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
void ProcessFileTransfer::process(int clientSocket)
{
	bool flag = true;
	int transferCount = 0;
	ServerStats* stats = ServerStats::getInstance();
	FileAccessManager* fileManager = FileAccessManager::getInstance();
	stats->incrementConnections();
	stats->addClientConnection(clientIP_);
	stats->addLogEntry("Client " + clientIP_ + ":" + std::to_string(clientPort_) + " connected");
	char buffer[1024];
	while(flag)
	{
		memset(buffer, 0, sizeof(buffer));
		ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer));
		if (bytesRead < 0)
		{
			std::cerr << "[" << clientIP_ << ":" << clientPort_ << "] Error reading from socket\n";
			flag = false;
		}
		else if(bytesRead == 0)
		{
			std::cout << "[" << clientIP_ << ":" << clientPort_ << "] Client disconnected\n";
			flag = false;
		}
		else
		{
			transferCount++;
			struct stat fileStat;
			long fileSize = 0;
			if (stat(filePath_.c_str(), &fileStat) == 0) {
				fileSize = fileStat.st_size;
			}
			fileManager->acquireReadLock(filePath_);
			FILE* file = fopen(filePath_.c_str(), "rb");
			if(file == NULL)
			{
				long errorSize = -1;
				write(clientSocket, &errorSize, sizeof(errorSize));
				std::string errorMsg = "file_error        ";
				write(clientSocket, errorMsg.c_str(), 18);
				fileManager->releaseReadLock(filePath_);
				stats->addLogEntry("Failed to open file for " + clientIP_);
				std::cout << "[" << clientIP_ << ":" << clientPort_ << "] Transfer #" << transferCount 
				          << " - Failed to open file\n";
			}
			else
			{
				fseek(file, 0, SEEK_END);
				long actualFileSize = ftell(file);
				fseek(file, 0, SEEK_SET);
				if(write(clientSocket, &actualFileSize, sizeof(actualFileSize)) < 0)
				{
					fclose(file);
					fileManager->releaseReadLock(filePath_);
					flag = false;
					continue;
				}
				char fileBuffer[4096];
				size_t bytesReadFromFile;
				long totalBytesSent = 0;
				bool transferSuccess = true;
				while((bytesReadFromFile = fread(fileBuffer, 1, sizeof(fileBuffer), file)) > 0)
				{
					ssize_t bytesSent = write(clientSocket, fileBuffer, bytesReadFromFile);
					if(bytesSent < 0)
					{
						transferSuccess = false;
						std::cerr << "[" << clientIP_ << ":" << clientPort_ << "] Error writing file data\n";
						break;
					}
					totalBytesSent += bytesSent;
				}
				fclose(file);
				fileManager->releaseReadLock(filePath_);
				if(transferSuccess)
				{
					std::string successMsg = "file_sent_ok      ";
					write(clientSocket, successMsg.c_str(), 18);
					stats->addBytesTransferred(totalBytesSent);
					stats->incrementFilesTransferred();
					stats->addLogEntry("File sent to " + clientIP_ + " (" + std::to_string(totalBytesSent) + " bytes)");
					std::cout << "[" << clientIP_ << ":" << clientPort_ << "] Transfer #" << transferCount 
					          << " - File sent successfully (" << totalBytesSent << " bytes)\n";
					stats->printStats();
				}
				else
				{
					std::string errorMsg = "transfer_failed   ";
					write(clientSocket, errorMsg.c_str(), 18);
					stats->addLogEntry("Transfer failed to " + clientIP_);
					flag = false;
				}
			}
			usleep(100000);
		}
	}
	stats->decrementConnections();
	stats->removeClientConnection(clientIP_);
	stats->addLogEntry("Client " + clientIP_ + " disconnected after " + std::to_string(transferCount) + " transfers");
	std::cout << "[" << clientIP_ << ":" << clientPort_ << "] Disconnected - Total transfers: " 
	          << transferCount << "\n";
	close(clientSocket);
}
}
