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
#include "ace/OS_Memory.h"
#include "ace/Proactor.h"

#include "common_defines.h"
#include "common_macros.h"
#include "common_tools.h"

#include "stream_iallocator.h"

#include "net_common_tools.h"
#include "net_configuration.h"
#include "net_defines.h"
#include "net_macros.h"

template <typename SocketType,
          typename ConfigurationType>
Net_AsynchUDPSocketHandler_T<SocketType,
                             ConfigurationType>::Net_AsynchUDPSocketHandler_T ()
 : inherited ()
 , inherited2 ()
 , inherited3 ()
 , inherited4 (NULL,                          // event handler handle
               ACE_Event_Handler::WRITE_MASK) // mask
 , address_ ()
 , counter_ (0) // initial count
#if defined (ACE_LINUX)
 , errorQueue_ (NET_SOCKET_DEFAULT_ERRORQUEUE)
#endif
 , inputStream_ ()
 , outputStream_ ()
 , PDUSize_ (NET_PROTOCOL_DEFAULT_UDP_BUFFER_SIZE)
 , writeHandle_ (ACE_INVALID_HANDLE)
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchUDPSocketHandler_T::Net_AsynchUDPSocketHandler_T"));

}

template <typename SocketType,
          typename ConfigurationType>
Net_AsynchUDPSocketHandler_T<SocketType,
                             ConfigurationType>::~Net_AsynchUDPSocketHandler_T ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchUDPSocketHandler_T::~Net_AsynchUDPSocketHandler_T"));

  int result = -1;

  if (writeHandle_ != ACE_INVALID_HANDLE)
  {
    result = ACE_OS::closesocket (writeHandle_);
    if (unlikely (result == -1))
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::closesocket(0x%@): \"%m\", continuing\n")));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::closesocket(%d): \"%m\", continuing\n")));
#endif
  } // end IF
}

template <typename SocketType,
          typename ConfigurationType>
void
Net_AsynchUDPSocketHandler_T<SocketType,
                             ConfigurationType>::open (ACE_HANDLE handle_in,
                                                       ACE_Message_Block& messageBlock_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchUDPSocketHandler_T::open"));

  ACE_UNUSED_ARG (messageBlock_in);

  static ACE_INET_Addr inet_addr_sap_any (ACE_sap_any_cast (const ACE_INET_Addr&));
  struct Net_UDPSocketConfiguration* socket_configuration_p = NULL;
  ACE_INET_Addr source_SAP;
  ACE_HANDLE handle = ACE_INVALID_HANDLE;
  int result = -1;
#if defined (ACE_LINUX)
  unsigned short port_number = 0;
  bool handle_privileges = false;
#endif
  bool handle_sockets = false;
  // *IMPORTANT NOTE*: associate the outbound socket with the peer address as
  //                   the data dispatch happens in the proactor context
  bool connect_socket = true;
  ACE_Proactor* proactor_p = ACE_Proactor::instance ();

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);
  // *TODO*: remove type inferences
  ACE_ASSERT (inherited::configuration_->socketConfiguration);

  socket_configuration_p =
    dynamic_cast<struct Net_UDPSocketConfiguration*> (inherited::configuration_->socketConfiguration);

  // sanity check(s)
  ACE_ASSERT (proactor_p);
  ACE_ASSERT (socket_configuration_p);

  //connect_socket = socket_configuration_p->connect;
  if (unlikely (socket_configuration_p->writeOnly))
  { ACE_ASSERT (socket_configuration_p->connect);
    //connect_socket = true;
  } // end IF

  // step0: configure addresses
  if (unlikely (socket_configuration_p->sourcePort))
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    struct _GUID interface_identifier;
#else
    std::string interface_identifier;
#endif
    if (unlikely (!Net_Common_Tools::IPAddressToInterface (socket_configuration_p->peerAddress,
                                                           interface_identifier)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_Common_Tools::IPAddressToInterface(%s), aborting\n"),
                  ACE_TEXT (Net_Common_Tools::IPAddressToString (socket_configuration_p->peerAddress).c_str ())));
      goto error;
    } // end IF
    ACE_INET_Addr gateway_address;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (unlikely (!Net_Common_Tools::interfaceToIPAddress (interface_identifier,
                                                           source_SAP,
                                                           gateway_address)))
#else
    if (unlikely (!Net_Common_Tools::interfaceToIPAddress (interface_identifier,
                                                           NULL,
                                                           source_SAP,
                                                           gateway_address)))
#endif
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(%s): \"%m\", aborting\n"),
                  ACE_TEXT (Net_Common_Tools::interfaceToString (interface_identifier).c_str ())));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(%s): \"%m\", aborting\n"),
                  ACE_TEXT (interface_identifier.c_str ())));
#endif
      goto error;
    } // end IF
    source_SAP.set_port_number (static_cast<u_short> (socket_configuration_p->sourcePort),
                                1);
  } // end IF

  // step1: open socket
  // *NOTE*: even if this is a write-only connection, the output stream still
  //         requires a valid handle; i.e. there are two distinct scenarios:
  //         - read-write: inherited2 maintains the inbound socket handle; open
  //                       an additional outbound socket (writeHandle_); attach
  //                       it to the outbound stream. Note that as 'this'
  //                       handles both input and output, inherited3 has no
  //                       definitive handle
  //                       --> maintain two handles
  //         - write-only: (re)use inherited2; ; attach its' handle to the
  //                       outbound stream. Set writeHandle_ and inherited3 to
  //                       inherited2
  //                       --> maintain one handle
#if defined (ACE_LINUX)
  port_number =
      (socket_configuration_p->writeOnly ? (socket_configuration_p->sourcePort ? source_SAP
                                                                               : inet_addr_sap_any)
                                         : socket_configuration_p->listenAddress).get_port_number ();
  // (temporarily) elevate privileges to open system sockets
  if (unlikely (!socket_configuration_p->writeOnly &&
                (port_number <= NET_ADDRESS_MAXIMUM_PRIVILEGED_PORT)))
  {
    if (!Common_Tools::setRootPrivileges ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_Tools::setRootPrivileges(): \"%m\", aborting\n")));
      goto error;
    } // end IF
    handle_privileges = true;
  } // end IF
#endif
  result =
    inherited2::open ((socket_configuration_p->writeOnly ? (socket_configuration_p->sourcePort ? source_SAP
                                                                                               : inet_addr_sap_any)
                                                         : socket_configuration_p->listenAddress), // local SAP
                      ACE_PROTOCOL_FAMILY_INET,                                                    // protocol family
                      0,                                                                           // protocol
                      1);                                                                          // reuse_addr
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SocketType::open(%s): \"%m\", aborting\n"),
                ACE_TEXT (Net_Common_Tools::IPAddressToString ((socket_configuration_p->writeOnly ? (socket_configuration_p->sourcePort ? source_SAP
                                                                                                                                        : inet_addr_sap_any)
                                                                                                  : socket_configuration_p->listenAddress)).c_str ())));
    goto error;
  } // end IF
#if defined (ACE_LINUX)
  if (handle_privileges)
    Common_Tools::dropRootPrivileges ();
#endif
  handle = inherited2::get_handle ();
  ACE_ASSERT (handle != ACE_INVALID_HANDLE);
  if (likely (!socket_configuration_p->writeOnly))
  { ACE_ASSERT (writeHandle_ == ACE_INVALID_HANDLE);
    writeHandle_ = ACE_OS::socket (AF_INET,    // family
                                   SOCK_DGRAM, // type
                                   0);         // protocol
    if (unlikely (writeHandle_ == ACE_INVALID_HANDLE))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::socket(%d,%d,0): \"%m\", aborting\n"),
                  AF_INET, SOCK_DGRAM));

      // clean up
      result = inherited2::close ();
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to SocketType::close(): \"%m\", continuing\n")));

      goto error;
    } // end IF

    // set source address ?
    if (unlikely (socket_configuration_p->sourcePort))
    {
      source_SAP.set (static_cast<u_short> (socket_configuration_p->sourcePort),
                      static_cast<ACE_UINT32> (INADDR_ANY),
                      1,
                      0);
      result =
        ACE_OS::bind (writeHandle_,
                      reinterpret_cast<struct sockaddr*> (source_SAP.get_addr ()),
                      source_SAP.get_addr_size ());
      if (unlikely (result == -1))
      {
        int error = ACE_OS::last_error ();
        ACE_UNUSED_ARG (error);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::bind(0x%@,%s): \"%m\", aborting\n"),
                    writeHandle_,
                    ACE_TEXT (Net_Common_Tools::IPAddressToString (source_SAP).c_str ())));
#else
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::bind(%d,%s): \"%m\", aborting\n"),
                    writeHandle_,
                    ACE_TEXT (Net_Common_Tools::IPAddressToString (source_SAP).c_str ())));
#endif
        goto error;
      } // end IF
    } // end IF
  } // end IF
  else
  {
    inherited3::handle (handle);
    writeHandle_ = handle;
  } // end ELSE    
  handle_sockets = true;

  // step0: initialize base class
  inherited3::proactor (proactor_p);

  // step1: connect ?
  if (likely (connect_socket &&
              !socket_configuration_p->peerAddress.is_any ()))
  {
    result =
        ACE_OS::connect (writeHandle_,
                         reinterpret_cast<struct sockaddr*> (socket_configuration_p->peerAddress.get_addr ()),
                         socket_configuration_p->peerAddress.get_addr_size ());
    if (unlikely (result == -1))
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::connect(0x%@,%s): \"%m\", aborting\n"),
                  writeHandle_,
                  ACE_TEXT (Net_Common_Tools::IPAddressToString (socket_configuration_p->peerAddress).c_str ())));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::connect(%d,%s): \"%m\", aborting\n"),
                  writeHandle_,
                  ACE_TEXT (Net_Common_Tools::IPAddressToString (socket_configuration_p->peerAddress).c_str ())));
#endif
      goto error;
    } // end IF
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("0x%@: connected datagram socket to %s\n"),
//                writeHandle_,
//                ACE_TEXT (Net_Common_Tools::IPAddressToString (socket_configuration_p->peerAddress).c_str ())));
//#else
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("%d: connected datagram socket to %s\n"),
//                writeHandle_,
//                ACE_TEXT (Net_Common_Tools::IPAddressToString (socket_configuration_p->peerAddress).c_str ())));
//#endif
  } // end IF

  if (likely (!socket_configuration_p->writeOnly))
  {
    // step3a: tweak inbound socket
    if (likely (socket_configuration_p->bufferSize))
      if (unlikely (!Net_Common_Tools::setSocketBuffer (handle,
                                                        SO_RCVBUF,
                                                        socket_configuration_p->bufferSize)))
      {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Net_Common_Tools::setSocketBuffer(0x%@,%d,%u), aborting\n"),
                    handle, SO_RCVBUF, socket_configuration_p->bufferSize));
#else
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Net_Common_Tools::setSocketBuffer(%d,%d,%u), aborting\n"),
                    handle, SO_RCVBUF, socket_configuration_p->bufferSize));
#endif
        goto error;
      } // end IF

    // *PORTABILITY*: (currently,) MS Windows (TM) UDP sockets do not support
    //                SO_LINGER
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
    if (unlikely (!Net_Common_Tools::setLinger (handle,
                                                socket_configuration_p->linger,
                                                -1)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_Common_Tools::setLinger(%d,%s,-1), aborting\n"),
                  handle, (socket_configuration_p->linger ? ACE_TEXT ("true") : ACE_TEXT ("false"))));
      goto error;
    } // end IF
#endif

    // step3b: initialize input stream
    result = inputStream_.open (*this,       // handler
                                handle,      // handle
                                NULL,        // completion key
                                proactor_p); // proactor handle
    if (unlikely (result == -1))
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize input stream (handle was: 0x%@), aborting\n"),
                  handle));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize input stream (handle was: %d), aborting\n"),
                  handle));
#endif
      goto error;
    } // end IF
  } // end IF

  // *TODO*: remove type inferences
  address_ = socket_configuration_p->peerAddress;
  // *NOTE*: IP_MTU works only on connect()ed sockets (see man 7 ip)
  // *TODO*: to be disambiguated
  PDUSize_ = Net_Common_Tools::getMTU (handle);
  if (!PDUSize_) // --> fall back
    PDUSize_ = socket_configuration_p->bufferSize;
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("maximum message size: %u\n"),
  //            PDUSize_));

  // step4a: tweak outbound socket (if any)
  if (likely (socket_configuration_p->bufferSize))
    if (unlikely (!Net_Common_Tools::setSocketBuffer (writeHandle_,
                                                      SO_SNDBUF,
                                                      socket_configuration_p->bufferSize)))
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_Common_Tools::setSocketBuffer(0x%@,%d,%u), aborting\n"),
                  writeHandle_, SO_SNDBUF, socket_configuration_p->bufferSize));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_Common_Tools::setSocketBuffer(%d,%d,%u), continuing\n"),
                  writeHandle_, SO_SNDBUF, socket_configuration_p->bufferSize));
#endif
      goto error;
    } // end IF

  // step4b: initialize output stream
  result = outputStream_.open (*this,        // handler
                               writeHandle_, // handle
                               NULL,         // completion key
                               proactor_p);  // proactor handle
  if (unlikely (result == -1))
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize output stream (handle was: 0x%@), aborting\n"),
                  writeHandle_));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize output stream (handle was: %d), aborting\n"),
                  writeHandle_));
#endif
    goto error;
  } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // *NOTE*: (on Linux), packet fragmentation is off by default, so sendto()-ing
  //         datagrams larger than MTU will trigger errno EMSGSIZE (90)
  //         --> enable packet fragmentation
  if (unlikely (!Net_Common_Tools::setPathMTUDiscovery (writeHandle_,
                                                        IP_PMTUDISC_WANT)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::disablePathMTUDiscovery(%d,%d), aborting\n"),
                writeHandle_, IP_PMTUDISC_WANT));
    goto error;
  } // end IF
#endif

#if defined (ACE_LINUX)
  if (errorQueue_)
  {
    if (likely (!socket_configuration_p->writeOnly))
      if (!Net_Common_Tools::enableErrorQueue (handle))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Net_Common_Tools::enableErrorQueue() (handle was: %d), aborting\n"),
                    handle));
        goto error;
      } // end IF
    if (!Net_Common_Tools::enableErrorQueue (writeHandle_))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_Common_Tools::enableErrorQueue() (handle was: %d), aborting\n"),
                  writeHandle_));
      goto error;
    } // end IF
  } // end IF
#endif

//  // debug info
//  unsigned int so_max_msg_size = Net_Common_Tools::getMaxMsgSize (handle);
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("maximum message size for UDP socket 0x%@: %u byte(s)\n"),
//              handle,
//              so_max_msg_size));
//#else
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("maximum message size for UDP socket %d: %u byte(s)\n"),
//              handle,
//              so_max_msg_size));
//#endif

  return;

error:
#if defined (ACE_LINUX)
  if (handle_privileges)
    Common_Tools::dropRootPrivileges ();
#endif
  if (handle_sockets)
  {
    result = inherited2::close ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SocketType::close(): \"%m\", continuing\n")));
    if (!socket_configuration_p->writeOnly)
    { ACE_ASSERT (writeHandle_ != ACE_INVALID_HANDLE);
      result = ACE_OS::closesocket (writeHandle_);
      if (result == -1)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::closesocket(0x%@): \"%m\", continuing\n"),
                    writeHandle_));
#else
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::closesocket(%d): \"%m\", continuing\n"),
                    writeHandle_));
#endif
      writeHandle_ = ACE_INVALID_HANDLE;
    } // end IF
  } // end IF
}

template <typename SocketType,
          typename ConfigurationType>
int
Net_AsynchUDPSocketHandler_T<SocketType,
                             ConfigurationType>::handle_close (ACE_HANDLE handle_in,
                                                               ACE_Reactor_Mask mask_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchUDPSocketHandler_T::handle_close"));

  ACE_UNUSED_ARG (handle_in);
  ACE_UNUSED_ARG (mask_in);

  int result = -1;
  int result_2 = -1;
  struct Net_UDPSocketConfiguration* socket_configuration_p = NULL;

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);
  // *TODO*: remove type inferences
  ACE_ASSERT (inherited::configuration_->socketConfiguration);

  socket_configuration_p =
    dynamic_cast<struct Net_UDPSocketConfiguration*> (inherited::configuration_->socketConfiguration);

  // sanity check(s)
  ACE_ASSERT (socket_configuration_p);

  // *BUG*: Posix_Proactor.cpp:1575 should read:
  //        int rc = ::aio_cancel (result->handle_, result);
  // *BUG*: WIN32_Asynch_IO.cpp:178 should read:
  //        int const result = (int) ::CancelIoEx (this->handle_, NULL);
  if (likely (!socket_configuration_p->writeOnly))
  {
    result = inputStream_.cancel ();
    if (unlikely ((result != 0) && (result != 1)))
    {
      int error = ACE_OS::last_error ();
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      if ((error != ERROR_INVALID_ACCESS) && //  12: operation was triggered from a different thread
          (error != ERROR_IO_PENDING))       // 997: 
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Asynch_Read_Dgram::cancel(0x%@): \"%s\", continuing\n"),
                    handle_in,
                    ACE_TEXT (Common_Tools::errorToString (error, false).c_str ())));
#else
      ACE_UNUSED_ARG (error);
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Asynch_Read_Dgram::cancel(%d): \"%m\", continuing\n"),
                  handle_in));
#endif
      result = -1;
    } // end IF
  } // end IF
  else
    result = 0;
  result_2 = outputStream_.cancel ();
  if (unlikely ((result_2 != 0) && (result_2 != 1)))
  {
    int error = ACE_OS::last_error ();
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if ((error != ERROR_INVALID_ACCESS) && //  12: operation was triggered from a different thread
        (error != ERROR_IO_PENDING))       // 997:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Asynch_Write_Dgram::cancel(0x%@): \"%s\", continuing\n"),
                  handle_in,
                  ACE_TEXT (Common_Tools::errorToString (error, false).c_str ())));
#else
    ACE_UNUSED_ARG (error);
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Asynch_Write_Dgram::cancel(%d): \"%m\", continuing\n"),
                handle_in));
#endif
    result_2 = -1;
  } // end IF

  return (((result == -1) || (result_2 == -1)) ? -1 : 0);
}

template <typename SocketType,
          typename ConfigurationType>
void
Net_AsynchUDPSocketHandler_T<SocketType,
                             ConfigurationType>::handle_wakeup ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchUDPSocketHandler_T::handle_wakeup"));

  int result = -1;

  try {
    result = handle_close (inherited2::get_handle (),
                           ACE_Event_Handler::ALL_EVENTS_MASK);
  } catch (...) {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Net_AsynchUDPSocketHandler_T::handle_close(0x%@,%d): \"%m\", continuing\n"),
                inherited2::get_handle (),
                ACE_Event_Handler::ALL_EVENTS_MASK));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Net_AsynchUDPSocketHandler_T::handle_close(%d,%d): \"%m\", continuing\n"),
                inherited2::get_handle (),
                ACE_Event_Handler::ALL_EVENTS_MASK));
#endif
    result = -1;
  }
  if (unlikely (result == -1))
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_AsynchUDPSocketHandler_T::handle_close(0x%@,%d): \"%m\", continuing\n"),
                inherited2::get_handle (),
                ACE_Event_Handler::ALL_EVENTS_MASK));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_AsynchUDPSocketHandler_T::handle_close(%d,%d): \"%m\", continuing\n"),
                inherited2::get_handle (),
                ACE_Event_Handler::ALL_EVENTS_MASK));
#endif
  } // end IF
}

template <typename SocketType,
          typename ConfigurationType>
int
Net_AsynchUDPSocketHandler_T<SocketType,
                             ConfigurationType>::notify (void)
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchUDPSocketHandler_T::notify"));

  // sanity check(s)
  ACE_ASSERT (writeHandle_ != ACE_INVALID_HANDLE);

  int result = -1;

  try {
    result = this->handle_output (writeHandle_);
  } catch (...) {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Net_AsynchUDPSocketHandler_T::handle_output(0x%@): \"%m\", continuing\n"),
                writeHandle_));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Net_AsynchUDPSocketHandler_T::handle_output(%d): \"%m\", continuing\n"),
                writeHandle_));
#endif
    result = -1;
  }
  if (unlikely (result == -1))
  {
    int result_2 = -1;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_AsynchUDPSocketHandler_T::handle_output(0x%@): \"%m\", continuing\n"),
                writeHandle_));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_AsynchUDPSocketHandler_T::handle_output(%d): \"%m\", continuing\n"),
                writeHandle_));
#endif

    result_2 = handle_close (ACE_INVALID_HANDLE,
                             ACE_Event_Handler::ALL_EVENTS_MASK);
    if (unlikely (result_2 == -1))
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_AsynchUDPSocketHandler_T::handle_close(0x%@,%d): \"%m\", continuing\n"),
                  ACE_INVALID_HANDLE,
                  ACE_Event_Handler::ALL_EVENTS_MASK));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_AsynchUDPSocketHandler_T::handle_close(%d,%d): \"%m\", continuing\n"),
                  ACE_INVALID_HANDLE,
                  ACE_Event_Handler::ALL_EVENTS_MASK));
#endif
    } // end IF
  } // end IF

  return result;
}

template <typename SocketType,
          typename ConfigurationType>
bool
Net_AsynchUDPSocketHandler_T<SocketType,
                             ConfigurationType>::initiate_read ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchUDPSocketHandler_T::initiate_read"));

  ssize_t result = -1;
  size_t bytes_received = 0;

  // step1: allocate a data buffer
  ACE_Message_Block* message_block_p = this->allocateMessage (PDUSize_);
  if (unlikely (!message_block_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_AsynchUDPSocketHandler_T::allocateMessage(%u), aborting\n"),
                PDUSize_));
    return false;
  } // end IF

  // step2: start (asynchronous) read
  int error = 0;
receive:
  result =
    inputStream_.recv (message_block_p,                      // buffer
                       bytes_received,                       // #bytes received
                       0,                                    // flags
                       ACE_PROTOCOL_FAMILY_INET,             // protocol family
                       NULL,                                 // asynchronous completion token
                       0,                                    // priority
                       COMMON_EVENT_PROACTOR_SIG_RT_SIGNAL); // signal
  if (unlikely (result == -1))
  {
    error = ACE_OS::last_error ();
    // *WARNING*: this could fail on multi-threaded proactors
    if (error == EAGAIN) // 11: happens on Linux
      goto receive;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if ((error != ENXIO)                && // 6 : *TODO*
        (error != EFAULT)               && // 14: *TODO*
        (error != ERROR_UNEXP_NET_ERR)  && // 59: *TODO*
        (error != ERROR_NETNAME_DELETED))  // 64: *TODO*
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("0x%@: failed to ACE_Asynch_Read_Dgram::recv(): \"%m\", aborting\n"),
                  inherited2::get_handle ()));
#else
    if (error)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%d: failed to ACE_Asynch_Read_Dgram::recv(): \"%m\", aborting\n"),
                  inherited2::get_handle ()));
#endif

    // clean up
    message_block_p->release ();

    return false;
  } // end IF

  return true;
}

template <typename SocketType,
          typename ConfigurationType>
void
Net_AsynchUDPSocketHandler_T<SocketType,
                             ConfigurationType>::handle_write_dgram (const ACE_Asynch_Write_Dgram::Result& result_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_AsynchUDPSocketHandler_T::handle_write_dgram"));

  int result = -1;
  bool close = false;

  // sanity check
  if (unlikely (!result_in.success ()))
  {
    unsigned long error = result_in.error ();
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if ((error != EPIPE)                 &&     // 32
        //(error != ERROR_INVALID_NETNAME) &&     // 1214: most probable reason: socket not connect()ed to a valid address
        //(error != ERROR_INVALID_USER_BUFFER) && // 1784
        (error != ECONNRESET))                  // 10054
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to write to output stream (handle was: 0x%@, address: %s): \"%s\", aborting\n"),
                  result_in.handle (), ACE_TEXT (Net_Common_Tools::IPAddressToString (Net_Common_Tools::getBoundAddress (result_in.handle ())).c_str ()),
                  ACE_TEXT (Common_Tools::errorToString (static_cast<DWORD> (error), false).c_str ())));
#else
    if ((error != EBADF) && // 9 : Linux (local close())
        (error != EPIPE))   // 32:
//        (error != EDESTADDRREQ) // 89:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to write to output stream (handle was: %d, address: %s): \"%s\", aborting\n"),
                  result_in.handle (), ACE_TEXT (Net_Common_Tools::IPAddressToString (Net_Common_Tools::getBoundAddress (result_in.handle ())).c_str ()),
                  ACE_TEXT (ACE_OS::strerror (error))));
#endif
  } // end IF

  switch (static_cast<int> (result_in.bytes_transferred ()))
  {
    case -1:
    case 0:
    {
      ACE_ASSERT (!result_in.success ());
      //      error = result_in.error ();
//      if ((error != ECONNRESET) &&
//          (error != EPIPE)      &&
//          (error != EBADF)) // 9 happens on Linux (local close())
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("failed to write to output stream (handle was: 0x%@): \"%s\", aborting\n"),
//                    result_in.handle (),
//                    ACE::sock_error (static_cast<int> (error))));
//#else
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("failed to write to output stream (handle was: %d): \"%s\", aborting\n"),
//                    result_in.handle (),
//                    ACE_TEXT (ACE_OS::strerror (error))));
//#endif
      close = true;
      break;
    }
    // *** GOOD CASES ***
    default:
    {
      // finished with this buffer ?
      if (unlikely (result_in.message_block ()->length () > 0))
      {
        // --> reschedule
        // *TODO*: this is broken
        ACE_ASSERT (false);
        result = this->handle_output (result_in.handle ());
        if (result == -1)
        {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Net_AsynchUDPSocketHandler_T::handle_output(0x%@): \"%m\", aborting\n"),
                      result_in.handle ()));
#else
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Net_AsynchUDPSocketHandler_T::handle_output(%d): \"%m\", aborting\n"),
                      result_in.handle ()));
#endif
          close = true;
        } // end IF
      } // end IF

      break;
    }
  } // end SWITCH

  result_in.message_block ()->release ();

  if (unlikely (close))
  {
    result = handle_close (result_in.handle (),
                           ACE_Event_Handler::ALL_EVENTS_MASK);
    if (unlikely (result == -1))
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_AsynchUDPSocketHandler_T::handle_close(0x%@,%d): \"%m\", continuing\n"),
                  result_in.handle (),
                  ACE_Event_Handler::ALL_EVENTS_MASK));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_AsynchUDPSocketHandler_T::handle_close(%d,%d): \"%m\", continuing\n"),
                  result_in.handle (),
                  ACE_Event_Handler::ALL_EVENTS_MASK));
#endif
    } // end IF
  } // end IF

  counter_.decrease ();
}
