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

#include "stream_cachedmessageallocator.h"
#include "stream_common.h"
#include "stream_control_message.h"
#include "stream_isessionnotify.h"
#include "stream_session_data.h"

#include "bittorrent_common.h"
#include "bittorrent_defines.h"
#include "bittorrent_message.h"
#include "bittorrent_sessionmessage.h"
#include "bittorrent_stream.h"
#include "bittorrent_stream_common.h"
#include "bittorrent_streamhandler.h"

#include "test_i_gtk_common.h"


struct BitTorrent_Client_ConnectionState;
struct BitTorrent_Client_UserData;
struct BitTorrent_Client_PeerSessionData
 : BitTorrent_PeerSessionData
{
  inline BitTorrent_Client_PeerSessionData ()
   : BitTorrent_PeerSessionData ()
   , connectionState (NULL)
//   , userData (NULL)
  {};
  inline BitTorrent_Client_PeerSessionData& operator+= (const BitTorrent_Client_PeerSessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    BitTorrent_PeerSessionData::operator+= (rhs_in);

    return *this;
  }

  struct BitTorrent_Client_ConnectionState* connectionState;
//  struct BitTorrent_Client_UserData*        userData;
};

struct BitTorrent_Client_TrackerSessionData
 : BitTorrent_TrackerSessionData
{
  inline BitTorrent_Client_TrackerSessionData ()
   : BitTorrent_TrackerSessionData ()
   , connectionState (NULL)
//   , userData (NULL)
  {};
  inline BitTorrent_Client_TrackerSessionData& operator+= (const BitTorrent_Client_TrackerSessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    BitTorrent_TrackerSessionData::operator+= (rhs_in);

    return *this;
  }

  struct BitTorrent_Client_ConnectionState* connectionState;
//  struct BitTorrent_Client_UserData*        userData;
};

typedef Stream_SessionData_T<struct BitTorrent_Client_PeerSessionData> BitTorrent_Client_PeerSessionData_t;
typedef Stream_SessionData_T<struct BitTorrent_Client_TrackerSessionData> BitTorrent_Client_TrackerSessionData_t;

typedef BitTorrent_Message_T<BitTorrent_Client_PeerSessionData_t,
                             struct BitTorrent_Client_UserData> BitTorrent_Client_PeerMessage_t;
typedef BitTorrent_TrackerMessage_T<BitTorrent_Client_TrackerSessionData_t,
                                    struct BitTorrent_Client_UserData> BitTorrent_Client_TrackerMessage_t;
typedef BitTorrent_SessionMessage_T<struct BitTorrent_Client_PeerSessionData,
                                    struct BitTorrent_Client_UserData> BitTorrent_Client_PeerSessionMessage_t;
typedef BitTorrent_SessionMessage_T<struct BitTorrent_Client_TrackerSessionData,
                                    struct BitTorrent_Client_UserData> BitTorrent_Client_TrackerSessionMessage_t;

typedef Stream_ISessionDataNotify_T<Net_ConnectionId_t,
                                    struct BitTorrent_Client_PeerSessionData,
                                    enum Stream_SessionMessageType,
                                    BitTorrent_Client_PeerMessage_t,
                                    BitTorrent_Client_PeerSessionMessage_t> BitTorrent_Client_IPeerNotify_t;
typedef Stream_ISessionDataNotify_T<Net_ConnectionId_t,
                                    struct BitTorrent_Client_TrackerSessionData,
                                    enum Stream_SessionMessageType,
                                    BitTorrent_Client_TrackerMessage_t,
                                    BitTorrent_Client_TrackerSessionMessage_t> BitTorrent_Client_ITrackerNotify_t;

typedef Stream_ControlMessage_T<enum Stream_ControlMessageType,
                                struct BitTorrent_AllocatorConfiguration,
                                BitTorrent_Client_PeerMessage_t,
                                BitTorrent_Client_PeerSessionMessage_t> BitTorrent_Client_PeerControlMessage_t;
typedef Stream_ControlMessage_T<enum Stream_ControlMessageType,
                                struct BitTorrent_AllocatorConfiguration,
                                BitTorrent_Client_TrackerMessage_t,
                                BitTorrent_Client_TrackerSessionMessage_t> BitTorrent_Client_TrackerControlMessage_t;

typedef BitTorrent_PeerStream_T<struct BitTorrent_Client_PeerStreamState,
                                struct BitTorrent_Client_StreamConfiguration,
                                BitTorrent_RuntimeStatistic_t,
                                struct BitTorrent_Client_ModuleHandlerConfiguration,
                                struct BitTorrent_Client_PeerSessionData,
                                BitTorrent_Client_PeerSessionData_t,
                                BitTorrent_Client_PeerControlMessage_t,
                                BitTorrent_Client_PeerMessage_t,
                                BitTorrent_Client_PeerSessionMessage_t,
                                struct BitTorrent_Client_Configuration,
                                struct BitTorrent_Client_ConnectionState,
                                struct BitTorrent_Client_SocketHandlerConfiguration,
                                struct BitTorrent_Client_SessionState,
                                struct BitTorrent_Client_GTK_CBData> BitTorrent_Client_PeerStream_t;
typedef BitTorrent_TrackerStream_T<struct BitTorrent_Client_TrackerStreamState,
                                   struct BitTorrent_Client_StreamConfiguration,
                                   BitTorrent_RuntimeStatistic_t,
                                   struct BitTorrent_Client_ModuleHandlerConfiguration,
                                   struct BitTorrent_Client_TrackerSessionData,
                                   BitTorrent_Client_TrackerSessionData_t,
                                   BitTorrent_Client_TrackerControlMessage_t,
                                   BitTorrent_Client_TrackerMessage_t,
                                   BitTorrent_Client_TrackerSessionMessage_t,
                                   BitTorrent_Client_PeerStream_t,
                                   struct BitTorrent_Client_Configuration,
                                   struct BitTorrent_Client_ConnectionState,
                                   struct BitTorrent_Client_SocketHandlerConfiguration,
                                   struct BitTorrent_Client_SessionState,
                                   struct BitTorrent_Client_GTK_CBData> BitTorrent_Client_TrackerStream_t;

struct BitTorrent_Client_GTK_CBData;
template <typename AddressType,
          typename ConnectionConfigurationType,
          typename ConnectionStateType,
          typename StatisticContainerType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename StreamType,
          typename StreamStatusType,
          typename ConfigurationType,
          typename StateType>
class BitTorrent_ISession_T;
struct BitTorrent_Client_Configuration;
//typedef Stream_Statistic BitTorrent_RuntimeStatistic_t;
struct Net_SocketConfiguration;
struct BitTorrent_Client_SocketHandlerConfiguration;
class BitTorrent_Client_PeerStream;
enum Stream_StateMachine_ControlState;
struct BitTorrent_Client_SessionConfiguration;
struct BitTorrent_Client_SessionState;
typedef BitTorrent_ISession_T<ACE_INET_Addr,
                              struct BitTorrent_Client_Configuration,
                              struct BitTorrent_Client_ConnectionState,
                              BitTorrent_RuntimeStatistic_t,
                              struct Net_SocketConfiguration,
                              struct BitTorrent_Client_SocketHandlerConfiguration,
                              BitTorrent_Client_PeerStream_t,
                              enum Stream_StateMachine_ControlState,
                              struct BitTorrent_Client_SessionConfiguration,
                              struct BitTorrent_Client_SessionState> BitTorrent_Client_ISession_t;
struct BitTorrent_Client_StreamConfiguration;
struct BitTorrent_Client_ModuleHandlerConfiguration
 : BitTorrent_ModuleHandlerConfiguration
{
  inline BitTorrent_Client_ModuleHandlerConfiguration ()
   : BitTorrent_ModuleHandlerConfiguration ()
   ///////////////////////////////////////
   , CBData (NULL)
   , session (NULL)
   , streamConfiguration (NULL)
   , subscriber (NULL)
   , trackerSubscriber (NULL)
   , userData (NULL)
  {};

  struct BitTorrent_Client_GTK_CBData*          CBData;
  BitTorrent_Client_ISession_t*                 session;
  struct BitTorrent_Client_StreamConfiguration* streamConfiguration;
  BitTorrent_Client_IPeerNotify_t*              subscriber; // (initial) subscriber
  BitTorrent_Client_ITrackerNotify_t*           trackerSubscriber; // (initial) subscriber

  struct BitTorrent_Client_UserData*            userData;
};

struct BitTorrent_Client_PeerStreamState
 : BitTorrent_PeerStreamState
{
  inline BitTorrent_Client_PeerStreamState ()
   : BitTorrent_PeerStreamState ()
   , sessionData (NULL)
   , userData (NULL)
  {};

  struct BitTorrent_Client_PeerSessionData* sessionData;
  struct BitTorrent_Client_UserData*        userData;
};
struct BitTorrent_Client_TrackerStreamState
 : BitTorrent_TrackerStreamState
{
  inline BitTorrent_Client_TrackerStreamState ()
   : BitTorrent_TrackerStreamState ()
   , sessionData (NULL)
   , userData (NULL)
  {};

  struct BitTorrent_Client_TrackerSessionData* sessionData;
  struct BitTorrent_Client_UserData*           userData;
};

struct BitTorrent_Client_StreamConfiguration
 : BitTorrent_StreamConfiguration
{
  inline BitTorrent_Client_StreamConfiguration ()
   : BitTorrent_StreamConfiguration ()
   , moduleHandlerConfiguration (NULL)
   , userData (NULL)
  {}

  struct BitTorrent_Client_ModuleHandlerConfiguration* moduleHandlerConfiguration; // module handler configuration

  struct BitTorrent_Client_UserData*                   userData;
};

typedef Stream_CachedMessageAllocator_T<ACE_MT_SYNCH,
                                        struct BitTorrent_AllocatorConfiguration,
                                        BitTorrent_Client_PeerControlMessage_t,
                                        BitTorrent_Client_PeerMessage_t,
                                        BitTorrent_Client_PeerSessionMessage_t> BitTorrent_Client_PeerMessageAllocator_t;
typedef Stream_CachedMessageAllocator_T<ACE_MT_SYNCH,
                                        struct BitTorrent_AllocatorConfiguration,
                                        BitTorrent_Client_TrackerControlMessage_t,
                                        BitTorrent_Client_TrackerMessage_t,
                                        BitTorrent_Client_TrackerSessionMessage_t> BitTorrent_Client_TrackerMessageAllocator_t;

//typedef BitTorrent_StreamHandler_T<struct BitTorrent_Client_SessionData,
//                                   struct BitTorrent_Client_GTK_CBData> BitTorrent_Client_StreamHandler_t;

#endif
