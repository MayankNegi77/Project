#include <iostream>
#include <iostream>     
#include <cstring>     
#include <errno.h>      
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>  
#include <unistd.h>    
#include<string.h>
#include<netinet/in.h> 
#include <cstring>      

namespace za{
int getLocalAddress(std::string& myIP);
}