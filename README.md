# 🚀 Simple HTTP Proxy Server (Windows)

A multi-threaded **HTTP proxy server written in C for Windows**, leveraging Winsock2.  
Handles client requests, forwards them to external servers, and logs detailed activity — with built-in security measures to block suspicious or unsafe requests.

---

## ✨ Features

### ✅ Proxy Core
- Listens on **TCP port `8080`**.
- Handles multiple clients concurrently using Windows `_beginthreadex` threads.
- Receives HTTP requests (supports `GET` and `POST`), forwards them to the target host, and relays responses.

### ✅ Security & Stability
- **Blocks requests** to:
  - `localhost`
  - `127.0.1`
  - `192.168.*.*`
  - `10.*.*.*`
- **Rejects oversized requests** (>4095 bytes) to prevent buffer overflow attacks.
- Designed to guard against DNS rebinding and basic misuse.

### ✅ Monitoring & Logging
- Logs:
  - Client connections (IP & port).
  - Total requests handled.
  - Number of blocked requests.
  - Active threads count (live connections).
  - Forwarded hosts.
- Prints full HTTP request received from client.

---

## 🛠 Prerequisites

- Windows OS  
- [MinGW](https://www.mingw-w64.org/) (recommended for compiling with `gcc`)  
- Winsock2 library (already included in Windows SDK)

---

## ⚙ Build & Run

```bash
# Compile with MinGW
gcc proxy.c -o proxy.exe -lws2_32

# Start proxy server
.\proxy.exe
