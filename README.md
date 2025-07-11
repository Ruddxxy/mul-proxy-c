<h1 align="center">🛡️ Multi-Threaded Secure HTTP Proxy</h1>

<p align="center">
  <img src="https://img.shields.io/badge/Language-C-blue?style=flat-square"/>
  <img src="https://img.shields.io/badge/Threads-Multi--Threaded-green?style=flat-square"/>
  <img src="https://img.shields.io/badge/Security-WAF%20%2B%20Rate%20Limiting-red?style=flat-square"/>
</p>

<p align="center">
  🚀 A low-level HTTP proxy server in C for Windows, with built-in security — from concurrency to firewall-like rules.
</p>

---

## ✨ Features

✅ **Multi-Threaded**
- Handles multiple concurrent client connections using `_beginthreadex`.

✅ **Rate Limiting + Auto-Ban**
- Per-IP tracking: bans any IP exceeding `50 requests in 15 seconds`.

✅ **Lightweight WAF**
- Blocks:
  - `../` path traversal
  - SQLi payloads (`' OR 1=1`, `union select`)
  - Suspicious User-Agents like `sqlmap`, `wpscan`, `dirbuster`

✅ **IPv4 + IPv6 Support**
- Proper detection, logging, and banning of both address types.

✅ **Security Audit Logging**
- Writes to `security_log.txt` detailing attacks, bans, timestamps.

✅ **Live Console Metrics**
- Displays active threads, total & blocked requests in real time.

---

## 📸 Demo

<p align='center'>
<img src=<img width="880" height="299" alt="Tap to view" src="https://github.com/user-attachments/assets/68608c7c-5f09-4bbb-914e-cf5faf653bed" />



---

## 🚀 Quick Start

### 🛠 Build (MinGW / Windows)
```bash
gcc proxy.c -o proxy -lws2_32
./proxy
