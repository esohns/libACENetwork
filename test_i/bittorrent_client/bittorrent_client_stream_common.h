/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef BITTORRENT_CLIENT_STREAM_COMMON_H
#define BITTORRENT_CLIENT_STREAM_COMMON_H

#include <list>

#include "common_configuration.h"

#include "common_timer_manager_common.h"

#include "stream_cachedmessageallocator.h"
#include "stream_common.h"
#include "stream_control_message.h"
#include "stream_isessionnotify.h"
#include "stream_session_data.h"

#include "bittorrent_common.h"
#include "bittorrent_defines.h"
#include "bittorrent_isession.h"
#include "bittorrent_message.h"
#include "bittorrent_sessionmessage.h"
#include "bittorrent_stream_common.h"
#include "bittorrent_stream_peer.h"
#include "bittorrent_stream_tracker.h"

#include "test_i_gtk_common.h"

#include "bittorrent_client_common.h"
#include "bittorrent_client_network.h"
#include "bittorrent_client_streamhandler.h"

//struct BitTorrent_Client_PeerUserData;
struct BitTorrent_Client_PeerSessionData
 : BitTorrent_PeerSessionData
{
  BitTorrent_Client_PeerSessionData ()
   : BitTorrent_PeerSessionData ()
   , connectionState (NULL)
   , userData (NULL)
  {}
  struct BitTorrent_Client_PeerSessionData& operator+= (const struct BitTorrent_Client_PeerSessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    BitTorrent_PeerSessionData::operator+= (rhs_in);

    return *this;
  }

  struct BitTorrent_Client_PeerConnectionState* connectionState;

  struct BitTorrent_Client_PeerUserData*        userData;
};

struct BitTorrent_Client_TrackerConnectionState;
//struct BitTorrent_Client_TrackerUserData;
struct BitTorrent_Client_TrackerSessionData
 : BitTorrent_TrackerSessionData
{
  BitTorrent_Client_TrackerSessionData ()
   : BitTorrent_TrackerSessionData ()
   , connectionState (NULL)
   , userData (NULL)
  {}
  
  struct BitTorrent_Client_TrackerSessionData& operator+= (const struct BitTorrent_Client_TrackerSessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    BitTorrent_TrackerSessionData::operator+= (rhs_in);

    return *this;
  }

  struct BitTorrent_Client_TrackerConnectionState* connectionState;

  struct BitTorrent_Client_TrackerUserData*        userData;
};

typedef Stream_SessionData_T<struct BitTorrent_Client_PeerSessionData> BitTorrent_Client_PeerSessionData_t;
typedef Stream_SessionData_T<struct BitTorrent_Client_TrackerSessionData> BitTorrent_Client_TrackerSessionData_t;

typedef BitTorrent_Message_T<BitTorrent_Client_PeerSessionData_t,
                             struct BitTorrent_Client_PeerUserData> BitTorrent_Client_PeerMessage_t;
typedef BitTorrent_TrackerMessage_T<BitTorrent_Client_TrackerSessionData_t,
                                    struct BitTorrent_Client_TrackerUserData> BitTorrent_Client_TrackerMessage_t;
typedef BitTorrent_SessionMessage_T<struct BitTorrent_Client_PeerSessionData,
                                    struct BitTorrent_Client_PeerUserData> BitTorrent_Client_PeerSessionMessage_t;
typedef BitTorrent_SessionMessage_T<struct BitTorrent_Client_TrackerSessionData,
                                    struct BitTorrent_Client_TrackerUserData> BitTorrent_Client_TrackerSessionMessage_t;

typedef Stream_ISessionDataNotify_T<Stream_SessionId_t,
                                    struct BitTorrent_Client_PeerSessionData,
                                    enum Stream_SessionMessageType,
                                    BitTorrent_Client_PeerMessage_t,
                                    BitTorrent_Client_PeerSessionMessage_t> BitTorrent_Client_IPeerNotify_t;
typedef Stream_ISessionDataNotify_T<Stream_SessionId_t,
                                    struct BitTorrent_Client_TrackerSessionData,
                                    enum Stream_SessionMessageType,
                                    BitTorrent_Client_TrackerMessage_t,
                                    BitTorrent_Client_TrackerSessionMessage_t> BitTorrent_Client_ITrackerNotify_t;
typedef std::list<BitTorrent_Client_IPeerNotify_t*> BitTorrent_Client_IPeerSubscribers_t;
typedef BitTorrent_Client_IPeerSubscribers_t::const_iterator BitTorrent_Client_IPeerSubscribersIterator_t;
typedef std::list<BitTorrent_Client_ITrackerNotify_t*> BitTorrent_Client_ITrackerSubscribers_t;
typedef BitTorrent_Client_ITrackerSubscribers_t::const_iterator BitTorrent_Client_ITrackerSubscribersIterator_t;

typedef Stream_ControlMessage_T<enum Stream_ControlType,
                                enum Stream_ControlMessageType,
                                struct BitTorrent_AllocatorConfiguration> BitTorrent_Client_ControlMessage_t;

//struct BitTorrent_Client_PeerUserData;
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 BitTorrent_Client_PeerConnectionConfiguration_t,
                                 struct BitTorrent_Client_PeerConnectionState,
                                 BitTorrent_Statistic_t,
                                 struct BitTorrent_Client_PeerUserData> BitTorrent_Client_PeerConnection_Manager_t;
typedef BitTorrent_PeerStream_T<struct BitTorrent_Client_PeerStreamState,
                                struct BitTorrent_Client_PeerStreamConfiguration,
                                BitTorrent_Statistic_t,
                                Common_Timer_Manager_t,
                                struct BitTorrent_Client_PeerModuleHandlerConfiguration,
                                struct BitTorrent_Client_PeerSessionData,
                                BitTorrent_Client_PeerSessionData_t,
                                BitTorrent_Client_ControlMessage_t,
                                BitTorrent_Client_PeerMessage_t,
                                BitTorrent_Client_PeerSessionMessage_t,
                                BitTorrent_Client_PeerConnectionConfiguration_t,
                                struct BitTorrent_Client_PeerConnectionState,
                                struct BitTorrent_Client_SocketHandlerConfiguration,
                                struct BitTorrent_Client_SessionState,
                                BitTorrent_Client_PeerConnection_Manager_t,
                                struct BitTorrent_Client_PeerUserData> BitTorrent_Client_PeerStream_t;

typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 BitTorrent_Client_TrackerConnectionConfiguration_t,
                                 struct BitTorrent_Client_TrackerConnectionState,
                                 BitTorrent_Statistic_t,
                                 struct BitTorrent_Client_TrackerUserData> BitTorrent_Client_TrackerConnection_Manager_t;
typedef BitTorrent_TrackerStream_T<struct BitTorrent_Client_TrackerStreamState,
                                   struct BitTorrent_Client_TrackerStreamConfiguration,
                                   BitTorrent_Statistic_t,
                                   Common_Timer_Manager_t,
                                   struct BitTorrent_Client_TrackerModuleHandlerConfiguration,
                                   struct BitTorrent_Client_TrackerSessionData,
                                   BitTorrent_Client_TrackerSessionData_t,
                                   BitTorrent_Client_ControlMessage_t,
                                   BitTorrent_Client_TrackerMessage_t,
                                   BitTorrent_Client_TrackerSessionMessage_t,
                                   BitTorrent_Client_PeerStream_t,
                                   BitTorrent_Client_TrackerConnectionConfiguration_t,
                                   struct BitTorrent_Client_TrackerConnectionState,
                                   struct BitTorrent_Client_SocketHandlerConfiguration,
                                   struct BitTorrent_Client_SessionState,
                                   BitTorrent_Client_TrackerConnection_Manager_t,
                                   struct BitTorrent_Client_TrackerUserData> BitTorrent_Client_TrackerStream_t;

//struct BitTorrent_Client_GTK_CBData;
//struct BitTorrent_Client_PeerConnectionConfiguration;
//struct BitTorrent_Client_TrackerConnectionConfiguration;
//struct BitTorrent_Client_PeerConnectionState;
//struct Net_SocketConfiguration;
//struct BitTorrent_Client_PeerSocketHandlerConfiguration;
//struct BitTorrent_Client_TrackerSocketHandlerConfiguration;
//class BitTorrent_Client_PeerStream;
//enum Stream_StateMachine_ControlState;
//struct BitTorrent_Client_SessionConfiguration;
//struct BitTorrent_Client_SessionState;
//typedef BitTorrent_ISession_T<ACE_INET_Addr,
//                              BitTorrent_Client_PeerConnectionConfiguration_t,
//                              BitTorrent_Client_TrackerConnectionConfiguration_t,
//                              struct BitTorrent_Client_PeerConnectionState,
//                              BitTorrent_Statistic_t,
//                              struct Net_SocketConfiguration,
//                              struct BitTorrent_Client_PeerSocketHandlerConfiguration,
//                              struct BitTorrent_Client_TrackerSocketHandlerConfiguration,
//                              BitTorrent_Client_PeerStream_t,
//                              enum Stream_StateMachine_ControlState,
//                              struct BitTorrent_Client_SessionConfiguration,
//                              struct BitTorrent_Client_SessionState> BitTorrent_Client_ISession_t;
//extern const char stream_name_string_[];
struct BitTorrent_AllocatorConfiguration;
struct BitTorrent_Client_PeerStreamConfiguration;
struct BitTorrent_Client_PeerModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct BitTorrent_AllocatorConfiguration,
                               struct BitTorrent_Client_PeerStreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct BitTorrent_Client_PeerModuleHandlerConfiguration> BitTorrent_Client_PeerStreamConfiguration_t;
struct BitTorrent_Client_PeerModuleHandlerConfiguration
 : BitTorrent_PeerModuleHandlerConfiguration
{
  BitTorrent_Client_PeerModuleHandlerConfiguration ()
   : BitTorrent_PeerModuleHandlerConfiguration ()
   ///////////////////////////////////////
   , CBData (NULL)
   , session (NULL)
   , streamConfiguration (NULL)
   , subscriber (NULL)
   , subscribers (NULL)
   , userData (NULL)
  {}

  struct BitTorrent_Client_GTK_CBData*         CBData;
  BitTorrent_Client_ISession_t*                session;
  BitTorrent_Client_PeerStreamConfiguration_t* streamConfiguration;
  BitTorrent_Client_IPeerNotify_t*             subscriber; // (initial) subscriber
  BitTorrent_Client_IPeerSubscribers_t*        subscribers;

  struct BitTorrent_Client_PeerUserData*       userData;
};

struct BitTorrent_AllocatorConfiguration;
struct BitTorrent_Client_TrackerStreamConfiguration;
struct BitTorrent_Client_TrackerModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct BitTorrent_AllocatorConfiguration,
                               struct BitTorrent_Client_TrackerStreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct BitTorrent_Client_TrackerModuleHandlerConfiguration> BitTorrent_Client_TrackerStreamConfiguration_t;
struct BitTorrent_Client_TrackerModuleHandlerConfiguration
 : BitTorrent_TrackerModuleHandlerConfiguration
{
  BitTorrent_Client_TrackerModuleHandlerConfiguration ()
   : BitTorrent_TrackerModuleHandlerConfiguration ()
   ///////////////////////////////////////
   , CBData (NULL)
   , session (NULL)
   , streamConfiguration (NULL)
   , subscriber (NULL)
   , subscribers (NULL)
   , userData (NULL)
  {}

  struct BitTorrent_Client_GTK_CBData*            CBData;
  BitTorrent_Client_ISession_t*                   session;
  BitTorrent_Client_TrackerStreamConfiguration_t* streamConfiguration;
  BitTorrent_Client_ITrackerNotify_t*             subscriber; // (initial) subscriber
  BitTorrent_Client_ITrackerSubscribers_t*        subscribers;

  struct BitTorrent_Client_TrackerUserData*       userData;
};

struct BitTorrent_Client_PeerStreamState
 : BitTorrent_PeerStreamState
{
  BitTorrent_Client_PeerStreamState ()
   : BitTorrent_PeerStreamState ()
   , sessionData (NULL)
   , userData (NULL)
  {}

  struct BitTorrent_Client_PeerSessionData* sessionData;

  struct BitTorrent_Client_PeerUserData*    userData;
};
struct BitTorrent_Client_TrackerStreamState
 : BitTorrent_TrackerStreamState
{
  BitTorrent_Client_TrackerStreamState ()
   : BitTorrent_TrackerStreamState ()
   , sessionData (NULL)
   , userData (NULL)
  {}

  struct BitTorrent_Client_TrackerSessionData* sessionData;

  struct BitTorrent_Client_TrackerUserData*    userData;
};

struct BitTorrent_Client_PeerModuleHandlerConfiguration;
struct BitTorrent_Client_PeerStreamConfiguration
 : BitTorrent_PeerStreamConfiguration
{
  BitTorrent_Client_PeerStreamConfiguration ()
   : BitTorrent_PeerStreamConfiguration ()
   , userData (NULL)
  {}

  struct BitTorrent_Client_PeerUserData* userData;
};
struct BitTorrent_Client_TrackerModuleHandlerConfiguration;
struct BitTorrent_Client_TrackerStreamConfiguration
 : BitTorrent_TrackerStreamConfiguration
{
  BitTorrent_Client_TrackerStreamConfiguration ()
   : BitTorrent_TrackerStreamConfiguration ()
   , userData (NULL)
  {}

  struct BitTorrent_Client_TrackerUserData* userData;
};

typedef Stream_CachedMessageAllocator_T<ACE_MT_SYNCH,
                                        struct BitTorrent_AllocatorConfiguration,
                                        BitTorrent_Client_ControlMessage_t,
                                        BitTorrent_Client_PeerMessage_t,
                                        BitTorrent_Client_PeerSessionMessage_t> BitTorrent_Client_PeerMessageAllocator_t;
typedef Stream_CachedMessageAllocator_T<ACE_MT_SYNCH,
                                        struct BitTorrent_AllocatorConfiguration,
                                        BitTorrent_Client_ControlMessage_t,
                                        BitTorrent_Client_TrackerMessage_t,
                                        BitTorrent_Client_TrackerSessionMessage_t> BitTorrent_Client_TrackerMessageAllocator_t;

//typedef BitTorrent_StreamHandler_T<struct BitTorrent_Client_SessionData,
//                                   struct BitTorrent_Client_GTK_CBData> BitTorrent_Client_StreamHandler_t;

#endif
