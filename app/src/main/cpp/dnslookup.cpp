#include <jni.h>
#include <string>
#include <netdb.h>
#include <android/log.h>
#include <arpa/inet.h>
#include <malloc.h>

#define TAG "abc"


extern "C"
JNIEXPORT void JNICALL
Java_net_cmoaciopm_test_ipv6test_MainActivity_nativeResolve(JNIEnv *env,
                                                            jobject instance,
                                                            jstring host_) {
    const char *host = env->GetStringUTFChars(host_, 0);
    const char *port = NULL;
    int err = 0;
    struct addrinfo hints;
    struct addrinfo *result;
    struct addrinfo *rp;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM; // tcp only
    err = getaddrinfo(host, port, &hints, &result);
    if (err != 0) {
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "getaddrinfo: %s", gai_strerror(err));
        return;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "Address: flags=[%d], family=[%d], socktype=[%d], protocol=[%d]",
                            rp->ai_flags, rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (rp->ai_addr->sa_family == AF_INET6) {
            char *resolvedAddress = (char *)malloc(INET6_ADDRSTRLEN);
            if (inet_ntop(AF_INET6, &((struct sockaddr_in6 *)rp->ai_addr)->sin6_addr,
                          resolvedAddress, INET6_ADDRSTRLEN) == NULL) {
                __android_log_print(ANDROID_LOG_DEBUG, TAG, "inet_ntop: %s", strerror(errno));
            } else {
                __android_log_print(ANDROID_LOG_DEBUG, TAG, "Resolved address: %s", resolvedAddress);
                free(resolvedAddress);
            }
        } else {
            char *resolvedAddress = (char *)malloc(INET6_ADDRSTRLEN);
            strncpy(resolvedAddress, inet_ntoa(((struct sockaddr_in *)rp->ai_addr)->sin_addr),
                    INET6_ADDRSTRLEN - 1);
            resolvedAddress[INET6_ADDRSTRLEN - 1] = '\0';
            __android_log_print(ANDROID_LOG_DEBUG, TAG, "Resolved address: %s", resolvedAddress);
            free(resolvedAddress);
        }
    }

    env->ReleaseStringUTFChars(host_, host);
}