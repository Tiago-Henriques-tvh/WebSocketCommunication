# WebSocket Communication

On server:

```bash
sudo apt update
sudo apt install libwebsockets-dev libcurl4-openssl-dev

```

When compiling, you’ll need to link both libraries:

```bash
g++ server.cpp -o server -lwebsockets -lcurl
./server
```

### To run the test client

```bash
pip install websocket-client
python client.py
```

> Make sure your WebSocket server is running
