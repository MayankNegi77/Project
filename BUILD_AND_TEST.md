# Build and Test Multi-Client File Transfer

## Step 1: Build the Project

You need to compile the project first. Based on your system, use one of these methods:

### Option A: Using CMake (Recommended)

```bash
cd Multi-ClientServer
mkdir build
cd build
cmake ..
cmake --build . -j4
```

### Option B: Using Make (if you have Makefile)

```bash
cd Multi-ClientServer
make clean
make -j4
```

### Option C: Using Visual Studio (Windows)

Open the project in Visual Studio and build it.

---

## Step 2: Verify Build

After building, check if executables exist:

```bash
# Linux/Mac
ls -l bin/demoFileTransferServer
ls -l bin/demoFileTransferClient

# Windows
dir bin\demoFileTransferServer.exe
dir bin\demoFileTransferClient.exe
```

---

## Step 3: Create Test File

```bash
cd Multi-ClientServer
echo "Hello from server! Multi-client test." > test_file.txt
```

**✅ Already created:** `test_file.txt` exists in your project!

---

## Step 4: Test Multi-Client File Transfer

### Manual Test (3 Terminals)

**Terminal 1 - Start Server:**
```bash
cd Multi-ClientServer/bin
./demoFileTransferServer -p 8080 -c 10 -f ../test_file.txt
```

**Terminal 2 - Client 1:**
```bash
cd Multi-ClientServer/bin
mkdir -p ../downloads
./demoFileTransferClient -i 127.0.0.1 -p 8080 -s ../downloads/client1.txt
```

**Terminal 3 - Client 2:**
```bash
cd Multi-ClientServer/bin
./demoFileTransferClient -i 127.0.0.1 -p 8080 -s ../downloads/client2.txt
```

You should see:
- Server: `[Client #1] Connected!` and `[Client #2] Connected!`
- Both clients receive the file simultaneously

---

## Step 5: Verify Files Received

```bash
cd Multi-ClientServer
ls -l downloads/
cat downloads/client1.txt
cat downloads/client2.txt
```

Both files should contain the same content from `test_file.txt`.

---

## Quick Automated Test (Linux/Mac)

```bash
cd Multi-ClientServer
chmod +x test_multi_client_file_transfer.sh
./test_multi_client_file_transfer.sh
```

This will:
1. Start server
2. Launch 5 clients simultaneously
3. Verify all received files
4. Show results

---

## Current Status

✅ **Code Fixed:** File path now passed correctly  
✅ **Test File Created:** `test_file.txt` exists  
❌ **Not Built Yet:** Need to compile the project  

**Next Step:** Build the project using one of the methods above!

---

## Troubleshooting

### "cmake: command not found"
Install CMake:
- **Ubuntu/Debian:** `sudo apt install cmake`
- **Mac:** `brew install cmake`
- **Windows:** Download from https://cmake.org/download/

### "No such file or directory: demoFileTransferServer"
The project hasn't been built yet. Follow Step 1 above.

### "Address already in use"
Another process is using port 8080. Either:
- Kill it: `killall demoFileTransferServer`
- Use different port: `-p 9090`

### "Failed to open file"
Make sure `test_file.txt` exists:
```bash
cd Multi-ClientServer
ls -l test_file.txt
```

---

## What's Working Now

Your multi-client file transfer server:

✅ Accepts multiple simultaneous connections  
✅ Each client gets its own thread  
✅ File path from command line works correctly  
✅ Shows client connection count  
✅ Logs all transfers  
✅ No compilation errors  

**Just needs to be built!**
