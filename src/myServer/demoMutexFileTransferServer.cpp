#include "MyServer.h"
#include "MyThread.h"
#include "Manager.h"
#include "ProcessFileTransfer.h"
#include "ServerStats.h"
#include "FileAccessManager.h"
#include <iostream>
#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <thread>
#include <chrono>
#include "GetLocalAddress.h"
#include <boost/program_options.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <algorithm>
#include <iterator>

using namespace boost;
namespace po = boost::program_options;

const char logName[] = "mutex_file_server.log";
bool serverRunning = true;

void signalHandler(int signal) {
	za::ServerStats* stats = za::ServerStats::getInstance();
	za::FileAccessManager* fileManager = za::FileAccessManager::getInstance();
	
	std::cout << "\n\n=== SERVER SHUTDOWN INITIATED ===\n";
	stats->printStats();
	stats->printClientList();
	fileManager->printFileAccess();
	
	std::vector<std::string> recentLogs = stats->getRecentLogs(10);
	std::cout << "\nRecent Activity Log:\n";
	std::cout << "========================================\n";
	for (const auto& log : recentLogs) {
		std::cout << log << "\n";
	}
	std::cout << "========================================\n";
	
	serverRunning = false;
	exit(0);
}

void statsMonitor() {
	za::ServerStats* stats = za::ServerStats::getInstance();
	za::FileAccessManager* fileManager = za::FileAccessManager::getInstance();
	
	while (serverRunning) {
		std::this_thread::sleep_for(std::chrono::seconds(10));
		
		if (stats->getActiveConnections() > 0) {
			std::cout << "\n=== PERIODIC STATUS UPDATE ===\n";
			stats->printStats();
			fileManager->printFileAccess();
		}
	}
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
		po::options_description desc("Mutex File Transfer Server");
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
			("log,l", po::value<std::string>(&logServer)->default_value("mutex_file_server.log")
				->implicit_value("mutex_file_server.log"), "server log name");
				
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
	za::FileAccessManager* fileManager = za::FileAccessManager::getInstance();
	
	za::MyServer myServer(localPort, maxConnexion);
	if(myServer.setLog(logPath + myServer.getCurrentTime() + logName) < 0) return -1;
	if(myServer.showServerIP() < 0) return -1;
	if(myServer.createServerForBindAndListen() < 0) return -1;
	if(myServer.bindServerToPortNumber() < 0) return -1;
	if(myServer.showServerPort() < 0) return -1;
	if(myServer.listenToNewClientConnexion() < 0) return -1;

	std::cout << "========================================\n";
	std::cout << "MUTEX-ENABLED MULTI-CLIENT FILE TRANSFER\n";
	std::cout << "========================================\n";
	std::cout << "File to send: " << fileToSend << "\n";
	std::cout << "Max connections: " << maxConnexion << "\n";
	std::cout << "Mutex protection: ENABLED\n";
	std::cout << "Statistics tracking: ENABLED\n";
	std::cout << "File access monitoring: ENABLED\n";
	std::cout << "Waiting for clients to connect...\n";
	std::cout << "Press Ctrl+C to view statistics and shutdown\n";
	std::cout << "========================================\n";

	std::thread monitorThread(statsMonitor);
	monitorThread.detach();

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
		std::cout << "  Active Connections: " << stats->getActiveConnections() + 1 << "\n";
		std::cout << "  Total Connections: " << stats->getTotalConnections() + 1 << "\n";
		std::cout << "  Spawning thread with MUTEX protection...\n";
		std::cout << "========================================\n";
		
		int clientSocket = myServer.getAcceptedSocket();
		za::MyThread *myThread = new za::MyThread(
			new za::ProcessFileTransfer(fileToSend, clientIP, clientPort), 
			clientSocket
		);
		myThread->createMyThread();
		
		if (clientCount % 3 == 0) {
			std::cout << "\n[MUTEX STATUS UPDATE]\n";
			fileManager->printFileAccess();
		}
	}
	
	return 0;
}