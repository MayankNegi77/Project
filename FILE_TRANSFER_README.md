# File Transfer Feature

## Overview

This extension adds file transfer capability from server to client in the Multi-ClientServer application. The server can now send files to connected clients using the existing TCP/IP socket infrastructure.

## New Features

### Server Side (MyServer class)
- `sendFileToClient(const std::string& filePath)` - Send a file to the connected client
- `sendFileToClient(const char* filePath)` - Overloaded version accepting C-string

### Client Side (MyClient class)
- `receiveFileFromServer(const std::string& savePath)` - Receive a file from server and save it
- `receiveFileFromServer(const char* savePath)` - Overloaded version accepting C-string

## How It Works

1. **File Size Transfer**: The server first sends the file size (as a `long` integer) to the client
2. **Error Handling**: If the server cannot open the file, it sends -1 as the file size
3. **Chunked Transfer**: The file is transferred in 4KB chunks for efficient memory usage
4. **Progress Tracking**: Both server and client log the transfer progress

## Usage Example

### Basic Integration

#### Server Side
```cpp
za::MyServer myServer(port, maxConnections);
// ... server initialization ...

// Send a file to connected client
if(myServer.sendFileToClient("./data/document.pdf") >= 0) {
    std::cout << "File sent successfully\n";
}
```

#### Client Side
```cpp
za::MyClient myClient(serverIP, serverPort);
// ... client initialization ...

// Receive file from server
if(myClient.receiveFileFromServer("./downloads/document.pdf") >= 0) {
    std::cout << "File received successfully\n";
}
```

### Demo Applications

Two demo applications are provided:

#### 1. File Transfer Server
```bash
./fileTransferServer -p 60000 -f test_file.txt
```

Options:
- `-p, --port`: Server port number (default: random available port)
- `-c, --connexion`: Maximum connections (default: 5)
- `-f, --file`: File to send to clients (default: test_file.txt)
- `-l, --log`: Log file name (default: file_server.log)

#### 2. File Transfer Client
```bash
./fileTransferClient -i 192.168.1.100 -p 60000 -s received_file.txt
```

Options:
- `-i, --ip`: Server IP address (required)
- `-p, --port`: Server port number (required)
- `-s, --save`: Path to save received file (default: received_file.txt)
- `-l, --log`: Log file name (default: file_client.log)

## Building the Project

To build with the new file transfer demos:

1. Update CMakeLists.txt to include the new demo directories
2. Build the project:
```bash
cd build
cmake ..
make -j4
```

## Protocol Details

### File Transfer Protocol

1. Client sends a request to server (existing mechanism)
2. Server sends file size (8 bytes, `long` type)
   - If file size < 0: Error occurred, file not available
   - If file size >= 0: File transfer will proceed
3. Server sends file data in chunks (4096 bytes per chunk)
4. Client receives and writes data until all bytes received
5. Server sends confirmation message (existing mechanism)

### Error Codes

#### Server Errors
- `-100014`: No active connection
- `-100015`: Failed to open file
- `-100016`: Failed to send file size
- `-100017`: Failed to send file data

#### Client Errors
- `-200005`: Failed to receive file size
- `-200006`: Server failed to open file
- `-200007`: Failed to create local file
- `-200008`: Failed to receive file data
- `-200009`: Connection closed prematurely

## Custom Processor Example

You can create custom processors for specific file transfer scenarios:

```cpp
#include "ProcessFileTransfer.h"

void ProcessFileTransfer::process(za::MyServer server)
{
    while(true)
    {
        // Read client request
        if(server.readIncomingMessageFromClient() >= 0)
        {
            // Parse request and determine which file to send
            const char* fileToSend = "./files/requested_file.dat";
            
            if(server.sendFileToClient(fileToSend) >= 0)
            {
                server.sendResponseToClient("file_sent", 200);
            }
            else
            {
                server.sendResponseToClient("file_error", 400);
            }
        }
        else
        {
            break; // Client disconnected
        }
    }
}
```

## Testing

1. Create a test file:
```bash
echo "Hello from server!" > test_file.txt
```

2. Start the server:
```bash
./fileTransferServer -p 60000 -f test_file.txt
```

3. In another terminal, start the client:
```bash
./fileTransferClient -i 127.0.0.1 -p 60000 -s received.txt
```

4. Verify the received file:
```bash
cat received.txt
```

## Limitations

- Binary safe: Works with both text and binary files
- File size limit: Limited by available memory and `long` type (typically 2GB on 32-bit, much larger on 64-bit)
- No compression: Files are sent as-is
- No encryption: Data is sent in plain text over TCP
- Single file per request: Each request transfers one file

## Future Enhancements

Potential improvements:
- Multiple file transfer in one session
- File compression before transfer
- Encryption support (TLS/SSL)
- Resume capability for interrupted transfers
- Progress callbacks
- Bidirectional file transfer (client to server)
- File metadata transfer (permissions, timestamps)

## License

Same as the main project - GNU General Public License v3.0
