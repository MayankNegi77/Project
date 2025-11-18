# Terminal-to-Terminal Communication Guide (Two Different PCs)

This guide shows how to communicate between two different Linux PCs over a network.

## Network Setup Requirements

### PC 1 (Server PC)
- Linux operating system
- Connected to network (WiFi or Ethernet)
- Firewall configured to allow incoming connections
- Static IP or known IP address

### PC 2 (Client PC)
- Linux operating system
- Connected to same network as Server PC
- Can reach Server PC (ping test)

## Step 1: Prepare Both PCs

### On Both PCs - Build the Project

```bash
cd Multi-ClientServer
mkdir -p build
cd build
cmake ..
make -j4
```

Verify executables are created:
```bash
ls -lh ../bin/
```

You should see:
- demoFileTransferServer
- demoFileTransferClient
- demoMutexFileTransferServer

## Step 2: Configure Server PC (PC 1)

### Find Server IP Address

```bash
hostname -I
```

Example output: `192.168.1.100 fe80::a00:27ff:fe4e:66a1`

The first IP (192.168.1.100) is your IPv4 address - **WRITE THIS DOWN**

Alternative method:
```bash
ip addr show | grep "inet " | grep -v 127.0.0.1
```

### Configure Firewall

Allow incoming connections on port 60000:

**For UFW (Ubuntu/Debian):**
```bash
sudo ufw allow 60000/tcp
sudo ufw status
```

**For firewalld (CentOS/RHEL/Fedora):**
```bash
sudo firewall-cmd --permanent --add-port=60000/tcp
sudo firewall-cmd --reload
sudo firewall-cmd --list-ports
```

**For iptables:**
```bash
sudo iptables -A INPUT -p tcp --dport 60000 -j ACCEPT
sudo iptables-save
```

### Start the Server

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
Server IP: [192.168.1.100]
Server Port Number: [60000]
Waiting for clients to connect...
========================================
```

**IMPORTANT:** Note the Server IP shown - this is what the client will use.

## Step 3: Configure Client PC (PC 2)

### Test Network Connectivity

First, verify you can reach the server:

```bash
ping 192.168.1.100
```

Replace `192.168.1.100` with your actual server IP.

Expected output:
```
64 bytes from 192.168.1.100: icmp_seq=1 ttl=64 time=0.5 ms
```

Press Ctrl+C to stop. If ping fails, check:
- Both PCs are on same network
- Server firewall is configured
- Server IP is correct

### Test Port Connectivity

```bash
telnet 192.168.1.100 60000
```

Or using nc (netcat):
```bash
nc -zv 192.168.1.100 60000
```

Expected: "Connection succeeded" or "open"

If connection fails:
- Check server is running
- Check firewall rules
- Check port number is correct

### Start the Client

```bash
cd Multi-ClientServer/bin
./demoFileTransferClient --ip 192.168.1.100 --port 60000 --save received_file.txt
```

Replace `192.168.1.100` with your actual server IP.

Expected output:
```
File Transfer Client started. Requesting file from server...
File size: XXXX bytes
File received successfully and saved to: received_file.txt
```

## Step 4: Verify on Server PC

On Server PC (Terminal 1), you should see:

```
========================================
[Client #1] Connected!
  IP Address: 192.168.1.200
  Port: 54321
  Spawning thread to handle file transfer...
========================================
[192.168.1.200:54321] Transfer #1 - File sent successfully (XXXX bytes)
```

## Step 5: Verify File Transfer on Client PC

```bash
ls -lh received_file.txt
cat received_file.txt
```

## Testing Multiple Clients from Different PCs

You can connect multiple client PCs simultaneously:

**PC 2 (Client 1):**
```bash
./demoFileTransferClient --ip 192.168.1.100 --port 60000 --save received_file.txt
```

**PC 3 (Client 2):**
```bash
./demoFileTransferClient --ip 192.168.1.100 --port 60000 --save received_file.txt
```

**PC 4 (Client 3):**
```bash
./demoFileTransferClient --ip 192.168.1.100 --port 60000 --save received_file.txt
```

The server will handle all connections simultaneously using threads.

## Using Mutex-Enabled Server

For production use with statistics and mutex protection:

**Server PC:**
```bash
cd Multi-ClientServer/bin
./demoMutexFileTransferServer --port 60000 --file ../test_file.txt --connexion 10
```

This version provides:
- Real-time connection statistics
- Thread-safe file access with mutex locks
- Active client monitoring
- Periodic status updates every 10 seconds
- Detailed logging

Press Ctrl+C to see final statistics:
```
=== SERVER SHUTDOWN INITIATED ===
========================================
           SERVER STATISTICS
========================================
Total Connections:    15
Active Connections:   0
Files Transferred:    45
Bytes Transferred:    1234567 bytes
========================================
```

## Network Topology Examples

### Same Local Network (LAN)
```
Router (192.168.1.1)
├── Server PC: 192.168.1.100
├── Client PC 1: 192.168.1.200
├── Client PC 2: 192.168.1.201
└── Client PC 3: 192.168.1.202
```

### Different Subnets (Requires Routing)
```
Router 1 (192.168.1.1)
└── Server PC: 192.168.1.100

Router 2 (192.168.2.1)
└── Client PC: 192.168.2.100
```

For different subnets, ensure routing is configured between networks.

## Troubleshooting

### Cannot Connect to Server

**Check 1: Server is running**
```bash
# On Server PC
ps aux | grep demoFileTransferServer
```

**Check 2: Port is listening**
```bash
# On Server PC
sudo netstat -tulpn | grep 60000
# or
sudo ss -tulpn | grep 60000
```

Expected: `LISTEN` state on port 60000

**Check 3: Firewall allows connection**
```bash
# On Server PC
sudo ufw status
# or
sudo firewall-cmd --list-all
```

**Check 4: Network connectivity**
```bash
# On Client PC
ping SERVER_IP
traceroute SERVER_IP
```

### Connection Refused

This means the port is blocked or server is not listening:

1. Restart server with verbose logging
2. Check firewall rules again
3. Try different port (e.g., 60001)
4. Disable firewall temporarily for testing:
   ```bash
   sudo ufw disable  # Ubuntu
   sudo systemctl stop firewalld  # CentOS
   ```

### Connection Timeout

This means packets are not reaching the server:

1. Check both PCs are on same network
2. Check router/switch configuration
3. Check for VPN or proxy interference
4. Try direct cable connection between PCs

### File Transfer Incomplete

If file transfer starts but doesn't complete:

1. Check network stability
2. Check disk space on client PC
3. Check file permissions
4. Review server logs in `./log/` directory

### Permission Denied Errors

```bash
# Make executables runnable
chmod +x demoFileTransferServer
chmod +x demoFileTransferClient

# Make test file readable
chmod 644 test_file.txt
```

## Advanced: Testing Across Internet

To test across internet (not just local network):

### Server PC Setup:
1. Configure port forwarding on router (port 60000 → Server PC IP)
2. Find public IP: `curl ifconfig.me`
3. Use dynamic DNS if IP changes frequently

### Client PC:
```bash
./demoFileTransferClient --ip PUBLIC_IP --port 60000 --save received_file.txt
```

**Security Warning:** For internet exposure, add authentication and encryption!

## Performance Testing

Test with large file:
```bash
# Create 100MB test file
dd if=/dev/urandom of=large_test.txt bs=1M count=100

# Server
./demoFileTransferServer --port 60000 --file large_test.txt

# Client (measure time)
time ./demoFileTransferClient --ip SERVER_IP --port 60000 --save received_large.txt

# Verify
md5sum large_test.txt received_large.txt
```

## Quick Connection Test Script

Save as `test_connection.sh` on Client PC:

```bash
#!/bin/bash
SERVER_IP="192.168.1.100"
SERVER_PORT="60000"

echo "Testing connection to $SERVER_IP:$SERVER_PORT"

# Test ping
echo -n "Ping test: "
if ping -c 1 -W 2 $SERVER_IP > /dev/null 2>&1; then
    echo "OK"
else
    echo "FAILED - Cannot reach server"
    exit 1
fi

# Test port
echo -n "Port test: "
if nc -zv -w 2 $SERVER_IP $SERVER_PORT 2>&1 | grep -q succeeded; then
    echo "OK"
else
    echo "FAILED - Port not accessible"
    exit 1
fi

echo "All tests passed! Ready to connect."
```

Run:
```bash
chmod +x test_connection.sh
./test_connection.sh
```
