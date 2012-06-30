/*****************************************************************
|
|      Android JNI Interface
|
|      (c) 2002-2012 Plutinosoft LLC
|      Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include <assert.h>
#include <jni.h>
#include <string.h>
#include <sys/types.h>

#include "platinum-jni.h"
#include "Platinum.h"

#include <android/log.h>

/*----------------------------------------------------------------------
|   logging
+---------------------------------------------------------------------*/
NPT_SET_LOCAL_LOGGER("platinum.android.jni")

/*----------------------------------------------------------------------
|   functions
+---------------------------------------------------------------------*/
__attribute__((constructor)) static void onDlOpen(void)
{
}

/*
 * Class:     com_plutinosoft_platinum_UPnP
 * Method:    _init
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_plutinosoft_platinum_UPnP__1init(JNIEnv *env, jclass)
{
    NPT_LOG_INFO("init");
    PLT_UPnP* self = new PLT_UPnP();
    return (jlong)self;
}

/*
 * Class:     com_plutinosoft_platinum_UPnP
 * Method:    _start
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_plutinosoft_platinum_UPnP__1start(JNIEnv *, jclass, jlong _self)
{
    PLT_UPnP* self = (PLT_UPnP*)_self;
    
    return self->Start();
}

/*
 * Class:     com_plutinosoft_platinum_UPnP
 * Method:    _stop
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_plutinosoft_platinum_UPnP__1stop(JNIEnv *, jclass, jlong _self)
{
    PLT_UPnP* self = (PLT_UPnP*)_self;
    
    return self->Stop();
}

