#include <jni.h>
#include <string>
#include <core/math/vector3f.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_eventhorizon_core_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {

    V3f urca{1.0f};
    std::string hello = "Hello from C++ " + urca.toString();

//    dadolib urca;
//    urca.ciao();

    return env->NewStringUTF(hello.c_str());
}
