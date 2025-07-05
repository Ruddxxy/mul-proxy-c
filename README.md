# multi-thread proxy server

## 🛠 How to build & run
### ⚙️ Prerequisites
- Windows machine with **MinGW** (or any GCC with Winsock2 support).

### 🔧 Build
make
./proxy

Use curl to make an HTTP request through your proxy.
curl -x localhost:8080 http://example.com
