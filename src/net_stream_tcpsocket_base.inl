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
#include "ace/OS.h"
#include "ace/Svc_Handler.h"

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
          typename SocketHandlerType>
Net_StreamTCPSocketBase_T<AddressType,
                          SocketConfigurationType,
                          ConfigurationType,
                          UserDataType,
                          SessionDataType,
                          StatisticContainerType,
                          StreamType,
                          SocketHandlerType>::Net_StreamTCPSocketBase_T (ICONNECTION_MANAGER_T* interfaceHandle_in,
                                                                         unsigned int statisticsCollectionInterval_in)
 : inherited ()
 , inherited2 (interfaceHandle_in,
               statisticsCollectionInterval_in)
 , currentReadBuffer_ (NULL)
 , currentWriteBuffer_ (NULL)
 , sendLock_ ()
 , stream_ ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::Net_StreamTCPSocketBase_T"));

}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
Net_StreamTCPSocketBase_T<AddressType,
                          SocketConfigurationType,
                          ConfigurationType,
                          UserDataType,
                          SessionDataType,
                          StatisticContainerType,
                          StreamType,
                          SocketHandlerType>::~Net_StreamTCPSocketBase_T ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::~Net_StreamTCPSocketBase_T"));

  if (inherited2::configuration_.streamConfiguration.module)
  {
    Stream_Module_t* module_p =
      stream_.find (inherited2::configuration_.streamConfiguration.module->name ());
    if (module_p)
    {
      int result =
        stream_.remove (inherited2::configuration_.streamConfiguration.module->name (),
                        ACE_Module_Base::M_DELETE_NONE);
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Stream::remove(\"%s\"): \"%m\", continuing\n"),
                    ACE_TEXT (inherited2::configuration_.streamConfiguration.module->name ())));
    } // end IF

    if (inherited2::configuration_.streamConfiguration.deleteModule)
      delete inherited2::configuration_.streamConfiguration.module;
  } // end IF

  if (currentReadBuffer_)
    currentReadBuffer_->release ();
  if (currentWriteBuffer_)
    currentWriteBuffer_->release ();
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
int
Net_StreamTCPSocketBase_T<AddressType,
                          SocketConfigurationType,
                          ConfigurationType,
                          UserDataType,
                          SessionDataType,
                          StatisticContainerType,
                          StreamType,
                          SocketHandlerType>::open (void* arg_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::open"));

  ACE_UNUSED_ARG (arg_in);

  // step0: initialize this
  // *IMPORTANT NOTE*: enable the reference counting policy, as this will
  // be registered with the reactor several times (1x READ_MASK, nx
  // WRITE_MASK); therefore several threads MAY be dispatching notifications
  // (yes, even concurrently; lock_ enforces the proper sequence order, see
  // handle_output()) on the SAME handler. When the socket closes, the event
  // handler should thus not be destroyed() immediately, but simply purge any
  // pending notifications (see handle_close()) and de-register; after the
  // last active notification has been dispatched, it will be safely deleted
  inherited::reference_counting_policy ().value (ACE_Event_Handler::Reference_Counting_Policy::ENABLED);
  // *IMPORTANT NOTE*: due to reference counting, the
  // ACE_Svc_Handle::shutdown() method will crash, as it references a
  // connection recycler AFTER removing the connection from the reactor (which
  // releases a reference). In the case that "this" is the final reference,
  // this leads to a crash. (see code)
  // --> avoid invoking ACE_Svc_Handle::shutdown()
  // --> this means that "manual" cleanup is necessary (see handle_close())
  inherited::closing_ = true;

  // step1: initialize/start stream
  // step1a: connect stream head message queue with the reactor notification
  // pipe ?
  if (!inherited2::configuration_.streamConfiguration.useThreadPerConnection)
  {
    // *NOTE*: must use 'this' (inherited2:: does not work here for some
    //         strange reason)...
    inherited2::configuration_.streamConfiguration.notificationStrategy =
      &this->notificationStrategy_;
  } // end IF
  // step1b: initialize final module (if any)
  if (inherited2::configuration_.streamConfiguration.module)
  {
    Stream_IModule_t* imodule_p = NULL;
    // need a downcast...
    imodule_p =
        dynamic_cast<Stream_IModule_t*> (inherited2::configuration_.streamConfiguration.module);
    if (!imodule_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: dynamic_cast<Stream_IModule_t*> failed, aborting\n"),
                  ACE_TEXT (inherited2::configuration_.streamConfiguration.module->name ())));
      return -1;
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
                  ACE_TEXT (inherited2::configuration_.streamConfiguration.module->name ())));
      return -1;
    }
    if (!clone_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to Stream_IModule::clone(), aborting\n"),
                  ACE_TEXT (inherited2::configuration_.streamConfiguration.module->name ())));
      return -1;
    }
    inherited2::configuration_.streamConfiguration.module = clone_p;
    inherited2::configuration_.streamConfiguration.deleteModule = true;
  } // end IF
  // step1c: initialize stream
  unsigned int session_id = 0;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  session_id =
    reinterpret_cast<unsigned int> (inherited::get_handle ()); // (== socket handle)
#else
  session_id =
    static_cast<unsigned int> (inherited::get_handle ()); // (== socket handle)
#endif
  // *TODO*: this clearly is a design glitch
  if (!stream_.initialize (session_id,
                           inherited2::configuration_.streamConfiguration,
                           inherited2::configuration_.protocolConfiguration,
                           inherited2::configuration_.streamSessionData))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize processing stream, aborting\n")));
    return -1;
  } // end IF
  //stream_.dump_state ();
  // *NOTE*: as soon as this returns, data starts arriving at
  // handle_output()/msg_queue()
  stream_.start ();
  if (!stream_.isRunning ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start processing stream, aborting\n")));
    return -1;
  } // end IF

  // step2: register with the connection manager (if any)
  // *IMPORTANT NOTE*: register with the connection manager FIRST, otherwise
  //                   a race condition might occur when using multi-threaded
  //                   proactors/reactors
  if (!inherited2::registerc ())
  {
    // *NOTE*: perhaps max# connections has been reached
    //ACE_DEBUG ((LM_ERROR,
    //            ACE_TEXT ("failed to Net_ConnectionBase_T::registerc(), aborting\n")));
    return -1;
  } // end IF

  // step3: tweak socket, register I/O handle with the reactor, ...
  // *NOTE*: as soon as this returns, data starts arriving at handle_input()
  int result = -1;
  result = inherited::open (&inherited2::configuration_.socketConfiguration);
  if (result == -1)
  {
    // *NOTE*: this can happen when the connection handle is still registered
    //         with the reactor (i.e. the reactor is still processing events on
    //         a file descriptor that has been closed and is now being reused by
    //         the system)
    // *NOTE*: more likely, this happened because the (select) reactor is out of
    //         "free" (read) slots
    int error = ACE_OS::last_error ();
    if (error);

    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SocketHandlerType::open(): \"%m\", aborting\n")));
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
          typename SocketHandlerType>
int
Net_StreamTCPSocketBase_T<AddressType,
                          SocketConfigurationType,
                          ConfigurationType,
                          UserDataType,
                          SessionDataType,
                          StatisticContainerType,
                          StreamType,
                          SocketHandlerType>::close (u_long arg_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::close"));
  // [*NOTE*: hereby we override the default behavior of a ACE_Svc_Handler,
  // which would call handle_close() AGAIN]

  // *NOTE*: this method will be invoked
  // - by any worker after returning from svc()
  //    --> in this case, this should be a NOP (triggered from handle_close(),
  //        which was invoked by the reactor) - we override the default
  //        behavior of a ACE_Svc_Handler, which would call handle_close() AGAIN
  // - by the connector/acceptor when open() fails (e.g. too many connections !)
  //    --> shutdown

  switch (arg_in)
  {
    // called by:
    // - any worker from ACE_Task_Base on clean-up
    // - acceptor/connector if there are too many connections (i.e. open()
    //   returned -1)
    case NORMAL_CLOSE_OPERATION:
    {
      // check specifically for the first case...
      if (ACE_OS::thr_equal (ACE_Thread::self (),
                             inherited::last_thread ()))
      {
//       if (inherited::module ())
//         ACE_DEBUG ((LM_DEBUG,
//                     ACE_TEXT ("\"%s\" worker thread (ID: %t) leaving...\n"),
//                     ACE_TEXT (inherited::name ())));
//       else
//         ACE_DEBUG ((LM_DEBUG,
//                     ACE_TEXT ("worker thread (ID: %t) leaving...\n")));

        break;
      } // end IF

      // too many connections: invoke inherited default behavior
      // --> simply fall through to the next case
    }
    // called by external (e.g. reactor) thread wanting to close the connection
    // (e.g. too many connections)
    // *NOTE*: this eventually calls handle_close() (see below)
    case CLOSE_DURING_NEW_CONNECTION:
    {
      ACE_HANDLE handle = inherited::get_handle ();

      // step1: release any connection resources
      int result = -1;
      result = inherited::close (CLOSE_DURING_NEW_CONNECTION);
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to SocketHandlerType::close(): \"%m\", aborting\n")));
        return -1;
      } // end IF

      //  step2: release the socket handle
      // *IMPORTANT NOTE*: this wakes up any reactor threads that may still be
      //                   using the handle. Makes sure this connection is
      //                   delete(d) ASAP
      if (handle != ACE_INVALID_HANDLE)
      {
        result = ACE_OS::closesocket (handle);
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_OS::closesocket(%d): \"%m\", continuing\n"),
                      handle));
        //    inherited::handle (ACE_INVALID_HANDLE);
      } // end IF

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid argument (was: %u), aborting\n"),
                  arg_in));
      return -1;
    }
  } // end SWITCH

  return 0;
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
int
Net_StreamTCPSocketBase_T<AddressType,
                          SocketConfigurationType,
                          ConfigurationType,
                          UserDataType,
                          SessionDataType,
                          StatisticContainerType,
                          StreamType,
                          SocketHandlerType>::handle_input (ACE_HANDLE handle_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::handle_input"));

  int result = -1;

  ACE_UNUSED_ARG (handle_in);

  // sanity check
  ACE_ASSERT (currentReadBuffer_ == NULL);

  // read some data from the socket
  currentReadBuffer_ =
      allocateMessage (inherited2::configuration_.streamConfiguration.bufferSize);
  if (!currentReadBuffer_)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocateMessage(%u), aborting\n"),
                inherited2::configuration_.streamConfiguration.bufferSize));
    return -1;
  } // end IF

  // read some data from the socket...
  size_t bytes_received =
      inherited::peer_.recv (currentReadBuffer_->wr_ptr (), // buffer
                             currentReadBuffer_->size (),   // #bytes to read
                             0);                            // flags
  switch (bytes_received)
  {
    case -1:
    {
      // *IMPORTANT NOTE*: a number of issues can occur here:
      // - connection reset by peer
      // - connection abort()ed locally
      int error = ACE_OS::last_error ();
      if ((error != ECONNRESET) &&
          (error != EPIPE)      && // <-- connection reset by peer
          // -------------------------------------------------------------------
          (error != EBADF)      &&
          (error != ENOTSOCK)   &&
          (error != ECONNABORTED)) // <-- connection abort()ed locally
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_SOCK_Stream::recv(): \"%m\", returning\n")));

      // clean up
      currentReadBuffer_->release ();
      currentReadBuffer_ = NULL;

      return -1;
    }
    // *** GOOD CASES ***
    case 0:
    {
//       ACE_DEBUG ((LM_DEBUG,
//                   ACE_TEXT ("[%u]: socket was closed by the peer...\n"),
//                   handle_in));

      // clean up
      currentReadBuffer_->release ();
      currentReadBuffer_ = NULL;

      return -1;
    }
    default:
    {
//       ACE_DEBUG ((LM_DEBUG,
//                   ACE_TEXT ("[%u]: received %u bytes...\n"),
//                   handle_in,
//                   bytes_received));

      // adjust write pointer
      currentReadBuffer_->wr_ptr (bytes_received);

      break;
    }
  } // end SWITCH

  // push the buffer onto our stream for processing
  // *NOTE*: the stream assumes ownership of the buffer
  result = stream_.put (currentReadBuffer_);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::put(): \"%m\", aborting\n")));

    // clean up
    currentReadBuffer_->release ();
    currentReadBuffer_ = NULL;

    return -1;
  } // end IF
  currentReadBuffer_ = NULL;

  return result;
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
int
Net_StreamTCPSocketBase_T<AddressType,
                          SocketConfigurationType,
                          ConfigurationType,
                          UserDataType,
                          SessionDataType,
                          StatisticContainerType,
                          StreamType,
                          SocketHandlerType>::handle_output (ACE_HANDLE handle_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::handle_output"));

  ACE_UNUSED_ARG (handle_in);

  int result = -1;

  // *IMPORTANT NOTE*: in a threaded environment, workers MAY be
  // dispatching the reactor notification queue concurrently (most notably,
  // ACE_TP_Reactor) --> enforce proper serialization
  if (inherited2::configuration_.streamConfiguration.serializeOutput)
    sendLock_.acquire ();

  if (currentWriteBuffer_ == NULL)
  {
    // send next data chunk from the stream...
    // *IMPORTANT NOTE*: should NEVER block, as available outbound data has
    // been notified to the reactor
    if (!inherited2::configuration_.streamConfiguration.useThreadPerConnection)
      result =
          stream_.get (currentWriteBuffer_,
                       const_cast<ACE_Time_Value*> (&ACE_Time_Value::zero));
    else
      result =
          inherited::getq (currentWriteBuffer_,
                           const_cast<ACE_Time_Value*> (&ACE_Time_Value::zero));
    if (result == -1)
    {
      // *IMPORTANT NOTE*: a number of issues can occur here:
      // - connection has been closed in the meantime
      // - queue has been deactivated
      int error = ACE_OS::last_error ();
      if ((error != EAGAIN) ||  // <-- connection has been closed
          (error != ESHUTDOWN)) // <-- queue has been deactivated
        ACE_DEBUG ((LM_ERROR,
                    (inherited2::configuration_.streamConfiguration.useThreadPerConnection ? ACE_TEXT ("failed to ACE_Task::getq(): \"%m\", aborting\n")
                                                                                           : ACE_TEXT ("failed to ACE_Stream::get(): \"%m\", aborting\n"))));

      // clean up
      if (inherited2::configuration_.streamConfiguration.serializeOutput)
        sendLock_.release ();

      return -1;
    } // end IF
  } // end IF
  ACE_ASSERT (currentWriteBuffer_);

  // finished ?
  if (inherited2::configuration_.streamConfiguration.useThreadPerConnection &&
      currentWriteBuffer_->msg_type () == ACE_Message_Block::MB_STOP)
  {
    currentWriteBuffer_->release ();
    currentWriteBuffer_ = NULL;

//       ACE_DEBUG ((LM_DEBUG,
//                   ACE_TEXT ("[%u]: finished sending...\n"),
//                   peer_.get_handle ()));

    // clean up
    if (inherited2::configuration_.streamConfiguration.serializeOutput)
      sendLock_.release ();

    return -1;
  } // end IF

  // put some data into the socket...
  ssize_t bytes_sent =
      inherited::peer_.send (currentWriteBuffer_->rd_ptr (), // buffer
                             currentWriteBuffer_->length (), // #bytes to send
                             0);                             // flags
  switch (bytes_sent)
  {
    case -1:
    {
      // *IMPORTANT NOTE*: a number of issues can occur here:
      // - connection reset by peer
      // - connection abort()ed locally
      int error = ACE_OS::last_error ();
      if ((error != ECONNRESET)   &&
          (error != ECONNABORTED) &&
          (error != EPIPE)        && // <-- connection reset by peer
          // -------------------------------------------------------------------
          (error != ENOTSOCK)     &&
          (error != EBADF))          // <-- connection abort()ed locally
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_SOCK_Stream::send(): \"%m\", aborting\n")));

      // clean up
      currentWriteBuffer_->release ();
      currentWriteBuffer_ = NULL;
      if (inherited2::configuration_.streamConfiguration.serializeOutput)
        sendLock_.release ();

      return -1;
    }
    // *** GOOD CASES ***
    case 0:
    {
//       ACE_DEBUG ((LM_DEBUG,
//                   ACE_TEXT ("[%u]: socket was closed by the peer...\n"),
//                   handle_in));

      // clean up
      currentWriteBuffer_->release ();
      currentWriteBuffer_ = NULL;
      if (inherited2::configuration_.streamConfiguration.serializeOutput)
        sendLock_.release ();

      return -1;
    }
    default:
    {
//       ACE_DEBUG ((LM_DEBUG,
//                   ACE_TEXT ("[%u]: sent %u bytes...\n"),
//                   handle_in,
//                   bytes_sent));

      // finished with this buffer ?
      currentWriteBuffer_->rd_ptr (static_cast<size_t> (bytes_sent));
      if (currentWriteBuffer_->length () > 0)
        break; // there's more data

      // clean up
      currentWriteBuffer_->release ();
      currentWriteBuffer_ = NULL;

      break;
    }
  } // end SWITCH

  // immediately reschedule sending ?
//  if ((currentWriteBuffer_ == NULL) && inherited::msg_queue ()->is_empty ())
//  {
//    if (inherited::reactor ()->cancel_wakeup (this,
//                                              ACE_Event_Handler::WRITE_MASK) == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_Reactor::cancel_wakeup(): \"%m\", continuing\n")));
//  } // end IF
//  else
  if (currentWriteBuffer_)
  {
    // clean up
    if (inherited2::configuration_.streamConfiguration.serializeOutput)
      sendLock_.release ();

    return 1;
  } // end IF
    //if (inherited::reactor ()->schedule_wakeup (this,
    //                                            ACE_Event_Handler::WRITE_MASK) == -1)
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to ACE_Reactor::schedule_wakeup(): \"%m\", continuing\n")));

  // clean up
  if (inherited2::configuration_.streamConfiguration.serializeOutput)
    sendLock_.release ();

  return 0;
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
int
Net_StreamTCPSocketBase_T<AddressType,
                          SocketConfigurationType,
                          ConfigurationType,
                          UserDataType,
                          SessionDataType,
                          StatisticContainerType,
                          StreamType,
                          SocketHandlerType>::handle_close (ACE_HANDLE handle_in,
                                                            ACE_Reactor_Mask mask_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::handle_close"));

  int result = -1;

  switch (mask_in)
  {
    case ACE_Event_Handler::READ_MASK:       // --> socket has been closed (receive failed)
//    case ACE_Event_Handler::ACCEPT_MASK:
    case ACE_Event_Handler::WRITE_MASK:      // --> socket has been closed (send failed) (DevPoll)
    case ACE_Event_Handler::EXCEPT_MASK:     // --> socket has been closed (send failed)
    case ACE_Event_Handler::ALL_EVENTS_MASK: // --> connect failed (e.g. connection refused) /
                                             //     accept failed (e.g. too many connections) /
                                             //     select failed (EBADF see Select_Reactor_T.cpp) /
                                             //     user abort
    {
      // step1: wait for all workers within the stream (if any)
      if (stream_.isRunning ())
      {
        stream_.stop ();
        stream_.waitForCompletion ();
      } // end IF

      // step2: purge any pending (!) notifications ?
      if (!inherited2::configuration_.streamConfiguration.useThreadPerConnection)
      {
        // *IMPORTANT NOTE*: in a multithreaded environment, in particular when
        //                   using a multithreaded reactor, there may still be
        //                   in-flight notifications being dispatched at this
        //                   stage. In that case, do not rely on releasing all
        //                   handler resources "manually", use reference
        //                   counting instead.
        //                   --> this just speeds things up a little.
        ACE_Reactor* reactor_p = inherited::reactor ();
        ACE_ASSERT (reactor_p);
        result =
            reactor_p->purge_pending_notifications (this,
                                                    ACE_Event_Handler::ALL_EVENTS_MASK);
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Reactor::purge_pending_notifications(0x%@, ALL_EVENTS_MASK): \"%m\", continuing\n"),
                      this));
        //else if (result > 0)
        //  ACE_DEBUG ((LM_DEBUG,
        //              ACE_TEXT ("flushed %d outbound messages (handle was: %u)\n"),
        //              result,
        //              handle_in));
      } // end IF

      break;
    }
    default:
      // *PORTABILITY*: this isn't entirely portable...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("handle_close called for unknown reasons (handle: %@, mask: %u) --> check implementation !, continuing\n"),
                  handle_in,
                  mask_in));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("handle_close called for unknown reasons (handle: %d, mask: %u) --> check implementation !, continuing\n"),
                  handle_in,
                  mask_in));
#endif
      break;
  } // end SWITCH

  // step3: invoke base-class maintenance
  bool deregister = inherited2::isRegistered_;
  // *IMPORTANT NOTE*: use get_handle() here to pass proper handle
  //                   otherwise, this fails for the usecase "accept failed"
  //                   (see above)
  ACE_HANDLE handle = inherited::get_handle ();
  // *IMPORTANT NOTE*: may delete 'this'
  result = inherited::handle_close (handle,
                                    mask_in);
  if (result == -1)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to SocketHandlerType::handle_close(%d, %d): \"%m\", continuing\n"),
                handle, mask_in));

  // step4: deregister with the connection manager (if any)
  if (deregister)
    inherited2::deregister ();

  // step5: release a reference (and let the reactor manage the lifecycle) ?
  // *IMPORTANT NOTE*: may 'delete this'
  //inherited2::decrease ();
  // *IMPORTANT NOTE*: when the connection is closed asynchronously (i.e. user
  //                   abort), the reactor may currently be reading/writing
  //                   the socket. This operation will fail, and the reactor
  //                   will invoke this method again (mask_in == EXCEPT_MASK)
  //                   --> do not decrease the reference count in that case
  //if (mask_in != ACE_Event_Handler::EXCEPT_MASK)
  //  decrease ();

  return result;
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
unsigned int
Net_StreamTCPSocketBase_T<AddressType,
                          SocketConfigurationType,
                          ConfigurationType,
                          UserDataType,
                          SessionDataType,
                          StatisticContainerType,
                          StreamType,
                          SocketHandlerType>::increase ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::increase"));

  //ACE_Event_Handler::Reference_Count count = inherited2::increase ();
  ACE_Event_Handler::Reference_Count count = inherited::add_reference ();
  //  ACE_DEBUG ((LM_DEBUG,
  //              ACE_TEXT ("%@/%u: added, count: %d\n"), this, id (), count));

  return count;
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
unsigned int
Net_StreamTCPSocketBase_T<AddressType,
                          SocketConfigurationType,
                          ConfigurationType,
                          UserDataType,
                          SessionDataType,
                          StatisticContainerType,
                          StreamType,
                          SocketHandlerType>::decrease ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::decrease"));

  //  void* this_ptr = this;
  //  unsigned int session_id = id ();

  //ACE_Event_Handler::Reference_Count count = inherited2::decrease ();
  ACE_Event_Handler::Reference_Count count = inherited::remove_reference ();
  //  if (count)
  //    ACE_DEBUG ((LM_DEBUG,
  //                ACE_TEXT ("%@/%u: removed, count: %d\n"), this_ptr, session_id, count));
  //  else
  //    ACE_DEBUG ((LM_DEBUG,
  //                ACE_TEXT ("%@/%u: removed, count: %d --> deleted !\n"), this_ptr, session_id, count));

  return count;
}

//template <typename AddressType,
//          typename SocketConfigurationType,
//          typename ConfigurationType,
//          typename UserDataType,
//          typename SessionDataType,
//          typename StatisticContainerType,
//          typename StreamType,
//          typename SocketHandlerType>
//ACE_Event_Handler::Reference_Count
//Net_StreamTCPSocketBase_T<AddressType,
//                          SocketConfigurationType,
//                          ConfigurationType,
//                          UserDataType,
//                          SessionDataType,
//                          StatisticContainerType,
//                          StreamType,
//                          SocketHandlerType>::add_reference (void)
//{
//  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::add_reference"));
//
//  //ACE_Event_Handler::Reference_Count count = inherited2::increase ();
//  ACE_Event_Handler::Reference_Count count = inherited::add_reference ();
////  ACE_DEBUG ((LM_DEBUG,
////              ACE_TEXT ("%@/%u: added, count: %d\n"), this, id (), count));
//
//  return count;
//}
//
//template <typename AddressType,
//          typename SocketConfigurationType,
//          typename ConfigurationType,
//          typename UserDataType,
//          typename SessionDataType,
//          typename StatisticContainerType,
//          typename StreamType,
//          typename SocketHandlerType>
//ACE_Event_Handler::Reference_Count
//Net_StreamTCPSocketBase_T<AddressType,
//                          SocketConfigurationType,
//                          ConfigurationType,
//                          UserDataType,
//                          SessionDataType,
//                          StatisticContainerType,
//                          StreamType,
//                          SocketHandlerType>::remove_reference (void)
//{
//  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::remove_reference"));
//
////  void* this_ptr = this;
////  unsigned int session_id = id ();
//
//  //ACE_Event_Handler::Reference_Count count = inherited2::decrease ();
//  ACE_Event_Handler::Reference_Count count = inherited::remove_reference ();
////  if (count)
////    ACE_DEBUG ((LM_DEBUG,
////                ACE_TEXT ("%@/%u: removed, count: %d\n"), this_ptr, session_id, count));
////  else
////    ACE_DEBUG ((LM_DEBUG,
////                ACE_TEXT ("%@/%u: removed, count: %d --> deleted !\n"), this_ptr, session_id, count));
//
//  return count;
//}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
bool
Net_StreamTCPSocketBase_T<AddressType,
                          SocketConfigurationType,
                          ConfigurationType,
                          UserDataType,
                          SessionDataType,
                          StatisticContainerType,
                          StreamType,
                          SocketHandlerType>::collect (StatisticContainerType& data_out)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::collect"));

  try
  {
    return stream_.collect (data_out);
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IStatistic_T::collect(), aborting\n")));
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
          typename SocketHandlerType>
void
Net_StreamTCPSocketBase_T<AddressType,
                          SocketConfigurationType,
                          ConfigurationType,
                          UserDataType,
                          SessionDataType,
                          StatisticContainerType,
                          StreamType,
                          SocketHandlerType>::report () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::report"));

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
          typename SocketHandlerType>
void
Net_StreamTCPSocketBase_T<AddressType,
                          SocketConfigurationType,
                          ConfigurationType,
                          UserDataType,
                          SessionDataType,
                          StatisticContainerType,
                          StreamType,
                          SocketHandlerType>::info (ACE_HANDLE& handle_out,
                                                    AddressType& localSAP_out,
                                                    AddressType& remoteSAP_out) const
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::info"));

  int result = -1;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  handle_out = inherited::get_handle ();
#else
  handle_out = inherited::get_handle ();
#endif

  result = inherited::peer_.get_local_addr (localSAP_out);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_SOCK::get_local_addr(): \"%m\", continuing\n")));
  result = inherited::peer_.get_remote_addr (remoteSAP_out);
  // *NOTE*: peer may have disconnected already --> not an error
  if (result == -1)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ACE_SOCK::get_remote_addr(): \"%m\", continuing\n")));
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
unsigned int
Net_StreamTCPSocketBase_T<AddressType,
                          SocketConfigurationType,
                          ConfigurationType,
                          UserDataType,
                          SessionDataType,
                          StatisticContainerType,
                          StreamType,
                          SocketHandlerType>::id () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::id"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  return reinterpret_cast<unsigned int> (inherited::get_handle ());
#else
  return static_cast<unsigned int> (inherited::get_handle ());
#endif
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketHandlerType>
void
Net_StreamTCPSocketBase_T<AddressType,
                          SocketConfigurationType,
                          ConfigurationType,
                          UserDataType,
                          SessionDataType,
                          StatisticContainerType,
                          StreamType,
                          SocketHandlerType>::dump_state () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::dump_state"));

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
              ACE_TEXT ("connection [Id: %u [%u]]: \"%s\" <--> \"%s\"\n"),
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
          typename SocketHandlerType>
void
Net_StreamTCPSocketBase_T<AddressType,
                          SocketConfigurationType,
                          ConfigurationType,
                          UserDataType,
                          SessionDataType,
                          StatisticContainerType,
                          StreamType,
                          SocketHandlerType>::close ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::close"));

  int result = -1;

  // step1: shutdown operations
  ACE_HANDLE handle = inherited::get_handle ();
  // *NOTE*: may 'delete this'
  result = handle_close (handle,
                         ACE_Event_Handler::ALL_EVENTS_MASK);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_StreamAsynchTCPSocketBase_T::handle_close(): \"%m\", continuing\n")));

  //  step2: release the socket handle
  // *IMPORTANT NOTE*: wakes up any reactor thread(s) that may be working on the
  //                   handle
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
          typename SocketHandlerType>
ACE_Message_Block*
Net_StreamTCPSocketBase_T<AddressType,
                          SocketConfigurationType,
                          ConfigurationType,
                          UserDataType,
                          SessionDataType,
                          StatisticContainerType,
                          StreamType,
                          SocketHandlerType>::allocateMessage (unsigned int requestedSize_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_StreamTCPSocketBase_T::allocateMessage"));

  // initialize return value(s)
  ACE_Message_Block* message_block_p = NULL;

  //if (inherited::configuration_.messageAllocator)
  if (inherited2::configuration_.streamConfiguration.messageAllocator)
  {
allocate:
    try
    {
      message_block_p =
        static_cast<ACE_Message_Block*> (inherited2::configuration_.streamConfiguration.messageAllocator->malloc (requestedSize_in));
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Stream_IAllocator::malloc(0), aborting\n")));
      return NULL;
    }

    // keep retrying ?
    if (!message_block_p &&
        !inherited2::configuration_.streamConfiguration.messageAllocator->block ())
        goto allocate;
  } // end IF
  else
    ACE_NEW_NORETURN (message_block_p,
                      ACE_Message_Block (requestedSize_in,
                                         ACE_Message_Block::MB_DATA,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY,
                                         ACE_Time_Value::zero,
                                         ACE_Time_Value::max_time,
                                         NULL,
                                         NULL));
  if (!message_block_p)
  {
    if (inherited2::configuration_.streamConfiguration.messageAllocator)
    {
      if (inherited2::configuration_.streamConfiguration.messageAllocator->block ())
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
    } // end IF
    else
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
  } // end IF

  return message_block_p;
}
