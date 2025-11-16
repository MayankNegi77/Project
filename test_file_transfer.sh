#!/bin/bash
# File Transfer Test Script
# This script demonstrates the file transfer feature

echo "=== File Transfer Test ==="
echo ""

# Step 1: Create test file
echo "Step 1: Creating test file..."
cd ~/project
echo "Hello from server! This is a test file." > test_file.txt
echo "✓ Created test_file.txt"
echo ""

# Step 2: Start server in background
echo "Step 2: Starting server on port 8080..."
cd ~/project/bin
./demoFileTransferServer -p 8080 -f ../test_file.txt -l file_server.log &
SERVER_PID=$!
echo "✓ Server started (PID: $SERVER_PID)"
sleep 2  # Give server time to start
echo ""

# Step 3: Create downloads directory
echo "Step 3: Creating downloads directory..."
mkdir -p ~/project/downloads
echo "✓ Downloads directory ready"
echo ""

# Step 4: Run client
echo "Step 4: Running client to receive file..."
./demoFileTransferClient -i 127.0.0.1 -p 8080 -s ../downloads/received.txt -l file_client.log
echo ""

# Step 5: Verify received file
echo "Step 5: Verifying received file..."
if [ -f ~/project/downloads/received.txt ]; then
    echo "✓ File received successfully!"
    echo ""
    echo "Content:"
    cat ~/project/downloads/received.txt
    echo ""
else
    echo "✗ File not received"
fi

# Step 6: Cleanup
echo ""
echo "Step 6: Stopping server..."
kill $SERVER_PID 2>/dev/null
echo "✓ Server stopped"
echo ""
echo "=== Test Complete ==="
