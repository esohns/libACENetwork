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

#ifndef TEST_U_CONNECTION_COMMON_H
#define TEST_U_CONNECTION_COMMON_H

#include <map>
#include <string>

#include "ace/INET_Addr.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "ace/Netlink_Addr.h"
#endif
#include "ace/SOCK_Connector.h"
#include "ace/Synch_Traits.h"

#include "common_timer_manager_common.h"

#include "stream_common.h"
#include "stream_session_data.h"

#include "stream_net_io_stream.h"

#include "net_asynch_udpsockethandler.h"
#include "net_common.h"
#include "net_connection_manager.h"
#include "net_sock_connector.h"
#include "net_stream_asynch_udpsocket_base.h"
#include "net_stream_udpsocket_base.h"
#include "net_udpconnection_base.h"
#include "net_udpsockethandler.h"

#include "net_client_asynchconnector.h"
#include "net_client_connector.h"

#include "dhcp_common.h"
#include "dhcp_configuration.h"

#include "test_u_common.h"

// forward declarations
class Test_U_InboundConnectionStream;
class Test_U_OutboundConnectionStream;

struct DHCPClient_ConnectionConfiguration;
struct DHCPClient_SocketHandlerConfiguration
 : DHCP_SocketHandlerConfiguration
{
  DHCPClient_SocketHandlerConfiguration ()
   : DHCP_SocketHandlerConfiguration ()
   ///////////////////////////////////////
   , connectionConfiguration (NULL)
   , userData (NULL)
  {};

  struct DHCPClient_ConnectionConfiguration* connectionConfiguration;

  struct Test_U_UserData*                    userData;
};


struct DHCPClient_ConnectionState
 : DHCP_ConnectionState
{
  DHCPClient_ConnectionState ()
   : DHCP_ConnectionState ()
   , configuration (NULL)
   , userData (NULL)
  {};

  struct DHCPClient_ConnectionConfiguration* configuration;

  struct Test_U_UserData*                    userData;
};

//extern const char stream_name_string_[];
struct DHCPClient_StreamConfiguration;
struct DHCPClient_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct Common_FlexParserAllocatorConfiguration,
                               struct DHCPClient_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct DHCPClient_ModuleHandlerConfiguration> DHCPClient_StreamConfiguration_t;
struct DHCPClient_ConnectionConfiguration;
typedef Net_StreamConnectionConfiguration_T<struct DHCPClient_ConnectionConfiguration,
                                            struct Common_FlexParserAllocatorConfiguration,
                                            DHCPClient_StreamConfiguration_t> DHCPClient_ConnectionConfiguration_t;
typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 DHCPClient_ConnectionConfiguration_t,
                                 struct DHCPClient_ConnectionState,
                                 DHCP_Statistic_t,
                                 struct Test_U_UserData> DHCPClient_IConnectionManager_t;
struct DHCPClient_ConnectionConfiguration
 : DHCP_ConnectionConfiguration
{
  DHCPClient_ConnectionConfiguration ()
   : DHCP_ConnectionConfiguration ()
   ///////////////////////////////////////
   , connectionManager (NULL)
   , socketHandlerConfiguration ()
   , userData (NULL)
  {};

  DHCPClient_IConnectionManager_t*             connectionManager;
  struct DHCPClient_SocketHandlerConfiguration socketHandlerConfiguration;

  struct Test_U_UserData*                      userData;
};
typedef std::map<std::string,
                 DHCPClient_ConnectionConfiguration_t> DHCPClient_ConnectionConfigurations_t;
typedef DHCPClient_ConnectionConfigurations_t::iterator DHCPClient_ConnectionConfigurationIterator_t;

//////////////////////////////////////////

typedef Net_IConnection_T<ACE_INET_Addr,
                          DHCPClient_ConnectionConfiguration_t,
                          struct DHCPClient_ConnectionState,
                          DHCP_Statistic_t> DHCPClient_IConnection_t;
typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                DHCPClient_ConnectionConfiguration_t,
                                struct DHCPClient_ConnectionState,
                                DHCP_Statistic_t,
                                struct Net_SocketConfiguration,
                                struct DHCPClient_SocketHandlerConfiguration,
                                Test_U_OutboundConnectionStream,
                                enum Stream_StateMachine_ControlState> DHCPClient_IStreamConnection_t;
typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                DHCPClient_ConnectionConfiguration_t,
                                struct DHCPClient_ConnectionState,
                                DHCP_Statistic_t,
                                struct Net_SocketConfiguration,
                                struct DHCPClient_SocketHandlerConfiguration,
                                Test_U_InboundConnectionStream,
                                enum Stream_StateMachine_ControlState> DHCPClient_IInboundStreamConnection_t;

//////////////////////////////////////////

// inbound
typedef Net_UDPSocketHandler_T<ACE_MT_SYNCH,
                               Net_SOCK_Dgram_Bcast,
                               struct DHCPClient_SocketHandlerConfiguration> DHCPClient_SocketHandlerBcast_t;
typedef Net_AsynchUDPSocketHandler_T<Net_SOCK_Dgram_Bcast,
                                     struct DHCPClient_SocketHandlerConfiguration> DHCPClient_AsynchSocketHandlerBcast_t;
typedef Net_UDPSocketHandler_T<ACE_MT_SYNCH,
                               Net_SOCK_Dgram,
                               struct DHCPClient_SocketHandlerConfiguration> DHCPClient_SocketHandler_t;
typedef Net_AsynchUDPSocketHandler_T<Net_SOCK_Dgram,
                                     struct DHCPClient_SocketHandlerConfiguration> DHCPClient_AsynchSocketHandler_t;

typedef Net_UDPConnectionBase_T<ACE_MT_SYNCH,
                                DHCPClient_SocketHandlerBcast_t,
                                DHCPClient_ConnectionConfiguration_t,
                                struct DHCPClient_ConnectionState,
                                DHCP_Statistic_t,
                                struct DHCPClient_SocketHandlerConfiguration,
                                Test_U_InboundConnectionStream,
                                Common_Timer_Manager_t,
                                struct Test_U_UserData> DHCPClient_ConnectionBcast_t;
typedef Net_AsynchUDPConnectionBase_T<DHCPClient_AsynchSocketHandlerBcast_t,
                                      DHCPClient_ConnectionConfiguration_t,
                                      struct DHCPClient_ConnectionState,
                                      DHCP_Statistic_t,
                                      struct DHCPClient_SocketHandlerConfiguration,
                                      Test_U_InboundConnectionStream,
                                      Common_Timer_Manager_t,
                                      struct Test_U_UserData> DHCPClient_AsynchConnectionBcast_t;
typedef Net_UDPConnectionBase_T<ACE_MT_SYNCH,
                                DHCPClient_SocketHandler_t,
                                DHCPClient_ConnectionConfiguration_t,
                                struct DHCPClient_ConnectionState,
                                DHCP_Statistic_t,
                                struct DHCPClient_SocketHandlerConfiguration,
                                Test_U_InboundConnectionStream,
                                Common_Timer_Manager_t,
                                struct Test_U_UserData> DHCPClient_Connection_t;
typedef Net_AsynchUDPConnectionBase_T<DHCPClient_AsynchSocketHandler_t,
                                      DHCPClient_ConnectionConfiguration_t,
                                      struct DHCPClient_ConnectionState,
                                      DHCP_Statistic_t,
                                      struct DHCPClient_SocketHandlerConfiguration,
                                      Test_U_InboundConnectionStream,
                                      Common_Timer_Manager_t,
                                      struct Test_U_UserData> DHCPClient_AsynchConnection_t;

// outbound
typedef Net_UDPSocketHandler_T<ACE_MT_SYNCH,
                               Net_SOCK_Dgram_Bcast,
                               struct DHCPClient_SocketHandlerConfiguration> DHCPClient_OutboundSocketHandlerBcast_t;
typedef Net_AsynchUDPSocketHandler_T<Net_SOCK_Dgram_Bcast,
                                     struct DHCPClient_SocketHandlerConfiguration> DHCPClient_OutboundAsynchSocketHandlerBcast_t;
typedef Net_UDPSocketHandler_T<ACE_MT_SYNCH,
                               Net_SOCK_CODgram,
                               struct DHCPClient_SocketHandlerConfiguration> DHCPClient_OutboundSocketHandler_t;
typedef Net_AsynchUDPSocketHandler_T<Net_SOCK_CODgram,
                                     struct DHCPClient_SocketHandlerConfiguration> DHCPClient_OutboundAsynchSocketHandler_t;

typedef Net_UDPConnectionBase_T<ACE_MT_SYNCH,
                                DHCPClient_OutboundSocketHandlerBcast_t,
                                DHCPClient_ConnectionConfiguration_t,
                                struct DHCPClient_ConnectionState,
                                DHCP_Statistic_t,
                                struct DHCPClient_SocketHandlerConfiguration,
                                Test_U_OutboundConnectionStream,
                                Common_Timer_Manager_t,
                                struct Test_U_UserData> DHCPClient_OutboundConnectionBcast_t;
typedef Net_AsynchUDPConnectionBase_T<DHCPClient_OutboundAsynchSocketHandlerBcast_t,
                                      DHCPClient_ConnectionConfiguration_t,
                                      struct DHCPClient_ConnectionState,
                                      DHCP_Statistic_t,
                                      struct DHCPClient_SocketHandlerConfiguration,
                                      Test_U_OutboundConnectionStream,
                                      Common_Timer_Manager_t,
                                      struct Test_U_UserData> DHCPClient_OutboundAsynchConnectionBcast_t;
typedef Net_UDPConnectionBase_T<ACE_MT_SYNCH,
                                DHCPClient_OutboundSocketHandler_t,
                                DHCPClient_ConnectionConfiguration_t,
                                struct DHCPClient_ConnectionState,
                                DHCP_Statistic_t,
                                struct DHCPClient_SocketHandlerConfiguration,
                                Test_U_OutboundConnectionStream,
                                Common_Timer_Manager_t,
                                struct Test_U_UserData> DHCPClient_OutboundConnection_t;
typedef Net_AsynchUDPConnectionBase_T<DHCPClient_OutboundAsynchSocketHandler_t,
                                      DHCPClient_ConnectionConfiguration_t,
                                      struct DHCPClient_ConnectionState,
                                      DHCP_Statistic_t,
                                      struct DHCPClient_SocketHandlerConfiguration,
                                      Test_U_OutboundConnectionStream,
                                      Common_Timer_Manager_t,
                                      struct Test_U_UserData> DHCPClient_OutboundAsynchConnection_t;

//////////////////////////////////////////

typedef Net_IConnector_T<ACE_INET_Addr,
                         DHCPClient_ConnectionConfiguration_t> DHCPClient_IConnector_t;

//////////////////////////////////////////

// inbound
typedef Net_Client_AsynchConnector_T<DHCPClient_AsynchConnectionBcast_t,
                                     ACE_INET_Addr,
                                     DHCPClient_ConnectionConfiguration_t,
                                     struct DHCPClient_ConnectionState,
                                     DHCP_Statistic_t,
                                     struct Net_UDPSocketConfiguration,
                                     struct DHCPClient_SocketHandlerConfiguration,
                                     Test_U_InboundConnectionStream,
                                     struct Test_U_UserData> DHCPClient_AsynchConnectorBcast_t;
typedef Net_Client_Connector_T<ACE_MT_SYNCH,
                               DHCPClient_ConnectionBcast_t,
                               Net_SOCK_Connector,
                               ACE_INET_Addr,
                               DHCPClient_ConnectionConfiguration_t,
                               struct DHCPClient_ConnectionState,
                               DHCP_Statistic_t,
                               struct Net_UDPSocketConfiguration,
                               struct DHCPClient_SocketHandlerConfiguration,
                               Test_U_InboundConnectionStream,
                               struct Test_U_UserData> DHCPClient_ConnectorBcast_t;
typedef Net_Client_AsynchConnector_T<DHCPClient_AsynchConnection_t,
                                     ACE_INET_Addr,
                                     DHCPClient_ConnectionConfiguration_t,
                                     struct DHCPClient_ConnectionState,
                                     DHCP_Statistic_t,
                                     struct Net_UDPSocketConfiguration,
                                     struct DHCPClient_SocketHandlerConfiguration,
                                     Test_U_InboundConnectionStream,
                                     struct Test_U_UserData> DHCPClient_AsynchConnector_t;
typedef Net_Client_Connector_T<ACE_MT_SYNCH,
                               DHCPClient_Connection_t,
                               Net_SOCK_Connector,
                               ACE_INET_Addr,
                               DHCPClient_ConnectionConfiguration_t,
                               struct DHCPClient_ConnectionState,
                               DHCP_Statistic_t,
                               struct Net_UDPSocketConfiguration,
                               struct DHCPClient_SocketHandlerConfiguration,
                               Test_U_InboundConnectionStream,
                               struct Test_U_UserData> DHCPClient_Connector_t;

// outbound
typedef Net_Client_AsynchConnector_T<DHCPClient_OutboundAsynchConnectionBcast_t,
                                     ACE_INET_Addr,
                                     DHCPClient_ConnectionConfiguration_t,
                                     struct DHCPClient_ConnectionState,
                                     DHCP_Statistic_t,
                                     struct Net_UDPSocketConfiguration,
                                     struct DHCPClient_SocketHandlerConfiguration,
                                     Test_U_OutboundConnectionStream,
                                     struct Test_U_UserData> DHCPClient_OutboundAsynchConnectorBcast_t;
typedef Net_Client_Connector_T<ACE_MT_SYNCH,
                               DHCPClient_OutboundConnectionBcast_t,
                               Net_SOCK_Connector,
                               ACE_INET_Addr,
                               DHCPClient_ConnectionConfiguration_t,
                               struct DHCPClient_ConnectionState,
                               DHCP_Statistic_t,
                               struct Net_UDPSocketConfiguration,
                               struct DHCPClient_SocketHandlerConfiguration,
                               Test_U_OutboundConnectionStream,
                               struct Test_U_UserData> DHCPClient_OutboundConnectorBcast_t;
typedef Net_Client_AsynchConnector_T<DHCPClient_OutboundAsynchConnection_t,
                                     ACE_INET_Addr,
                                     DHCPClient_ConnectionConfiguration_t,
                                     struct DHCPClient_ConnectionState,
                                     DHCP_Statistic_t,
                                     struct Net_UDPSocketConfiguration,
                                     struct DHCPClient_SocketHandlerConfiguration,
                                     Test_U_OutboundConnectionStream,
                                     struct Test_U_UserData> DHCPClient_OutboundAsynchConnector_t;
typedef Net_Client_Connector_T<ACE_MT_SYNCH,
                               DHCPClient_OutboundConnection_t,
                               Net_SOCK_Connector,
                               ACE_INET_Addr,
                               DHCPClient_ConnectionConfiguration_t,
                               struct DHCPClient_ConnectionState,
                               DHCP_Statistic_t,
                               struct Net_UDPSocketConfiguration,
                               struct DHCPClient_SocketHandlerConfiguration,
                               Test_U_OutboundConnectionStream,
                               struct Test_U_UserData> DHCPClient_OutboundConnector_t;

#endif
