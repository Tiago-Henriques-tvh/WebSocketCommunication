package com.teuapp

class WebSocketClient {
    companion object {
        init {
            System.loadLibrary("native-lib") // Nome da biblioteca NDK compilada
        }
    }

    external fun nativeStartClient()
}
