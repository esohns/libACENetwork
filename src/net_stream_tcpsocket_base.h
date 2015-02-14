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

#ifndef Net_STREAM_TCPSOCKET_BASE_H
#define Net_STREAM_TCPSOCKET_BASE_H

#include "net_stream_common.h"

#include "ace/Global_Macros.h"
#include "ace/Event_Handler.h"
#include "ace/Message_Block.h"
#include "ace/Synch.h"

template <typename ConfigurationType,
          typename StatisticsContainerType,
          typename StreamType,
          typename SocketHandlerType>
class Net_StreamTCPSocketBase_T
 : public SocketHandlerType
{
 public:
  // override some task-based members
  virtual int open (void* = NULL); // args
  //virtual int close (u_long = 0); // args

  // *NOTE*: enqueue any received data onto our stream for further processing
  virtual int handle_input (ACE_HANDLE = ACE_INVALID_HANDLE);
  // *NOTE*: send any enqueued data back to the client...
  virtual int handle_output (ACE_HANDLE = ACE_INVALID_HANDLE);
  // *NOTE*: this is called when:
  // - handle_xxx() returns -1
  virtual int handle_close (ACE_HANDLE = ACE_INVALID_HANDLE,
                            ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK);

 protected:
  Net_StreamTCPSocketBase_T ();
  virtual ~Net_StreamTCPSocketBase_T ();

  ConfigurationType* configuration_;
  StreamType         stream_;
  ACE_Message_Block* currentReadBuffer_;
  ACE_Thread_Mutex   sendLock_;
  ACE_Message_Block* currentWriteBuffer_;

  // helper method(s)
  ACE_Message_Block* allocateMessage (unsigned int); // requested size

 private:
  typedef SocketHandlerType inherited;

//  ACE_UNIMPLEMENTED_FUNC (Net_StreamUDPSocketBase_T ());
  ACE_UNIMPLEMENTED_FUNC (Net_StreamTCPSocketBase_T (const Net_StreamTCPSocketBase_T&));
  ACE_UNIMPLEMENTED_FUNC (Net_StreamTCPSocketBase_T& operator= (const Net_StreamTCPSocketBase_T&));

  // *IMPORTANT NOTE*: in a threaded environment, workers MAY
  // dispatch the reactor notification queue concurrently (most notably,
  // ACE_TP_Reactor) --> enforce proper serialization
  bool               serializeOutput_;
  // *NOTE*: this is a transient handle, used only to initialize the session ID
  Net_StreamState_t* state_;
};

// include template implementation
#include "net_stream_tcpsocket_base.inl"

#endif
