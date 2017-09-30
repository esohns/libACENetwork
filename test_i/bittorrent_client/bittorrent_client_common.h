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

#ifndef BITTORRENT_CLIENT_COMMON_H
#define BITTORRENT_CLIENT_COMMON_H

#include <string>

#include "common.h"

#include "stream_common.h"

#include "net_common.h"
#include "net_defines.h"

#include "bittorrent_common.h"
#include "bittorrent_isession.h"
#include "bittorrent_stream_common.h"

#include "bittorrent_client_stream_common.h"

struct BitTorrent_Client_CursesState;
struct BitTorrent_Client_SignalHandlerConfiguration
 : Common_SignalHandlerConfiguration
{
  inline BitTorrent_Client_SignalHandlerConfiguration ()
   : Common_SignalHandlerConfiguration ()
   , cursesState (NULL)
  {};

  struct BitTorrent_Client_CursesState* cursesState;
};

struct BitTorrent_Client_PeerConnectionConfiguration;
struct BitTorrent_Client_TrackerConnectionConfiguration;
struct BitTorrent_Client_PeerConnectionState;
struct BitTorrent_Client_PeerSocketHandlerConfiguration;
struct BitTorrent_Client_TrackerSocketHandlerConfiguration;
struct BitTorrent_Client_PeerStreamState;
struct BitTorrent_Client_PeerStreamConfiguration;
struct BitTorrent_Client_TrackerStreamConfiguration;
struct BitTorrent_Client_PeerModuleHandlerConfiguration;
struct BitTorrent_Client_TrackerModuleHandlerConfiguration;
struct BitTorrent_Client_PeerSessionData;
template <typename DataType>
class Stream_SessionData_T;
typedef Stream_SessionData_T<struct BitTorrent_Client_PeerSessionData> BitTorrent_Client_PeerSessionData_t;
template <typename ControlType,
          typename MessageType,
          typename AllocatorConfigurationType>
class Stream_ControlMessage_T;
struct BitTorrent_Client_PeerUserData;
struct BitTorrent_Client_TrackerUserData;
template <typename SessionDataType,
          typename UserDataType>
class BitTorrent_Message_T;
typedef BitTorrent_Message_T<BitTorrent_Client_PeerSessionData_t,
                             struct BitTorrent_Client_PeerUserData> BitTorrent_Client_PeerMessage_t;
template <typename SessionDataType,
          typename UserDataType>
class BitTorrent_SessionMessage_T;
typedef BitTorrent_SessionMessage_T<struct BitTorrent_Client_PeerSessionData,
                                    struct BitTorrent_Client_PeerUserData> BitTorrent_Client_PeerSessionMessage_t;
typedef Stream_ControlMessage_T<enum Stream_ControlType,
                                enum Stream_ControlMessageType,
                                struct BitTorrent_AllocatorConfiguration> BitTorrent_Client_ControlMessage_t;
template <typename StreamStateType,
          typename ConfigurationType,
          typename StatisticContainerType,
          typename TimerManagerType,
          typename ModuleHandlerConfigurationType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConnectionConfigurationType,
          typename ConnectionStateType,
          typename HandlerConfigurationType,
          typename SessionStateType,
          typename CBDataType,
          typename UserDataType>
class BitTorrent_PeerStream_T;
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
                                struct BitTorrent_Client_PeerConnectionConfiguration,
                                struct BitTorrent_Client_PeerConnectionState,
                                struct BitTorrent_Client_SocketHandlerConfiguration,
                                struct BitTorrent_Client_SessionState,
                                struct BitTorrent_Client_GTK_CBData,
                                struct BitTorrent_Client_PeerUserData> BitTorrent_Client_PeerStream_t;
//typedef BitTorrent_PeerStream_T<struct BitTorrent_Client_PeerStreamState,
//                                struct BitTorrent_Client_PeerStreamConfiguration,
//                                BitTorrent_Statistic_t,
//                                BitTorrent_StatisticHandler_Proactor_t,
//                                struct BitTorrent_Client_PeerModuleHandlerConfiguration,
//                                struct BitTorrent_Client_PeerSessionData,
//                                BitTorrent_Client_PeerSessionData_t,
//                                BitTorrent_Client_ControlMessage_t,
//                                BitTorrent_Client_PeerMessage_t,
//                                BitTorrent_Client_PeerSessionMessage_t,
//                                struct BitTorrent_Client_PeerConnectionConfiguration,
//                                struct BitTorrent_Client_PeerConnectionState,
//                                struct BitTorrent_Client_SocketHandlerConfiguration,
//                                struct BitTorrent_Client_SessionState,
//                                struct BitTorrent_Client_GTK_CBData,
//                                struct BitTorrent_Client_PeerUserData> BitTorrent_Client_AsynchPeerStream_t;
struct BitTorrent_Client_PeerConfigurationConfiguration;
struct BitTorrent_Client_TrackerConfigurationConfiguration;
struct BitTorrent_Client_PeerConnectionState;
struct BitTorrent_Client_PeerSocketHandlerConfiguration;
struct BitTorrent_Client_TrackerSocketHandlerConfiguration;
struct BitTorrent_Client_SessionConfiguration;
struct BitTorrent_Client_SessionState;
//typedef BitTorrent_ISession_T<ACE_INET_Addr,
//                              struct BitTorrent_Client_PeerConfigurationConfiguration,
//                              struct BitTorrent_Client_TrackerConfigurationConfiguration,
//                              struct BitTorrent_Client_PeerConnectionState,
//                              BitTorrent_Statistic_t,
//                              struct Net_SocketConfiguration,
//                              struct BitTorrent_Client_PeerSocketHandlerConfiguration,
//                              struct BitTorrent_Client_TrackerSocketHandlerConfiguration,
//                              BitTorrent_Client_PeerStream_t,
//                              enum Stream_StateMachine_ControlState,
//                              struct BitTorrent_Client_SessionConfiguration,
//                              struct BitTorrent_Client_SessionState> BitTorrent_Client_ISession_t;
template <typename SessionInterfaceType>
class BitTorrent_IControl_T;
typedef BitTorrent_IControl_T<BitTorrent_Client_ISession_t> BitTorrent_Client_IControl_t;
struct BitTorrent_Client_ModuleHandlerConfiguration;
struct BitTorrent_Client_ThreadData
{
  inline BitTorrent_Client_ThreadData ()
   : configuration (NULL)
   , controller (NULL)
   , cursesState (NULL)
   , filename ()
//   , groupID (-1)
   , useReactor (NET_EVENT_USE_REACTOR)
  {};

  struct BitTorrent_Client_Configuration* configuration;
  BitTorrent_Client_IControl_t*           controller;
  struct BitTorrent_Client_CursesState*   cursesState;
  std::string                             filename; // metainfo (aka '.torrent') file URI
//  int                                     groupID;
  bool                                    useReactor;
};

#endif
