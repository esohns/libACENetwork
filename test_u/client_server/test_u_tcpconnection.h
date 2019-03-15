/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#ifndef Test_U_TCPConnection_H
#define Test_U_TCPConnection_H

#include "ace/Acceptor.h"
#include "ace/Asynch_Acceptor.h"
#include "ace/Asynch_Connector.h"
#include "ace/Connector.h"
#include "ace/Global_Macros.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Connector.h"
#include "ace/Time_Value.h"

#include "common_timer_manager_common.h"

#include "net_tcpconnection_base.h"

#include "net_client_common.h"

#include "test_u_configuration.h"
#include "test_u_connection_common.h"
#include "test_u_socket_common.h"

// forward declarations
class Test_U_Stream;

class Test_U_TCPConnection
 : public Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                  Test_U_TCPSocketHandler_t,
                                  Test_U_TCPConnectionConfiguration,
                                  struct Test_U_ConnectionState,
                                  Net_Statistic_t,
                                  struct Test_U_SocketHandlerConfiguration,
                                  struct Server_ListenerConfiguration,
                                  Test_U_Stream,
                                  Common_Timer_Manager_t,
                                  struct Net_UserData>
 , public Net_IPing
{
  typedef Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                  Test_U_TCPSocketHandler_t,
                                  Test_U_TCPConnectionConfiguration,
                                  struct Test_U_ConnectionState,
                                  Net_Statistic_t,
                                  struct Test_U_SocketHandlerConfiguration,
                                  struct Server_ListenerConfiguration,
                                  Test_U_Stream,
                                  Common_Timer_Manager_t,
                                  struct Net_UserData> inherited;

  friend class ACE_Acceptor<Test_U_TCPConnection, ACE_SOCK_ACCEPTOR>;
  friend class ACE_Connector<Test_U_TCPConnection, ACE_SOCK_CONNECTOR>;

 public:
  Test_U_TCPConnection (bool = true); // managed ?
  inline virtual ~Test_U_TCPConnection () {}

  // implement Net_IPing
  inline virtual void ping () { stream_.ping (); }

 private:
  // *TODO*: if there is no default ctor, MSVC will not compile this code.
  //         For some reason, the compiler will not accept the overloaded
  //         make_svc_handler() method of ACE_Connector/ACE_Acceptor
  Test_U_TCPConnection ();
  ACE_UNIMPLEMENTED_FUNC (Test_U_TCPConnection (const Test_U_TCPConnection&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_TCPConnection& operator= (const Test_U_TCPConnection&))
};

//////////////////////////////////////////

class Test_U_AsynchTCPConnection
 : public Net_AsynchTCPConnectionBase_T<Test_U_AsynchTCPSocketHandler_t,
                                        Test_U_TCPConnectionConfiguration,
                                        struct Test_U_ConnectionState,
                                        Net_Statistic_t,
                                        struct Test_U_SocketHandlerConfiguration,
                                        struct Server_ListenerConfiguration,
                                        Test_U_Stream,
                                        Common_Timer_Manager_t,
                                        struct Net_UserData>
 , public Net_IPing
{
 friend class ACE_Asynch_Acceptor<Test_U_AsynchTCPConnection>;
 friend class ACE_Asynch_Connector<Test_U_AsynchTCPConnection>;

 public:
  Test_U_AsynchTCPConnection (bool = true); // managed ?
  inline virtual ~Test_U_AsynchTCPConnection () {}

  // implement Net_IPing
  inline virtual void ping () { stream_.ping (); }

 private:
  typedef Net_AsynchTCPConnectionBase_T<Test_U_AsynchTCPSocketHandler_t,
                                        Test_U_TCPConnectionConfiguration,
                                        struct Test_U_ConnectionState,
                                        Net_Statistic_t,
                                        struct Test_U_SocketHandlerConfiguration,
                                        struct Server_ListenerConfiguration,
                                        Test_U_Stream,
                                        Common_Timer_Manager_t,
                                        struct Net_UserData> inherited;

  // *TODO*: if there is no default ctor, MSVC will not compile this code.
  //         For some reason, the compiler will not accept the overloaded
  //         make_handler() method of ACE_AsynchConnector/ACE_AsynchAcceptor
  Test_U_AsynchTCPConnection ();
  ACE_UNIMPLEMENTED_FUNC (Test_U_AsynchTCPConnection (const Test_U_AsynchTCPConnection&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_AsynchTCPConnection& operator= (const Test_U_AsynchTCPConnection&))
};

#endif
