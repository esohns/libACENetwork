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

#ifndef Net_STREAM_ASYNCH_TCPSOCKET_BASE_H
#define Net_STREAM_ASYNCH_TCPSOCKET_BASE_H

#include "ace/Global_Macros.h"
#include "ace/Event_Handler.h"
#include "ace/Message_Block.h"
#include "ace/Asynch_IO.h"

#include "net_connection_base.h"
//#include "net_exports.h"

// forward declarations
struct Stream_State_t;

template <typename ConfigurationType,
          typename SessionDataType,
          typename ITransportLayerType,
          typename StatisticsContainerType,
          typename StreamType,
          typename SocketHandlerType>
class Net_StreamAsynchTCPSocketBase_T
 : public SocketHandlerType
 , public ACE_Event_Handler
 , public Net_ConnectionBase_T<ConfigurationType,
                               SessionDataType,
                               ITransportLayerType,
                               StatisticsContainerType>
{
 public:
  typedef Net_ConnectionBase_T<ConfigurationType,
                               SessionDataType,
                               ITransportLayerType,
                               StatisticsContainerType> CONNECTION_BASE_T;

  virtual ~Net_StreamAsynchTCPSocketBase_T ();

  // override some service methods
  virtual void open (ACE_HANDLE,          // (socket) handle
                     ACE_Message_Block&); // initial data (if any)
  virtual int handle_output (ACE_HANDLE); // (socket) handle
  virtual int handle_close (ACE_HANDLE,        // (socket) handle
                            ACE_Reactor_Mask); // (select) mask
  virtual void act (const void*); // (user) data handle

 protected:
  typedef Net_IConnectionManager_T<ConfigurationType,
                                   SessionDataType,
                                   ITransportLayerType,
                                   StatisticsContainerType> ICONNECTION_MANAGER_T;

  Net_StreamAsynchTCPSocketBase_T (ICONNECTION_MANAGER_T*);

  virtual void handle_read_stream (const ACE_Asynch_Read_Stream::Result&); // result

  StreamType      stream_;
  // *TODO*: (try to) handle short writes gracefully...
//  ACE_Message_Block* buffer_;

 private:
  typedef SocketHandlerType inherited;
  typedef Net_ConnectionBase_T<ConfigurationType,
                               SessionDataType,
                               ITransportLayerType,
                               StatisticsContainerType> inherited2;

  ACE_UNIMPLEMENTED_FUNC (Net_StreamAsynchTCPSocketBase_T ());
  ACE_UNIMPLEMENTED_FUNC (Net_StreamAsynchTCPSocketBase_T (const Net_StreamAsynchTCPSocketBase_T&));
  ACE_UNIMPLEMENTED_FUNC (Net_StreamAsynchTCPSocketBase_T& operator= (const Net_StreamAsynchTCPSocketBase_T&));

  // *NOTE*: this is a transient handle, used only to initialize the session ID
  Stream_State_t* state_;
};

// include template definition
#include "net_stream_asynch_tcpsocket_base.inl"

#endif
