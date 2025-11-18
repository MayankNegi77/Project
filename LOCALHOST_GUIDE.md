# Terminal-to-Terminal Communication Guide (Same PC)

This guide shows how to test the server and client on the same PC using two terminals.

## Prerequisites
- Linux operating system
- CMake installed
- Boost libraries installed
- Two terminal windows open

## Step 1: Build the Project

Open Terminal 1 and build the project:

```bash
cd Multi-ClientServer
mkdir -p build
cd build
cmake ..
make -j4
```

## Step 2: Start the Server

In Terminal 1, run the server:

```bash
cd Multi-ClientServer/bin
./demoFileTransferServer --port 60000 --file ../test_file.txt --connexion 10
```

Expected output:
```
========================================
Multi-Client File Transfer Server Started
========================================
File to send: ../test_file.txt
Max connections: 10
Server IP: [YOUR_LOCAL_IP]
Server Port Number: [60000]
Waiting for clients to connect...
========================================
```

## Step 3: Start the Client

Open Terminal 2 and run the client:

```bash
cd Multi-ClientServer/bin
./demoFileTransferClient --ip 127.0.0.1 --port 60000 --save received_file.txt
```

Expected output:
```
File Transfer Client started. Requesting file from server...
File received successfully and saved to: received_file.txt
```

## Step 4: Verify File Transfer

In Terminal 2, verify the received file:

```bash
ls -lh received_file.txt
cat received_file.txt
```

Compare with original:
```bash
diff ../test_file.txt received_file.txt
```

If no output from diff, files are identical - SUCCESS!

## Testing Multiple Clients

You can open more terminals and run multiple clients simultaneously:

Terminal 3:
```bash
cd Multi-ClientServer/bin
./demoFileTransferClient --ip 127.0.0.1 --port 60000 --save received_file2.txt
```

Terminal 4:
```bash
cd Multi-ClientServer/bin
./demoFileTransferClient --ip 127.0.0.1 --port 60000 --save received_file3.txt
```

## Using Mutex-Enabled Server

For testing with mutex protection and statistics:

Terminal 1:
```bash
cd Multi-ClientServer/bin
./demoMutexFileTransferServer --port 60000 --file ../test_file.txt --connexion 10
```

This version shows:
- Real-time statistics
- Active connections count
- Mutex lock/unlock operations
- File access monitoring

Press Ctrl+C to see final statistics.

## Troubleshooting

### Port Already in Use
If you get "Address already in use" error:
```bash
sudo lsof -i :60000
kill -9 <PID>
```

Or use a different port:
```bash
./demoFileTransferServer --port 60001 --file ../test_file.txt
./demoFileTransferClient --ip 127.0.0.1 --port 60001 --save received_file.txt
```

### Permission Denied
If you get permission errors:
```bash
chmod +x demoFileTransferServer
chmod +x demoFileTransferClient
```

### File Not Found
Make sure test_file.txt exists:
```bash
cd Multi-ClientServer
echo "This is a test file for transfer" > test_file.txt
```

## Quick Test Script

Create a test script to automate:

```bash
#!/bin/bash
cd Multi-ClientServer/bin

# Start server in background
./demoFileTransferServer --port 60000 --file ../test_file.txt &
SERVER_PID=$!
sleep 2

# Run client
./demoFileTransferClient --ip 127.0.0.1 --port 60000 --save received_file.txt

# Kill server
kill $SERVER_PID

# Verify
if diff ../test_file.txt received_file.txt > /dev/null; then
    echo "SUCCESS: File transfer completed correctly!"
else
    echo "FAILED: Files do not match!"
fi
```

Save as `test_localhost.sh`, make executable, and run:
```bash
chmod +x test_localhost.sh
./test_localhost.sh
```
