package com.plutinosoft.platinum;

public class UPnP {
    public UPnP() {
        cSelf = _init();
    }

    public void start() {
        _start(cSelf);
    }
    
    public void stop() {
        _stop(cSelf);
    }

    // C glue
    private static native long _init();
    private static native int _start(long self);
    private static native int _stop(long self);
    private final long     cSelf;

    static {
        System.loadLibrary("platinum-jni");
    }
}
