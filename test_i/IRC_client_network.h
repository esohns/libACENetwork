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

#include <string>

#include "ace/INET_Addr.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"
#include "ace/Time_Value.h"

#include "stream_common.h"

#include "net_asynch_tcpsockethandler.h"
#include "net_configuration.h"
#include "net_connection_manager.h"
#include "net_iconnectionmanager.h"
#include "net_stream_asynch_tcpsocket_base.h"
#include "net_stream_common.h"
#include "net_stream_tcpsocket_base.h"
#include "net_tcpsockethandler.h"
#include "net_tcpconnection_base.h"

#include "net_client_asynchconnector.h"
#include "net_client_connector.h"

#include "IRC_client_common.h"
#include "IRC_client_configuration.h"
#include "IRC_client_curses.h"
#include "IRC_client_iIRCControl.h"
#include "IRC_client_stream.h"
#include "IRC_client_stream_common.h"

//typedef Stream_Statistic IRC_Client_RuntimeStatistic_t;
struct IRC_Client_ConnectionState
{
  inline IRC_Client_ConnectionState ()
    : away (false)
    , channel ()
    , channelModes ()
    , controller (NULL)
    , cursesState (NULL)
    , isFirstMessage (false)
    , lock ()
    , nickname ()
    , userModes ()
    //////////////////////////////////////
    , configuration ()
    , status (NET_CONNECTION_STATUS_INVALID)
    , currentStatistics ()
    , lastCollectionTimestamp (ACE_Time_Value::zero)
    , userData (NULL)
  {
  };

  // *TODO*: couldn't it be done without this one ?
  bool                      away;
  std::string               channel;
  IRC_Client_ChannelModes_t channelModes;
  IRC_Client_IIRCControl*   controller;
  IRC_Client_CursesState*   cursesState;
  bool                      isFirstMessage;
  ACE_SYNCH_MUTEX           lock;
  std::string               nickname;
  IRC_Client_UserModes_t    userModes;

  ///////////////////////////////////////

  // *TODO*: consider making this a separate entity (i.e. a pointer)
  IRC_Client_Configuration  configuration;

  Net_Connection_Status     status;

  Stream_Statistic          currentStatistics;
  ACE_Time_Value            lastCollectionTimestamp;

  Net_StreamUserData*       userData;
};

/////////////////////////////////////////

typedef Net_StreamTCPSocketBase_T<ACE_INET_Addr,
                                  Net_SocketConfiguration,
                                  IRC_Client_Configuration,
                                  IRC_Client_StreamModuleConfiguration,
                                  Net_StreamUserData,
                                  IRC_Client_ConnectionState,
                                  Stream_Statistic,
                                  IRC_Client_Stream,
                                  Net_TCPSocketHandler_T<IRC_Client_SocketHandlerConfiguration> > IRC_Client_TCPHandler_t;
typedef Net_StreamAsynchTCPSocketBase_T<ACE_INET_Addr,
                                        Net_SocketConfiguration,
                                        IRC_Client_Configuration,
                                        IRC_Client_StreamModuleConfiguration,
                                        Net_StreamUserData,
                                        IRC_Client_ConnectionState,
                                        Stream_Statistic,
                                        IRC_Client_Stream,
                                        Net_AsynchTCPSocketHandler_T<IRC_Client_SocketHandlerConfiguration> > IRC_Client_AsynchTCPHandler_t;
typedef Net_TCPConnectionBase_T<IRC_Client_Configuration,
                                Net_StreamUserData,
                                IRC_Client_ConnectionState,
                                IRC_Client_Stream,
                                IRC_Client_TCPHandler_t,
                                IRC_Client_SocketHandlerConfiguration> IRC_Client_TCPConnection_t;
typedef Net_AsynchTCPConnectionBase_T<IRC_Client_Configuration,
                                      Net_StreamUserData,
                                      IRC_Client_ConnectionState,
                                      IRC_Client_Stream,
                                      IRC_Client_AsynchTCPHandler_t,
                                      IRC_Client_SocketHandlerConfiguration> IRC_Client_AsynchTCPConnection_t;

/////////////////////////////////////////

typedef Net_IConnection_T<ACE_INET_Addr,
                          IRC_Client_Configuration,
                          IRC_Client_ConnectionState,
                          Stream_Statistic,
                          IRC_Client_Stream> IRC_Client_IConnection_t;
//typedef Net_ISession_T<ACE_INET_Addr,
//                       Net_SocketConfiguration,
//                       IRC_Client_Configuration,
//                       IRC_Client_ConnectionState,
//                       Stream_Statistic,
//                       IRC_Client_Stream> IRC_Client_ISession_t;

/////////////////////////////////////////

//typedef Net_Client_IConnector_T<ACE_INET_Addr,
//                                IRC_Client_SocketHandlerConfiguration> IRC_Client_IClientConnector_t;
typedef Net_Client_IConnector_T<ACE_INET_Addr,
                                IRC_Client_ConnectorConfiguration> IRC_Client_IClientConnector_t;
typedef Net_Client_Connector_T<ACE_INET_Addr,
                               Net_SocketConfiguration,
                               IRC_Client_Configuration,
                               IRC_Client_ConnectorConfiguration,
                               Net_StreamUserData,
                               IRC_Client_ConnectionState,
                               IRC_Client_Stream,
                               IRC_Client_TCPConnection_t> IRC_Client_Connector_t;
typedef Net_Client_AsynchConnector_T<ACE_INET_Addr,
                                     Net_SocketConfiguration,
                                     IRC_Client_Configuration,
                                     IRC_Client_ConnectorConfiguration,
                                     Net_StreamUserData,
                                     IRC_Client_ConnectionState,
                                     IRC_Client_Stream,
                                     IRC_Client_AsynchTCPConnection_t> IRC_Client_AsynchConnector_t;

/////////////////////////////////////////

typedef Net_IConnectionManager_T<ACE_INET_Addr,
                                 Net_SocketConfiguration,
                                 IRC_Client_Configuration,
                                 Net_StreamUserData,
                                 IRC_Client_ConnectionState,
                                 Stream_Statistic,
                                 IRC_Client_Stream> IRC_Client_IConnection_Manager_t;
typedef Net_Connection_Manager_T<ACE_INET_Addr,
                                 Net_SocketConfiguration,
                                 IRC_Client_Configuration,
                                 Net_StreamUserData,
                                 IRC_Client_ConnectionState,
                                 Stream_Statistic,
                                 IRC_Client_Stream> IRC_Client_Connection_Manager_t;

typedef ACE_Singleton<IRC_Client_Connection_Manager_t,
                      ACE_SYNCH_MUTEX> IRC_CLIENT_CONNECTIONMANAGER_SINGLETON;

#endif
