#include <jni.h>
#include <string>

extern "C"
{
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
}

#include <android/log.h>
#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"haisheng",FORMAT,##__VA_ARGS__)
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"haisheng",FORMAT,##__VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL
Java_com_aispeech_opensl_1es_1test_OpenSLActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

// 引擎接口
static SLObjectItf engineObject = NULL;
static SLEngineItf engineEngine = NULL;

//混音器
static SLObjectItf outputMixObject = NULL;
static SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

//pcm
static SLObjectItf pcmPlayerObject = NULL;
static SLPlayItf pcmPlayerPlay = NULL;
static SLVolumeItf pcmPlayerVolume = NULL;

//缓冲器队列接口
static SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;

FILE *pcmFile;
void *buffer;
uint8_t *out_buffer;

size_t getPcmData(void **pcm)
{

    while(!feof(pcmFile))
    {
        size_t size = 0;
        size = fread(out_buffer, 44100 * 2 * 2, 1, pcmFile);
        LOGI("read %d",size);
        if(size <= 0)
        {
            LOGI("%s", "read end");
            return size;
        } else{

        }
        *pcm = out_buffer;
        return size;
    }
}

void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void* context)
{
  size_t size = getPcmData(&buffer);
  // for streaming playback, replace this test by logic to find and fill the next buffer
  if (size > 0 ) {
    SLresult result;
    // enqueue another buffer
    result = (*pcmBufferQueue)->Enqueue(pcmBufferQueue, buffer, 44100*2*2);
    LOGI("Enqueue %d",size);
    // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
    // which for this code example would indicate a programming error
  }
}


extern "C" JNIEXPORT void JNICALL
Java_com_aispeech_opensl_1es_1test_OpenSLActivity_playpcm(
        JNIEnv* env,
        jobject instance,jstring jurl) {

    const char* url =  env->GetStringUTFChars(jurl,0);
    // TODO
    //读取pcm文件
    pcmFile = fopen(url, "r");
    if(pcmFile == NULL)
    {
        LOGE("%s", "fopen file error");
        return;
    }
    out_buffer = (uint8_t *) malloc(44100 * 2 * 2);


    SLresult result;
    //第一步------------------------------------------
    // 创建引擎对象
    slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);


    //第二步-------------------------------------------
    // 创建混音器
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mids, mreq);
    (void)result;
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    (void)result;
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB, &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        LOGI("SL_RESULT_SUCCESS == result");
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void)result;
    }else{
        LOGI("SL_RESULT_Falu eeee ");
    }



    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};

    // 第三步--------------------------------------------
    // 创建播放器
    SLDataLocator_AndroidSimpleBufferQueue android_queue={SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,2};
    SLDataFormat_PCM pcm={
            SL_DATAFORMAT_PCM,//播放pcm格式的数据
            2,//2个声道（立体声）
            SL_SAMPLINGRATE_44_1,//44100hz的频率
            SL_PCMSAMPLEFORMAT_FIXED_16,//位数 16位
            SL_PCMSAMPLEFORMAT_FIXED_16,//和位数一致就行
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//立体声（前左前右）
            SL_BYTEORDER_LITTLEENDIAN//结束标志
    };

    SLDataSource slDataSource = {&android_queue, &pcm};
    SLDataSink audioSnk = {&outputMix, NULL};
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &slDataSource, &audioSnk, 3, ids, req);
    // 初始化播放器
    (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);

    //得到接口后调用  获取Player接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY, &pcmPlayerPlay);

    //第四步---------------------------------------
    // 创建缓冲区和回调函数
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);

    //缓冲接口回调
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, NULL);
    //获取音量接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_VOLUME, &pcmPlayerVolume);

    //第五步----------------------------------------
    // 设置播放状态
    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);


    //第六步----------------------------------------
    // 主动调用回调函数开始工作
    pcmBufferCallBack(pcmBufferQueue, NULL);


    env->ReleaseStringUTFChars(jurl,url);
}
