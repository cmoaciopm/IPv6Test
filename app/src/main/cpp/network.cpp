#include <jni.h>
#include <android/log.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <android/multinetwork.h> // for android_getaddrinfofornetwork
#include <netinet/in.h> // for in6_addr
#include <arpa/inet.h> // for inet_ntop

#define TAG "abc"


/*
 * Copy from dns64.c
 * Refer https://android.googlesource.com/platform/external/android-clat/+/master/dns64.c
 */
extern "C"
JNIEXPORT jint JNICALL
Java_net_cmoaciopm_test_ipv6test_MainActivity_nativePrintIPv6Prefix(JNIEnv *env,
                                                                    jobject instance,
                                                                    jlong netHandle,
                                                                    jstring host) {

    const struct addrinfo hints = {
        .ai_family = AF_INET6
    };
    const char *node = NULL;
    int status;
    struct addrinfo *result = NULL;
    int ret;
    struct in6_addr plat_addr;
    const char *prefix = NULL;
    char plat_addr_str[INET6_ADDRSTRLEN];

    net_handle_t handle = (net_handle_t)netHandle;
    node = env->GetStringUTFChars(host, NULL);
    ret = android_getaddrinfofornetwork(handle, node, NULL, &hints, &result);
    __android_log_print(ANDROID_LOG_ERROR, TAG, "return value is %d", ret);
    if (ret != 0 || result == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Invoke android_getaddrinfofornetwork failed");
        return 0;
    }
    if (result->ai_family != AF_INET6) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Unexpected ai_family: %d", result->ai_family);
        return 0;
    }
    plat_addr = ((struct sockaddr_in6 *)result->ai_addr)->sin6_addr;
    plat_addr.s6_addr32[3] = 0;

    prefix = inet_ntop(AF_INET6, &plat_addr, plat_addr_str, sizeof(plat_addr_str));
    __android_log_print(ANDROID_LOG_ERROR, TAG, "prefix is %s/96", prefix);
    return ret;
}