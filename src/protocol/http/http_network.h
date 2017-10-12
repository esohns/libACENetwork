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

#ifndef HTTP_NETWORK_H
#define HTTP_NETWORK_H

#include <string>

#include "ace/INET_Addr.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"
#include "ace/Time_Value.h"
#include "ace/SSL/SSL_SOCK_Connector.h"

#include "common_timer_manager_common.h"

#include "stream_common.h"

#include "net_asynch_tcpsockethandler.h"
#include "net_configuration.h"
#include "net_connection_manager.h"
#include "net_iconnectionmanager.h"
#include "net_iconnector.h"
#include "net_stream_asynch_tcpsocket_base.h"
#include "net_stream_tcpsocket_base.h"
#include "net_tcpsockethandler.h"
#include "net_tcpconnection_base.h"

#include "net_client_asynchconnector.h"
#include "net_client_connector.h"
#include "net_client_ssl_connector.h"

#include "http_common.h"
#include "http_stream.h"
#include "http_stream_common.h"

typedef HTTP_Stream_T<struct HTTP_StreamState,
                      struct HTTP_StreamConfiguration,
                      HTTP_Statistic_t,
                      Common_Timer_Manager_t,
                      struct HTTP_ModuleHandlerConfiguration,
                      struct HTTP_Stream_SessionData,
                      HTTP_Stream_SessionData_t,
                      ACE_Message_Block,
                      HTTP_Message_t,
                      HTTP_SessionMessage,
                      struct HTTP_Stream_UserData> HTTP_Stream_t;
//typedef HTTP_Stream_T<struct HTTP_StreamState,
//                      struct HTTP_StreamConfiguration,
//                      HTTP_Statistic_t,
//                      HTTP_StatisticHandler_Proactor_t,
//                      struct HTTP_ModuleHandlerConfiguration,
//                      struct HTTP_Stream_SessionData,
//                      HTTP_Stream_SessionData_t,
//                      ACE_Message_Block,
//                      HTTP_Message_t,
//                      HTTP_SessionMessage,
//                      struct HTTP_Stream_UserData> HTTP_AsynchStream_t;

//struct HTTP_ConnectionConfiguration;
struct HTTP_ConnectionState
 : Net_ConnectionState
{
  inline HTTP_ConnectionState ()
   : Net_ConnectionState ()
   , configuration (NULL)
   , userData (NULL)
  {};

  //struct HTTP_ConnectionConfiguration* configuration;
  struct Net_ConnectionConfiguration* configuration;

  struct HTTP_Stream_UserData*        userData;
};

struct HTTP_SocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  inline HTTP_SocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   //////////////////////////////////////
   , socketConfiguration_2 ()
   , userData (NULL)
  {
    socketConfiguration = &socketConfiguration_2;
  };

  struct Net_TCPSocketConfiguration socketConfiguration_2;

  struct HTTP_Stream_UserData*      userData;
};

/////////////////////////////////////////

typedef Net_TCPSocketHandler_T<ACE_MT_SYNCH,
                               ACE_SOCK_STREAM,
                               struct HTTP_SocketHandlerConfiguration> HTTP_TCPSocketHandler_t;
typedef Net_AsynchTCPSocketHandler_T<struct HTTP_SocketHandlerConfiguration> HTTP_AsynchTCPSocketHandler_t;

typedef Net_TCPConnectionBase_T<ACE_MT_SYNCH,
                                HTTP_TCPSocketHandler_t,
                                struct Net_ConnectionConfiguration,
                                struct HTTP_ConnectionState,
                                HTTP_Statistic_t,
                                struct HTTP_SocketHandlerConfiguration,
                                struct Net_ListenerConfiguration,
                                HTTP_Stream_t,
                                Common_Timer_Manager_t,
                                struct HTTP_Stream_UserData> HTTP_TCPConnection_t;
typedef Net_AsynchTCPConnectionBase_T<HTTP_AsynchTCPSocketHandler_t,
                                      struct Net_ConnectionConfiguration,
                                      struct HTTP_ConnectionState,
                                      HTTP_Statistic_t,
                                      struct HTTP_SocketHandlerConfiguration,
                                      struct Net_ListenerConfiguration,
                                      HTTP_Stream_t,
                                      Common_Timer_Manager_t,
                                      struct HTTP_Stream_UserData> HTTP_AsynchTCPConnection_t;

/////////////////////////////////////////

typedef Net_IConnection_T<ACE_INET_Addr,
                          struct Net_ConnectionConfiguration,
                          struct HTTP_ConnectionState,
                          HTTP_Statistic_t> HTTP_IConnection_t;
typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                struct Net_ConnectionConfiguration,
                                struct HTTP_ConnectionState,
                                HTTP_Statistic_t,
                                struct Net_SocketConfiguration,
                                struct HTTP_SocketHandlerConfiguration,
                                HTTP_Stream_t,
                                enum Stream_StateMachine_ControlState> HTTP_IStreamConnection_t;

/////////////////////////////////////////

typedef Net_IConnector_T<ACE_INET_Addr,
                         struct HTTP_ConnectionConfiguration> HTTP_IConnector_t;

typedef Net_Client_Connector_T<ACE_MT_SYNCH,
                               HTTP_TCPConnection_t,
                               Net_SOCK_Connector,
                               ACE_INET_Addr,
                               struct Net_ConnectionConfiguration,
                               struct HTTP_ConnectionState,
                               HTTP_Statistic_t,
                               HTTP_Stream_t,
                               struct Net_TCPSocketConfiguration,
                               struct HTTP_SocketHandlerConfiguration,
                               struct HTTP_Stream_UserData> HTTP_Connector_t;
typedef Net_Client_SSL_Connector_T<HTTP_TCPConnection_t,
                                   ACE_SSL_SOCK_Connector,
                                   ACE_INET_Addr,
                                   struct Net_ConnectionConfiguration,
                                   struct HTTP_ConnectionState,
                                   HTTP_Statistic_t,
                                   HTTP_Stream_t,
                                   struct HTTP_SocketHandlerConfiguration,
                                   struct HTTP_Stream_UserData> HTTP_SSL_Connector_t;
typedef Net_Client_AsynchConnector_T<HTTP_AsynchTCPConnection_t,
                                     ACE_INET_Addr,
                                     struct Net_ConnectionConfiguration,
                                     struct HTTP_ConnectionState,
                                     HTTP_Statistic_t,
                                     HTTP_Stream_t,
                                     struct Net_TCPSocketConfiguration,
                                     struct HTTP_SocketHandlerConfiguration,
                                     struct HTTP_Stream_UserData> HTTP_AsynchConnector_t;

/////////////////////////////////////////

typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 struct Net_ConnectionConfiguration,
                                 struct HTTP_ConnectionState,
                                 HTTP_Statistic_t,
                                 struct HTTP_Stream_UserData> HTTP_IConnection_Manager_t;
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 struct Net_ConnectionConfiguration,
                                 struct HTTP_ConnectionState,
                                 HTTP_Statistic_t,
                                 struct HTTP_Stream_UserData> HTTP_Connection_Manager_t;

typedef ACE_Singleton<HTTP_Connection_Manager_t,
                      ACE_SYNCH_MUTEX> HTTP_CONNECTIONMANAGER_SINGLETON;

#endif
