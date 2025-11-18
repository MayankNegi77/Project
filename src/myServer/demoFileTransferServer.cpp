#include "MyServer.h"
#include "MyThread.h"
#include "Manager.h"
#include "ProcessFileTransfer.h"
#include "ServerStats.h"
#include <iostream>
#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include "GetLocalAddress.h"
#include <boost/program_options.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <algorithm>
#include <iterator>

using namespace boost;
namespace po = boost::program_options;

const char logName[] = "file_server.log";

void signalHandler(int signal) {
	za::ServerStats* stats = za::ServerStats::getInstance();
	std::cout << "\n\nServer shutting down...\n";
	stats->printStats();
	stats->printClientList();
	
	std::vector<std::string> recentLogs = stats->getRecentLogs(5);
	std::cout << "\nRecent Activity:\n";
	for (const auto& log : recentLogs) {
		std::cout << log << "\n";
	}
	
	exit(0);
}

int main(int argc, char const *argv[])
{
	int returnValue;
	std::string localIP;
	std::string logServer;
	std::string logPath = "./log/";
	std::string fileToSend;
	unsigned short localPort;
	unsigned short maxConnexion;
	
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

	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	
	za::ServerStats* stats = za::ServerStats::getInstance();
	
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
		
		std::string clientIP = myServer.getClientIPAddress();
		unsigned short clientPort = myServer.getClientPortNumber();
		
		std::cout << "\n========================================\n";
		std::cout << "[Client #" << clientCount << "] Connected!\n";
		std::cout << "  IP Address: " << clientIP << "\n";
		std::cout << "  Port: " << clientPort << "\n";
		std::cout << "  Spawning thread to handle file transfer...\n";
		std::cout << "========================================\n";
		
		int clientSocket = myServer.getAcceptedSocket();
		za::MyThread *myThread = new za::MyThread(
			new za::ProcessFileTransfer(fileToSend, clientIP, clientPort), 
			clientSocket
		);
		myThread->createMyThread();
	}
	
	return 0;
}
