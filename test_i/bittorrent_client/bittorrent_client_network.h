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

#ifndef BITTORRENT_CLIENT_NETWORK_H
#define BITTORRENT_CLIENT_NETWORK_H

#include <map>
#include <string>

#include <ace/Global_Macros.h>
#include <ace/INET_Addr.h>
#include <ace/Singleton.h>
#include <ace/Synch_Traits.h>

#include "net_asynch_tcpsockethandler.h"
#include "net_connection_manager.h"
#include "net_iconnectionmanager.h"
#include "net_iconnector.h"
#include "net_stream_asynch_tcpsocket_base.h"
#include "net_stream_tcpsocket_base.h"
#include "net_tcpsockethandler.h"
#include "net_tcpconnection_base.h"

#include "net_client_asynchconnector.h"
#include "net_client_connector.h"

#include "bittorrent_isession.h"
#include "bittorrent_network.h"
#include "bittorrent_stream_common.h"

#include "bittorrent_client_common.h"
//#include "bittorrent_client_stream.h"
#include "bittorrent_client_stream_common.h"

// forward declarations
struct BitTorrent_Client_PeerConnectionConfiguration;
struct BitTorrent_Client_TrackerConnectionConfiguration;
struct BitTorrent_Client_PeerConnectionState;
struct BitTorrent_Client_TrackerConnectionState;
struct BitTorrent_Client_PeerUserData;
struct BitTorrent_Client_TrackerUserData;
struct BitTorrent_Client_PeerSocketHandlerConfiguration;
struct BitTorrent_Client_TrackerSocketHandlerConfiguration;

//////////////////////////////////////////

typedef Net_IConnection_T<ACE_INET_Addr,
                          struct BitTorrent_Client_PeerConnectionConfiguration,
                          struct BitTorrent_Client_PeerConnectionState,
                          BitTorrent_RuntimeStatistic_t> BitTorrent_Client_IPeerConnection_t;
typedef Net_IConnection_T<ACE_INET_Addr,
                          struct BitTorrent_Client_TrackerConnectionConfiguration,
                          struct BitTorrent_Client_TrackerConnectionState,
                          BitTorrent_RuntimeStatistic_t> BitTorrent_Client_ITrackerConnection_t;

typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                struct BitTorrent_Client_PeerConnectionConfiguration,
                                struct BitTorrent_Client_PeerConnectionState,
                                BitTorrent_RuntimeStatistic_t,
                                struct Net_SocketConfiguration,
                                struct BitTorrent_Client_PeerSocketHandlerConfiguration,
                                BitTorrent_Client_PeerStream_t,
                                enum Stream_StateMachine_ControlState> BitTorrent_Client_IPeerStreamConnection_t;
typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                struct BitTorrent_Client_TrackerConnectionConfiguration,
                                struct BitTorrent_Client_TrackerConnectionState,
                                BitTorrent_RuntimeStatistic_t,
                                struct Net_SocketConfiguration,
                                struct BitTorrent_Client_TrackerSocketHandlerConfiguration,
                                BitTorrent_Client_TrackerStream_t,
                                enum Stream_StateMachine_ControlState> BitTorrent_Client_ITrackerStreamConnection_t;

//typedef Net_ISession_T<ACE_INET_Addr,
//                       struct Net_SocketConfiguration,
//                       struct BitTorrent_Client_Configuration,
//                       struct BitTorrent_Client_ConnectionState,
//                       BitTorrent_Client_RuntimeStatistic_t,
//                       BitTorrent_Client_PeerStream_t> BitTorrent_Client_ISession_t;

//////////////////////////////////////////

struct BitTorrent_Client_PeerSocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  inline BitTorrent_Client_PeerSocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   ///////////////////////////////////////
   , connectionConfiguration (NULL)
   , userData (NULL)
  {};

  struct BitTorrent_Client_PeerConnectionConfiguration* connectionConfiguration;

  struct BitTorrent_Client_PeerUserData*                userData;
};
struct BitTorrent_Client_TrackerSocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  inline BitTorrent_Client_TrackerSocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   ///////////////////////////////////////
   , connectionConfiguration (NULL)
   , userData (NULL)
  {};

  struct BitTorrent_Client_TrackerConnectionConfiguration* connectionConfiguration;

  struct BitTorrent_Client_TrackerUserData*                userData;
};

struct BitTorrent_Client_PeerStreamConfiguration;
struct BitTorrent_Client_PeerConnectionConfiguration
 : BitTorrent_PeerConnectionConfiguration
{
  inline BitTorrent_Client_PeerConnectionConfiguration ()
   : BitTorrent_PeerConnectionConfiguration ()
   ///////////////////////////////////////
   , socketHandlerConfiguration ()
   , streamConfiguration (NULL)
   , userData (NULL)
  {};

  struct BitTorrent_Client_PeerSocketHandlerConfiguration socketHandlerConfiguration;
  struct BitTorrent_Client_PeerStreamConfiguration*       streamConfiguration;

  struct BitTorrent_Client_PeerUserData*                   userData;
};
typedef std::map<std::string,
                 struct BitTorrent_Client_PeerConnectionConfiguration> BitTorrent_Client_PeerConnectionConfigurations_t;
typedef BitTorrent_Client_PeerConnectionConfigurations_t::iterator BitTorrent_Client_PeerConnectionConfigurationIterator_t;

struct BitTorrent_Client_TrackerStreamConfiguration;
struct BitTorrent_Client_TrackerConnectionConfiguration
 : BitTorrent_TrackerConnectionConfiguration
{
  inline BitTorrent_Client_TrackerConnectionConfiguration ()
   : BitTorrent_TrackerConnectionConfiguration ()
   ///////////////////////////////////////
   , socketHandlerConfiguration ()
   , streamConfiguration (NULL)
   , userData (NULL)
  {};

  struct BitTorrent_Client_TrackerSocketHandlerConfiguration socketHandlerConfiguration;
  struct BitTorrent_Client_TrackerStreamConfiguration*       streamConfiguration;

  struct BitTorrent_Client_TrackerUserData*                  userData;
};
typedef std::map<std::string,
                 struct BitTorrent_Client_TrackerConnectionConfiguration> BitTorrent_Client_TrackerConnectionConfigurations_t;
typedef BitTorrent_Client_TrackerConnectionConfigurations_t::iterator BitTorrent_Client_TrackerConnectionConfigurationIterator_t;

struct BitTorrent_Client_PeerConnectionConfiguration;
struct BitTorrent_Client_TrackerConnectionConfiguration;
struct BitTorrent_Client_PeerConnectionState;
struct BitTorrent_Client_PeerSocketHandlerConfiguration;
struct BitTorrent_Client_TrackerSocketHandlerConfiguration;
struct BitTorrent_Client_SessionConfiguration;
struct BitTorrent_Client_SessionState;
//typedef BitTorrent_ISession_T<ACE_INET_Addr,
//                              struct BitTorrent_Client_PeerConnectionConfiguration,
//                              struct BitTorrent_Client_TrackerConnectionConfiguration,
//                              struct BitTorrent_Client_PeerConnectionState,
//                              BitTorrent_RuntimeStatistic_t,
//                              struct Net_SocketConfiguration,
//                              struct BitTorrent_Client_PeerSocketHandlerConfiguration,
//                              struct BitTorrent_Client_TrackerSocketHandlerConfiguration,
//                              BitTorrent_Client_PeerStream_t,
//                              enum Stream_StateMachine_ControlState,
//                              struct BitTorrent_Client_SessionConfiguration,
//                              struct BitTorrent_Client_SessionState> BitTorrent_Client_ISession_t;
struct BitTorrent_Client_PeerConnectionState
 : BitTorrent_PeerConnectionState
{
  inline BitTorrent_Client_PeerConnectionState ()
   : BitTorrent_PeerConnectionState ()
   , configuration (NULL)
   , connection (NULL)
   , session (NULL)
   , userData (NULL)
  {};

  struct BitTorrent_Client_Configuration* configuration;
  BitTorrent_Client_IPeerConnection_t*    connection;
  BitTorrent_Client_ISession_t*           session;

  struct BitTorrent_Client_PeerUserData*  userData;
};
struct BitTorrent_Client_TrackerConnectionState
 : BitTorrent_TrackerConnectionState
{
  inline BitTorrent_Client_TrackerConnectionState ()
   : BitTorrent_TrackerConnectionState ()
   , configuration (NULL)
   , connection (NULL)
   , session (NULL)
   , userData (NULL)
  {};

  struct BitTorrent_Client_Configuration*   configuration;
  BitTorrent_Client_ITrackerConnection_t*   connection;
  BitTorrent_Client_ISession_t*             session;

  struct BitTorrent_Client_TrackerUserData* userData;
};

//////////////////////////////////////////

typedef Net_StreamTCPSocketBase_T<Net_TCPSocketHandler_T<struct BitTorrent_Client_PeerSocketHandlerConfiguration,
                                                         ACE_SOCK_STREAM>,
                                  ACE_INET_Addr,
                                  struct BitTorrent_Client_PeerConnectionConfiguration,
                                  struct BitTorrent_Client_PeerConnectionState,
                                  BitTorrent_RuntimeStatistic_t,
                                  BitTorrent_Client_PeerStream_t,
                                  struct BitTorrent_Client_PeerUserData,
                                  struct Stream_ModuleConfiguration,
                                  struct BitTorrent_Client_PeerModuleHandlerConfiguration> BitTorrent_Client_PeerTCPHandler_t;
typedef Net_StreamAsynchTCPSocketBase_T<Net_AsynchTCPSocketHandler_T<struct BitTorrent_Client_PeerSocketHandlerConfiguration>,
                                        ACE_INET_Addr,
                                        struct BitTorrent_Client_PeerConnectionConfiguration,
                                        struct BitTorrent_Client_PeerConnectionState,
                                        BitTorrent_RuntimeStatistic_t,
                                        BitTorrent_Client_PeerStream_t,
                                        struct BitTorrent_Client_PeerUserData,
                                        struct Stream_ModuleConfiguration,
                                        struct BitTorrent_Client_PeerModuleHandlerConfiguration> BitTorrent_Client_AsynchPeerTCPHandler_t;
typedef Net_TCPConnectionBase_T<BitTorrent_Client_PeerTCPHandler_t,
                                struct BitTorrent_Client_PeerConnectionConfiguration,
                                struct BitTorrent_Client_PeerConnectionState,
                                BitTorrent_RuntimeStatistic_t,
                                struct BitTorrent_Client_PeerSocketHandlerConfiguration,
                                struct Net_ListenerConfiguration,
                                BitTorrent_Client_PeerStream_t,
                                struct BitTorrent_Client_PeerUserData> BitTorrent_Client_PeerTCPConnection_t;
typedef Net_AsynchTCPConnectionBase_T<BitTorrent_Client_AsynchPeerTCPHandler_t,
                                      struct BitTorrent_Client_PeerConnectionConfiguration,
                                      struct BitTorrent_Client_PeerConnectionState,
                                      BitTorrent_RuntimeStatistic_t,
                                      struct BitTorrent_Client_PeerSocketHandlerConfiguration,
                                      struct Net_ListenerConfiguration,
                                      BitTorrent_Client_PeerStream_t,
                                      struct BitTorrent_Client_PeerUserData> BitTorrent_Client_AsynchPeerTCPConnection_t;

//----------------------------------------

typedef Net_StreamTCPSocketBase_T<Net_TCPSocketHandler_T<struct BitTorrent_Client_TrackerSocketHandlerConfiguration,
                                                         ACE_SOCK_STREAM>,
                                  ACE_INET_Addr,
                                  struct BitTorrent_Client_TrackerConnectionConfiguration,
                                  struct BitTorrent_Client_TrackerConnectionState,
                                  BitTorrent_RuntimeStatistic_t,
                                  BitTorrent_Client_TrackerStream_t,
                                  struct BitTorrent_Client_TrackerUserData,
                                  struct Stream_ModuleConfiguration,
                                  struct BitTorrent_Client_TrackerModuleHandlerConfiguration> BitTorrent_Client_TrackerTCPHandler_t;
typedef Net_StreamAsynchTCPSocketBase_T<Net_AsynchTCPSocketHandler_T<struct BitTorrent_Client_TrackerSocketHandlerConfiguration>,
                                        ACE_INET_Addr,
                                        struct BitTorrent_Client_TrackerConnectionConfiguration,
                                        struct BitTorrent_Client_TrackerConnectionState,
                                        BitTorrent_RuntimeStatistic_t,
                                        BitTorrent_Client_TrackerStream_t,
                                        struct BitTorrent_Client_TrackerUserData,
                                        struct Stream_ModuleConfiguration,
                                        struct BitTorrent_Client_TrackerModuleHandlerConfiguration> BitTorrent_Client_AsynchTrackerTCPHandler_t;
typedef Net_TCPConnectionBase_T<BitTorrent_Client_TrackerTCPHandler_t,
                                struct BitTorrent_Client_TrackerConnectionConfiguration,
                                struct BitTorrent_Client_TrackerConnectionState,
                                BitTorrent_RuntimeStatistic_t,
                                struct BitTorrent_Client_TrackerSocketHandlerConfiguration,
                                struct Net_ListenerConfiguration,
                                BitTorrent_Client_TrackerStream_t,
                                struct BitTorrent_Client_TrackerUserData> BitTorrent_Client_TrackerTCPConnection_t;
typedef Net_AsynchTCPConnectionBase_T<BitTorrent_Client_AsynchTrackerTCPHandler_t,
                                      struct BitTorrent_Client_TrackerConnectionConfiguration,
                                      struct BitTorrent_Client_TrackerConnectionState,
                                      BitTorrent_RuntimeStatistic_t,
                                      struct BitTorrent_Client_TrackerSocketHandlerConfiguration,
                                      struct Net_ListenerConfiguration,
                                      BitTorrent_Client_TrackerStream_t,
                                      struct BitTorrent_Client_TrackerUserData> BitTorrent_Client_AsynchTrackerTCPConnection_t;

//////////////////////////////////////////

typedef Net_IConnector_T<ACE_INET_Addr,
                         struct BitTorrent_Client_PeerConnectionConfiguration> BitTorrent_Client_IPeerConnector_t;
typedef Net_IConnector_T<ACE_INET_Addr,
                         struct BitTorrent_Client_TrackerConnectionConfiguration> BitTorrent_Client_ITrackerConnector_t;

// peer
typedef Net_Client_Connector_T<BitTorrent_Client_PeerTCPConnection_t,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               struct BitTorrent_Client_PeerConnectionConfiguration,
                               struct BitTorrent_Client_PeerConnectionState,
                               BitTorrent_RuntimeStatistic_t,
                               struct BitTorrent_Client_PeerSocketHandlerConfiguration,
                               BitTorrent_Client_PeerStream_t,
                               struct BitTorrent_Client_PeerUserData> BitTorrent_Client_PeerConnector_t;
typedef Net_Client_AsynchConnector_T<BitTorrent_Client_AsynchPeerTCPConnection_t,
                                     ACE_INET_Addr,
                                     struct BitTorrent_Client_PeerConnectionConfiguration,
                                     struct BitTorrent_Client_PeerConnectionState,
                                     BitTorrent_RuntimeStatistic_t,
                                     struct BitTorrent_Client_PeerSocketHandlerConfiguration,
                                     BitTorrent_Client_PeerStream_t,
                                     struct BitTorrent_Client_PeerUserData> BitTorrent_Client_AsynchPeerConnector_t;

// tracker
typedef Net_Client_Connector_T<BitTorrent_Client_TrackerTCPConnection_t,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               struct BitTorrent_Client_TrackerConnectionConfiguration,
                               struct BitTorrent_Client_TrackerConnectionState,
                               BitTorrent_RuntimeStatistic_t,
                               struct BitTorrent_Client_TrackerSocketHandlerConfiguration,
                               BitTorrent_Client_TrackerStream_t,
                               struct BitTorrent_Client_TrackerUserData> BitTorrent_Client_TrackerConnector_t;
typedef Net_Client_AsynchConnector_T<BitTorrent_Client_AsynchTrackerTCPConnection_t,
                                     ACE_INET_Addr,
                                     struct BitTorrent_Client_TrackerConnectionConfiguration,
                                     struct BitTorrent_Client_TrackerConnectionState,
                                     BitTorrent_RuntimeStatistic_t,
                                     struct BitTorrent_Client_TrackerSocketHandlerConfiguration,
                                     BitTorrent_Client_TrackerStream_t,
                                     struct BitTorrent_Client_TrackerUserData> BitTorrent_Client_AsynchTrackerConnector_t;

//////////////////////////////////////////

typedef Net_IConnectionManager_T<ACE_INET_Addr,
                                 struct BitTorrent_Client_PeerConnectionConfiguration,
                                 struct BitTorrent_Client_PeerConnectionState,
                                 BitTorrent_RuntimeStatistic_t,
                                 struct BitTorrent_Client_PeerUserData> BitTorrent_Client_IPeerConnection_Manager_t;
typedef Net_Connection_Manager_T<ACE_INET_Addr,
                                 struct BitTorrent_Client_PeerConnectionConfiguration,
                                 struct BitTorrent_Client_PeerConnectionState,
                                 BitTorrent_RuntimeStatistic_t,
                                 struct BitTorrent_Client_PeerUserData> BitTorrent_Client_PeerConnection_Manager_t;
typedef Net_IConnectionManager_T<ACE_INET_Addr,
                                 struct BitTorrent_Client_TrackerConnectionConfiguration,
                                 struct BitTorrent_Client_TrackerConnectionState,
                                 BitTorrent_RuntimeStatistic_t,
                                 struct BitTorrent_Client_TrackerUserData> BitTorrent_Client_ITrackerConnection_Manager_t;
typedef Net_Connection_Manager_T<ACE_INET_Addr,
                                 struct BitTorrent_Client_TrackerConnectionConfiguration,
                                 struct BitTorrent_Client_TrackerConnectionState,
                                 BitTorrent_RuntimeStatistic_t,
                                 struct BitTorrent_Client_TrackerUserData> BitTorrent_Client_TrackerConnection_Manager_t;

typedef ACE_Singleton<BitTorrent_Client_PeerConnection_Manager_t,
                      ACE_SYNCH_MUTEX> BITTORRENT_CLIENT_PEERCONNECTION_MANAGER_SINGLETON;
typedef ACE_Singleton<BitTorrent_Client_TrackerConnection_Manager_t,
                      ACE_SYNCH_MUTEX> BITTORRENT_CLIENT_TRACKERCONNECTION_MANAGER_SINGLETON;

#endif
