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

#ifndef NET_CLIENT_CONNECTOR_COMMON_H
#define NET_CLIENT_CONNECTOR_COMMON_H

#include "ace/INET_Addr.h"

#include "net_configuration.h"
#include "net_stream_common.h"
#include "net_tcpconnection.h"

#include "net_client_asynchconnector.h"
#include "net_client_connector.h"
#include "net_client_iconnector.h"

typedef Net_Client_IConnector_T<ACE_INET_Addr,
                                Net_SocketHandlerConfiguration_t> Net_Client_IConnector_t;

typedef Net_Client_AsynchConnector_T<ACE_INET_Addr,
                                     Net_SocketConfiguration_t,
                                     Net_Configuration_t,
                                     Net_SocketHandlerConfiguration_t,
                                     Net_UserData_t,
                                     Net_StreamSessionData_t,
                                     Net_AsynchTCPConnection> Net_Client_AsynchConnector_t;
typedef Net_Client_Connector_T<ACE_INET_Addr,
                               Net_SocketConfiguration_t,
                               Net_Configuration_t,
                               Net_SocketHandlerConfiguration_t,
                               Net_UserData_t,
                               Net_StreamSessionData_t,
                               Net_TCPConnection> Net_Client_Connector_t;

#endif
