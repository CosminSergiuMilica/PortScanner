# Port Scanner

## Description
This is a port scanner written in C++. The project aims to familiarize me with how a port scanner works and to improve my C++ skills. The scanner supports both TCP and UDP scans and can be used to scan a single IP or an entire network range.

## Usage
The application is designed to run from the command line with the following arguments:
```
pscan -ip <target_ip>
      -net <target_ip_network/mask>
      -sp <start_port> (*optional, default: 1)
      -ep <end_port> (*optional, default: 65535)
      -mode <[tcp/udp/both]> (*optional, default: both)
```

## How Scanning Works
The scanner first determines if a target device is alive before performing port scans. It follows these steps:
1. **Network Discovery**: If scanning a network, it generates a list of IPs based on the subnet mask.
2. **Host Detection**: Uses ICMP ping to check if a device is active at a given IP.
3. **Port Scanning**:
   - **TCP Scan**: Attempts to establish a connection using `connect()`. If successful, the port is open.
   - **UDP Scan**: Sends a small packet to the port and waits for a response. If a reply is received, the port is open.
4. **Threading**: The scanner runs multiple scans in parallel to speed up the process.

## Project Structure
```
├── argumentParser
│   ├── argumentParser.cpp
│   └── ArgumentParser.h
├── main.cpp
└── scanner
    ├── interface
    │   └── scanner.h
    ├── ipScanner
    │   ├── portScanner.cpp
    │   └── PortScanner.h
    └── networkScanner
        ├── networkScanner.cpp
        └── NetworkScanner.h
```

## Compilation
To compile the project, use the following command:
```
g++ -o pscan main.cpp \
    -I./argumentParser \
    -I./scanner/interface \
    -I./scanner/networkScanner \
    -I./scanner/ipScanner \
    scanner/networkScanner/networkScanner.cpp \
    scanner/ipScanner/portScanner.cpp \
    argumentParser/argumentParser.cpp \
    -std=c++11 -pthread
```

## Features
- Supports scanning a single IP or an entire network.
- Scans both TCP and UDP ports.
- Uses threading for faster scanning.
- Includes host detection to avoid unnecessary scans.
- Runs efficiently with optimized concurrency.

## Known Issues with UDP Scan
UDP scan can be unreliable due to the connectionless nature of UDP. If a port is open but does not respond, or if a firewall blocks ICMP/UDP responses, the scanner might incorrectly report the port status. This functionality needs further improvement.

## Current UDP Scan Method

1. **Send a UDP Packet** to the target port.

2. **Wait for a Response**:
   - If an **ICMP Destination Unreachable** message is received, the port is considered closed.
   - If no response is received, the port is considered open.

3. **Limitations**:
   - UDP is connectionless, so no response doesn't always mean the port is closed. This scan might not be fully reliable.

This method needs improvement for more reliable checks, as a port not responding isn't necessarily closed.

