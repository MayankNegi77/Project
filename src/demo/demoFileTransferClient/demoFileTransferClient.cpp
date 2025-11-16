#include <iostream>
#include "MyClient.h"
#include <unistd.h>
#include <boost/program_options.hpp>
#include <algorithm>
#include <iterator>

using namespace boost;
namespace po = boost::program_options;

int main(int argc, char *argv[])
{
	int returnValue;
	std::string serverIP;
	std::string logClient;
	std::string logPath = "./log/";
	std::string saveFilePath;
	unsigned short serverPort;
	
	try 
	{
		po::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "produce help message")
			("ip,i", po::value<std::string>(),
				"server IP address")
			("port,p", po::value<unsigned short>(),
				"server port number.")
			("save,s", po::value<std::string>(&saveFilePath)->default_value("received_file.txt"),
				"path to save received file")
			("log,l", po::value<std::string>(&logClient)->default_value("file_client.log")
				->implicit_value("file_client.log"), "client log path");
				
		po::variables_map vm;        
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);  

		if (vm.count("help")) 
		{
			std::cout << "Usage: options_description [options]\n";
			std::cout << desc;
			return 0;
		}
		
		if (vm.count("ip")) 
		{
			serverIP = vm["ip"].as<std::string>();
		} 
		else 
		{
			std::cout << "Server IP was not set.\n";
			return -1;
		}
		
		if (vm.count("port")) 
		{
			serverPort = vm["port"].as<unsigned short>();
		} 
		else 
		{
			std::cout << "Server port number was not set.\n";
			return -1;
		}
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << "\n";
		return 1;
	}

	za::MyClient myClient(&serverIP[0], serverPort);
	if(myClient.setLog(logPath + std::get<0>(myClient.getCurrentTime()) + logClient) < 0) 
		return -1;
	
	std::cout << "File Transfer Client started. Requesting file from server...\n";
	
	// Send request to server
	if(myClient.sendRequestToServer() < 0) return -1;
	
	// Receive file from server
	if(myClient.receiveFileFromServer(saveFilePath) < 0)
	{
		std::cout << "Failed to receive file from server\n";
		return -1;
	}
	
	std::cout << "File received successfully and saved to: " << saveFilePath << "\n";
	
	// Receive confirmation response
	if(myClient.receiveResponseFromServer() < 0) return -1;
	
	if(myClient.closeConnexion() < 0) return -1;
	
	return 0;
}
