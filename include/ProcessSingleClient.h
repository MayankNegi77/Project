#ifndef PROCESS_SINGLE_CLIENT_H
#define PROCESS_SINGLE_CLIENT_H
#include "MyProcessor.h"
namespace za{
class ProcessSingleClient:public za::MyProcessor
{
public:
	void process(int clientSocket);
};
}
#endif

