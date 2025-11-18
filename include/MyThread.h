#ifndef MY_THREAD_H
#define MY_THREAD_H
#include <pthread.h>
#include "MyProcessor.h"
namespace za{
class MyThread
{
public:
	pthread_t threadID;
	MyProcessor *processor;
	int clientSocket;
	void createMyThread();
	static void *functionThread(void *arg);
	MyThread(MyProcessor * _processor_, int _clientSocket_);
	~MyThread();
};
}
#endif