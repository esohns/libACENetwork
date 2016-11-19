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

#include <ace/Log_Msg.h>

#include "net_macros.h"

template <typename AddressType,
          typename ConfigurationType,
          typename ConnectionStateType,
          typename StatisticContainerType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename ConnectionType,
          typename ConnectionManagerType,
          typename ConnectorType,
          typename StateType,
          typename SessionInterfaceType>
Net_SessionBase_T<AddressType,
                  ConfigurationType,
                  ConnectionStateType,
                  StatisticContainerType,
                  SocketConfigurationType,
                  HandlerConfigurationType,
                  ConnectionType,
                  ConnectionManagerType,
                  ConnectorType,
                  StateType,
                  SessionInterfaceType>::Net_SessionBase_T (const HandlerConfigurationType& configuration_in,
                                                            ConnectionManagerType* interfaceHandle_in,
                                                            bool asynchronous_in)
 : configuration_ (&const_cast<HandlerConfigurationType&> (configuration_in))
 , connectionManager_ (interfaceHandle_in)
 , isAsynch_ (asynchronous_in)
 , lock_ ()
 , state_ ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_SessionBase_T::Net_SessionBase_T"));

}

template <typename AddressType,
          typename ConfigurationType,
          typename ConnectionStateType,
          typename StatisticContainerType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename ConnectionType,
          typename ConnectionManagerType,
          typename ConnectorType,
          typename StateType,
          typename SessionInterfaceType>
Net_SessionBase_T<AddressType,
                  ConfigurationType,
                  ConnectionStateType,
                  StatisticContainerType,
                  SocketConfigurationType,
                  HandlerConfigurationType,
                  ConnectionType,
                  ConnectionManagerType,
                  ConnectorType,
                  StateType,
                  SessionInterfaceType>::~Net_SessionBase_T ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_SessionBase_T::~Net_SessionBase_T"));

//  int result = -1;

//  if (close_)
//  {
    //result = file_.close ();
    //if (result == -1)
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to ACE_FILE_Stream::close(): \"%m\", continuing\n")));
//    result = output_.close ();
//    if (result == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to BitTorrent_Client_IOStream_t::close(): \"%m\", continuing\n")));
//  } // end IF
}

template <typename AddressType,
          typename ConfigurationType,
          typename ConnectionStateType,
          typename StatisticContainerType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename ConnectionType,
          typename ConnectionManagerType,
          typename ConnectorType,
          typename StateType,
          typename SessionInterfaceType>
void
Net_SessionBase_T<AddressType,
                  ConfigurationType,
                  ConnectionStateType,
                  StatisticContainerType,
                  SocketConfigurationType,
                  HandlerConfigurationType,
                  ConnectionType,
                  ConnectionManagerType,
                  ConnectorType,
                  StateType,
                  SessionInterfaceType>::connect (const AddressType& address_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_SessionBase_T::connect"));

  ConnectorType connector (connectionManager_,
                           ACE_Time_Value::zero);
  ACE_HANDLE handle = ACE_INVALID_HANDLE;

  // debug info
  ACE_TCHAR buffer[BUFSIZ];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  int result = address_in.addr_to_string (buffer,
                                          sizeof (buffer));
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));

  // step1: initialize connector
  typename ConnectorType::ICONNECTOR_T* iconnector_p = &connector;
  ACE_ASSERT (configuration_);
  if (!iconnector_p->initialize (*configuration_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize connector: \"%m\", returning\n")));
    return;
  } // end IF

  // step2: try to connect
  handle = iconnector_p->connect (address_in);
  if (isAsynch_)
  {

  } // end ELSE
  else
  if (handle == ACE_INVALID_HANDLE)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to connect to \"%s\": \"%m\", returning\n"),
                buffer));
    return;
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("connected to \"%s\"...\n"),
              buffer));
}

template <typename AddressType,
          typename ConfigurationType,
          typename ConnectionStateType,
          typename StatisticContainerType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename ConnectionType,
          typename ConnectionManagerType,
          typename ConnectorType,
          typename StateType,
          typename SessionInterfaceType>
void
Net_SessionBase_T<AddressType,
                  ConfigurationType,
                  ConnectionStateType,
                  StatisticContainerType,
                  SocketConfigurationType,
                  HandlerConfigurationType,
                  ConnectionType,
                  ConnectionManagerType,
                  ConnectorType,
                  StateType,
                  SessionInterfaceType>::disconnect (const AddressType& address_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_SessionBase_T::disconnect"));

  // sanity check(s)
  ACE_ASSERT (connectionManager_);

  // debug info
  ACE_TCHAR buffer[BUFSIZ];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  int result = address_in.addr_to_string (buffer,
                                          sizeof (buffer));
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));

  ConnectionType* connection_p = connectionManager_->get (address_in);
  if (!connection_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("connection (peer was: \"%s\") not found, returning\n"),
                buffer));
    return;
  } // end IF

  connection_p->close ();
  connection_p->decrease ();
}

//////////////////////////////////////////

template <typename AddressType,
          typename ConfigurationType,
          typename ConnectionStateType,
          typename StatisticContainerType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename ConnectionType,
          typename ConnectionManagerType,
          typename ConnectorType,
          typename StateType,
          typename SessionInterfaceType>
void
Net_SessionBase_T<AddressType,
                  ConfigurationType,
                  ConnectionStateType,
                  StatisticContainerType,
                  SocketConfigurationType,
                  HandlerConfigurationType,
                  ConnectionType,
                  ConnectionManagerType,
                  ConnectorType,
                  StateType,
                  SessionInterfaceType>::connect (Net_ConnectionId_t id_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_SessionBase_T::connect"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("new connection (id was: %d)...\n"),
              id_in));
}

template <typename AddressType,
          typename ConfigurationType,
          typename ConnectionStateType,
          typename StatisticContainerType,
          typename SocketConfigurationType,
          typename HandlerConfigurationType,
          typename ConnectionType,
          typename ConnectionManagerType,
          typename ConnectorType,
          typename StateType,
          typename SessionInterfaceType>
void
Net_SessionBase_T<AddressType,
                  ConfigurationType,
                  ConnectionStateType,
                  StatisticContainerType,
                  SocketConfigurationType,
                  HandlerConfigurationType,
                  ConnectionType,
                  ConnectionManagerType,
                  ConnectorType,
                  StateType,
                  SessionInterfaceType>::disconnect (Net_ConnectionId_t id_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_SessionBase_T::disconnect"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("connection lost (id was: %d)...\n"),
              id_in));
}
