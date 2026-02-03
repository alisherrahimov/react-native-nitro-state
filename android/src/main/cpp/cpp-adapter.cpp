#include <jni.h>
#include "nitrostateOnLoad.hpp"

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
  return margelo::nitro::nitrostate::initialize(vm);
}
