# Integration Example: Adding File Transfer to Existing Code

## Quick Integration Guide

### Option 1: Modify Existing ProcessSingleClient

You can add file transfer capability to your existing `ProcessSingleClient` class:

```cpp
// In ProcessSingleClient.cpp
#include "ProcessSingleClient.h"

namespace za{
void ProcessSingleClient::process(za::MyServer _serverProcessingAcceptedConnexion_)
{
    bool flag = true;
    int requestCount = 0;

    while(flag)
    {
        if(_serverProcessingAcceptedConnexion_.readIncomingMessageFromClient() >= 0)
        {
            _serverProcessingAcceptedConnexion_.sendResponseToClient("success", 200);
            requestCount++;
            
            // After 3 requests, send a file to the client
            if(requestCount == 3)
            {
                // Send a file to the client
                if(_serverProcessingAcceptedConnexion_.sendFileToClient("./data/report.txt") >= 0)
                {
                    std::cout << "File sent to client successfully\n";
                }
                requestCount = 0; // Reset counter
            }
            
            usleep(6000000);
        } 
        else 
        {
            _serverProcessingAcceptedConnexion_.sendResponseToClient("fail", 400);        
            flag = false;
        }
    }
}
}
```

### Option 2: Create a Command-Based System

Implement a simple command protocol where clients can request specific files:

#### Server Side (ProcessSingleClient.cpp)
```cpp
#include "ProcessSingleClient.h"
#include <string>
#include <cstring>

namespace za{
void ProcessSingleClient::process(za::MyServer _serverProcessingAcceptedConnexion_)
{
    bool flag = true;

    while(flag)
    {
        if(_serverProcessingAcceptedConnexion_.readIncomingMessageFromClient() >= 0)
        {
            // Parse the client message to check for file request
            // Assuming message format: "FILE:filename.txt" or "TIME"
            
            // For simplicity, if message contains "FILE:", send a file
            // Otherwise, send normal response
            
            // Example: Send a specific file based on request
            if(_serverProcessingAcceptedConnexion_.sendFileToClient("./files/data.bin") >= 0)
            {
                _serverProcessingAcceptedConnexion_.sendResponseToClient("file_sent", 200);
            }
            else
            {
                _serverProcessingAcceptedConnexion_.sendResponseToClient("success", 200);
            }
            
            usleep(6000000);
        } 
        else 
        {
            _serverProcessingAcceptedConnexion_.sendResponseToClient("fail", 400);        
            flag = false;
        }
    }
}
}
```

#### Client Side (demoClient.cpp)
```cpp
#include <iostream>
#include "MyClient.h"
#include <unistd.h>
#include <boost/program_options.hpp>

using namespace boost;
namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    // ... (existing command line parsing) ...
    
    za::MyClient myClient(&serverIP[0], serverPort);
    if(myClient.setLog(logPath + std::get<0>(myClient.getCurrentTime()) + logClient) < 0) 
        return -1;
    
    int requestCount = 0;
    
    while(true)
    {
        // Send time request
        if(myClient.sendRequestToServer() < 0) return -1;
        
        requestCount++;
        
        // Every 3rd request, expect a file transfer
        if(requestCount == 3)
        {
            // Receive file from server
            std::string filename = "received_" + std::to_string(time(NULL)) + ".txt";
            if(myClient.receiveFileFromServer(filename) >= 0)
            {
                std::cout << "File received: " << filename << "\n";
            }
            requestCount = 0;
        }
        
        // Receive normal response
        if(myClient.receiveResponseFromServer() < 0) return -1;
        
        usleep(6000000);
    }

    if(myClient.closeConnexion() < 0) return -1;
    return 0;
}
```

### Option 3: Separate File Transfer Session

Create a dedicated file transfer session after initial handshake:

```cpp
// Server sends a signal that file transfer is available
_serverProcessingAcceptedConnexion_.sendResponseToClient("file_ready", 200);

// Then immediately send the file
_serverProcessingAcceptedConnexion_.sendFileToClient("./important_data.dat");

// Continue with normal operations
_serverProcessingAcceptedConnexion_.sendResponseToClient("file_complete", 200);
```

## Complete Working Example

Here's a minimal complete example:

### server_with_file.cpp
```cpp
#include "MyServer.h"
#include "MyThread.h"
#include "ProcessSingleClient.h"

int main()
{
    za::MyServer myServer(60000, 5);
    myServer.setLog("./log/server.log");
    myServer.createServerForBindAndListen();
    myServer.bindServerToPortNumber();
    myServer.listenToNewClientConnexion();

    while (true)
    {
        myServer.acceptToMakeConnexionWithClient();
        
        // Simple inline processing with file transfer
        if(myServer.readIncomingMessageFromClient() >= 0)
        {
            // Send a file
            myServer.sendFileToClient("./test.txt");
            myServer.sendResponseToClient("done", 200);
        }
    }
    
    return 0;
}
```

### client_receive_file.cpp
```cpp
#include "MyClient.h"

int main()
{
    za::MyClient myClient("127.0.0.1", 60000);
    myClient.setLog("./log/client.log");
    
    // Send request
    myClient.sendRequestToServer();
    
    // Receive file
    myClient.receiveFileFromServer("./received.txt");
    
    // Receive response
    myClient.receiveResponseFromServer();
    
    myClient.closeConnexion();
    return 0;
}
```

## Testing Your Integration

1. **Create a test file:**
```bash
echo "Test data for file transfer" > test.txt
```

2. **Compile your modified code:**
```bash
cd build
cmake ..
make -j4
```

3. **Run server:**
```bash
./bin/server -p 60000
```

4. **Run client:**
```bash
./bin/client -i 127.0.0.1 -p 60000
```

5. **Verify the file was received:**
```bash
ls -la received.txt
cat received.txt
```

## Tips

- Always check return values from file transfer functions
- Ensure the file exists on the server before attempting to send
- Make sure the client has write permissions in the save directory
- Use absolute paths or ensure working directory is correct
- Check logs for detailed error messages
- Test with both small and large files
- Test with binary files (images, PDFs, etc.) to ensure binary safety

## Common Issues

1. **File not found**: Ensure the file path is correct relative to the server's working directory
2. **Permission denied**: Check file and directory permissions
3. **Connection reset**: Ensure both client and server are using the same protocol sequence
4. **Incomplete transfer**: Check network stability and buffer sizes
5. **Wrong file size**: Ensure endianness is consistent (not an issue on same architecture)
