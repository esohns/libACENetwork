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
#include "ace/SOCK_Stream.h"
#include "ace/Svc_Handler.h"

#include "common.h"

#include "stream_common.h"

#include "net_common.h"
#include "net_defines.h"
#include "net_macros.h"

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
Net_SocketConnectionBase_T<HandlerType,
                           AddressType,
                           ConfigurationType,
                           StateType,
                           StatisticContainerType,
                           StreamType,
                           SocketConfigurationType,
                           HandlerConfigurationType,
                           UserDataType>::Net_SocketConnectionBase_T (ICONNECTION_MANAGER_T* interfaceHandle_in,
                                                                      unsigned int statisticCollectionInterval_in)
 : inherited (interfaceHandle_in,
              statisticCollectionInterval_in)
 , configuration_ ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_SocketConnectionBase_T::Net_SocketConnectionBase_T"));

}

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
Net_SocketConnectionBase_T<HandlerType,
                           AddressType,
                           ConfigurationType,
                           StateType,
                           StatisticContainerType,
                           StreamType,
                           SocketConfigurationType,
                           HandlerConfigurationType,
                           UserDataType>::~Net_SocketConnectionBase_T ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_SocketConnectionBase_T::~Net_SocketConnectionBase_T"));

  int result = -1;

  // wait for our worker (if any)
  // *TODO*: remove type inference
  if (inherited::CONNECTION_BASE_T::configuration_.streamConfiguration.useThreadPerConnection)
  {
    result = inherited::wait ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Task_Base::wait(): \"%m\", continuing\n")));
  } // end IF
}

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
bool
Net_SocketConnectionBase_T<HandlerType,
                           AddressType,
                           ConfigurationType,
                           StateType,
                           StatisticContainerType,
                           StreamType,
                           SocketConfigurationType,
                           HandlerConfigurationType,
                           //UserDataType>::get () const
                           UserDataType>::send (ACE_Message_Block*& messageBlock_inout)
{
  NETWORK_TRACE (ACE_TEXT ("Net_SocketConnectionBase_T::send"));

  int result = -1;

  Stream_Module_t* module_p = NULL;
  result = inherited::stream_.top (module_p);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::top(): \"%m\", aborting\n")));

    // clean up
    messageBlock_inout->release ();
    messageBlock_inout = NULL;

    return false;
  } // end IF
  ACE_ASSERT (module_p);
  Stream_Task_t* task_p = module_p->writer ();
  ACE_ASSERT (task_p);
  result = task_p->reply (messageBlock_inout, NULL);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task::reply(): \"%m\", aborting\n")));

    // clean up
    messageBlock_inout->release ();
    messageBlock_inout = NULL;

    return false;
  } // end IF

  return (result >= 0);
}

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
const StreamType&
Net_SocketConnectionBase_T<HandlerType,
                           AddressType,
                           ConfigurationType,
                           StateType,
                           StatisticContainerType,
                           StreamType,
                           SocketConfigurationType,
                           HandlerConfigurationType,
                           //UserDataType>::get () const
                           UserDataType>::stream () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_SocketConnectionBase_T::stream"));

  return inherited::stream_;
}

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
const HandlerConfigurationType&
Net_SocketConnectionBase_T<HandlerType,
                           AddressType,
                           ConfigurationType,
                           StateType,
                           StatisticContainerType,
                           StreamType,
                           SocketConfigurationType,
                           HandlerConfigurationType,
                           //UserDataType>::get () const
                           UserDataType>::get ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_SocketConnectionBase_T::get"));

  return configuration_;
}
template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
bool
Net_SocketConnectionBase_T<HandlerType,
                           AddressType,
                           ConfigurationType,
                           StateType,
                           StatisticContainerType,
                           StreamType,
                           SocketConfigurationType,
                           HandlerConfigurationType,
                           UserDataType>::initialize (const HandlerConfigurationType& configuration_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_SocketConnectionBase_T::initialize"));

  configuration_ = configuration_in;

  return true;
}

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
void
Net_SocketConnectionBase_T<HandlerType,
                           AddressType,
                           ConfigurationType,
                           StateType,
                           StatisticContainerType,
                           StreamType,
                           SocketConfigurationType,
                           HandlerConfigurationType,
                           UserDataType>::set (Net_ClientServerRole role_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_SocketConnectionBase_T::set"));

  // sanity check(s)
  SocketConfigurationType socket_configuration;
  //// *TODO*: remove type inference
  //if (configuration_.socketConfiguration)
  //  socket_configuration = *configuration_.socketConfiguration;

  ITRANSPORTLAYER_T* itransportlayer_p = this;
  ACE_ASSERT (itransportlayer_p);

  if (!itransportlayer_p->initialize (this->dispatch (),
                                      role_in,
                                      socket_configuration))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_ITransportLayer_T::initialize(), continuing\n")));
}

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
void
Net_SocketConnectionBase_T<HandlerType,
                           AddressType,
                           ConfigurationType,
                           StateType,
                           StatisticContainerType,
                           StreamType,
                           SocketConfigurationType,
                           HandlerConfigurationType,
                           UserDataType>::ping ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_SocketConnectionBase_T::ping"));

  inherited::stream_.ping ();
}

//template <typename HandlerType,
//          typename ITransportLayerType,
//          typename ConfigurationType,
//          typename StateType,
//          typename StatisticContainerType>
//int
//Net_SocketConnectionBase_T<SocketHandlerType,
//                           ITransportLayerType,
//                           ConfigurationType,
//                           StateType,
//                           StatisticContainerType>::svc (void)
//{
//  NETWORK_TRACE (ACE_TEXT ("Net_SocketConnectionBase_T::svc"));

//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("(%t) worker (connection: %u) starting...\n"),
//              inherited::get_handle ()));

//  int result = -1;
//  ssize_t bytes_sent = 0;
//  while (true)
//  {
//    if (!inherited::currentWriteBuffer_)
//    {
//      result = inherited::stream_.get (inherited::currentWriteBuffer_,
//                                       NULL); // block
//      if (result == -1)
//      {
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("failed to ACE_Stream::get(): \"%m\", aborting\n")));

//        return -1;
//      } // end IF
//    } // end IF

//    // finished ?
//    if (inherited::currentWriteBuffer_->msg_type () == ACE_Message_Block::MB_STOP)
//    {
//      // clean up
//      inherited::currentWriteBuffer_->release ();
//      inherited::currentWriteBuffer_ = NULL;

////       ACE_DEBUG ((LM_DEBUG,
////                   ACE_TEXT ("[%u]: finished sending...\n"),
////                   peer_.get_handle ()));

//      break;
//    } // end IF

//    // put some data into the socket...
//    bytes_sent =
//        inherited::peer_.send (inherited::currentWriteBuffer_->rd_ptr (),
//                               inherited::currentWriteBuffer_->length (),
//                               0, // default behavior
//                               //MSG_DONTWAIT, // don't block
//                               NULL); // block if necessary...
//    switch (bytes_sent)
//    {
//      case -1:
//      {
//        // connection reset by peer/broken pipe ? --> not an error
//        int error = ACE_OS::last_error ();
//        if ((error != ECONNRESET) &&
//            (error != ENOTSOCK) &&
//            (error != EPIPE))
//          ACE_DEBUG ((LM_ERROR,
//                      ACE_TEXT ("failed to ACE_SOCK_Stream::send(): \"%m\", returning\n")));

//        // clean up
//        inherited::currentWriteBuffer_->release ();
//        inherited::currentWriteBuffer_ = NULL;

//        // nothing to do but wait for the shutdown signal (see above)...
//        break;
//      }
//      // *** GOOD CASES ***
//      case 0:
//      {
////        ACE_DEBUG ((LM_DEBUG,
////                    ACE_TEXT ("[%u]: socket was closed by the peer...\n"),
////                    inherited::peer_.get_handle ()));

//        // clean up
//        inherited::currentWriteBuffer_->release ();
//        inherited::currentWriteBuffer_ = NULL;

//        // nothing to do but wait for the shutdown signal (see above)...
//        break;
//      }
//      default:
//      {
////        ACE_DEBUG ((LM_DEBUG,
////                    ACE_TEXT ("[%u]: sent %u bytes...\n"),
////                    peer_.get_handle (),
////                    bytes_sent));

//        // finished with this buffer ?
//        if (static_cast<size_t> (bytes_sent) == inherited::currentWriteBuffer_->length ())
//        {
//          // get the next one...
//          inherited::currentWriteBuffer_->release ();
//          inherited::currentWriteBuffer_ = NULL;
//        } // end IF
//        else
//        {
//          // there's more data --> adjust read pointer
//          inherited::currentWriteBuffer_->rd_ptr (bytes_sent);
//        } // end ELSE

//        break;
//      }
//    } // end SWITCH
//  } // end WHILE

//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("(%t) worker (connection: %u) joining...\n"),
//              inherited::get_handle ()));

//  return 0;
//}

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
int
Net_SocketConnectionBase_T<HandlerType,
                           AddressType,
                           ConfigurationType,
                           StateType,
                           StatisticContainerType,
                           StreamType,
                           SocketConfigurationType,
                           HandlerConfigurationType,
                           UserDataType>::open (void* arg_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_SocketConnectionBase_T::open"));

  // step0: initialize this connection
  // *NOTE*: client-side: arg_in is a handle to the connector
  //         server-side: arg_in is a handle to the listener
  const HandlerConfigurationType* handler_configuration_p = NULL;
  switch (this->role ())
  {
    case NET_ROLE_CLIENT:
    {
      ICONNECTOR_T* iconnector_p = static_cast<ICONNECTOR_T*> (arg_in);
      ACE_ASSERT (iconnector_p);
      handler_configuration_p = &(iconnector_p->get ());
      break;
    }
    case NET_ROLE_SERVER:
    {
      ILISTENER_T* ilistener_p = static_cast<ILISTENER_T*> (arg_in);
      ACE_ASSERT (ilistener_p);
      handler_configuration_p = &(ilistener_p->get ());
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown role (was: %d), aborting\n"),
                  this->role ()));
      return -1;
    }
  } // end SWITCH
  ACE_ASSERT (handler_configuration_p);
  // *TODO*: remove type inference
  ACE_ASSERT (handler_configuration_p->userData);
  ConfigurationType* configuration_p =
    handler_configuration_p->userData->configuration;
  ACE_ASSERT (configuration_p);
  if (!inherited::CONNECTION_BASE_T::initialize (*configuration_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_ConnectionBase_T::initialize(): \"%m\", aborting\n")));
    return -1;
  } // end IF

  // step1: initialize/start stream, tweak socket, register reading data with
  //        reactor, ...
  int result = inherited::open (configuration_p);
  if (result == -1)
  {
    // *NOTE*: this can happen when the connection handle is still registered
    //         with the reactor (i.e. the reactor is still processing events on
    //         a file descriptor that has been closed and is now being reused by
    //         the system)
    // *NOTE*: more likely, this happened because the (select) reactor is out of
    //         "free" (read) slots
    int error = ACE_OS::last_error ();
    ACE_UNUSED_ARG (error);

    // *NOTE*: perhaps max# connections has been reached
    //ACE_DEBUG ((LM_ERROR,
    //            ACE_TEXT ("failed to HandlerType::open(): \"%m\", aborting\n")));
    return -1;
  } // end IF

//  // step2: start a worker ?
//  if (inherited::configuration_.streamConfiguration.useThreadPerConnection)
//  {
//    ACE_thread_t thread_ids[1];
//    thread_ids[0] = 0;
//    ACE_hthread_t thread_handles[1];
//    thread_handles[0] = 0;
//    char thread_name[BUFSIZ];
//    ACE_OS::memset (thread_name, 0, sizeof (thread_name));
//    ACE_OS::strcpy (thread_name,
//                    ACE_TEXT_ALWAYS_CHAR (NET_CONNECTION_HANDLER_THREAD_NAME));
//    const char* thread_names[1];
//    thread_names[0] = thread_name;
//    result = inherited::activate ((THR_NEW_LWP      |
//                                   THR_JOINABLE     |
//                                   THR_INHERIT_SCHED),                    // flags
//                                  1,                                      // # threads
//                                  0,                                      // force spawning
//                                  ACE_DEFAULT_THREAD_PRIORITY,            // priority
//                                  NET_CONNECTION_HANDLER_THREAD_GROUP_ID, // group id
//                                  NULL,                                   // corresp. task --> use 'this'
//                                  thread_handles,                         // thread handle(s)
//                                  NULL,                                   // thread stack(s)
//                                  NULL,                                   // thread stack size(s)
//                                  thread_ids,                             // thread id(s)
//                                  thread_names);                          // thread name(s)
//    if (result == -1)
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_Task_Base::activate(): \"%m\", aborting\n")));
//      return -1;
//    } // end IF
//  } // end IF

  return 0;
}

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
int
Net_SocketConnectionBase_T<HandlerType,
                           AddressType,
                           ConfigurationType,
                           StateType,
                           StatisticContainerType,
                           StreamType,
                           SocketConfigurationType,
                           HandlerConfigurationType,
                           UserDataType>::close (u_long arg_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_SocketConnectionBase_T::close"));

  int result = -1;

  // [*NOTE*: hereby override the default behavior of a ACE_Svc_Handler,
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
      result = ACE_OS::thr_equal (ACE_Thread::self (),
                                  inherited::last_thread ());
      if (result)
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
    // (e.g. cannot connect, too many connections, ...)
    // *NOTE*: this (eventually) calls handle_close() (see below)
    case CLOSE_DURING_NEW_CONNECTION:
    case NET_CONNECTION_CLOSE_REASON_INITIALIZATION:
    case NET_CONNECTION_CLOSE_REASON_USER_ABORT:
    {
      // step1: release resources
      result = inherited::close (arg_in);
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to HandlerType::close(%u): \"%m\", continuing\n"),
                    arg_in));

      // step2: delete this ?
      // *TODO*: remove type inference
      if ((arg_in == NORMAL_CLOSE_OPERATION) &&
          (inherited::state_.status == NET_CONNECTION_STATUS_INITIALIZATION_FAILED))
        inherited::decrease ();

      break;
    } // end IF
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

//template <typename HandlerType,
//          typename ITransportLayerType,
//          typename ConfigurationType,
//          typename StateType,
//          typename StatisticContainerType>
//int
//Net_SocketConnectionBase_T<SocketHandlerType,
//                           ITransportLayerType,
//                           ConfigurationType,
//                           StateType,
//                           StatisticContainerType>::handle_close (ACE_HANDLE handle_in,
//                                                                   ACE_Reactor_Mask mask_in)
//{
//  NETWORK_TRACE (ACE_TEXT ("Net_SocketConnectionBase_T::handle_close"));

//  ACE_UNUSED_ARG (handle_in);

//  switch (mask_in)
//  {
//    case ACE_Event_Handler::READ_MASK:       // --> socket has been closed
//    case ACE_Event_Handler::ALL_EVENTS_MASK: // --> connect failed (e.g. connection refused) /
//      //     accept failed (e.g. too many connections) /
//      //     select failed (EBADF see Select_Reactor_T.cpp)
//      //     asynch abort
//      break;
//    case ACE_Event_Handler::EXCEPT_MASK:
//      // *IMPORTANT NOTE*: the TP_Reactor dispatches notifications in parallel
//      // to regular socket events, thus several notifications may be in flight
//      // at the same time. In order to avoid confusion when the socket closes,
//      // proper synchronization is handled through the reference counting
//      // mechanism, i.e. the handler is only deleted after the last reference
//      // has been released. Still, handling notifications after this occurs
//      // will fail, and the reactor will invoke this method each time. As there
//      // is no need to go through the shutdown procedure several times, bail
//      // out early here
//      //if (handle_in == ACE_INVALID_HANDLE) // <-- notification has completed (!useThreadPerConnection)
//      //  ACE_DEBUG((LM_ERROR,
//      //             ACE_TEXT("notification completed, continuing\n")));
//      return 0;
//    default:
//      // *PORTABILITY*: this isn't entirely portable...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("handle_close called for unknown reasons (handle: %@, mask: %u) --> check implementation !, continuing\n"),
//                  ACE_Event_Handler::get_handle (),
//                  mask_in));
//#else
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("handle_close called for unknown reasons (handle: %d, mask: %u) --> check implementation !, continuing\n"),
//                  ACE_Event_Handler::get_handle (),
//                  mask_in));
//#endif
//      break;
//  } // end SWITCH

//  //// step1: connection shutting down --> signal any worker(s)
//  //if (inherited::myConfiguration.useThreadPerConnection)
//  //  shutdown ();

////  // step2: de-register this connection
////  if (inherited::manager_ &&
////      inherited::isRegistered_)
////  { // (try to) de-register with the connection manager
////    try
////    {
////      inherited::manager_->deregisterConnection (this);
////    }
////    catch (...)
////    {
////      ACE_DEBUG ((LM_ERROR,
////                  ACE_TEXT ("caught exception in Net_IConnectionManager::deregisterConnection(), continuing\n")));
////    }
////  } // end IF

//  // step3: invoke base-class maintenance
//  int result = inherited::handle_close (ACE_Event_Handler::get_handle (), // event handle
//                                        mask_in);     // event mask
//  if (result == -1)
//    // *PORTABILITY*: this isn't entirely portable...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Net_StreamSocketBase::handle_close(%@, %d): \"%m\", continuing\n"),
//                ACE_Event_Handler::get_handle (),
//                mask_in));
//#else
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Net_StreamSocketBase::handle_close(%d, %d): \"%m\", continuing\n"),
//                ACE_Event_Handler::get_handle (),
//                mask_in));
//#endif

//  return result;
//}

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
void
Net_SocketConnectionBase_T<HandlerType,
                           AddressType,
                           ConfigurationType,
                           StateType,
                           StatisticContainerType,
                           StreamType,
                           SocketConfigurationType,
                           HandlerConfigurationType,
                           UserDataType>::open (ACE_HANDLE handle_in,
                                                ACE_Message_Block& messageBlock_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_SocketConnectionBase_T::open"));

  ACE_UNUSED_ARG (handle_in);
  ACE_UNUSED_ARG (messageBlock_in);

  ACE_ASSERT (false);
  ACE_NOTSUP;

  ACE_NOTREACHED (return);
}

//template <typename HandlerType,
//          typename ITransportLayerType,
//          typename ConfigurationType,
//          typename StateType,
//          typename StatisticContainerType>
//void
//Net_SocketConnectionBase_T<SocketHandlerType,
//                           ITransportLayerType,
//                           ConfigurationType,
//                           StateType,
//                           StatisticContainerType>::shutdown ()
//{
//  NETWORK_TRACE (ACE_TEXT ("Net_SocketConnectionBase_T::shutdown"));

//  ACE_Message_Block* stop_mb = NULL;
//  ACE_NEW_NORETURN (stop_mb,
//                    ACE_Message_Block (0,                                  // size
//                                       ACE_Message_Block::MB_STOP,         // type
//                                       NULL,                               // continuation
//                                       NULL,                               // data
//                                       NULL,                               // buffer allocator
//                                       NULL,                               // locking strategy
//                                       ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY, // priority
//                                       ACE_Time_Value::zero,               // execution time
//                                       ACE_Time_Value::max_time,           // deadline time
//                                       NULL,                               // data block allocator
//                                       NULL));                             // message allocator)
//  if (!stop_mb)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to allocate ACE_Message_Block: \"%m\", aborting\n")));
//    return;
//  } // end IF

//  if (inherited::putq (stop_mb, NULL) == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_Task::putq(): \"%m\", continuing\n")));

//    stop_mb->release ();
//  } // end IF

//  // *NOTE*: defer waiting for any worker(s) to the dtor
//}

/////////////////////////////////////////

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
Net_AsynchSocketConnectionBase_T<HandlerType,
                                 AddressType,
                                 ConfigurationType,
                                 StateType,
                                 StatisticContainerType,
                                 StreamType,
                                 SocketConfigurationType,
                                 HandlerConfigurationType,
                                 UserDataType>::Net_AsynchSocketConnectionBase_T (ICONNECTION_MANAGER_T* interfaceHandle_in,
                                                                                  unsigned int statisticCollectionInterval_in)
 : inherited (interfaceHandle_in,
              statisticCollectionInterval_in)
 , configuration_ ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchSocketConnectionBase_T::Net_AsynchSocketConnectionBase_T"));

}

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
Net_AsynchSocketConnectionBase_T<HandlerType,
                                 AddressType,
                                 ConfigurationType,
                                 StateType,
                                 StatisticContainerType,
                                 StreamType,
                                 SocketConfigurationType,
                                 HandlerConfigurationType,
                                 UserDataType>::~Net_AsynchSocketConnectionBase_T ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchSocketConnectionBase_T::~Net_AsynchSocketConnectionBase_T"));

}

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
void
Net_AsynchSocketConnectionBase_T<HandlerType,
                                 AddressType,
                                 ConfigurationType,
                                 StateType,
                                 StatisticContainerType,
                                 StreamType,
                                 SocketConfigurationType,
                                 HandlerConfigurationType,
                                 UserDataType>::open (ACE_HANDLE handle_in,
                                                      ACE_Message_Block& messageBlock_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchSocketConnectionBase_T::open"));

  // initialize/start stream, tweak socket, register reading data with the
  // reactor, ...
  inherited::open (handle_in,
                   messageBlock_in);
}

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
void
Net_AsynchSocketConnectionBase_T<HandlerType,
                                 AddressType,
                                 ConfigurationType,
                                 StateType,
                                 StatisticContainerType,
                                 StreamType,
                                 SocketConfigurationType,
                                 HandlerConfigurationType,
                                 UserDataType>::act (const void* act_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchSocketConnectionBase_T::act"));

  // initialize this connection

  // *NOTE*: client-side: arg_in is a handle to the connector
  //         server-side: arg_in is a handle to the listener
  const HandlerConfigurationType* handler_configuration_p = NULL;
  switch (this->role ())
  {
    case NET_ROLE_CLIENT:
    {
      const ICONNECTOR_T* iconnector_p =
        static_cast<const ICONNECTOR_T*> (act_in);
      ACE_ASSERT (iconnector_p);
      handler_configuration_p = &iconnector_p->get ();
      break;
    }
    case NET_ROLE_SERVER:
    {
      const ILISTENER_T* ilistener_p = static_cast<const ILISTENER_T*> (act_in);
      ACE_ASSERT (ilistener_p);
      handler_configuration_p = &ilistener_p->get ();
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown role (was: %d), returning\n"),
                  this->role ()));
      return;
    }
  } // end SWITCH
  ACE_ASSERT (handler_configuration_p);
  // *TODO*: remove type inference
  ACE_ASSERT (handler_configuration_p->userData);
  ConfigurationType* configuration_p =
    handler_configuration_p->userData->configuration;
  ACE_ASSERT (configuration_p);
  if (!inherited::CONNECTION_BASE_T::initialize (*configuration_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_ConnectionBase_T::initialize(): \"%m\", returning\n")));
    return;
  } // end IF
}

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
bool
Net_AsynchSocketConnectionBase_T<HandlerType,
                                 AddressType,
                                 ConfigurationType,
                                 StateType,
                                 StatisticContainerType,
                                 StreamType,
                                 SocketConfigurationType,
                                 HandlerConfigurationType,
                                 UserDataType>::send (ACE_Message_Block*& messageBlock_inout)
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchSocketConnectionBase_T::send"));

  int result = -1;

  Stream_Module_t* module_p = NULL;
  result = inherited::stream_.top (module_p);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::top(): \"%m\", aborting\n")));

    // clean up
    messageBlock_inout->release ();
    messageBlock_inout = NULL;

    return false;
  } // end IF
  ACE_ASSERT (module_p);
  Stream_Task_t* task_p = module_p->writer ();
  ACE_ASSERT (task_p);
  result = task_p->reply (messageBlock_inout, NULL);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task::reply(): \"%m\", aborting\n")));

    // clean up
    messageBlock_inout->release ();
    messageBlock_inout = NULL;

    return false;
  } // end IF

  return (result >= 0);
}

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
bool
Net_AsynchSocketConnectionBase_T<HandlerType,
                                 AddressType,
                                 ConfigurationType,
                                 StateType,
                                 StatisticContainerType,
                                 StreamType,
                                 SocketConfigurationType,
                                 HandlerConfigurationType,
                                 UserDataType>::initialize (const HandlerConfigurationType& configuration_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchSocketConnectionBase_T::initialize"));

  configuration_ = configuration_in;

  return true;
}
template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
const StreamType&
Net_AsynchSocketConnectionBase_T<HandlerType,
                                 AddressType,
                                 ConfigurationType,
                                 StateType,
                                 StatisticContainerType,
                                 StreamType,
                                 SocketConfigurationType,
                                 HandlerConfigurationType,
                                 //UserDataType>::get () const
                                 UserDataType>::stream () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchSocketConnectionBase_T::stream"));

  return inherited::stream_;
}
template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
const HandlerConfigurationType&
Net_AsynchSocketConnectionBase_T<HandlerType,
                                 AddressType,
                                 ConfigurationType,
                                 StateType,
                                 StatisticContainerType,
                                 StreamType,
                                 SocketConfigurationType,
                                 HandlerConfigurationType,
                                 //UserDataType>::get () const
                                 UserDataType>::get ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchSocketConnectionBase_T::get"));

  return configuration_;
}

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
void
Net_AsynchSocketConnectionBase_T<HandlerType,
                                 AddressType,
                                 ConfigurationType,
                                 StateType,
                                 StatisticContainerType,
                                 StreamType,
                                 SocketConfigurationType,
                                 HandlerConfigurationType,
                                 UserDataType>::set (Net_ClientServerRole role_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchSocketConnectionBase_T::set"));

  // sanity check(s)
  SocketConfigurationType socket_configuration;
  //// *TODO*: remove type inference
  //if (configuration_.socketConfiguration)
  //  socket_configuration = *configuration_.socketConfiguration;

  ITRANSPORTLAYER_T* itransportlayer_p = this;
  ACE_ASSERT (itransportlayer_p);

  if (!itransportlayer_p->initialize (this->dispatch (),
                                      role_in,
                                      *configuration_.socketConfiguration))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_ITransportLayer_T::initialize(), continuing\n")));
}

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
void
Net_AsynchSocketConnectionBase_T<HandlerType,
                                 AddressType,
                                 ConfigurationType,
                                 StateType,
                                 StatisticContainerType,
                                 StreamType,
                                 SocketConfigurationType,
                                 HandlerConfigurationType,
                                 UserDataType>::ping ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchSocketConnectionBase_T::ping"));

  inherited::stream_.ping ();
}

template <typename HandlerType,
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename UserDataType>
int
Net_AsynchSocketConnectionBase_T<HandlerType,
                                 AddressType,
                                 ConfigurationType,
                                 StateType,
                                 StatisticContainerType,
                                 StreamType,
                                 SocketConfigurationType,
                                 HandlerConfigurationType,
                                 UserDataType>::open (void* arg_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchSocketConnectionBase_T::open"));

  ACE_UNUSED_ARG (arg_in);

  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (-1);

#if defined (_MSC_VER)
  ACE_NOTREACHED (true);
#endif
}
