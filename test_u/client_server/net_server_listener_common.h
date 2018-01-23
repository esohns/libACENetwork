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

#ifndef NET_SERVER_LISTENER_COMMON_H
#define NET_SERVER_LISTENER_COMMON_H

#include "ace/INET_Addr.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "net_common.h"

#include "net_client_asynchconnector.h"
#include "net_server_asynchlistener.h"
#include "net_server_common.h"
#include "net_client_connector.h"
#include "net_server_listener.h"

#include "test_u_connection_common.h"
#include "test_u_stream_common.h"
#include "test_u_tcpconnection.h"
#include "test_u_udpconnection.h"

// foward declarations
class Test_U_Stream;

typedef Net_Server_AsynchListener_T<ClientServer_AsynchTCPConnection,
                                    ACE_INET_Addr,
                                    struct Server_ListenerConfiguration,
                                    struct ClientServer_ConnectionState,
                                    ClientServer_ConnectionConfiguration_t,
                                    Test_U_Stream,
                                    struct Test_U_UserData> Server_AsynchListener_t;
typedef Net_Server_Listener_T<ClientServer_TCPConnection,
                              ACE_SOCK_ACCEPTOR,
                              ACE_INET_Addr,
                              struct Server_ListenerConfiguration,
                              struct ClientServer_ConnectionState,
                              ClientServer_ConnectionConfiguration_t,
                              Test_U_Stream,
                              struct Test_U_UserData> Server_Listener_t;

typedef Net_Client_AsynchConnector_T<ClientServer_AsynchUDPConnection,
                                     ACE_INET_Addr,
                                     ClientServer_ConnectionConfiguration_t,
                                     struct ClientServer_ConnectionState,
                                     Test_U_Statistic_t,
                                     struct Net_UDPSocketConfiguration,
                                     struct ClientServer_SocketHandlerConfiguration,
                                     Test_U_Stream,
                                     struct Test_U_UserData> Server_UDP_AsynchConnector_t;
typedef Net_Client_Connector_T<ACE_MT_SYNCH,
                               ClientServer_UDPConnection,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               ClientServer_ConnectionConfiguration_t,
                               struct ClientServer_ConnectionState,
                               Test_U_Statistic_t,
                               struct Net_UDPSocketConfiguration,
                               struct ClientServer_SocketHandlerConfiguration,
                               Test_U_Stream,
                               struct Test_U_UserData> Server_UDP_Connector_t;

typedef ACE_Singleton<Server_AsynchListener_t,
                      ACE_SYNCH_RECURSIVE_MUTEX> SERVER_ASYNCHLISTENER_SINGLETON;
typedef ACE_Singleton<Server_Listener_t,
                      ACE_SYNCH_RECURSIVE_MUTEX> SERVER_LISTENER_SINGLETON;

#endif
