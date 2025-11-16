# Quick Start: File Transfer Feature

## What Was Added

File transfer capability from server to client has been added to your Multi-ClientServer project.

## New Functions

### Server (MyServer class)
```cpp
int sendFileToClient(const std::string& filePath);
int sendFileToClient(const char* filePath);
```

### Client (MyClient class)
```cpp
int receiveFileFromServer(const std::string& savePath);
int receiveFileFromServer(const char* savePath);
```

## Simplest Usage

### Server sends a file:
```cpp
myServer.sendFileToClient("./myfile.txt");
```

### Client receives a file:
```cpp
myClient.receiveFileFromServer("./received_file.txt");
```

## Return Values
- `0` = Success
- `< 0` = Error (check logs for details)

## Example Workflow

1. **Server side:**
```cpp
// After accepting client connection
if(myServer.readIncomingMessageFromClient() >= 0)
{
    // Send a file
    if(myServer.sendFileToClient("./data.bin") >= 0)
    {
        myServer.sendResponseToClient("file_sent", 200);
    }
}
```

2. **Client side:**
```cpp
// After sending request
myClient.sendRequestToServer();

// Receive the file
if(myClient.receiveFileFromServer("./downloaded.bin") >= 0)
{
    std::cout << "File received!\n";
}

// Get server response
myClient.receiveResponseFromServer();
```

## File Types Supported
- Text files (.txt, .log, .csv, etc.)
- Binary files (.pdf, .jpg, .png, .zip, etc.)
- Any file type - the transfer is binary-safe

## Demo Applications

Two complete demo applications are provided:

### File Transfer Server
Location: `src/demo/demoFileTransferServer/`

Run:
```bash
./fileTransferServer -p 60000 -f myfile.txt
```

### File Transfer Client
Location: `src/demo/demoFileTransferClient/`

Run:
```bash
./fileTransferClient -i 192.168.1.100 -p 60000 -s received.txt
```

## Testing

1. Create a test file:
```bash
echo "Hello World" > test.txt
```

2. Modify your existing server code to send it:
```cpp
// In ProcessSingleClient.cpp, add:
_serverProcessingAcceptedConnexion_.sendFileToClient("./test.txt");
```

3. Modify your client code to receive it:
```cpp
// In demoClient.cpp, add after sendRequestToServer():
myClient.receiveFileFromServer("./received.txt");
```

4. Rebuild and test:
```bash
cd build
cmake ..
make -j4
```

## Important Notes

- File paths are relative to the executable's working directory
- Server must have read permission on the file
- Client must have write permission in the save directory
- Large files are transferred in 4KB chunks (memory efficient)
- Check return values to handle errors properly
- All transfers are logged automatically

## Need Help?

- Check `FILE_TRANSFER_README.md` for detailed documentation
- Check `INTEGRATION_EXAMPLE.md` for integration examples
- Review the demo applications in `src/demo/demoFileTransfer*/`
- Check log files in `./log/` directory for error details

## Error Codes Quick Reference

### Server Errors
- `-100014`: No active connection
- `-100015`: Cannot open file
- `-100016`: Cannot send file size
- `-100017`: Cannot send file data

### Client Errors
- `-200005`: Cannot receive file size
- `-200006`: Server cannot open file
- `-200007`: Cannot create local file
- `-200008`: Cannot receive file data
- `-200009`: Connection closed during transfer
