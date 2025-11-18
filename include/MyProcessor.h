#ifndef MY_PROCESSOR_H
#define MY_PROCESSOR_H
namespace za{
class MyProcessor
{
public:
	virtual void process(int clientSocket) = 0;
	virtual ~MyProcessor() {}
};
}
#endif 
