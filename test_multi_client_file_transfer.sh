#!/bin/bash
# Multi-Client File Transfer Test Script
# This demonstrates multiple clients receiving files simultaneously

echo "=========================================="
echo "Multi-Client File Transfer Test"
echo "=========================================="
echo ""

# Step 1: Create test file
echo "Step 1: Creating test file..."
cd ~/project
echo "Hello from server! This file is sent to multiple clients simultaneously." > test_file.txt
echo "✓ Created test_file.txt"
echo ""

# Step 2: Start server in background
echo "Step 2: Starting multi-client server on port 8080..."
cd ~/project/bin
./demoFileTransferServer -p 8080 -c 10 -f ../test_file.txt -l file_server.log &
SERVER_PID=$!
echo "✓ Server started (PID: $SERVER_PID)"
sleep 2  # Give server time to start
echo ""

# Step 3: Create downloads directory
echo "Step 3: Creating downloads directory..."
mkdir -p ~/project/downloads
echo "✓ Downloads directory ready"
echo ""

# Step 4: Launch multiple clients simultaneously
echo "Step 4: Launching 5 clients simultaneously..."
echo ""

for i in {1..5}
do
    echo "  [Client $i] Connecting..."
    ./demoFileTransferClient -i 127.0.0.1 -p 8080 \
        -s ../downloads/received_client_$i.txt \
        -l file_client_$i.log &
    CLIENT_PIDS[$i]=$!
    sleep 0.2  # Small delay between client launches
done

echo ""
echo "Waiting for all clients to finish..."
wait

echo ""
echo "=========================================="
echo "Step 5: Verifying received files..."
echo "=========================================="

SUCCESS_COUNT=0
for i in {1..5}
do
    if [ -f ~/project/downloads/received_client_$i.txt ]; then
        echo "✓ Client $i: File received successfully"
        SUCCESS_COUNT=$((SUCCESS_COUNT + 1))
    else
        echo "✗ Client $i: File NOT received"
    fi
done

echo ""
echo "=========================================="
echo "Results: $SUCCESS_COUNT/5 clients received the file"
echo "=========================================="

if [ $SUCCESS_COUNT -eq 5 ]; then
    echo ""
    echo "Sample content from Client 1:"
    cat ~/project/downloads/received_client_1.txt
    echo ""
fi

# Step 6: Cleanup
echo ""
echo "Step 6: Stopping server..."
kill $SERVER_PID 2>/dev/null
echo "✓ Server stopped"
echo ""
echo "=========================================="
echo "Test Complete!"
echo "=========================================="
echo ""
echo "Files are saved in: ~/project/downloads/"
echo "Logs are saved in: ~/project/bin/log/"
