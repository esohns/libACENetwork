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

#ifndef NET_TCPCONNECTION_BASE_H
#define NET_TCPCONNECTION_BASE_H

#include "ace/Acceptor.h"
#include "ace/Asynch_Acceptor.h"
#include "ace/Asynch_Connector.h"
#include "ace/Connector.h"
#include "ace/Global_Macros.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Connector.h"
#include "ace/Time_Value.h"

#if defined (SSL_SUPPORT)
#include "ace/SSL/SSL_SOCK_Connector.h"
#endif // SSL_SUPPORT

#include "stream_statemachine_control.h"

#include "net_iconnectionmanager.h"
#include "net_sock_acceptor.h"
#include "net_sock_connector.h"
#include "net_streamconnection_base.h"
#include "net_transportlayer_tcp.h"

template <ACE_SYNCH_DECL, // 'send' lock strategy
          typename HandlerType, // implements Net_TCPSocketHandler_T
          ////////////////////////////////
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          ////////////////////////////////
          typename HandlerConfigurationType, // socket-
          typename ListenerConfigurationType,
          ////////////////////////////////
          typename StreamType,
          ////////////////////////////////
          typename TimerManagerType, // implements Common_ITimer
          ////////////////////////////////
          typename UserDataType>
class Net_TCPConnectionBase_T
 : public Net_StreamConnectionBase_T<ACE_SYNCH_USE,
                                     HandlerType,
                                     ACE_INET_Addr,
                                     ConfigurationType,
                                     StateType,
                                     StatisticContainerType,
                                     Net_TCPSocketConfiguration_t,
                                     HandlerConfigurationType,
                                     ListenerConfigurationType,
                                     StreamType,
                                     enum Stream_StateMachine_ControlState,
                                     TimerManagerType,
                                     UserDataType>
 , public Net_TransportLayer_TCP
{
  typedef Net_StreamConnectionBase_T<ACE_SYNCH_USE,
                                     HandlerType,
                                     ACE_INET_Addr,
                                     ConfigurationType,
                                     StateType,
                                     StatisticContainerType,
                                     Net_TCPSocketConfiguration_t,
                                     HandlerConfigurationType,
                                     ListenerConfigurationType,
                                     StreamType,
                                     enum Stream_StateMachine_ControlState,
                                     TimerManagerType,
                                     UserDataType> inherited;
  typedef Net_TransportLayer_TCP inherited2;

  friend class ACE_Acceptor<Net_TCPConnectionBase_T<ACE_SYNCH_USE,
                                                    HandlerType,
                                                    ConfigurationType,
                                                    StateType,
                                                    StatisticContainerType,
                                                    HandlerConfigurationType,
                                                    ListenerConfigurationType,
                                                    StreamType,
                                                    TimerManagerType,
                                                    UserDataType>,
                            ACE_SOCK_ACCEPTOR>;
  friend class ACE_Acceptor<Net_TCPConnectionBase_T<ACE_SYNCH_USE,
                                                    HandlerType,
                                                    ConfigurationType,
                                                    StateType,
                                                    StatisticContainerType,
                                                    HandlerConfigurationType,
                                                    ListenerConfigurationType,
                                                    StreamType,
                                                    TimerManagerType,
                                                    UserDataType>,
                            Net_SOCK_Acceptor>;
  friend class ACE_Connector<Net_TCPConnectionBase_T<ACE_SYNCH_USE,
                                                     HandlerType,
                                                     ConfigurationType,
                                                     StateType,
                                                     StatisticContainerType,
                                                     HandlerConfigurationType,
                                                     ListenerConfigurationType,
                                                     StreamType,
                                                     TimerManagerType,
                                                     UserDataType>,
                             ACE_SOCK_CONNECTOR>;
  friend class ACE_Connector<Net_TCPConnectionBase_T<ACE_SYNCH_USE,
                                                     HandlerType,
                                                     ConfigurationType,
                                                     StateType,
                                                     StatisticContainerType,
                                                     HandlerConfigurationType,
                                                     ListenerConfigurationType,
                                                     StreamType,
                                                     TimerManagerType,
                                                     UserDataType>,
                             Net_SOCK_Connector>;
#if defined (SSL_SUPPORT)
  friend class ACE_Connector<Net_TCPConnectionBase_T<ACE_SYNCH_USE,
                                                     HandlerType,
                                                     ConfigurationType,
                                                     StateType,
                                                     StatisticContainerType,
                                                     HandlerConfigurationType,
                                                     ListenerConfigurationType,
                                                     StreamType,
                                                     TimerManagerType,
                                                     UserDataType>,
                             ACE_SSL_SOCK_Connector>;
#endif // SSL_SUPPORT

 public:
  // convenient types
  typedef Net_StreamConnectionBase_T<ACE_SYNCH_USE,
                                     HandlerType,
                                     ACE_INET_Addr,
                                     ConfigurationType,
                                     StateType,
                                     StatisticContainerType,
                                     Net_TCPSocketConfiguration_t,
                                     HandlerConfigurationType,
                                     ListenerConfigurationType,
                                     StreamType,
                                     enum Stream_StateMachine_ControlState,
                                     TimerManagerType,
                                     UserDataType> STREAM_CONNECTION_BASE_T;

  Net_TCPConnectionBase_T (bool = true); // managed ?
  inline virtual ~Net_TCPConnectionBase_T () {};

  // override some ACE_Event_Handler methods
  // *NOTE*: stream any received data for further processing
  virtual int handle_input (ACE_HANDLE = ACE_INVALID_HANDLE);
  // *NOTE*: send stream data to the peer
  virtual int handle_output (ACE_HANDLE = ACE_INVALID_HANDLE);

 private:
  // *TODO*: if there is no default ctor, MSVC will not compile this code.
  //         For some reason, the compiler will not accept the overloaded
  //         make_svc_handler() method of ACE_Connector/ACE_Acceptor
  Net_TCPConnectionBase_T ();
  ACE_UNIMPLEMENTED_FUNC (Net_TCPConnectionBase_T (const Net_TCPConnectionBase_T&))
  ACE_UNIMPLEMENTED_FUNC (Net_TCPConnectionBase_T& operator= (const Net_TCPConnectionBase_T&))
};

//////////////////////////////////////////

template <typename HandlerType,
          ////////////////////////////////
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          ////////////////////////////////
          typename HandlerConfigurationType, // socket-
          typename ListenerConfigurationType,
          ////////////////////////////////
          typename StreamType,
          ////////////////////////////////
          typename TimerManagerType, // implements Common_ITimer
          ////////////////////////////////
          typename UserDataType>
class Net_AsynchTCPConnectionBase_T
 : public Net_AsynchStreamConnectionBase_T<HandlerType,
                                           ACE_INET_Addr,
                                           ConfigurationType,
                                           StateType,
                                           StatisticContainerType,
                                           Net_TCPSocketConfiguration_t,
                                           HandlerConfigurationType,
                                           ListenerConfigurationType,
                                           StreamType,
                                           enum Stream_StateMachine_ControlState,
                                           TimerManagerType,
                                           UserDataType>
 , public Net_TransportLayer_TCP
{
  typedef Net_AsynchStreamConnectionBase_T<HandlerType,
                                           ACE_INET_Addr,
                                           ConfigurationType,
                                           StateType,
                                           StatisticContainerType,
                                           Net_TCPSocketConfiguration_t,
                                           HandlerConfigurationType,
                                           ListenerConfigurationType,
                                           StreamType,
                                           enum Stream_StateMachine_ControlState,
                                           TimerManagerType,
                                           UserDataType> inherited;
  typedef Net_TransportLayer_TCP inherited2;

  friend class ACE_Asynch_Acceptor<Net_AsynchTCPConnectionBase_T<HandlerType,
                                                                 ConfigurationType,
                                                                 StateType,
                                                                 StatisticContainerType,
                                                                 HandlerConfigurationType,
                                                                 ListenerConfigurationType,
                                                                 StreamType,
                                                                 TimerManagerType,
                                                                 UserDataType> >;
  friend class ACE_Asynch_Connector<Net_AsynchTCPConnectionBase_T<HandlerType,
                                                                  ConfigurationType,
                                                                  StateType,
                                                                  StatisticContainerType,
                                                                  HandlerConfigurationType,
                                                                  ListenerConfigurationType,
                                                                  StreamType,
                                                                  TimerManagerType,
                                                                  UserDataType> >;

 public:
  // convenient types
  typedef Net_AsynchStreamConnectionBase_T<HandlerType,
                                           ACE_INET_Addr,
                                           ConfigurationType,
                                           StateType,
                                           StatisticContainerType,
                                           Net_TCPSocketConfiguration_t,
                                           HandlerConfigurationType,
                                           ListenerConfigurationType,
                                           StreamType,
                                           enum Stream_StateMachine_ControlState,
                                           TimerManagerType,
                                           UserDataType> STREAM_CONNECTION_BASE_T;

  Net_AsynchTCPConnectionBase_T (bool = true); // managed ?
  inline virtual ~Net_AsynchTCPConnectionBase_T () {};

  // override (part of) ACE_Service_Handler
  virtual void open (ACE_HANDLE,          // handle
                     ACE_Message_Block&); // (initial) data (if any)

  // override some ACE_Event_Handler methods
  // *NOTE*: send stream data to the peer
  virtual int handle_output (ACE_HANDLE = ACE_INVALID_HANDLE);

  // implement (part of) Net_IStreamConnection_T
  inline virtual void info (ACE_HANDLE& handle_out, ACE_INET_Addr& localSAP_out, ACE_INET_Addr& peerSAP_out) const { handle_out = inherited::handle (); localSAP_out = inherited::localSAP_; peerSAP_out = inherited::peerSAP_; };

 protected:
  // *IMPORTANT NOTE*: supports synchronicity-agnostic connections
  inline virtual int open (void* = NULL) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (-1); ACE_NOTREACHED (return -1;) }

 private:
  // *TODO*: if there is no default ctor, MSVC will not compile this code.
  //         For some reason, the compiler will not accept the overloaded
  //         make_handler() method of ACE_AsynchConnector/ACE_AsynchAcceptor
  Net_AsynchTCPConnectionBase_T ();
  ACE_UNIMPLEMENTED_FUNC (Net_AsynchTCPConnectionBase_T (const Net_AsynchTCPConnectionBase_T&))
  ACE_UNIMPLEMENTED_FUNC (Net_AsynchTCPConnectionBase_T& operator= (const Net_AsynchTCPConnectionBase_T&))
};

// include template definition
#include "net_tcpconnection_base.inl"

#endif
