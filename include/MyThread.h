#ifndef MY_THREAD_H
#define MY_THREAD_H

#include <pthread.h>
#include "MyProcessor.h"
#include "MyServer.h"

namespace za{
class MyThread
{

public:

	pthread_t threadID;
	MyProcessor *processor;
	za::MyServer serverProcessingAcceptedConnexion;
	void createMyThread();
	static void *functionThread(void *arg);
	MyThread(MyProcessor * _processor_, za::MyServer& _servserverProcessingAcceptedConnexion_);
};
}
#endif