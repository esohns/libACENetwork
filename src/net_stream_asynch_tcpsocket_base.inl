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

#include "net_macros.h"

template <typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename ITransportLayerType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
Net_StreamAsynchTCPSocketBase_T<ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                ITransportLayerType,
                                StatisticContainerType,
                                StreamType,
                                SocketHandlerType>::Net_StreamAsynchTCPSocketBase_T (ICONNECTION_MANAGER_T* interfaceHandle_in,
                                                                                     unsigned int statisticCollectionInterval_in)
 : inherited2 (interfaceHandle_in,
               statisticCollectionInterval_in)
 //, configuration_ (NULL)
// , stream_ ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchTCPSocketBase_T::Net_StreamAsynchTCPSocketBase_T"));

}

template <typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename ITransportLayerType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
Net_StreamAsynchTCPSocketBase_T<ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                ITransportLayerType,
                                StatisticContainerType,
                                StreamType,
                                SocketHandlerType>::~Net_StreamAsynchTCPSocketBase_T ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchTCPSocketBase_T::~Net_StreamAsynchTCPSocketBase_T"));

  // step1: remove enqueued module (if any)
  if (inherited2::configuration_.streamConfiguration.module)
  {
    if (stream_.find (inherited2::configuration_.streamConfiguration.module->name ()))
      if (stream_.remove (inherited2::configuration_.streamConfiguration.module->name (),
                          ACE_Module_Base::M_DELETE_NONE) == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Stream::remove(\"%s\"): \"%m\", continuing\n"),
                    inherited2::configuration_.streamConfiguration.module->name ()));

    if (inherited2::configuration_.streamConfiguration.deleteModule)
      delete inherited2::configuration_.streamConfiguration.module;
  } // end IF
}

template <typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename ITransportLayerType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
void
Net_StreamAsynchTCPSocketBase_T<ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                ITransportLayerType,
                                StatisticContainerType,
                                StreamType,
                                SocketHandlerType>::open (ACE_HANDLE handle_in,
                                                          ACE_Message_Block& messageBlock_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchTCPSocketBase_T::open"));

  // step1: tweak socket, init I/O
  inherited::open (handle_in, messageBlock_in);

  // step2: init/start stream
  // step2a: connect stream head message queue with a notification pipe/queue ?
  if (!inherited2::configuration_.streamConfiguration.useThreadPerConnection)
    inherited2::configuration_.streamConfiguration.notificationStrategy = this;
  // step2b: init final module (if any)
  if (inherited2::configuration_.streamConfiguration.module)
  {
    Stream_IModule_t* imodule_handle = NULL;
    // need a downcast...
    imodule_handle =
      dynamic_cast<Stream_IModule_t*> (inherited2::configuration_.streamConfiguration.module);
    if (!imodule_handle)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: dynamic_cast<Stream_IModule> failed, aborting\n"),
                  ACE_TEXT (inherited2::configuration_.streamConfiguration.module->name ())));

      // clean up
      handle_close (handle_in,
                    ACE_Event_Handler::ALL_EVENTS_MASK);

      return;
    } // end IF
    Common_Module_t* clone = NULL;
    try
    {
      clone = imodule_handle->clone ();
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: caught exception in Stream_IModule::clone(), aborting\n"),
                  ACE_TEXT (inherited2::configuration_.streamConfiguration.module->name ())));

      // clean up
      handle_close (handle_in,
                    ACE_Event_Handler::ALL_EVENTS_MASK);

      return;
    }
    if (!clone)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to Stream_IModule::clone(), aborting\n"),
                  ACE_TEXT (inherited2::configuration_.streamConfiguration.module->name ())));

      // clean up
      handle_close (handle_in,
                    ACE_Event_Handler::ALL_EVENTS_MASK);

      return;
    }
    inherited2::configuration_.streamConfiguration.module = clone;
    inherited2::configuration_.streamConfiguration.deleteModule = true;
  } // end IF

  unsigned int session_id = 0;
#if defined (_MSC_VER)
  session_id =
    reinterpret_cast<unsigned int> (handle_in); // (== socket handle)
#else
  session_id =
    static_cast<unsigned int> (handle_in); // (== socket handle)
#endif
  // *TODO*: this clearly is a design glitch
  if (!stream_.initialize (session_id,
                           inherited2::configuration_.streamConfiguration,
                           inherited2::configuration_.protocolConfiguration,
                           inherited2::configuration_.streamSessionData))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize processing stream, aborting\n")));

    // clean up
    handle_close (handle_in,
                  ACE_Event_Handler::ALL_EVENTS_MASK);

    return;
  } // end IF
  //stream_.dump_state ();
  // *NOTE*: as soon as this returns, data starts arriving at handle_output()[/msg_queue()]
  stream_.start ();
  if (!stream_.isRunning ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start processing stream, aborting\n")));

    // clean up
    handle_close (handle_in,
                  ACE_Event_Handler::ALL_EVENTS_MASK);

    return;
  } // end IF

  // step3: start reading (need to pass any data ?)
  if (messageBlock_in.length () == 0)
   inherited::initiate_read_stream ();
  else
  {
    ACE_Message_Block* duplicate = messageBlock_in.duplicate ();
    if (!duplicate)
    {
      ACE_ERROR ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Message_Block::duplicate(): \"%m\", aborting\n")));

      // clean up
      handle_close (handle_in,
                    ACE_Event_Handler::ALL_EVENTS_MASK);

      return;
    } // end IF
    // fake a result to emulate regular behavior...
    ACE_Asynch_Read_Stream_Result_Impl* fake_result =
      inherited::proactor ()->create_asynch_read_stream_result (inherited::proxy (), // handler proxy
                                                                handle_in,           // socket handle
                                                                *duplicate,          // buffer
                                                                duplicate->size (),  // (max) bytes to read
                                                                NULL,                // ACT
                                                                ACE_INVALID_HANDLE,  // event
                                                                0,                   // priority
                                                                0);                  // signal number
    if (!fake_result)
    {
      ACE_ERROR ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Proactor::create_asynch_read_stream_result: \"%m\", aborting\n")));

      // clean up
      handle_close (handle_in,
                    ACE_Event_Handler::ALL_EVENTS_MASK);

      return;
    } // end IF
    size_t bytes_transferred = duplicate->length ();
    // <complete> for Accept would have already moved the <wr_ptr>
    // forward; update it to the beginning position
    duplicate->wr_ptr (duplicate->wr_ptr () - bytes_transferred);
    // invoke ourselves (see handle_read_stream)
    fake_result->complete (duplicate->length (), // bytes read
                           1,                    // success
                           NULL,                 // ACT
                           0);                   // error

    // clean up
    delete fake_result;
  } // end ELSE
}

template <typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename ITransportLayerType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
int
Net_StreamAsynchTCPSocketBase_T<ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                ITransportLayerType,
                                StatisticContainerType,
                                StreamType,
                                SocketHandlerType>::handle_output (ACE_HANDLE handle_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchTCPSocketBase_T::handle_output"));

  ACE_UNUSED_ARG (handle_in);

  ACE_Message_Block* message_block = NULL;
//  if (buffer_ == NULL)
//  {
  // send next data chunk from the stream...
  // *IMPORTANT NOTE*: this should NEVER block, as available outbound data has
  // been notified
//  if (stream_.get (buffer_, &ACE_Time_Value::zero) == -1)
  if (stream_.get (message_block,
                   &const_cast<ACE_Time_Value&> (ACE_Time_Value::zero)) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::get(): \"%m\", aborting\n")));

    return -1;
  } // end IF
//  } // end IF

  // start (asynch) write
  // *NOTE*: this is a fire-and-forget API for message_block...
//  if (inherited::outputStream_.write (*buffer_,               // data
  if (inherited::outputStream_.write (*message_block,         // data
                                      message_block->size (), // bytes to write
                                      NULL,                   // ACT
                                      0,                      // priority
                                      ACE_SIGRTMIN) == -1)    // signal number
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Asynch_Write_Stream::write(%u): \"%m\", aborting\n"),
//               buffer_->size ()));
               message_block->size ()));

    // clean up
    message_block->release ();

    return -1;
  } // end IF

  return 0;
}

template <typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename ITransportLayerType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
int
Net_StreamAsynchTCPSocketBase_T<ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                ITransportLayerType,
                                StatisticContainerType,
                                StreamType,
                                SocketHandlerType>::handle_close (ACE_HANDLE handle_in,
                                                                  ACE_Reactor_Mask mask_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchTCPSocketBase_T::handle_close"));

  // step1: wait for all workers within the stream (if any)
  if (stream_.isRunning ())
  {
    stream_.stop ();
    stream_.waitForCompletion ();
  } // end IF

  // step2: purge any pending notifications ?
  // *WARNING: do this here, while still holding on to the current write buffer
  if (!inherited2::configuration_.streamConfiguration.useThreadPerConnection)
  {
    Stream_Iterator_t iterator (stream_);
    const Common_Module_t* module = NULL;
    if (iterator.next (module) == 0)
    {
      ACE_ASSERT (module);
      Common_Task_t* task = const_cast<Common_Module_t*> (module)->writer ();
      ACE_ASSERT (task);
      if (task->msg_queue ()->flush () == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_MessageQueue::flush(): \"%m\", continuing\n")));
    } // end IF
  } // end IF

  // step3: invoke base class maintenance
  int result = inherited::handle_close (handle_in,
                                        mask_in);

//  // step4: deregister ?
//  if (inherited::manager_)
//  {
//    if (inherited::isRegistered_)
//    { // (try to) deregister with the connection manager...
//      try
//      {
//        inherited::manager_->deregisterConnection (this);
//      }
//      catch (...)
//      {
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("caught exception in Net_IConnectionManager::deregisterConnection(), continuing\n")));
//      }
//    } // end IF
//  } // end IF
//  else
    //inherited::decrease ();

  return result;
}

template <typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename ITransportLayerType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
void
Net_StreamAsynchTCPSocketBase_T<ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                ITransportLayerType,
                                StatisticContainerType,
                                StreamType,
                                SocketHandlerType>::act (const void* act_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchTCPSocketBase_T::act"));

  //inherited2::configuration_ = *reinterpret_cast<ConfigurationType*> (const_cast<void*> (act_in));
}

template <typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename ITransportLayerType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
bool
Net_StreamAsynchTCPSocketBase_T<ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                ITransportLayerType,
                                StatisticContainerType,
                                StreamType,
                                SocketHandlerType>::collect (StatisticContainerType& data_out)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchTCPSocketBase_T::collect"));

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

template <typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename ITransportLayerType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
void
Net_StreamAsynchTCPSocketBase_T<ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                ITransportLayerType,
                                StatisticContainerType,
                                StreamType,
                                SocketHandlerType>::report () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchTCPSocketBase_T::report"));

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

template <typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename ITransportLayerType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
void
Net_StreamAsynchTCPSocketBase_T<ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                ITransportLayerType,
                                StatisticContainerType,
                                StreamType,
                                SocketHandlerType>::info (ACE_HANDLE& handle_out,
                                                          ACE_INET_Addr& localSAP_out,
                                                          ACE_INET_Addr& remoteSAP_out) const
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchTCPSocketBase_T::info"));

  handle_out = inherited::handle ();
  localSAP_out = inherited::localSAP_;
  remoteSAP_out = inherited::remoteSAP_;
}

template <typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename ITransportLayerType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
unsigned int
Net_StreamAsynchTCPSocketBase_T<ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                ITransportLayerType,
                                StatisticContainerType,
                                StreamType,
                                SocketHandlerType>::id () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchTCPSocketBase_T::id"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  return *static_cast<unsigned int*> (inherited::handle ());
#else
  return static_cast<unsigned int> (inherited::handle ());
#endif
}

template <typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename ITransportLayerType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
void
Net_StreamAsynchTCPSocketBase_T<ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                ITransportLayerType,
                                StatisticContainerType,
                                StreamType,
                                SocketHandlerType>::dump_state () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchTCPSocketBase_T::dump_state"));

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

template <typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename ITransportLayerType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
void
Net_StreamAsynchTCPSocketBase_T<ConfigurationType,
                                UserDataType,
                                SessionDataType,
                                ITransportLayerType,
                                StatisticContainerType,
                                StreamType,
                                SocketHandlerType>::handle_read_stream (const ACE_Asynch_Read_Stream::Result& result)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamAsynchTCPSocketBase_T::handle_read_stream"));

  // sanity check
  if (result.success () == 0)
  {
    // connection reset (by peer) ? --> not an error
    if ((result.error () != ECONNRESET) &&
        (result.error () != EPIPE))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to read from input stream (%d): \"%s\", continuing\n"),
                  result.handle (),
                  ACE_TEXT (ACE_OS::strerror (result.error ()))));
  } // end IF

  switch (result.bytes_transferred ())
  {
    case -1:
    {
      // connection reset (by peer) ? --> not an error
      if ((result.error () != ECONNRESET) &&
          (result.error () != EPIPE))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to read from input stream (%d): \"%s\", aborting\n"),
                    result.handle (),
                    ACE_TEXT (ACE_OS::strerror (result.error ()))));

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
//                   result.handle (),
//                   result.bytes_transferred ()));

      // push the buffer onto our stream for processing
      if (stream_.put (&result.message_block (), NULL) == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Stream::put(): \"%m\", aborting\n")));

        break;
      } // end IF

      // start next read
      inherited::initiate_read_stream ();

      return;
    }
  } // end SWITCH

  // clean up
  result.message_block ().release ();
  handle_close (inherited::handle (),
                ACE_Event_Handler::ALL_EVENTS_MASK);
}
