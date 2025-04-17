#include <jni.h>
#include <string>
#include <libwebsockets.h>
#include <android/log.h>
#include <thread>

#define TAG "WebSocketNDK"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_websocketapp_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

static struct lws *wsi_client = nullptr;
static struct lws_context *context = nullptr;
static bool is_running = true;

static int CallbackWs(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            LOGI("Connected to WebSocket server");
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            LOGI("Response from server: %.*s", (int)len, (char *)in);
            break;

        case LWS_CALLBACK_CLIENT_CLOSED:
            LOGI("WebSocket connection closed");
            is_running = false;
            break;

        default:
            break;
    }
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_teuapp_WebSocketClient_nativeStartClient(JNIEnv *env, jobject obj) {
    struct lws_context_creation_info info = {};
    info.port = CONTEXT_PORT_NO_LISTEN;
    static struct lws_protocols protocols[] = {
        { "ws-protocol", CallbackWs, 0, 4096 },
        { nullptr, nullptr, 0, 0 }
    };
    info.protocols = protocols;

    context = lws_create_context(&info);
    if (!context) {
        LOGI("Error creating WebSocket context");
        return;
    }

    struct lws_client_connect_info ccinfo = {};
    ccinfo.context = context;
    ccinfo.address = "raspberrypi.local";  // IP or hostname of the Raspberry Pi
    ccinfo.port = 8765;
    ccinfo.path = "/";
    ccinfo.host = ccinfo.address;
    ccinfo.origin = ccinfo.address;
    ccinfo.protocol = protocols[0].name;
    ccinfo.ietf_version_or_minus_one = -1;
    wsi_client = lws_client_connect_via_info(&ccinfo);

    if (!wsi_client) {
        LOGI("Failed to connect to WebSocket server");
        return;
    }

    while (is_running) {
        lws_service(context, 50);
    }

    lws_context_destroy(context);
}
