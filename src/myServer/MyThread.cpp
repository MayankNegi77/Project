#include "MyThread.h"
namespace za{
void *MyThread::functionThread(void *arg)
{
	MyThread * myThread = (MyThread *) arg;
	myThread->processor->process(myThread->clientSocket);
	delete myThread->processor;
	delete myThread;
	return NULL;
}
void MyThread::createMyThread()
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&this->threadID, &attr, &this->functionThread, this);
	pthread_attr_destroy(&attr);
}
MyThread::MyThread(za::MyProcessor *_processor_, int _clientSocket_)
{
	processor = _processor_;
	clientSocket = _clientSocket_;
}
MyThread::~MyThread()
{
}
}