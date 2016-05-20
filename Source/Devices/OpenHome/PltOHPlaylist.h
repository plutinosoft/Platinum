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

#pragma once

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "PltDeviceHost.h"

/*----------------------------------------------------------------------
|   PLT_OHPlaylistDelegate
+---------------------------------------------------------------------*/
class PLT_OHPlaylistDelegate
{
	public:
		virtual ~PLT_OHPlaylistDelegate() {}

		/* OH Playlist */
		virtual NPT_Result OnPlaylistInsert(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistDeleteAll(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistDeleteId(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistReadList(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistIdArray(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistSeekIndex(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistSeekId(PLT_ActionReference& action) = 0;

		virtual NPT_Result OnPlaylistPlay(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistPause(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistStop(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistNext(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistPrevious(PLT_ActionReference& action) = 0;

		virtual NPT_Result OnPlaylistSeekSecondAbsolute(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistSeekSecondRelative(PLT_ActionReference& action) = 0;

		virtual NPT_Result OnPlaylistSetRepeat(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistRepeat(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistSetShuffle(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistShuffle(PLT_ActionReference& action) = 0;

		/* OH Volume */
		virtual NPT_Result OnPlaylistSetVolume(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistVolume(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistVolumeInc(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistVolumeDec(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistSetMute(PLT_ActionReference& action) = 0;
		virtual NPT_Result OnPlaylistMute(PLT_ActionReference& action) = 0;
};

/*----------------------------------------------------------------------
|   PLT_OHPlaylist
+---------------------------------------------------------------------*/
class PLT_OHPlaylist : public PLT_DeviceHost
{
	public:
		PLT_OHPlaylist(const char*  friendly_name,
					   bool         show_ip = false,
					   const char*  uuid = NULL,
					   unsigned int port = 0,
					   bool         port_rebind = false);

		// methods
		virtual void SetDelegate(PLT_OHPlaylistDelegate* delegate) { m_Delegate = delegate; }

		// PLT_DeviceHost methods
		virtual NPT_Result SetupServices();
		virtual NPT_Result OnAction(PLT_ActionReference&          action,
									const PLT_HttpRequestContext& context);

	protected:
		virtual ~PLT_OHPlaylist();

		/* OH Playlist */
		virtual NPT_Result OnPlaylistInsert(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistDeleteAll(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistDeleteId(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistReadList(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistIdArray(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistSeekIndex(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistSeekId(PLT_ActionReference& action);

		virtual NPT_Result OnPlaylistPlay(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistPause(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistStop(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistNext(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistPrevious(PLT_ActionReference& action);

		virtual NPT_Result OnPlaylistSeekSecondAbsolute(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistSeekSecondRelative(PLT_ActionReference& action);

		virtual NPT_Result OnPlaylistSetRepeat(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistRepeat(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistSetShuffle(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistShuffle(PLT_ActionReference& action);

		/* OH Volume */
		virtual NPT_Result OnPlaylistSetVolume(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistVolume(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistVolumeInc(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistVolumeDec(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistSetMute(PLT_ActionReference& action);
		virtual NPT_Result OnPlaylistMute(PLT_ActionReference& action);

	private:
		PLT_OHPlaylistDelegate* m_Delegate;
};

