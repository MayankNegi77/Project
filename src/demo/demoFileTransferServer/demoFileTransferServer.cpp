#include "MyServer.h"
#include "MyThread.h"
#include "Manager.h"
#include "ProcessFileTransfer.h"
#include <iostream>
#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "GetLocalAddress.h"
#include <boost/program_options.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <algorithm>
#include <iterator>

using namespace boost;
namespace po = boost::program_options;

const char logName[] = "file_server.log";

int main(int argc, char const *argv[])
{
	int returnValue;
	std::string localIP;
	std::string logServer;
	std::string logPath = "./log/";
	std::string fileToSend;
	unsigned short localPort;
	unsigned short maxConnexion;
	
	// Command parser
	try 
	{
		po::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "produce help message")
			("port,p", po::value<unsigned short>(&localPort)->implicit_value(60000)
				->default_value(0,"no"),
				"server port number.")
			("connexion,c", po::value<unsigned short>(&maxConnexion)->implicit_value(10)
				->default_value(5,"few"),
				"server max connexion.")
			("file,f", po::value<std::string>(&fileToSend)->default_value("test_file.txt"),
				"file to send to clients")
			("log,l", po::value<std::string>(&logServer)->default_value("file_server.log")
				->implicit_value("file_server.log"), "server log name");
				
		po::variables_map vm;        
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);  

		if (vm.count("help")) 
		{
			std::cout << "Usage: options_description [options]\n";
			std::cout << desc;
			return 0;
		}
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << "\n";
		return 1;
	}

	za::MyServer myServer(localPort, maxConnexion);
	if(myServer.setLog(logPath + myServer.getCurrentTime() + logName) < 0) return -1;
	if(myServer.showServerIP() < 0) return -1;
	if(myServer.createServerForBindAndListen() < 0) return -1;
	if(myServer.bindServerToPortNumber() < 0) return -1;
	if(myServer.showServerPort() < 0) return -1;
	if(myServer.listenToNewClientConnexion() < 0) return -1;

	std::cout << "========================================\n";
	std::cout << "Multi-Client File Transfer Server Started\n";
	std::cout << "========================================\n";
	std::cout << "File to send: " << fileToSend << "\n";
	std::cout << "Max connections: " << maxConnexion << "\n";
	std::cout << "Waiting for clients to connect...\n";
	std::cout << "========================================\n";

	int clientCount = 0;
	while (true)
	{
		if(myServer.acceptToMakeConnexionWithClient() < 0) return -1;
		clientCount++;
		std::cout << "\n[Client #" << clientCount << "] Connected! Spawning thread to handle file transfer...\n";
		za::MyThread *myThread = new za::MyThread(new za::ProcessFileTransfer(fileToSend), myServer);
		myThread->createMyThread();
	}
	
	return 0;
}
