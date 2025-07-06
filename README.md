# Simple HTTP Proxy Server

## Overview
A lightweight HTTP proxy server implemented in C for Windows. Handles client requests, forwards them to external servers, and returns responses while implementing basic security measures.

## Key Features
- **TCP Proxy Core**
  - Listens on port 8080
  - Forwards HTTP requests to target servers
  - Returns server responses to clients
- **Security Measures**
  - Blocks requests to `localhost`, `127.0.1`, `192.168.*.*`, and `10.*.*.*`
  - Rejects oversized requests (>4095 bytes)
  - Prevents DNS rebinding attacks
- **Logging & Monitoring**
  - Tracks client connections (IP/port)
  - Logs total requests handled
  - Records forwarded hosts

## Prerequisites
- Windows OS
- C compiler (MinGW recommended)
- Winsock2 library (included in Windows SDK)

## Build & Run
```bash
# Compile with MinGW
gcc proxy.c -o proxy.exe -lws2_32

# Start proxy
.\proxy.exe