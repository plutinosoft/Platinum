#include <iostream>
#include <memory>

#include "PltUPnP.h"
#include "PltMediaRenderer.h"
#include "NptLogging.h"
#include "media_renderer.h"

NPT_SET_LOCAL_LOGGER("media_render.upnp.renderer")

namespace media_renderer {
class PLT_MediaRendererDelegateImpl : public PLT_MediaRendererDelegate
{
public:
    PLT_MediaRendererDelegateImpl(PLT_MediaRenderer * renderer, MediaRendererDelegate * delegate)
        : m_Delegate(delegate)
        , m_Renderer(renderer) {
    }

    virtual ~PLT_MediaRendererDelegateImpl() {}

    // ConnectionManager
    virtual NPT_Result OnGetCurrentConnectionInfo(PLT_ActionReference& action) {
        if (!m_Delegate)
            return NPT_ERROR_NOT_IMPLEMENTED;
        NPT_String curURI;
        action->GetArgumentValue("CurrentTransportState", curURI);
        NPT_String metaData;
        action->GetArgumentValue("CurrentURIMetaData", metaData);

        m_Delegate->OnGetCurrentConnectionInfo(curURI.GetChars(), metaData.GetChars());

        return NPT_SUCCESS;
    }

    // AVTransport
    virtual NPT_Result OnNext(PLT_ActionReference& action){
        if (!m_Delegate)
            return NPT_ERROR_NOT_IMPLEMENTED;
        NPT_String curURI;
        action->GetArgumentValue("NextURI", curURI);
        NPT_String metaData;
        action->GetArgumentValue("NextURIMetaData", metaData);

        m_Delegate->OnNext(curURI.GetChars(), metaData.GetChars());

        return NPT_SUCCESS;
    }

    virtual NPT_Result OnPause(PLT_ActionReference& action){
        if (!m_Delegate)
            return NPT_ERROR_NOT_IMPLEMENTED;
        NPT_String curURI;
        action->GetArgumentValue("CurrentURI", curURI);
        NPT_String metaData ;
        action->GetArgumentValue("CurrentURIMetaData", metaData);

        m_Delegate->OnPause(curURI.GetChars(), metaData.GetChars());

        return NPT_SUCCESS;
    }

    virtual NPT_Result OnPlay(PLT_ActionReference& action){
        if (!m_Delegate)
            return NPT_ERROR_NOT_IMPLEMENTED;
        NPT_String curURI;
        action->GetArgumentValue("CurrentURI", curURI);
        NPT_String metaData ;
        action->GetArgumentValue("CurrentURIMetaData", metaData);

        m_Delegate->OnPlay(curURI.GetChars(), metaData.GetChars());

        PLT_Service* service;
        NPT_CHECK_SEVERE(m_Renderer->FindServiceByType("urn:schemas-upnp-org:service:AVTransport:1", service));

        NPT_CHECK_SEVERE(service->GetStateVariableValue("AVTransportURI", curURI));
        NPT_CHECK_SEVERE(service->GetStateVariableValue("AVTransportURIMetaData", metaData));

        service->SetStateVariable("TransportState", "PLAYING");
        service->SetStateVariable("TransportStatus", "OK");
        service->SetStateVariable("AVTransportURI", curURI);
        service->SetStateVariable("AVTransportURIMetaData", metaData);

        service->SetStateVariable("NextAVTransportURI", "");
        service->SetStateVariable("NextAVTransportURIMetaData", "");

        NPT_CHECK_SEVERE(action->SetArgumentsOutFromStateVariable());
        return NPT_SUCCESS;
    }

    virtual NPT_Result OnPrevious(PLT_ActionReference& action){
        if (!m_Delegate)
            return NPT_ERROR_NOT_IMPLEMENTED;
        NPT_String curURI;
        action->GetArgumentValue("CurrentURI", curURI);
        NPT_String metaData ;
        action->GetArgumentValue("CurrentURIMetaData", metaData);

        m_Delegate->OnPrevious(curURI.GetChars(), metaData.GetChars());

        return NPT_SUCCESS;

    }

    virtual NPT_Result OnSeek(PLT_ActionReference& action) {
        if (!m_Delegate)
            return NPT_ERROR_NOT_IMPLEMENTED;
        NPT_String curURI;
        action->GetArgumentValue("Target", curURI);
        NPT_String metaData;
        action->GetArgumentValue("Unit", metaData);

        m_Delegate->OnSeek(curURI.GetChars(), metaData.GetChars());

        return NPT_SUCCESS;
    }

    virtual NPT_Result OnStop(PLT_ActionReference& action) {
        if (!m_Delegate)
            return NPT_ERROR_NOT_IMPLEMENTED;
        NPT_String curURI;
        action->GetArgumentValue("CurrentURI", curURI);
        NPT_String metaData ;
        action->GetArgumentValue("CurrentURIMetaData", metaData);
        m_Delegate->OnStop(curURI.GetChars(), metaData.GetChars());

        return NPT_SUCCESS;
    }

    virtual NPT_Result OnSetAVTransportURI(PLT_ActionReference& action) {
        if (!m_Delegate)
            return NPT_ERROR_NOT_IMPLEMENTED;
        NPT_String curURI;
        action->GetArgumentValue("CurrentURI", curURI);
        NPT_String metaData ;
        action->GetArgumentValue("CurrentURIMetaData", metaData);

        m_Delegate->OnSetAVTransportURI(curURI.GetChars(), metaData.GetChars());

        PLT_Service* service;
        NPT_CHECK_SEVERE(m_Renderer->FindServiceByType("urn:schemas-upnp-org:service:AVTransport:1", service));

        service->SetStateVariable("TransportState", "PLAYING");
        service->SetStateVariable("TransportStatus", "OK");
        service->SetStateVariable("AVTransportURI", curURI);
        service->SetStateVariable("AVTransportURIMetaData", metaData);

        service->SetStateVariable("NextAVTransportURI", "");
        service->SetStateVariable("NextAVTransportURIMetaData", "");

        NPT_CHECK_SEVERE(action->SetArgumentsOutFromStateVariable());

        return NPT_SUCCESS;
    }

    virtual NPT_Result OnSetPlayMode(PLT_ActionReference& action) {
        if (!m_Delegate)
            return NPT_ERROR_NOT_IMPLEMENTED;
        NPT_String playMode;
        action->GetArgumentValue("NewPlayMode", playMode);
        NPT_String metaData ;
        action->GetArgumentValue("CurrentURIMetaData", metaData);
        m_Delegate->OnSetPlayMode(playMode.GetChars(), metaData.GetChars());

        return NPT_SUCCESS;

    }

    // RenderingControl
    virtual NPT_Result OnSetVolume(PLT_ActionReference& action) {
        if (!m_Delegate)
            return NPT_ERROR_NOT_IMPLEMENTED;
        NPT_String curURI;
        action->GetArgumentValue("DesiredVolume", curURI);
        NPT_String metaData ;
        action->GetArgumentValue("Channel", metaData);

        m_Delegate->OnSetVolume(curURI.GetChars(), metaData.GetChars());

        return NPT_SUCCESS;
    }

    virtual NPT_Result OnSetVolumeDB(PLT_ActionReference& action){
        if (!m_Delegate)
            return NPT_ERROR_NOT_IMPLEMENTED;
        NPT_String curURI;
        action->GetArgumentValue("CurrentURI", curURI);
        NPT_String metaData ;
        action->GetArgumentValue("CurrentURIMetaData", metaData);
        m_Delegate->OnSetVolumeDB(curURI.GetChars(), metaData.GetChars());

        return NPT_SUCCESS;

    }

    virtual NPT_Result OnGetVolumeDBRange(PLT_ActionReference& action){
        if (!m_Delegate)
            return NPT_ERROR_NOT_IMPLEMENTED;
        NPT_String curURI;
        action->GetArgumentValue("CurrentURI", curURI);
        NPT_String metaData;
        action->GetArgumentValue("CurrentURIMetaData", metaData);
        m_Delegate->OnGetVolumeDBRange(curURI.GetChars(), metaData.GetChars());

        return NPT_SUCCESS;

    }

    virtual NPT_Result OnSetMute(PLT_ActionReference& action){
        if (!m_Delegate)
            return NPT_ERROR_NOT_IMPLEMENTED;

        NPT_String curURI;
        action->GetArgumentValue("DesiredMute", curURI);
        NPT_String metaData ;
        action->GetArgumentValue("CurrentURIMetaData", metaData);
        m_Delegate->OnSetMute(curURI.GetChars(), metaData.GetChars());

        return NPT_SUCCESS;
    }


private:
    std::shared_ptr<MediaRendererDelegate> m_Delegate;
    PLT_MediaRenderer * m_Renderer;
};

class MediaRendererImpl : public MediaRenderer {
public:
    MediaRendererImpl(MediaRendererDelegate * delegate,
                      const char*  friendly_name,
                      bool         show_ip,
                      const char*  uuid,
                      unsigned int port,
                      bool         port_rebind)
        : m_Upnp()
        , m_Delegate(delegate)
        , m_FriendlyName(friendly_name)
        , m_ShowIp(show_ip)
        , m_Uuid(uuid)
        , m_Port(port)
        , m_PortRebind(port_rebind) {

        PLT_MediaRenderer * renderer =
                new PLT_MediaRenderer(m_FriendlyName,
                                      m_ShowIp,
                                      m_Uuid,
                                      m_Port,
                                      m_PortRebind);
        m_PLTDelegate = std::make_shared<PLT_MediaRendererDelegateImpl>(renderer, delegate);
        renderer->SetDelegate(m_PLTDelegate.get());

        PLT_DeviceHostReference device(renderer);

        m_Upnp.AddDevice(device);
    }

    virtual ~MediaRendererImpl() {
    }

public:
    virtual void Start();
    virtual void Stop();

private:
    PLT_UPnP m_Upnp;

    std::shared_ptr<MediaRendererDelegate> m_Delegate;

    const char*  m_FriendlyName;
    bool         m_ShowIp;
    const char*  m_Uuid;
    unsigned int m_Port;
    bool         m_PortRebind;

    std::shared_ptr<PLT_MediaRendererDelegateImpl> m_PLTDelegate;
};

void MediaRendererImpl::Start() {
    m_Upnp.Start();
}

void MediaRendererImpl::Stop() {
    m_Upnp.Stop();
}

}

MediaRenderer * create_media_renderer(MediaRendererDelegate * delegate,
                                      const char*  friendly_name,
                                      bool         show_ip,
                                      const char*  uuid,
                                      unsigned int port,
                                      bool         port_rebind) {
    return new media_renderer::MediaRendererImpl(delegate,
                                                 friendly_name,
                                                 show_ip,
                                                 uuid,
                                                 port,
                                                 port_rebind);
}

void release_media_renderer(MediaRenderer * renderer) {
    if (renderer)
        delete renderer;
}
