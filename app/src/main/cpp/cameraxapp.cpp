// Write C++ code here.
#include <jni.h>
#include "utils.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_cameraxapp_VidProcPost_00024MyClass_00024Companion_stringFromJNI(
        JNIEnv *env,
        jobject p_this,/* this */
        jstring input,
        jstring out_path) {
    const char* path = env->GetStringUTFChars(input, nullptr);
    const char* cPath = env->GetStringUTFChars(out_path, nullptr);
    String output = openVid(path, cPath);
    return env->NewStringUTF(output.c_str());
}

// Or, in RealTime.kt:
//    companion object {
//      init {
//         System.loadLibrary("cameraxapp")
//      }
//    }