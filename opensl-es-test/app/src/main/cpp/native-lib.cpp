#include <jni.h>
#include <string>

extern "C"
{
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
}

#include <android/log.h>




extern "C" JNIEXPORT jstring JNICALL
Java_com_aispeech_opensl_1es_1test_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

static SLObjectItf engineObject = NULL;
static SLObjectItf engineEngine = NULL;

extern "C" JNIEXPORT void JNICALL
Java_com_aispeech_opensl_1es_1test_MainActivity_playpcm(
        JNIEnv* env,
        jobject instance,jstring jurl) {

  const char* url =  env->GetStringUTFChars(jurl,0);

    SLresult result;

    //第一步------------------------------------------
    // 创建引擎对象-create
    slCreateEngine(&engineObject,0,NULL,0,NULL,NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
    // 创建引擎对象-realize
    (*engineObject)->Realize(engineObject,SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
    // get the engine interface, which is needed in order to create other objects
    //GetInterface
    (*engineObject)->GetInterface(engineObject,SL_IID_ENGINE,&engineEngine);


  env->ReleaseStringUTFChars(jurl,url);
}
