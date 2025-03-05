# WebSocket Communication

On server:

```bash
sudo apt update
sudo apt install libwebsockets-dev libcurl4-openssl-dev
```

---

```bash
g++ server.cpp -o server -lwebsockets -lcurl
g++ client.cpp -o client -lwebsockets
```
