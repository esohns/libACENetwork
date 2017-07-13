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

#ifndef IRC_CLIENT_NETWORK_H
#define IRC_CLIENT_NETWORK_H

#include <map>
#include <string>

#include "ace/Global_Macros.h"
#include "ace/INET_Addr.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "irc_network.h"

#include "IRC_client_common.h"
#include "IRC_client_configuration.h"
#include "IRC_client_curses.h"
#include "IRC_client_stream.h"
#include "IRC_client_stream_common.h"

//////////////////////////////////////////

struct IRC_Client_ConnectionConfiguration;
struct IRC_Client_SocketHandlerConfiguration
 : IRC_SocketHandlerConfiguration
{
  inline IRC_Client_SocketHandlerConfiguration ()
   : IRC_SocketHandlerConfiguration ()
   ///////////////////////////////////////
   , connectionConfiguration (NULL)
   , userData (NULL)
  {};

  struct IRC_Client_ConnectionConfiguration* connectionConfiguration;

  struct IRC_Client_UserData*                userData;
};

struct IRC_StreamConfiguration;
struct IRC_Client_ConnectionConfiguration
 : IRC_ConnectionConfiguration
{
  inline IRC_Client_ConnectionConfiguration ()
   : IRC_ConnectionConfiguration ()
   ///////////////////////////////////////
   , cursesState (NULL)
   , logToFile (IRC_CLIENT_SESSION_DEFAULT_LOG)
   , protocolConfiguration (NULL)
   , socketHandlerConfiguration ()
   , streamConfiguration (NULL)
   , useReactor (NET_EVENT_USE_REACTOR)
   , userData (NULL)
  {};

  struct IRC_Client_CursesState*               cursesState;
  bool                                         logToFile;
  struct IRC_ProtocolConfiguration*            protocolConfiguration;
  struct IRC_Client_SocketHandlerConfiguration socketHandlerConfiguration;
  IRC_Client_StreamConfiguration_t*            streamConfiguration;
  bool                                         useReactor;

  struct IRC_Client_UserData*                  userData;
};
typedef std::map<std::string,
                 struct IRC_Client_ConnectionConfiguration> IRC_Client_ConnectionConfigurations_t;
typedef IRC_Client_ConnectionConfigurations_t::iterator IRC_Client_ConnectionConfigurationIterator_t;

struct IRC_Client_ConnectionState
 : IRC_ConnectionState
{
  inline IRC_Client_ConnectionState ()
   : IRC_ConnectionState ()
   , configuration (NULL)
   , controller (NULL)
   , userData (NULL)
  {};

  struct IRC_Client_ConnectionConfiguration* configuration;
  IRC_IControl*                              controller;

  struct IRC_Client_UserData*                userData;
};

// *TODO*: remove this ASAP
struct IRC_Client_SessionState
 : IRC_Client_ConnectionState
{
  inline IRC_Client_SessionState ()
   : IRC_Client_ConnectionState ()
   , away (false)
   , channel ()
   , channelModes ()
   , isFirstMessage (false)
   , nickName ()
   , userModes ()
  {};

  // *TODO*: remove this
  bool               away;
  std::string        channel;
  IRC_ChannelModes_t channelModes;
  bool               isFirstMessage;
  std::string        nickName;
  IRC_UserModes_t    userModes;
};

//////////////////////////////////////////

typedef Net_StreamTCPSocketBase_T<Net_TCPSocketHandler_T<struct IRC_Client_SocketHandlerConfiguration,
                                                         ACE_SOCK_STREAM>,
                                  ACE_INET_Addr,
                                  struct IRC_Client_ConnectionConfiguration,
                                  struct IRC_Client_SessionState,
                                  IRC_RuntimeStatistic_t,
                                  IRC_Client_Stream,
                                  struct IRC_Client_UserData,
                                  struct Stream_ModuleConfiguration,
                                  struct IRC_Client_ModuleHandlerConfiguration> IRC_Client_TCPHandler_t;
typedef Net_StreamAsynchTCPSocketBase_T<Net_AsynchTCPSocketHandler_T<struct IRC_Client_SocketHandlerConfiguration>,
                                        ACE_INET_Addr,
                                        struct IRC_Client_ConnectionConfiguration,
                                        struct IRC_Client_SessionState,
                                        IRC_RuntimeStatistic_t,
                                        IRC_Client_Stream,
                                        struct IRC_Client_UserData,
                                        struct Stream_ModuleConfiguration,
                                        struct IRC_Client_ModuleHandlerConfiguration> IRC_Client_AsynchTCPHandler_t;
typedef Net_TCPConnectionBase_T<IRC_Client_TCPHandler_t,
                                struct IRC_Client_ConnectionConfiguration,
                                struct IRC_Client_SessionState,
                                IRC_RuntimeStatistic_t,
                                struct IRC_Client_SocketHandlerConfiguration,
                                struct IRC_Client_ListenerConfiguration,
                                IRC_Client_Stream,
                                struct IRC_Client_UserData> IRC_Client_TCPConnection_t;
typedef Net_AsynchTCPConnectionBase_T<IRC_Client_AsynchTCPHandler_t,
                                      struct IRC_Client_ConnectionConfiguration,
                                      struct IRC_Client_SessionState,
                                      IRC_RuntimeStatistic_t,
                                      struct IRC_Client_SocketHandlerConfiguration,
                                      struct IRC_Client_ListenerConfiguration,
                                      IRC_Client_Stream,
                                      struct IRC_Client_UserData> IRC_Client_AsynchTCPConnection_t;

//////////////////////////////////////////

typedef Net_IConnection_T<ACE_INET_Addr,
                          struct IRC_Client_ConnectionConfiguration,
                          struct IRC_Client_SessionState,
                          IRC_RuntimeStatistic_t> IRC_Client_IConnection_t;
typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                struct IRC_Client_ConnectionConfiguration,
                                struct IRC_Client_SessionState,
                                IRC_RuntimeStatistic_t,
                                struct Net_SocketConfiguration,
                                struct IRC_Client_SocketHandlerConfiguration,
                                IRC_Client_Stream,
                                enum Stream_StateMachine_ControlState> IRC_Client_IStreamConnection_t;
//typedef Net_ISession_T<ACE_INET_Addr,
//                       struct Net_SocketConfiguration,
//                       struct IRC_Client_ConnectionConfiguration,
//                       struct IRC_Client_ConnectionState,
//                       IRC_Client_RuntimeStatistic_t,
//                       IRC_Client_Stream> IRC_Client_ISession_t;

//////////////////////////////////////////

typedef Net_IConnector_T<ACE_INET_Addr,
                         struct IRC_Client_ConnectionConfiguration> IRC_Client_IConnector_t;

typedef Net_Client_Connector_T<IRC_Client_TCPConnection_t,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               struct IRC_Client_ConnectionConfiguration,
                               struct IRC_Client_SessionState,
                               IRC_RuntimeStatistic_t,
                               struct Net_TCPSocketConfiguration,
                               struct IRC_Client_SocketHandlerConfiguration,
                               IRC_Client_Stream,
                               struct IRC_Client_UserData> IRC_Client_Connector_t;
typedef Net_Client_AsynchConnector_T<IRC_Client_AsynchTCPConnection_t,
                                     ACE_INET_Addr,
                                     struct IRC_Client_ConnectionConfiguration,
                                     struct IRC_Client_SessionState,
                                     IRC_RuntimeStatistic_t,
                                     struct Net_TCPSocketConfiguration,
                                     struct IRC_Client_SocketHandlerConfiguration,
                                     IRC_Client_Stream,
                                     struct IRC_Client_UserData> IRC_Client_AsynchConnector_t;

//////////////////////////////////////////

typedef Net_IConnectionManager_T<ACE_INET_Addr,
                                 struct IRC_Client_ConnectionConfiguration,
                                 struct IRC_Client_SessionState,
                                 IRC_RuntimeStatistic_t,
                                 struct IRC_Client_UserData> IRC_Client_IConnection_Manager_t;
typedef Net_Connection_Manager_T<ACE_INET_Addr,
                                 struct IRC_Client_ConnectionConfiguration,
                                 struct IRC_Client_SessionState,
                                 IRC_RuntimeStatistic_t,
                                 struct IRC_Client_UserData> IRC_Client_Connection_Manager_t;

typedef ACE_Singleton<IRC_Client_Connection_Manager_t,
                      ACE_SYNCH_MUTEX> IRC_CLIENT_CONNECTIONMANAGER_SINGLETON;

#endif
