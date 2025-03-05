#include <iostream>
#include <string>
#include <libwebsockets.h>
#include <curl/curl.h>

#define PORT 8765
#define DEEPSEEK_URL "http://localhost:8000/v1/chat/completions"

struct ResponseData {
    std::string data;
};

// Callback para acumular a resposta do DeepSeek
static size_t WriteCallback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total_size = size * nmemb;
    ResponseData *res = static_cast<ResponseData *>(userdata);
    res->data.append(static_cast<char *>(ptr), total_size);
    return total_size;
}

// Função para enviar a pergunta ao DeepSeek
std::string AskDeepSeek(const std::string &question) {
    CURL *curl = curl_easy_init();
    if (!curl) return "Erro ao inicializar o cURL.";

    ResponseData res;
    std::string post_data = R"({"model":"deepseek-chat","messages":[{"role":"user","content":")" + question + R"("}]})";
    
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, DEEPSEEK_URL);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
    
    CURLcode res_code = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res_code != CURLE_OK) {
        return "Erro ao comunicar com o DeepSeek.";
    }
    return res.data;
}

// Callback WebSocket
static int CallbackWs(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    if (reason == LWS_CALLBACK_RECEIVE) {
        std::string question(static_cast<char *>(in), len);
        std::cout << "Recebido: " << question << std::endl;

        std::string reply = AskDeepSeek(question);

        lws_write(wsi, (unsigned char *)reply.c_str(), reply.length(), LWS_WRITE_TEXT);
        std::cout << "Enviado: " << reply << std::endl;
    }
    return 0;
}

// Definir protocolo
static struct lws_protocols protocols[] = {
    { "ws-protocol", CallbackWs, 0, 4096 },
    { nullptr, nullptr, 0, 0 }
};

// Inicializar servidor
int main() {
    struct lws_context_creation_info info = {};
    
    info.port = PORT;
    info.protocols = protocols;
    info.options = LWS_SERVER_OPTION_DISABLE_IPV6;

    struct lws_context *context = lws_create_context(&info);
    if (!context) {
        std::cerr << "Erro ao criar contexto WebSocket" << std::endl;
        return -1;
    }

    std::cout << "Servidor WebSocket iniciado na porta " << PORT << "..." << std::endl;
    while (true) {
        lws_service(context, 50);
    }

    lws_context_destroy(context);
    return 0;
}
