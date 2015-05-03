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

#include "net_connection_manager_common.h"
#include "net_macros.h"

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename SocketHandlerType>
Net_Client_AsynchConnector_T<AddressType,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             SocketHandlerType>::Net_Client_AsynchConnector_T (const SocketHandlerConfigurationType* configuration_in,
                                                                               ICONNECTION_MANAGER_T* interfaceHandle_in,
                                                                               unsigned int statisticCollectionInterval_in)
 : configuration_ (configuration_in)
 , interfaceHandle_ (interfaceHandle_in)
 , statisticCollectionInterval_ (statisticCollectionInterval_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::Net_Client_AsynchConnector_T"));

  int result = -1;

  // initialize base class
  result = inherited::open (true,  // pass addresses ?
                            NULL,  // default proactor
                            true); // validate new connections ?
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Asynch_Connector::open(): \"%m\", continuing\n")));
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename SocketHandlerType>
Net_Client_AsynchConnector_T<AddressType,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             SocketHandlerType>::~Net_Client_AsynchConnector_T ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::~Net_Client_AsynchConnector_T"));

}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename SocketHandlerType>
int
Net_Client_AsynchConnector_T<AddressType,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             SocketHandlerType>::validate_connection (const ACE_Asynch_Connect::Result& result_in,
                                                                      const ACE_INET_Addr& remoteSAP_in,
                                                                      const ACE_INET_Addr& localSAP_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::validate_connection"));

  // *NOTE*: on error, the addresses are not passed through...

  int result = -1;

  // success ?
  result = result_in.success ();
  if (result != 1)
  {
    ACE_TCHAR buffer[BUFSIZ];
    ACE_OS::memset (buffer, 0, sizeof (buffer));
    result = remoteSAP_in.addr_to_string (buffer, sizeof (buffer));
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Client_AsynchConnector_T::connect(\"%s\"): \"%s\", aborting\n"),
                buffer,
                ACE_OS::strerror (result_in.error ())));
  } // end IF

  return ((result_in.success () == 1) ? 0 : -1);
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename SocketHandlerType>
const SocketHandlerConfigurationType*
Net_Client_AsynchConnector_T<AddressType,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             SocketHandlerType>::getConfiguration () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::getConfiguration"));

  return configuration_;
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename SocketHandlerType>
bool
Net_Client_AsynchConnector_T<AddressType,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             SocketHandlerType>::connect (const AddressType& address_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::connect"));

  int result = -1;

  // sanity check(s)
  ACE_ASSERT (configuration_);

  const void* act_p = reinterpret_cast<const void*> (configuration_);
  result =
      inherited::connect (address_in,                            // remote SAP
                          ACE_sap_any_cast (const AddressType&), // local SAP
                          1,                                     // re-use address (SO_REUSEADDR) ?
                          act_p);                                // asynchronous completion token
  if (result == -1)
  {
    ACE_TCHAR buffer[BUFSIZ];
    ACE_OS::memset (buffer, 0, sizeof (buffer));
    // *TODO*: find a replacement for ACE_Netlink_Addr::addr_to_string
//    result = address_in.addr_to_string (buffer, sizeof (buffer));
//    if (result == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to AddressType::addr_to_string(): \"%m\", continuing\n")));
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Asynch_Connector::connect(\"%s\"): \"%m\", aborting\n"),
                buffer));
    return false;
  } // end IF

  return true;
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename SocketHandlerType>
void
Net_Client_AsynchConnector_T<AddressType,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             SocketHandlerType>::abort ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::abort"));

  int result = -1;

  result = inherited::cancel ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Asynch_Connector::cancel(): \"%m\", continuing\n")));
}

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename SocketHandlerType>
SocketHandlerType*
Net_Client_AsynchConnector_T<AddressType,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             SocketHandlerType>::make_handler (void)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::make_handler"));

  // init return value(s)
  SocketHandlerType* handler_p = NULL;

  ACE_NEW_NORETURN (handler_p,
                    SocketHandlerType (interfaceHandle_,
                                    statisticCollectionInterval_));
  if (!handler_p)
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));

  return handler_p;
}

/////////////////////////////////////////

template <typename SocketConfigurationType,
          typename SocketHandlerType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType>
Net_Client_AsynchConnector_T<ACE_INET_Addr,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             Net_AsynchUDPConnection_T<UserDataType,
                                                       SessionDataType,
                                                       SocketHandlerType> >::Net_Client_AsynchConnector_T (const SocketHandlerConfigurationType* configuration_in,
                                                                                                           ICONNECTION_MANAGER_T* interfaceHandle_in,
                                                                                                           unsigned int statisticCollectionInterval_in)
 : configuration_ (configuration_in)
 , interfaceHandle_ (interfaceHandle_in)
 , statisticCollectionInterval_ (statisticCollectionInterval_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::Net_Client_AsynchConnector_T"));

  int result = -1;

  // initialize base class
  result = inherited::open (true,  // pass addresses ?
                            NULL,  // default proactor
                            true); // validate new connections ?
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Asynch_Connector::open(): \"%m\", continuing\n")));
}

template <typename SocketConfigurationType,
          typename SocketHandlerType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType>
Net_Client_AsynchConnector_T<ACE_INET_Addr,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             Net_AsynchUDPConnection_T<UserDataType,
                                                       SessionDataType,
                                                       SocketHandlerType> >::~Net_Client_AsynchConnector_T ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::~Net_Client_AsynchConnector_T"));

}

template <typename SocketConfigurationType,
          typename SocketHandlerType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType>
int
Net_Client_AsynchConnector_T<ACE_INET_Addr,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             Net_AsynchUDPConnection_T<UserDataType,
                                                       SessionDataType,
                                                       SocketHandlerType> >::validate_connection (const ACE_Asynch_Connect::Result& result_in,
                                                                                                  const ACE_INET_Addr& remoteSAP_in,
                                                                                                  const ACE_INET_Addr& localSAP_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::validate_connection"));

  // *NOTE*: on error, the addresses are not passed through...

  int result = -1;

  // success ?
  result = result_in.success ();
  if (result != 1)
  {
    ACE_TCHAR buffer[BUFSIZ];
    ACE_OS::memset (buffer, 0, sizeof (buffer));
    result = remoteSAP_in.addr_to_string (buffer, sizeof (buffer));
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Client_AsynchConnector_T::connect(\"%s\"): \"%s\", aborting\n"),
                buffer,
                ACE_OS::strerror (result_in.error ())));
  } // end IF

  return ((result_in.success () == 1) ? 0 : -1);
}

template <typename SocketConfigurationType,
          typename SocketHandlerType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType>
const SocketHandlerConfigurationType*
Net_Client_AsynchConnector_T<ACE_INET_Addr,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             Net_AsynchUDPConnection_T<UserDataType,
                                                       SessionDataType,
                                                       SocketHandlerType> >::getConfiguration () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::getConfiguration"));

  return configuration_;
}

template <typename SocketConfigurationType,
          typename SocketHandlerType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType>
void
Net_Client_AsynchConnector_T<ACE_INET_Addr,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             Net_AsynchUDPConnection_T<UserDataType,
                                                       SessionDataType,
                                                       SocketHandlerType> >::abort ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::abort"));

  int result = -1;

  result = inherited::cancel ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Asynch_Connector::cancel(): \"%m\", continuing\n")));
}

template <typename SocketConfigurationType,
          typename SocketHandlerType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType>
bool
Net_Client_AsynchConnector_T<ACE_INET_Addr,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             Net_AsynchUDPConnection_T<UserDataType,
                                                       SessionDataType,
                                                       SocketHandlerType> >::connect (const ACE_INET_Addr& address_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::connect"));

  CONNECTION_T* handler_p = NULL;
  handler_p = make_handler ();
  if (!handler_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Client_AsynchConnector_T<Net_AsynchUDPConnection>::make_handler(): \"%m\", aborting\n")));
    return false;
  } // end IF
  ACE_ASSERT (handler_p);
  handler_p->act (configuration_);

  // *NOTE*: the handler registers with the reactor and will be freed (or
  //         recycled) automatically
  ACE_Message_Block message_block;
  handler_p->open (ACE_INVALID_HANDLE,
                   message_block);

  return true;
}

template <typename SocketConfigurationType,
          typename SocketHandlerType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType>
Net_AsynchUDPConnection_T<UserDataType,
                          SessionDataType,
                          SocketHandlerType>*
Net_Client_AsynchConnector_T<ACE_INET_Addr,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             Net_AsynchUDPConnection_T<UserDataType,
                                                       SessionDataType,
                                                       SocketHandlerType> >::make_handler (void)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::make_handler"));

  // init return value(s)
  CONNECTION_T* handler_p = NULL;

  ACE_NEW_NORETURN (handler_p,
                    CONNECTION_T (interfaceHandle_,
                                  statisticCollectionInterval_));
  if (!handler_p)
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));

  return handler_p;
}

/////////////////////////////////////////

#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
template <typename SocketConfigurationType,
          typename SocketHandlerType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType>
Net_Client_AsynchConnector_T<ACE_Netlink_Addr,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             SocketHandlerType>::Net_Client_AsynchConnector_T (const SocketHandlerConfigurationType* configuration_in,
                                                                               ICONNECTION_MANAGER_T* interfaceHandle_in,
                                                                               unsigned int statisticCollectionInterval_in)
 : configuration_ (configuration_in)
 , interfaceHandle_ (interfaceHandle_in)
 , statisticCollectionInterval_ (statisticCollectionInterval_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::Net_Client_AsynchConnector_T"));

  int result = -1;

  // initialize base class
  result = inherited::open (true,  // pass addresses ?
                            NULL,  // default proactor
                            true); // validate new connections ?
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Asynch_Connector::open(): \"%m\", continuing\n")));
}

template <typename SocketConfigurationType,
          typename SocketHandlerType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType>
Net_Client_AsynchConnector_T<ACE_Netlink_Addr,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             SocketHandlerType>::~Net_Client_AsynchConnector_T ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::~Net_Client_AsynchConnector_T"));

}

template <typename SocketConfigurationType,
          typename SocketHandlerType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType>
int
Net_Client_AsynchConnector_T<ACE_Netlink_Addr,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             SocketHandlerType>::validate_connection (const ACE_Asynch_Connect::Result& result_in,
                                                                      const ACE_Netlink_Addr& remoteSAP_in,
                                                                      const ACE_Netlink_Addr& localSAP_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::validate_connection"));

  // *NOTE*: on error, the addresses are not passed through...

  int result = -1;

  // success ?
  result = result_in.success ();
  if (result != 1)
  {
    ACE_TCHAR buffer[BUFSIZ];
    ACE_OS::memset (buffer, 0, sizeof (buffer));
    // *TODO*: find a replacement for ACE_INET_Addr::addr_to_string
//    result = remoteSAP_in.addr_to_string (buffer, sizeof (buffer));
//    if (result == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Client_AsynchConnector_T::connect(\"%s\"): \"%s\", aborting\n"),
                buffer,
                ACE_OS::strerror (result_in.error ())));
  } // end IF

  return ((result_in.success () == 1) ? 0 : -1);
}

template <typename SocketConfigurationType,
          typename SocketHandlerType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType>
const SocketHandlerConfigurationType*
Net_Client_AsynchConnector_T<ACE_Netlink_Addr,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             SocketHandlerType>::getConfiguration () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::getConfiguration"));

  return configuration_;
}

template <typename SocketConfigurationType,
          typename SocketHandlerType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType>
void
Net_Client_AsynchConnector_T<ACE_Netlink_Addr,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             SocketHandlerType>::abort ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::abort"));

  int result = -1;

  result = inherited::cancel ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Asynch_Connector::cancel(): \"%m\", continuing\n")));
}

template <typename SocketConfigurationType,
          typename SocketHandlerType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType>
bool
Net_Client_AsynchConnector_T<ACE_Netlink_Addr,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             SocketHandlerType>::connect (const ACE_Netlink_Addr& address_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::connect"));

  SocketHandlerType* handler_p = NULL;
  handler_p = make_handler ();
  if (!handler_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Client_AsynchConnector_T::make_handler(): \"%m\", aborting\n")));
    return false;
  } // end IF
  ACE_ASSERT (handler_p);
  handler_p->act (configuration_);

  // *NOTE*: the handler registers with the reactor and will be freed (or
  //         recycled) automatically
  ACE_Message_Block message_block;
  handler_p->open (ACE_INVALID_HANDLE,
                   message_block);

  return true;
}

template <typename SocketConfigurationType,
          typename SocketHandlerType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType>
SocketHandlerType*
Net_Client_AsynchConnector_T<ACE_Netlink_Addr,
                             SocketConfigurationType,
                             ConfigurationType,
                             SocketHandlerConfigurationType,
                             UserDataType,
                             SessionDataType,
                             SocketHandlerType>::make_handler (void)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_AsynchConnector_T::make_handler"));

  // init return value(s)
  SocketHandlerType* handler_p = NULL;

  ACE_NEW_NORETURN (handler_p,
                    SocketHandlerType (interfaceHandle_,
                                       statisticCollectionInterval_));
  if (!handler_p)
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));

  return handler_p;
}
#endif
