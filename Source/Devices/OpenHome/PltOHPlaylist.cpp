/*****************************************************************
|
|   Platinum -  OpenHome playlist
|
| Copyright (c) 2015-2016, Albis Technologies AG.
| All rights reserved.
| http://www.albistechnologies.com
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of the GNU General Public License
| as published by the Free Software Foundation; either version 2
| of the License, or (at your option) any later version.
|
| OEMs, ISVs, VARs and other distributors that combine and
| distribute commercially licensed software with Platinum software
| and do not wish to distribute the source code for the commercially
| licensed software under version 2, or (at your option) any later
| version, of the GNU General Public License (the "GPL") must enter
| into a commercial license agreement with Albis Technologies AG and
| Plutinosoft, LLC.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; see the file LICENSE.txt. If not, write to
| the Free Software Foundation, Inc.,
| 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
| http://www.gnu.org/licenses/gpl-2.0.html
|
****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include "PltOHPlaylist.h"
#include "PltService.h"

NPT_SET_LOCAL_LOGGER("platinum.oh.playlist")

/*----------------------------------------------------------------------
|   external references
+---------------------------------------------------------------------*/
extern NPT_UInt8 OH_InfoSCPD[];
extern NPT_UInt8 OH_PlaylistSCPD[];
extern NPT_UInt8 OH_ProductSCPD[];
extern NPT_UInt8 OH_TimeSCPD[];
extern NPT_UInt8 OH_VolumeSCPD[];

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::PLT_OHPlaylist
+---------------------------------------------------------------------*/
PLT_OHPlaylist::PLT_OHPlaylist(const char*  friendly_name,
                                     bool         show_ip     /* = false */, 
                                     const char*  uuid        /* = NULL */, 
                                     unsigned int port        /* = 0 */,
                                     bool         port_rebind /* = false */) :  
    PLT_DeviceHost("/", 
                   uuid, 
                   "urn:av-openhome-org:service:Playlist:1",
                   friendly_name, 
                   show_ip, 
                   port, 
                   port_rebind),
    m_Delegate(NULL)
{
    m_ModelDescription = "Albis Technologies OpenHome Playlist";
    m_ModelName        = "OpenHome Playlist";
    m_ModelURL         = "http://www.albistechnologies.com";
    m_DlnaDoc          = "DMR-1.50";
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::~PLT_OHPlaylist
+---------------------------------------------------------------------*/
PLT_OHPlaylist::~PLT_OHPlaylist()
{
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::SetupServices
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::SetupServices()
{
    NPT_Reference<PLT_Service> service;

    {
        /* OHProduct */
        service = new PLT_Service(
            this,
            "urn:av-openhome-org:service:Product:1",
            "urn:av-openhome-org:serviceId:Product",
            "Product",
            "urn:schemas-upnp-org:service-1-0");
        NPT_CHECK_FATAL(service->SetSCPDXML((const char*) OH_ProductSCPD));
        NPT_CHECK_FATAL(AddService(service.AsPointer()));

        service->SetStateVariableRate("LastChange", NPT_TimeInterval(0.2f));

        NPT_String csxml = "<SourceList>\n";
        csxml+= NPT_String(" <Source>\n") + "  <Name>" + "Playlist" + "</Name>\n" + "  <Type>" + "Playlist" + "</Type>\n" +
                "  <Visible>true</Visible>\n" + "  </Source>\n" + "</SourceList>\n";

        service->SetStateVariable("ManufacturerName", "Albis Technologies");
        service->SetStateVariable("ManufacturerInfo", "SNK");
        service->SetStateVariable("ManufacturerUrl", "http://www.albistechnologies.com");
        service->SetStateVariable("ManufacturerImageUri", "http://www.albistechnologies.com");
        service->SetStateVariable("ModelName", "ModelName");
        service->SetStateVariable("ModelInfo", "ModelInfo");
        service->SetStateVariable("ModelUrl", "ModelUrl");
        service->SetStateVariable("ModelImageUri", "ModelImageUri");
        service->SetStateVariable("ProductRoom", "ProductRoom");
        service->SetStateVariable("ProductName", "ProductName");
        service->SetStateVariable("ProductInfo", "ProductInfo");
        service->SetStateVariable("ProductUrl", "ProductUrl");
        service->SetStateVariable("ProductImageUri", "ProductImageUri");
        service->SetStateVariable("Standby", "0");
        service->SetStateVariable("SourceCount", "1");
        service->SetStateVariable("SourceXml", csxml);
        service->SetStateVariable("SourceIndex", "0");
        service->SetStateVariable("Attributes", "Info Time Volume");

        service.Detach();
        service = NULL;
    }

    {
        /* OHVolume */
        service = new PLT_Service(
            this,
            "urn:av-openhome-org:service:Volume:1",
            "urn:av-openhome-org:serviceId:Volume",
            "Volume",
            "urn:schemas-upnp-org:service-1-0");
        NPT_CHECK_FATAL(service->SetSCPDXML((const char*) OH_VolumeSCPD));
        NPT_CHECK_FATAL(AddService(service.AsPointer()));

        service->SetStateVariableRate("LastChange", NPT_TimeInterval(0.2f));

        service->SetStateVariable("Volume", "100");
        service->SetStateVariable("Mute", "0");
        service->SetStateVariable("Balance", "0");
        service->SetStateVariable("Fade", "0");
        service->SetStateVariable("VolumeLimit", "100");
        service->SetStateVariable("VolumeMax", "100");
        service->SetStateVariable("VolumeUnity", "100");
        service->SetStateVariable("VolumeSteps", "100");
        service->SetStateVariable("VolumeMilliDbPerStep", "1024");
        service->SetStateVariable("BalanceMax", "0");
        service->SetStateVariable("FadeMax", "0");

        service.Detach();
        service = NULL;
    }

    {
        /* OHTime */
        service = new PLT_Service(
            this,
            "urn:av-openhome-org:service:Time:1",
            "urn:av-openhome-org:serviceId:Time",
            "Time",
            "urn:schemas-upnp-org:service-1-0");
        NPT_CHECK_FATAL(service->SetSCPDXML((const char*) OH_TimeSCPD));
        NPT_CHECK_FATAL(AddService(service.AsPointer()));

        service->SetStateVariableRate("LastChange", NPT_TimeInterval(0.2f));

        service->SetStateVariable("TrackCount", "0");
        service->SetStateVariable("Duration", "0");
        service->SetStateVariable("Seconds", "0");

        service->SetStateVariable("TrackCount", "");
        service->SetStateVariable("Duration", "");
        service->SetStateVariable("Seconds", "");

        service.Detach();
        service = NULL;
    }

    {
        /* OHInfo */
        service = new PLT_Service(
            this,
            "urn:av-openhome-org:service:Info:1",
            "urn:av-openhome-org:serviceId:Info",
            "Info",
            "urn:schemas-upnp-org:service-1-0");
        NPT_CHECK_FATAL(service->SetSCPDXML((const char*) OH_InfoSCPD));
        NPT_CHECK_FATAL(AddService(service.AsPointer()));

        service->SetStateVariableRate("LastChange", NPT_TimeInterval(0.2f));

        service->SetStateVariable("TrackCount", "0");
        service->SetStateVariable("DetailsCount", "0");
        service->SetStateVariable("MetatextCount", "0");
        service->SetStateVariable("Uri", "0");
        service->SetStateVariable("Metadata", "0");
        service->SetStateVariable("Duration", "0");
        service->SetStateVariable("BitRate", "0");
        service->SetStateVariable("BitDepth", "0");
        service->SetStateVariable("SampleRate", "0");
        service->SetStateVariable("Lossless", "0");
        service->SetStateVariable("CodecName", "0");
        service->SetStateVariable("Metatext", "0");

        NPT_String meta =
                "<DIDL-Lite xmlns:dc=\"http://purl.org/dc/elements/1.1/\" "
                "xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" "
                "xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" "
                "xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\">"
                "<item restricted=\"1\">"
                "<orig>snk</orig>"
                "</item></DIDL-Lite>";

        service->SetStateVariable("TrackCount", "0");
        service->SetStateVariable("DetailsCount", "0");
        service->SetStateVariable("MetatextCount", "0");
        service->SetStateVariable("Uri", "");
        service->SetStateVariable("Metadata", meta);
        service->SetStateVariable("Duration", "0");
        service->SetStateVariable("BitRate", "0");
        service->SetStateVariable("BitDepth", "0");
        service->SetStateVariable("SampleRate", "0");
        service->SetStateVariable("Lossless", "0");
        service->SetStateVariable("CodecName", "");
        service->SetStateVariable("Metatext", "");

        service.Detach();
        service = NULL;
    }

    {
        /* OHPlaylist */
        service = new PLT_Service(
            this,
            "urn:av-openhome-org:service:Playlist:1",
            "urn:av-openhome-org:serviceId:Playlist",
            "Playlist",
            "urn:schemas-upnp-org:service-1-0");
        NPT_CHECK_FATAL(service->SetSCPDXML((const char*) OH_PlaylistSCPD));
        NPT_CHECK_FATAL(AddService(service.AsPointer()));

        service->SetStateVariableRate("LastChange", NPT_TimeInterval(0.2f));

        service->SetStateVariable("TransportState", "Stopped");
        service->SetStateVariable("Repeat", "0");
        service->SetStateVariable("Shuffle", "0");
        service->SetStateVariable("Id", "0");
        service->SetStateVariable("TracksMax", "5000");
        /* can be override by inherent class */
        service->SetStateVariable("ProtocolInfo", "http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_PRO,http-get:*:video/x-ms-asf:DLNA.ORG_PN=MPEG4_P2_ASF_SP_G726,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_FULL,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_MED,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_BASE,http-get:*:audio/L16;rate=44100;channels=1:DLNA.ORG_PN=LPCM,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_NTSC,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_PRO,http-get:*:audio/L16;rate=44100;channels=2:DLNA.ORG_PN=LPCM,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_SM,http-get:*:video/x-ms-asf:DLNA.ORG_PN=VC1_ASF_AP_L1_WMA,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMDRM_WMABASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_FULL,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAFULL,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMABASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPLL_BASE,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_NTSC_XAC3,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMDRM_WMVSPLL_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPML_BASE,http-get:*:video/x-ms-asf:DLNA.ORG_PN=MPEG4_P2_ASF_ASP_L5_SO_G726,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_LRG,http-get:*:audio/mpeg:DLNA.ORG_PN=MP3,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL_XAC3,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAPRO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG1,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_TN,http-get:*:video/x-ms-asf:DLNA.ORG_PN=MPEG4_P2_ASF_ASP_L4_SO_G726,http-get:*:audio/L16;rate=48000;channels=2:DLNA.ORG_PN=LPCM,http-get:*:audio/mpeg:DLNA.ORG_PN=MP3X,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPML_MP3,http-get:*:video/x-ms-wmv:*");

        service.Detach();
        service = NULL;
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::OnAction
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnAction(PLT_ActionReference&          action,
                            const PLT_HttpRequestContext& context)
{
    NPT_COMPILER_UNUSED(context);

    NPT_String tmp;

    /* parse the action name */
    NPT_String name = action->GetActionDesc().GetName();

    NPT_String serviceType = action->GetActionDesc().GetService()->GetServiceType();

#if 0
    printf("PLT_OHPlaylist::OnAction serviceType %s name %s\n", serviceType.GetChars(), name.GetChars());
#endif

    if (serviceType.Compare("urn:av-openhome-org:service:Playlist:1", true) == 0) {
        if (name.Compare("Insert", true) == 0) {
           return OnPlaylistInsert(action);
        }
        if (name.Compare("DeleteAll", true) == 0) {
           return OnPlaylistDeleteAll(action);
        }
        if (name.Compare("ReadList", true) == 0) {
           return OnPlaylistReadList(action);
        }
        if (name.Compare("IdArray", true) == 0) {
           return OnPlaylistIdArray(action);
        }
        if (name.Compare("SeekIndex", true) == 0) {
           return OnPlaylistSeekIndex(action);
        }
        if (name.Compare("DeleteId", true) == 0) {
           return OnPlaylistDeleteId(action);
        }
        if (name.Compare("SeekId", true) == 0) {
           return OnPlaylistSeekId(action);
        }
        if (name.Compare("Play", true) == 0) {
           return OnPlaylistPlay(action);
        }
        if (name.Compare("Pause", true) == 0) {
           return OnPlaylistPause(action);
        }
        if (name.Compare("Stop", true) == 0) {
           return OnPlaylistStop(action);
        }
        if (name.Compare("Next", true) == 0) {
           return OnPlaylistNext(action);
        }
        if (name.Compare("Previous", true) == 0) {
           return OnPlaylistPrevious(action);
        }
        if (name.Compare("SeekSecondAbsolute", true) == 0) {
           return OnPlaylistSeekSecondAbsolute(action);
        }
        if (name.Compare("SeekSecondRelative", true) == 0) {
           return OnPlaylistSeekSecondRelative(action);
        }
        if (name.Compare("SetRepeat", true) == 0) {
           return OnPlaylistSetRepeat(action);
        }
        if (name.Compare("Repeat", true) == 0) {
           return OnPlaylistRepeat(action);
        }
        if (name.Compare("SetShuffle", true) == 0) {
           return OnPlaylistSetShuffle(action);
        }
        if (name.Compare("Shuffle", true) == 0) {
           return OnPlaylistShuffle(action);
        }
    }

    if (serviceType.Compare("urn:av-openhome-org:service:Volume:1", true) == 0) {
        if (name.Compare("SetVolume", true) == 0) {
           return OnPlaylistSetVolume(action);
        }
        if (name.Compare("VolumeInc", true) == 0) {
           return OnPlaylistVolumeInc(action);
        }
        if (name.Compare("VolumeDec", true) == 0) {
           return OnPlaylistVolumeDec(action);
        }
        if (name.Compare("Volume", true) == 0) {
           return OnPlaylistVolume(action);
        }
        if (name.Compare("SetMute", true) == 0) {
           return OnPlaylistSetMute(action);
        }
        if (name.Compare("Mute", true) == 0) {
           return OnPlaylistMute(action);
        }
    }

    // other actions rely on state variables
    NPT_CHECK_LABEL_WARNING(action->SetArgumentsOutFromStateVariable(), failure);
    return NPT_SUCCESS;

failure:
    action->SetError(401,"No Such Action.");
    return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistInsert
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistInsert(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistInsert(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistDeleteAll
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistDeleteAll(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistDeleteAll(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistReadList
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistReadList(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistReadList(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistIdArray
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistIdArray(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistIdArray(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistSeekIndex
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistSeekIndex(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistSeekIndex(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistDeleteId
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistDeleteId(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistDeleteId(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistSeekId
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistSeekId(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistSeekId(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistPlay
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistPlay(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistPlay(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistPause
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistPause(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistPause(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistStop
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistStop(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistStop(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistNext
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistNext(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistNext(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistPrevious
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistPrevious(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistPrevious(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistSeekSecondAbsolute
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistSeekSecondAbsolute(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistSeekSecondAbsolute(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistSeekSecondRelative
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistSeekSecondRelative(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistSeekSecondRelative(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistSetRepeat
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistSetRepeat(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistSetRepeat(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistRepeat
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistRepeat(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistRepeat(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistSetShuffle
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistSetShuffle(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistSetShuffle(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistShuffle
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistShuffle(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistShuffle(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistSetVolume
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistSetVolume(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistSetVolume(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistVolume
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistVolume(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistVolume(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistVolumeInc
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistVolumeInc(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistVolumeInc(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistVolumeDec
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistVolumeDec(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistVolumeDec(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistSetMute
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistSetMute(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistSetMute(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_OHPlaylist::OnPlaylistMute
+---------------------------------------------------------------------*/
NPT_Result
PLT_OHPlaylist::OnPlaylistMute(PLT_ActionReference& action)
{
    if (m_Delegate) {
        return m_Delegate->OnPlaylistMute(action);
    }
    return NPT_ERROR_NOT_IMPLEMENTED;
}
