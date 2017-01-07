/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns                                      *
 *   erik.sohns@web.de                                                     *
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

#ifndef IRC_NETWORK_H
#define IRC_NETWORK_H

#include <string>

#include <ace/Global_Macros.h>
#include <ace/INET_Addr.h>
#include <ace/Singleton.h>
#include <ace/Synch_Traits.h>
#include <ace/Time_Value.h>

#include "stream_common.h"

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

#include "irc_common.h"
#include "irc_configuration.h"
//#include "irc_stream.h"
#include "irc_stream_common.h"

struct IRC_StreamConfiguration;
struct IRC_ConnectionConfiguration
 : Net_ConnectionConfiguration
{
  inline IRC_ConnectionConfiguration ()
   : Net_ConnectionConfiguration ()
   ///////////////////////////////////////
   , streamConfiguration (NULL)
   //, userData (NULL)
  {};

  struct IRC_StreamConfiguration* streamConfiguration;

  //struct IRC_UserData*            userData;
};

//////////////////////////////////////////

typedef Net_StreamTCPSocketBase_T<Net_TCPSocketHandler_T<struct IRC_SocketHandlerConfiguration,
                                                         ACE_SOCK_STREAM>,
                                  ACE_INET_Addr,
                                  struct IRC_ConnectionConfiguration,
                                  struct IRC_ConnectionState,
                                  IRC_RuntimeStatistic_t,
                                  IRC_Stream_t,
                                  struct Stream_UserData,
                                  struct Stream_ModuleConfiguration,
                                  struct IRC_ModuleHandlerConfiguration> IRC_TCPHandler_t;
typedef Net_StreamAsynchTCPSocketBase_T<Net_AsynchTCPSocketHandler_T<struct IRC_SocketHandlerConfiguration>,
                                        ACE_INET_Addr,
                                        struct IRC_ConnectionConfiguration,
                                        struct IRC_ConnectionState,
                                        IRC_RuntimeStatistic_t,
                                        IRC_Stream_t,
                                        struct Stream_UserData,
                                        struct Stream_ModuleConfiguration,
                                        struct IRC_ModuleHandlerConfiguration> IRC_AsynchTCPHandler_t;
typedef Net_TCPConnectionBase_T<IRC_TCPHandler_t,
                                struct IRC_ConnectionConfiguration,
                                struct IRC_ConnectionState,
                                IRC_RuntimeStatistic_t,
                                struct IRC_SocketHandlerConfiguration,
                                IRC_Stream_t,
                                struct Stream_UserData> IRC_TCPConnection_t;
typedef Net_AsynchTCPConnectionBase_T<IRC_AsynchTCPHandler_t,
                                      struct IRC_ConnectionConfiguration,
                                      struct IRC_ConnectionState,
                                      IRC_RuntimeStatistic_t,
                                      struct IRC_SocketHandlerConfiguration,
                                      IRC_Stream_t,
                                      struct Stream_UserData> IRC_AsynchTCPConnection_t;

//////////////////////////////////////////

typedef Net_IConnection_T<ACE_INET_Addr,
                          struct IRC_ConnectionConfiguration,
                          struct IRC_ConnectionState,
                          IRC_RuntimeStatistic_t> IRC_IConnection_t;
typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                struct IRC_ConnectionConfiguration,
                                struct IRC_ConnectionState,
                                IRC_RuntimeStatistic_t,
                                struct Net_SocketConfiguration,
                                struct IRC_SocketHandlerConfiguration,
                                IRC_Stream_t,
                                enum Stream_StateMachine_ControlState> IRC_IStreamConnection_t;
//typedef Net_ISession_T<ACE_INET_Addr,
//                       Net_SocketConfiguration,
//                       IRC_ConnectionConfiguration,
//                       IRC_ConnectionState,
//                       IRC_RuntimeStatistic_t,
//                       IRC_Stream> IRC_ISession_t;

//////////////////////////////////////////

typedef Net_IConnector_T<ACE_INET_Addr,
                         struct IRC_SocketHandlerConfiguration> IRC_IConnector_t;
//typedef Net_IConnector_T<ACE_INET_Addr,
//                         struct IRC_ConnectorConfiguration> IRC_IConnector_t;
typedef Net_Client_Connector_T<IRC_TCPConnection_t,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               struct IRC_ConnectionConfiguration,
                               struct IRC_ConnectionState,
                               IRC_RuntimeStatistic_t,
                               struct IRC_SocketHandlerConfiguration,
                               IRC_Stream_t,
                               struct Stream_UserData> IRC__Connector_t;
typedef Net_Client_AsynchConnector_T<IRC_AsynchTCPConnection_t,
                                     ACE_INET_Addr,
                                     struct IRC_ConnectionConfiguration,
                                     struct IRC_ConnectionState,
                                     IRC_RuntimeStatistic_t,
                                     struct IRC_SocketHandlerConfiguration,
                                     IRC_Stream_t,
                                     struct Stream_UserData> IRC_AsynchConnector_t;

//////////////////////////////////////////

typedef Net_IConnectionManager_T<ACE_INET_Addr,
                                 struct IRC_ConnectionConfiguration,
                                 struct IRC_ConnectionState,
                                 IRC_RuntimeStatistic_t,
                                 struct Stream_UserData> IRC_IConnection_Manager_t;
typedef Net_Connection_Manager_T<ACE_INET_Addr,
                                 struct IRC_ConnectionConfiguration,
                                 struct IRC_ConnectionState,
                                 IRC_RuntimeStatistic_t,
                                 struct Stream_UserData> IRC_Connection_Manager_t;

typedef ACE_Singleton<IRC_Connection_Manager_t,
                      ACE_SYNCH_MUTEX> IRC_CONNECTIONMANAGER_SINGLETON;

#endif
