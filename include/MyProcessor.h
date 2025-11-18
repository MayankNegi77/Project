#ifndef MY_PROCESSOR_H
#define MY_PROCESSOR_H
#include "MyServer.h"

namespace za{
class MyProcessor
{
public:

	virtual void process(za::MyServer serverProcessingAcceptedConnexion) = 0;

};
}

#endif 
