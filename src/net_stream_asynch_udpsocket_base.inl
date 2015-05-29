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

#include "ace/Log_Msg.h"

#include "stream_common.h"

#include "net_defines.h"
#include "net_macros.h"

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                SocketType,
                                SocketHandlerType>::Net_StreamAsynchUDPSocketBase_T (ICONNECTION_MANAGER_T* interfaceHandle_in,
                                                                                     unsigned int statisticsCollectionInterval_in)
 : inherited4 (interfaceHandle_in,
               statisticsCollectionInterval_in)
// , buffer_ (NULL)
 //, stream_ ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::Net_StreamAsynchUDPSocketBase_T"));

}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                SocketType,
                                SocketHandlerType>::~Net_StreamAsynchUDPSocketBase_T ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::~Net_StreamAsynchUDPSocketBase_T"));

  // step1: remove enqueued module (if any)
  if (inherited4::configuration_.streamConfiguration.streamConfiguration.module)
  {
    Stream_Module_t* module_p =
      stream_.find (inherited4::configuration_.streamConfiguration.streamConfiguration.module->name ());
    if (module_p)
    {
      int result =
        stream_.remove (inherited4::configuration_.streamConfiguration.streamConfiguration.module->name (),
                        ACE_Module_Base::M_DELETE_NONE);
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Stream::remove(\"%s\"): \"%m\", continuing\n"),
                    inherited4::configuration_.streamConfiguration.streamConfiguration.module->name ()));
    } // end IF
    if (inherited4::configuration_.streamConfiguration.streamConfiguration.deleteModule)
      delete inherited4::configuration_.streamConfiguration.streamConfiguration.module;
  } // end IF

//  if (buffer_)
//    buffer_->release ();
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
void
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                SocketType,
                                SocketHandlerType>::open (ACE_HANDLE handle_in,
                                                          ACE_Message_Block& messageBlock_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::open"));

  ACE_UNUSED_ARG (handle_in);

  int result = -1;
  unsigned int session_id = 0;
  Net_SocketHandlerConfiguration_t socket_handler_configuration;

  // step1: open socket
  ACE_INET_Addr local_SAP (inherited4::configuration_.socketConfiguration.peerAddress.get_port_number (),
                           (inherited4::configuration_.socketConfiguration.useLoopbackDevice ? INADDR_LOOPBACK
                                                                                             : INADDR_ANY));
  result = inherited2::open (local_SAP,                // local SAP
                             ACE_PROTOCOL_FAMILY_INET, // protocol family
                             0,                        // protocol
                             1);                       // reuse_addr
  if (result == -1)
  {
    ACE_TCHAR buffer[BUFSIZ];
    ACE_OS::memset (buffer, 0, sizeof (buffer));
    std::string local_address;
    if (local_SAP.addr_to_string (buffer,
                                  sizeof (buffer)) == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
    local_address = buffer;
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SocketType::open(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (local_address.c_str ())));
    goto close;
  } // end IF

  // step2a: initialize base-class
  // *TODO*: this should not be happening here...
  socket_handler_configuration.bufferSize =
    inherited4::configuration_.protocolConfiguration.bufferSize;
  socket_handler_configuration.messageAllocator =
    inherited4::configuration_.streamConfiguration.streamConfiguration.messageAllocator;
  socket_handler_configuration.socketConfiguration =
    inherited4::configuration_.socketConfiguration;
  if (!inherited::initialize (socket_handler_configuration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_SocketHandlerBase::initialize(): \"%m\", aborting\n")));
    goto close;
  } // end IF

  // step2b: tweak socket, initialize I/O
  inherited::open (inherited2::get_handle (), messageBlock_in);

  // step3: initialize/start stream
  // step3a: connect stream head message queue with a notification pipe/queue ?
  if (!inherited4::configuration_.streamConfiguration.streamConfiguration.useThreadPerConnection)
    inherited4::configuration_.streamConfiguration.streamConfiguration.notificationStrategy = this;
  // step3b: initialize final module (if any)
  if (inherited4::configuration_.streamConfiguration.streamConfiguration.module)
  {
    Stream_IModule_t* imodule_p = NULL;
    // need a downcast...
    imodule_p =
      dynamic_cast<Stream_IModule_t*> (inherited4::configuration_.streamConfiguration.streamConfiguration.module);
    if (!imodule_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: dynamic_cast<Stream_IModule> failed, aborting\n"),
                  ACE_TEXT (inherited4::configuration_.streamConfiguration.streamConfiguration.module->name ())));
      goto close;
    } // end IF
    Stream_Module_t* clone_p = NULL;
    try
    {
      clone_p = imodule_p->clone ();
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: caught exception in Stream_IModule::clone(), aborting\n"),
                  ACE_TEXT (inherited4::configuration_.streamConfiguration.streamConfiguration.module->name ())));
      goto close;
    }
    if (!clone_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to Stream_IModule::clone(), aborting\n"),
                  ACE_TEXT (inherited4::configuration_.streamConfiguration.streamConfiguration.module->name ())));
      goto close;
    }
    inherited4::configuration_.streamConfiguration.streamConfiguration.module = clone_p;
    inherited4::configuration_.streamConfiguration.streamConfiguration.deleteModule = true;
  } // end IF
  // *TODO*: this clearly is a design glitch
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  inherited4::configuration_.streamConfiguration.sessionID =
    reinterpret_cast<unsigned int> (inherited2::get_handle ()); // (== socket handle)
#else
  inherited4::configuration_.streamConfiguration.sessionID =
    static_cast<unsigned int> (inherited2::get_handle ()); // (== socket handle)
#endif
  if (!stream_.initialize (inherited4::configuration_.streamConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize processing stream, aborting\n")));
    goto close;
  } // end IF
  //stream_.dump_state ();
  // *NOTE*: as soon as this returns, data starts arriving at handle_output()[/msg_queue()]
  stream_.start ();
  if (!stream_.isRunning ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start processing stream, aborting\n")));
    goto close;
  } // end IF

  // step4: start reading (need to pass any data ?)
  if (messageBlock_in.length () == 0)
    inherited::initiate_read_dgram ();
  else
  {
    ACE_Message_Block* message_block_p = messageBlock_in.duplicate ();
    if (!message_block_p)
    {
      ACE_ERROR ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Message_Block::duplicate(): \"%m\", aborting\n")));
      goto close;
    } // end IF
    // fake a result to emulate regular behavior...
    ACE_Asynch_Read_Dgram_Result_Impl* fake_result_p =
      inherited::proactor ()->create_asynch_read_dgram_result (inherited::proxy (),                  // handler proxy
                                                               handle_in,                            // socket handle
                                                               message_block_p,                      // buffer
                                                               message_block_p->size (),             // #bytes to read
                                                               0,                                    // flags
                                                               PF_INET,                              // protocol family
                                                               NULL,                                 // ACT
                                                               ACE_INVALID_HANDLE,                   // event
                                                               0,                                    // priority
                                                               COMMON_EVENT_PROACTOR_SIG_RT_SIGNAL); // signal
    if (!fake_result_p)
    {
      ACE_ERROR ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Proactor::create_asynch_read_dgram_result: \"%m\", aborting\n")));
      goto close;
    } // end IF
    size_t bytes_transferred = message_block_p->length ();
    // <complete> for Accept would have already moved the <wr_ptr>
    // forward; update it to the beginning position
    message_block_p->wr_ptr (message_block_p->wr_ptr () - bytes_transferred);
    // invoke ourselves (see handle_read_stream)
    fake_result_p->complete (message_block_p->length (), // bytes read
                             1,                          // success
                             NULL,                       // ACT
                             0);                         // error

    // clean up
    delete fake_result_p;
  } // end ELSE

  // step5: register with the connection manager (if any)
  if (!inherited4::registerc ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_ConnectionBase_T::registerc(), aborting\n")));
    goto close;
  } // end IF

  return;

close:
  result = handle_close (inherited2::get_handle (),
                         ACE_Event_Handler::ALL_EVENTS_MASK);
  if (result == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to Net_StreamAsynchUDPSocketBase_T::handle_close(): \"%m\", continuing\n")));
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
int
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                SocketType,
                                SocketHandlerType>::handle_output (ACE_HANDLE handle_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::handle_output"));

  ACE_UNUSED_ARG (handle_in);

  int result = -1;
  ACE_Message_Block* message_block_p = NULL;
  size_t bytes_to_send = 0;
  ssize_t result_2 = -1;

//  if (!buffer_)
//  {
    // send next data chunk from the stream...
    // *IMPORTANT NOTE*: should NEVER block, as available outbound data has
    //                   been notified
//    result = stream_.get (buffer_,
    result = stream_.get (message_block_p,
                          &const_cast<ACE_Time_Value&> (ACE_Time_Value::zero));
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Stream::get(): \"%m\", aborting\n")));
      return -1;
    } // end IF
//  } // end IF
//  ACE_ASSERT (buffer_);
  ACE_ASSERT (message_block_p);

  // start (asynchronous) write
//  bytes_to_send = buffer_->size ();
  bytes_to_send = message_block_p->size ();
  result_2 =
//      inherited::outputStream_.send (buffer_,                                                    // data
      inherited::outputStream_.send (message_block_p,                                            // data
                                     bytes_to_send,                                              // #bytes to send
                                     0,                                                          // flags
                                     inherited4::configuration_.socketConfiguration.peerAddress, // remote address
                                     NULL,                                                       // ACT
                                     0,                                                          // priority
                                     COMMON_EVENT_PROACTOR_SIG_RT_SIGNAL);                       // signal
  if (result_2 == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Asynch_Write_Dgram::send(%u): \"%m\", aborting\n"),
                message_block_p->size ()));

    // clean up
//    buffer_->release ();
//    buffer_ = NULL;
    message_block_p->release ();

    return -1;
  } // end IF

  return 0;
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
int
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                SocketType,
                                SocketHandlerType>::handle_close (ACE_HANDLE handle_in,
                                                                  ACE_Reactor_Mask mask_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::handle_close"));

  ACE_UNUSED_ARG (handle_in);

  int result = -1;

  // step1: wait for all workers within the stream (if any)
  if (stream_.isRunning ())
  {
    stream_.stop ();
    stream_.waitForCompletion ();
  } // end IF

  // step2: purge any pending notifications ?
  // *WARNING: do this here, while still holding on to the current write buffer
  if (!inherited4::configuration_.streamConfiguration.streamConfiguration.useThreadPerConnection)
  {
    Stream_Iterator_t iterator (stream_);
    const Stream_Module_t* module_p = NULL;
    result = iterator.next (module_p);
    if (result == 1)
    {
      ACE_ASSERT (module_p);
      Stream_Task_t* task_p =
          const_cast<Stream_Module_t*> (module_p)->writer ();
      ACE_ASSERT (task_p);
      result = task_p->msg_queue ()->flush ();
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_MessageQueue::flush(): \"%m\", continuing\n")));
    } // end IF
  } // end IF

  // step3: invoke base-class maintenance
  result = inherited::handle_close (inherited::handle (),
                                    mask_in);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SocketHandlerType::handle_close(): \"%m\", continuing\n")));

  // step4: deregister with the connection manager (if any)
  inherited4::deregister ();

  // step5: release a reference
  // *IMPORTANT NOTE*: may 'delete this'
  //inherited4::decrease ();
  this->decrease ();

  return result;
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
bool
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                SocketType,
                                SocketHandlerType>::collect (StatisticContainerType& data_out)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::collect"));

  try
  {
    return stream_.collect (data_out);
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IStatistic::collect(), aborting\n")));
  }

  return false;
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
void
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                SocketType,
                                SocketHandlerType>::report () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::report"));

  try
  {
    return stream_.report ();
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IStatistic::report(), aborting\n")));
  }
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
void
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                SocketType,
                                SocketHandlerType>::info (ACE_HANDLE& handle_out,
                                                          AddressType& localSAP_out,
                                                          AddressType& remoteSAP_out) const
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::info"));

  int result = -1;

  handle_out = inherited2::get_handle ();
  result = inherited2::get_local_addr (localSAP_out);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_SOCK_DGram::get_local_addr(): \"%m\", continuing\n")));
  remoteSAP_out = inherited4::configuration_.socketConfiguration.peerAddress;
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
unsigned int
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                SocketType,
                                SocketHandlerType>::id () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::id"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  return reinterpret_cast<unsigned int> (inherited::handle ());
#else
  return static_cast<unsigned int> (inherited::handle ());
#endif
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
void
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                SocketType,
                                SocketHandlerType>::dump_state () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::dump_state"));

  ACE_HANDLE handle = ACE_INVALID_HANDLE;
  ACE_INET_Addr local_inet_address, peer_inet_address;
  info (handle,
        local_inet_address,
        peer_inet_address);

  ACE_TCHAR buffer[BUFSIZ];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  std::string local_address;
  if (local_inet_address.addr_to_string (buffer,
                                         sizeof (buffer)) == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
  else
    local_address = buffer;
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  std::string peer_address;
  if (peer_inet_address.addr_to_string (buffer,
                                        sizeof (buffer)) == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
  else
    peer_address = buffer;

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("connection [id: %u [%d]]: \"%s\" <--> \"%s\"\n"),
              id (), handle,
              ACE_TEXT (local_address.c_str ()),
              ACE_TEXT (peer_address.c_str ())));
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
void
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                SocketType,
                                SocketHandlerType>::close ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::close"));

  int result = -1;

  // step1: shutdown operations
  ACE_HANDLE handle = inherited::handle ();
  // *NOTE*: may 'delete this'
  result = handle_close (handle,
                         ACE_Event_Handler::ALL_EVENTS_MASK);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_StreamAsynchTCPSocketBase_T::handle_close(): \"%m\", continuing\n")));

  //  step2: release the socket handle
  if (handle != ACE_INVALID_HANDLE)
  {
    result = ACE_OS::closesocket (handle);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::closesocket(%d): \"%m\", continuing\n"),
                  handle));
//    inherited::handle (ACE_INVALID_HANDLE);
  } // end IF
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
void
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                SocketType,
                                SocketHandlerType>::handle_read_dgram (const ACE_Asynch_Read_Dgram::Result& result_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::handle_read_dgram"));

  int result = -1;
  unsigned long error = 0;

  // sanity check
  result = result_in.success ();
  if (result == 0)
  {
    // connection closed/reset ? --> not an error
    error = result_in.error ();
    if ((error != ECONNRESET) &&
        (error != EPIPE)      &&
        (error != EBADF)      && // local close (), happens on Linux
        (error != 64)) // *TODO*: EHOSTDOWN (- 10000), happens on Win32
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to read from input stream (%d): %u --> \"%s\", aborting\n"),
                  result_in.handle (),
                  error, ACE_TEXT (ACE_OS::strerror (error))));
  } // end IF

  switch (result_in.bytes_transferred ())
  {
    case -1:
    {
      // connection closed/reset (by peer) ? --> not an error
      error = result_in.error ();
      if ((error != ECONNRESET) &&
          (error != EPIPE)      &&
          (error != EBADF)      && // local close (), happens on Linux
          (error != 64)) // *TODO*: EHOSTDOWN (- 10000), happens on Win32
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to read from input stream (%d): %u --> \"%s\", aborting\n"),
                    result_in.handle (),
                    error, ACE_TEXT (ACE_OS::strerror (error))));

      break;
    }
    // *** GOOD CASES ***
    case 0:
    {
//       ACE_DEBUG ((LM_DEBUG,
//                   ACE_TEXT ("[%u]: socket was closed by the peer...\n"),
//                   handle_));

      break;
    }
    default:
    {
//       ACE_DEBUG ((LM_DEBUG,
//                   ACE_TEXT ("[%d]: received %u bytes...\n"),
//                   result_in.handle (),
//                   result_in.bytes_transferred ()));

      // push the buffer onto the stream for processing
      result = stream_.put (result_in.message_block (), NULL);
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Stream::put(): \"%m\", aborting\n")));
        break;
      } // end IF

      // start next read
      inherited::initiate_read_dgram ();

      return;
    }
  } // end SWITCH

  // clean up
  result_in.message_block ()->release ();

  result = handle_close (inherited::handle (),
                         ACE_Event_Handler::ALL_EVENTS_MASK);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Svc_Handler::handle_close(): \"%m\", continuing\n")));
}

/////////////////////////////////////////

#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType>
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                ACE_SOCK_NETLINK,
                                Net_AsynchNetlinkSocketHandler>::Net_StreamAsynchUDPSocketBase_T (ICONNECTION_MANAGER_T* interfaceHandle_in,
                                                                                                  unsigned int statisticsCollectionInterval_in)
 : inherited4 (interfaceHandle_in,
               statisticsCollectionInterval_in)
// , buffer_ (NULL)
 //, userData_ ()
 //, stream_ ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::Net_StreamAsynchUDPSocketBase_T"));

}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType>
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                ACE_SOCK_NETLINK,
                                Net_AsynchNetlinkSocketHandler>::~Net_StreamAsynchUDPSocketBase_T ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::~Net_StreamAsynchUDPSocketBase_T"));

  // step1: remove enqueued module (if any)
  if (inherited4::configuration_.streamConfiguration.module)
  {
    Stream_Module_t* module_p =
      stream_.find (inherited4::configuration_.streamConfiguration.module->name ());
    if (module_p)
    {
      int result =
        stream_.remove (inherited4::configuration_.streamConfiguration.module->name (),
                        ACE_Module_Base::M_DELETE_NONE);
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Stream::remove(\"%s\"): \"%m\", continuing\n"),
                    inherited4::configuration_.streamConfiguration.module->name ()));
    } // end IF
    if (inherited4::configuration_.streamConfiguration.deleteModule)
      delete inherited4::configuration_.streamConfiguration.module;
  } // end IF

//  if (buffer_)
//    buffer_->release ();
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType>
void
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                ACE_SOCK_NETLINK,
                                Net_AsynchNetlinkSocketHandler>::open (ACE_HANDLE handle_in,
                                                                       ACE_Message_Block& messageBlock_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::open"));

  ACE_UNUSED_ARG (handle_in);

  int result = -1;
  unsigned int session_id = 0;
  Net_SocketHandlerConfiguration_t socket_handler_configuration;

  // *TODO*: ???
  ACE_Netlink_Addr address =
      inherited4::configuration_.socketConfiguration.netlinkAddress;
  // step1: open socket
  result =
      inherited2::open (address,                                                         // local SAP
                        ACE_PROTOCOL_FAMILY_NETLINK,                                     // protocol family
                        inherited4::configuration_.socketConfiguration.netlinkProtocol); // protocol
                        // NETLINK_USERSOCK);                                             // protocol
                        //NETLINK_GENERIC);                                              // protocol
  if (result == -1)
  {
    ACE_TCHAR buffer[BUFSIZ];
    ACE_OS::memset (buffer, 0, sizeof (buffer));
    std::string local_address;
    // *TODO*: find a replacement for ACE_INET_Addr::addr_to_string
//    if (local_SAP.addr_to_string (buffer,
//                                  sizeof (buffer)) == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
    local_address = buffer;
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SocketType::open(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (local_address.c_str ())));
    goto close;
  } // end IF

  // step2a: initialize base-class
  // *TODO*: this should not be happening here...
  socket_handler_configuration.bufferSize =
      inherited4::configuration_.protocolConfiguration.bufferSize;
  socket_handler_configuration.messageAllocator =
      inherited4::configuration_.streamConfiguration.messageAllocator;
  socket_handler_configuration.socketConfiguration =
      inherited4::configuration_.socketConfiguration;
  if (!inherited::initialize (socket_handler_configuration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_SocketHandlerBase::initialize(): \"%m\", aborting\n")));
    goto close;
  } // end IF

  // step2b: tweak socket, initialize I/O
  inherited::open (inherited2::get_handle (), messageBlock_in);

  // step3: initialize/start stream
  // step3a: connect stream head message queue with a notification pipe/queue ?
  if (!inherited4::configuration_.streamConfiguration.useThreadPerConnection)
    inherited4::configuration_.streamConfiguration.notificationStrategy = this;
  // step3b: initialize final module (if any)
  if (inherited4::configuration_.streamConfiguration.module)
  {
    Stream_IModule_t* imodule_p =
      dynamic_cast<Stream_IModule_t*> (inherited4::configuration_.streamConfiguration.module);
    if (!imodule_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: dynamic_cast<Stream_IModule> failed, aborting\n"),
                  ACE_TEXT (inherited4::configuration_.streamConfiguration.module->name ())));
      goto close;
    } // end IF
    Stream_Module_t* clone_p = NULL;
    try
    {
      clone_p = imodule_p->clone ();
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: caught exception in Stream_IModule::clone(), aborting\n"),
                  ACE_TEXT (inherited4::configuration_.streamConfiguration.module->name ())));
      goto close;
    }
    if (!clone_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to Stream_IModule::clone(), aborting\n"),
                  ACE_TEXT (inherited4::configuration_.streamConfiguration.module->name ())));
      goto close;
    }
    inherited4::configuration_.streamConfiguration.module = clone_p;
    inherited4::configuration_.streamConfiguration.deleteModule = true;
  } // end IF
#if defined (_MSC_VER)
  session_id =
    reinterpret_cast<unsigned int> (inherited2::get_handle ()); // (== socket handle)
#else
  session_id =
    static_cast<unsigned int> (inherited2::get_handle ()); // (== socket handle)
#endif
  // *TODO*: this clearly is a design glitch
  if (!stream_.initialize (session_id,
                           inherited4::configuration_.streamConfiguration,
                           inherited4::configuration_.protocolConfiguration,
                           inherited4::configuration_.streamSessionData))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize processing stream, aborting\n")));
    goto close;
  } // end IF
  //stream_.dump_state ();
  // *NOTE*: as soon as this returns, data starts arriving at handle_output()[/msg_queue()]
  stream_.start ();
  if (!stream_.isRunning ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start processing stream, aborting\n")));
    goto close;
  } // end IF

  // step4: start reading (need to pass any data ?)
  if (messageBlock_in.length () == 0)
    inherited::initiate_read_dgram ();
  else
  {
    ACE_Message_Block* message_block_p = messageBlock_in.duplicate ();
    if (!message_block_p)
    {
      ACE_ERROR ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Message_Block::duplicate(): \"%m\", aborting\n")));
      goto close;
    } // end IF
    // fake a result to emulate regular behavior...
    ACE_Asynch_Read_Dgram_Result_Impl* fake_result_p =
      inherited::proactor ()->create_asynch_read_dgram_result (inherited::proxy (),                  // handler proxy
                                                               inherited2::get_handle (),            // socket handle
                                                               message_block_p,                      // buffer
                                                               message_block_p->size (),             // #bytes to read
                                                               0,                                    // flags
                                                               PF_INET,                              // protocol family
                                                               NULL,                                 // ACT
                                                               ACE_INVALID_HANDLE,                   // event
                                                               0,                                    // priority
                                                               COMMON_EVENT_PROACTOR_SIG_RT_SIGNAL); // signal
    if (!fake_result_p)
    {
      ACE_ERROR ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Proactor::create_asynch_read_dgram_result: \"%m\", aborting\n")));
      goto close;
    } // end IF
    size_t bytes_transferred = message_block_p->length ();
    // <complete> for Accept would have already moved the <wr_ptr>
    // forward; update it to the beginning position
    message_block_p->wr_ptr (message_block_p->wr_ptr () - bytes_transferred);
    // invoke ourselves (see handle_read_stream)
    fake_result_p->complete (message_block_p->length (), // bytes read
                             1,                          // success
                             NULL,                       // ACT
                             0);                         // error

    // clean up
    delete fake_result_p;
  } // end ELSE

  // step5: register with the connection manager (if any)
  if (!inherited4::registerc ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_ConnectionBase_T::registerc(), aborting\n")));
    goto close;
  } // end IF

  return;

close:
  result = handle_close (inherited2::get_handle (),
                         ACE_Event_Handler::ALL_EVENTS_MASK);
  if (result == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to Net_StreamAsynchUDPSocketBase_T::handle_close(): \"%m\", continuing\n")));
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType>
int
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                ACE_SOCK_NETLINK,
                                Net_AsynchNetlinkSocketHandler>::handle_output (ACE_HANDLE handle_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::handle_output"));

  ACE_UNUSED_ARG (handle_in);

  int result = -1;
  ssize_t result_2 = -1;
  ACE_Message_Block* message_block_p = NULL;
//  if (buffer_ == NULL)
//  {
  // send next data chunk from the stream...
  // *IMPORTANT NOTE*: this should NEVER block, as available outbound data has
  // been notified
  //  result = stream_.get (buffer_, &ACE_Time_Value::zero);
  result = stream_.get (message_block_p,
                        &const_cast<ACE_Time_Value&> (ACE_Time_Value::zero));
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::get(): \"%m\", aborting\n")));
    return -1;
  } // end IF
//  } // end IF

  // start (asynchronous) write
  // *NOTE*: this is a fire-and-forget API for message_block...
  size_t bytes_to_send = message_block_p->size ();
  ACE_Netlink_Addr peer_address;
  peer_address.set (0, 0); // send to kernel
//  result_2 = inherited::outputStream_.send (buffer_,     // data
  result_2 = inherited::outputStream_.send (message_block_p,                      // data
                                            bytes_to_send,                        // #bytes to send
                                            0,                                    // flags
                                            peer_address,                         // peer address
                                            NULL,                                 // ACT
                                            0,                                    // priority
                                            COMMON_EVENT_PROACTOR_SIG_RT_SIGNAL); // signal
  if (result_2 == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Asynch_Write_Dgram::send(%u): \"%m\", aborting\n"),
                message_block_p->size ()));

    // clean up
//    buffer_->release ();
//    buffer_ = NULL;
    message_block_p->release ();

    return -1;
  } // end IF

  return 0;
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType>
int
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                ACE_SOCK_NETLINK,
                                Net_AsynchNetlinkSocketHandler>::handle_close (ACE_HANDLE handle_in,
                                                                               ACE_Reactor_Mask mask_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::handle_close"));

  ACE_UNUSED_ARG (handle_in);

  int result = -1;

  // step1: wait for all workers within the stream (if any)
  if (stream_.isRunning ())
  {
    stream_.stop ();
    stream_.waitForCompletion ();
  } // end IF

  // step2: purge any pending notifications ?
  // *WARNING: do this here, while still holding on to the current write buffer
  if (!inherited4::configuration_.streamConfiguration.useThreadPerConnection)
  {
    Stream_Iterator_t iterator (stream_);
    const Stream_Module_t* module_p = NULL;
    result = iterator.next (module_p);
    if (result == 1)
    {
      ACE_ASSERT (module_p);
      Stream_Task_t* task_p =
          const_cast<Stream_Module_t*> (module_p)->writer ();
      ACE_ASSERT (task_p);
      if (task_p->msg_queue ()->flush () == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_MessageQueue::flush(): \"%m\", continuing\n")));
    } // end IF
  } // end IF

  // step3: invoke base-class maintenance
  result = inherited::handle_close (inherited2::get_handle (),
                                    mask_in);
  if (result == -1)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to SocketHandlerType::handle_close(): \"%m\", continuing\n")));

  // step4: deregister with the connection manager (if any)
  inherited4::deregister ();

  // step5: release a reference
  // *IMPORTANT NOTE*: may 'delete this'
  //inherited4::decrease ();
  this->decrease ();

  return result;
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType>
bool
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                ACE_SOCK_NETLINK,
                                Net_AsynchNetlinkSocketHandler>::collect (StatisticContainerType& data_out)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::collect"));

  try
  {
    return stream_.collect (data_out);
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IStatistic::collect(), aborting\n")));
  }

  return false;
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType>
void
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                ACE_SOCK_NETLINK,
                                Net_AsynchNetlinkSocketHandler>::report () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::report"));

  try
  {
    return stream_.report ();
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IStatistic::report(), aborting\n")));
  }
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType>
void
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                ACE_SOCK_NETLINK,
                                Net_AsynchNetlinkSocketHandler>::info (ACE_HANDLE& handle_out,
                                                                       AddressType& localSAP_out,
                                                                       AddressType& remoteSAP_out) const
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::info"));

  int result = -1;

  handle_out = inherited::handle ();
  result = inherited2::get_local_addr (localSAP_out);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_SOCK_DGram::get_local_addr(): \"%m\", continuing\n")));
  remoteSAP_out = inherited4::configuration_->socketConfiguration.peerAddress;
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType>
unsigned int
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                ACE_SOCK_NETLINK,
                                Net_AsynchNetlinkSocketHandler>::id () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::id"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  return reinterpret_cast<unsigned int> (inherited::handle ());
#else
  return static_cast<unsigned int> (inherited::handle ());
#endif
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType>
void
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                ACE_SOCK_NETLINK,
                                Net_AsynchNetlinkSocketHandler>::dump_state () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::dump_state"));

  ACE_HANDLE handle = ACE_INVALID_HANDLE;
  ACE_INET_Addr local_inet_address, peer_inet_address;
  info (handle,
        local_inet_address,
        peer_inet_address);

  ACE_TCHAR buffer[BUFSIZ];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  std::string local_address;
  if (local_inet_address.addr_to_string (buffer,
                                         sizeof (buffer)) == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
  else
    local_address = buffer;
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  std::string peer_address;
  if (peer_inet_address.addr_to_string (buffer,
                                        sizeof (buffer)) == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
  else
    peer_address = buffer;

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("connection [id: %u [%d]]: \"%s\" <--> \"%s\"\n"),
              id (), handle,
              ACE_TEXT (local_address.c_str ()),
              ACE_TEXT (peer_address.c_str ())));
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType>
void
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                ACE_SOCK_NETLINK,
                                Net_AsynchNetlinkSocketHandler>::close ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::close"));

  int result = -1;

  // step1: shutdown operations
  ACE_HANDLE handle = inherited::handle ();
  // *NOTE*: may 'delete this'
  result = handle_close (handle,
                         ACE_Event_Handler::ALL_EVENTS_MASK);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_StreamAsynchTCPSocketBase_T::handle_close(): \"%m\", continuing\n")));

  //  step2: release the socket handle
  if (handle != ACE_INVALID_HANDLE)
  {
    result = ACE_OS::closesocket (handle);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::closesocket(%d): \"%m\", continuing\n"),
                  handle));
//    inherited::handle (ACE_INVALID_HANDLE);
  } // end IF
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType>
void
Net_StreamAsynchUDPSocketBase_T<AddressType,
                                SocketConfigurationType,
                                ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                StatisticContainerType,
                                StreamType,
                                ACE_SOCK_NETLINK,
                                Net_AsynchNetlinkSocketHandler>::handle_read_dgram (const ACE_Asynch_Read_Dgram::Result& result_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchUDPSocketBase_T::handle_read_dgram"));

  int result = -1;
  unsigned long error = 0;

  // sanity check
  result = result_in.success ();
  if (result == 0)
  {
    // connection closed/reset ? --> not an error
    error = result_in.error ();
    if ((error != ECONNRESET) &&
        (error != EPIPE)      &&
        (error != EBADF)      && // local close (), happens on Linux
        (error != 64)) // *TODO*: EHOSTDOWN (- 10000), happens on Win32
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to read from input stream (%d): %u --> \"%s\", aborting\n"),
                  result_in.handle (),
                  error, ACE_TEXT (ACE_OS::strerror (error))));
  } // end IF

  switch (result_in.bytes_transferred ())
  {
    case -1:
    {
      // connection closed/reset (by peer) ? --> not an error
      error = result_in.error ();
      if ((error != ECONNRESET) &&
          (error != EPIPE)      &&
          (error != EBADF)      && // local close (), happens on Linux
          (error != 64)) // *TODO*: EHOSTDOWN (- 10000), happens on Win32
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to read from input stream (%d): %u --> \"%s\", aborting\n"),
                    result_in.handle (),
                    error, ACE_TEXT (ACE_OS::strerror (error))));

      break;
    }
    // *** GOOD CASES ***
    case 0:
    {
//       ACE_DEBUG ((LM_DEBUG,
//                   ACE_TEXT ("[%u]: socket was closed by the peer...\n"),
//                   handle_));

      break;
    }
    default:
    {
//       ACE_DEBUG ((LM_DEBUG,
//                   ACE_TEXT ("[%d]: received %u bytes...\n"),
//                   result_in.handle (),
//                   result_in.bytes_transferred ()));

      // push the buffer onto our stream for processing
      result = stream_.put (result_in.message_block (), NULL);
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Stream::put(): \"%m\", aborting\n")));
        break;
      } // end IF

      // start next read
      inherited::initiate_read_dgram ();

      return;
    }
  } // end SWITCH

  // clean up
  result_in.message_block ()->release ();

  result = handle_close (inherited::handle (),
                         ACE_Event_Handler::ALL_EVENTS_MASK);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Svc_Handler::handle_close(): \"%m\", continuing\n")));
}
#endif
