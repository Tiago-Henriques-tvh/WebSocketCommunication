import websocket

def on_message(ws, message):
    print("Received:", message)

def on_open(ws):
    print("Connect.")
    ws.send("What is the capital of Portugal?")

ws = websocket.WebSocketApp("ws://localhost:8765",
                            subprotocols=["ws-protocol"],
                            on_message=on_message,
                            on_open=on_open)

ws.run_forever()

