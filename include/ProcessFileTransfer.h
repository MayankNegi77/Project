#ifndef PROCESS_FILE_TRANSFER_H
#define PROCESS_FILE_TRANSFER_H
#include "MyProcessor.h"

namespace za{
class ProcessFileTransfer:public za::MyProcessor
{
public:

	std::string filePath_;
	std::string clientIP_;
	unsigned short clientPort_;

	ProcessFileTransfer();
	ProcessFileTransfer(const std::string& filePath);
	ProcessFileTransfer(const std::string& filePath, const std::string& clientIP, unsigned short clientPort);
	void process(za::MyServer _serverProcessingAcceptedConnexion_);

};
}
#endif
