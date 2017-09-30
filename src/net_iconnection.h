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

#ifndef NET_ICONNECTION_T_H
#define NET_ICONNECTION_T_H

#include "ace/config-macros.h"
#include "ace/INET_Addr.h"
#include "ace/Message_Block.h"

#include "common_idumpstate.h"
#include "common_iget.h"
#include "common_iinitialize.h"
#include "common_ireferencecount.h"
#include "common_istatistic.h"

#include "stream_imessagequeue.h"

#include "net_common.h"
#include "net_itransportlayer.h"

// forward declarations
class ACE_Notification_Strategy;
enum Net_Connection_Status;

//////////////////////////////////////////

class Net_IAsynchSocketHandler
{
 public:
  virtual bool initiate_read () = 0;

  // *NOTE*: this cancels all outstanding asynchronous operations
  virtual int handle_close (ACE_HANDLE = ACE_INVALID_HANDLE,                            // handle
                            ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK) = 0; // event mask

  ////////////////////////////////////////
  virtual int handle_output (ACE_HANDLE = ACE_INVALID_HANDLE) = 0; // handle
};

//////////////////////////////////////////

template <typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType>
class Net_IConnection_T
 : public virtual Common_IReferenceCount
 , public virtual Common_IStatistic_T<StatisticContainerType>
 , public Common_IInitialize_T<ConfigurationType>
 , public Common_IGetR_T<ConfigurationType>
 , public virtual Common_IDumpState
{
 public:
  // convenient types
  typedef StateType STATE_T;

  virtual void info (ACE_HANDLE&,             // return value: I/O handle
                     AddressType&,            // return value: local SAP
                     AddressType&) const = 0; // return value: remote SAP
  virtual Net_ConnectionId_t id () const = 0;

  virtual ACE_Notification_Strategy* notification () = 0;
  virtual const StateType& state () const = 0;
  virtual Net_Connection_Status status () const = 0;

  // *NOTE*: see ACE_Svc_Handler/ACE_Task_Base
  //         (and net_common.h / ACE_Svc_Handler.h for reason codes)
//  virtual int close (u_long = 0) = 0; // reason
  virtual void close () = 0;
  //// *IMPORTANT NOTE*: fire-and-forget API
  //virtual bool send (ACE_Message_Block*&) = 0;
  // *NOTE*: waits for any (queued) data to be dispatched to the kernel.
  //         Depending on OS (and protocol) specifics, this could mean that it
  //         has been successfully transmitted (YMMV). As current OS kernel(s)
  //         encapsulate most of the TCP/UDP protocol functionality, this API
  //         represents the appropriate asynchronous means to ensure that the
  //         forwarded data has in fact been transmitted successfully
  // *IMPORTANT NOTE*: (As far as yours truly is aware) (Berkeley) sockets
  //                   provide no means of verification of successful
  //                   transmission of the data from (!) the kernel socket
  //                   buffers (Note that this feature may also be protocol-
  //                   specific); the send()-man pages do not clear things
  //                   either, leaving socket data delivery shrouded in mystery.
  //                   In this sense, the more platform-specific, asynchronous
  //                   APIs, as supported by the 'proactive' framework(s), may
  //                   deliver more reliable, protocol-agnostic results (again,
  //                   YMMV).
  //                   For the reactor based, blocking (!) API, this could be a
  //                   NOP
  virtual void waitForCompletion (bool = true) = 0; // wait for any worker thread(s) ?
};

//////////////////////////////////////////

template <typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          ////////////////////////////////
          typename SocketConfigurationType,
          typename HandlerConfigurationType> // socket-
class Net_ISocketConnection_T
 : public virtual Net_IConnection_T<AddressType,
                                    ConfigurationType,
                                    StateType,
                                    StatisticContainerType>
 , public virtual Net_ITransportLayer_T<SocketConfigurationType>
 , public virtual Common_IInitialize_T<HandlerConfigurationType>
 , public Common_IGetR_2_T<HandlerConfigurationType>
{
 public:
  // convenient types
  typedef Net_IConnection_T<AddressType,
                            ConfigurationType,
                            StateType,
                            StatisticContainerType> ICONNECTION_T;
  typedef Net_ITransportLayer_T<SocketConfigurationType> ITRANSPORTLAYER_T;

  // *IMPORTANT NOTE*: fire-and-forget API
  virtual void send (ACE_Message_Block*&) = 0;
};

//////////////////////////////////////////

template <typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          ////////////////////////////////
          typename SocketConfigurationType,
          typename HandlerConfigurationType, // socket-
          ////////////////////////////////
          typename StreamType,
          typename StreamStatusType>
class Net_IStreamConnection_T
 : public virtual Net_ISocketConnection_T<AddressType,
                                          ConfigurationType,
                                          StateType,
                                          StatisticContainerType,
                                          SocketConfigurationType,
                                          HandlerConfigurationType>
 , public Stream_IMessageQueue
{
 public:
  // convenient types
  typedef StreamType STREAM_T;
  typedef Net_ISocketConnection_T<AddressType,
                                  ConfigurationType,
                                  StateType,
                                  StatisticContainerType,
                                  SocketConfigurationType,
                                  HandlerConfigurationType> ISOCKETCONNECTION_T;

  virtual const StreamType& stream () const = 0;

  virtual bool wait (StreamStatusType,
                     const ACE_Time_Value* = NULL) = 0; // timeout (absolute) ? : block
};

//////////////////////////////////////////

template <typename AddressType>
class Net_ISessionBase_T
{
 public:
  virtual void connect (const AddressType&) = 0; // peer address
  virtual void disconnect (const AddressType&) = 0; // peer address

  virtual void close (bool = false) = 0; // wait ?

  ////////////////////////////////////////
  // callbacks
  // *TODO*: remove ASAP
  virtual void connect (Net_ConnectionId_t) = 0;    // connection id
  virtual void disconnect (Net_ConnectionId_t) = 0; // connection id
};

typedef Net_ISessionBase_T<ACE_INET_Addr> Net_IInetSession_t;

template <typename AddressType,
          typename ConnectionConfigurationType,
          typename ConnectionStateType,
          typename StatisticContainerType,
          ////////////////////////////////
          typename SocketConfigurationType,
          typename HandlerConfigurationType, // socket-
          ////////////////////////////////
          typename PeerStreamType,
          typename StreamStatusType,
          ////////////////////////////////
          typename ConfigurationType,
          typename StateType>
class Net_ISession_T
 : public Net_ISessionBase_T<AddressType>
 , public Common_IInitialize_T<ConfigurationType>
// : public typename StreamType::IDATA_NOTIFY_T
{
 public:
  virtual const StateType& state () const = 0;

//  // convenient types
//  typedef Net_IConnection_T<AddressType,
//                            ConnectionConfigurationType,
//                            StateType,
//                            StatisticContainerType> ICONNECTION_T;
//  typedef Net_IStreamConnection_T<AddressType,
//                                  ConnectionConfigurationType,
//                                  ConnectionStateType,
//                                  StatisticContainerType,
//                                  SocketConfigurationType,
//                                  HandlerConfigurationType,
//                                  StreamType,
//                                  StreamStatusType> ISTREAM_CONNECTION_T;
};

#endif
