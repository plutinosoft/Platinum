#ifndef __MEDIA_RENDER_H__
#define __MEDIA_RENDER_H__

class MediaRendererDelegate
{
public:
    virtual ~MediaRendererDelegate() {}

    // ConnectionManager
    virtual int OnGetCurrentConnectionInfo(const char * uri, const char * data) = 0;

    // AVTransport
    virtual int OnNext(const char * uri, const char * data) = 0;
    virtual int OnPause(const char * uri, const char * data) = 0;
    virtual int OnPlay(const char * uri, const char * data) = 0;
    virtual int OnPrevious(const char * uri, const char * data) = 0;
    virtual int OnSeek(const char * uri, const char * data) = 0;
    virtual int OnStop(const char * uri, const char * data) = 0;
    virtual int OnSetAVTransportURI(const char * uri, const char * data) = 0;
    virtual int OnSetPlayMode(const char * uri, const char * data) = 0;

    // RenderingControl
    virtual int OnSetVolume(const char * uri, const char * data) = 0;
    virtual int OnSetVolumeDB(const char * uri, const char * data) = 0;
    virtual int OnGetVolumeDBRange(const char * uri, const char * data) = 0;
    virtual int OnSetMute(const char * uri, const char * data) = 0;
};


class MediaRenderer {
public:
    virtual ~MediaRenderer() {}

public:
    virtual void Start() = 0;
    virtual void Stop() = 0;
};

MediaRenderer * create_media_renderer(MediaRendererDelegate * delegate,
                                      const char*  friendly_name,
                                      bool         show_ip = false,
                                      const char*  uuid = NULL,
                                      unsigned int port = 0,
                                      bool         port_rebind = false);

#endif //__MEDIA_RENDER_H__
