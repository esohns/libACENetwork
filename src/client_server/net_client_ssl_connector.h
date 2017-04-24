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

#ifndef NET_CLIENT_SSL_CONNECTOR_H
#define NET_CLIENT_SSL_CONNECTOR_H

#include <ace/Global_Macros.h>
#include <ace/INET_Addr.h>
#include <ace/Time_Value.h>
#include <ace/SSL/SSL_SOCK_Connector.h>

#include "stream_statemachine_control.h"

#include "net_connection_manager.h"
#include "net_iconnectionmanager.h"
#include "net_iconnector.h"

template <typename HandlerType,
          typename ConnectorType, // ACE_SSL_SOCK_Connector
          ////////////////////////////////
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          ////////////////////////////////
          typename HandlerConfigurationType,
          ////////////////////////////////
          typename StreamType,
          ////////////////////////////////
          typename UserDataType>
class Net_Client_SSL_Connector_T
 : public ACE_SSL_SOCK_Connector
 , public Net_IConnector_T<AddressType,
                           HandlerConfigurationType>
{
 public:
  typedef AddressType ADDRESS_T;
  typedef StreamType STREAM_T;

  typedef Net_IConnection_T<AddressType,
                            ConfigurationType,
                            StateType,
                            StatisticContainerType> ICONNECTION_T;
//  typedef Net_ISocketConnection_T<AddressType,
//                                  ConfigurationType,
//                                  StateType,
//                                  StatisticContainerType,
//                                  struct Net_SocketConfiguration,
//                                  HandlerConfigurationType> ISOCKET_CONNECTION_T;
  typedef Net_IStreamConnection_T<AddressType,
                                  ConfigurationType,
                                  StateType,
                                  StatisticContainerType,
                                  struct Net_SocketConfiguration,
                                  HandlerConfigurationType,
                                  StreamType,
                                  enum Stream_StateMachine_ControlState> ISTREAM_CONNECTION_T;

  typedef Net_Connection_Manager_T<AddressType,
                                   ConfigurationType,
                                   StateType,
                                   StatisticContainerType,
                                   UserDataType> CONNECTION_MANAGER_T;
  typedef Net_IConnectionManager_T<AddressType,
                                   ConfigurationType,
                                   StateType,
                                   StatisticContainerType,
                                   UserDataType> ICONNECTION_MANAGER_T;

  typedef Net_IConnector_T<AddressType,
                           HandlerConfigurationType> ICONNECTOR_T;

  Net_Client_SSL_Connector_T (ICONNECTION_MANAGER_T* = NULL,                 // connection manager handle
                              const ACE_Time_Value& = ACE_Time_Value::zero); // statistic collecting interval [ACE_Time_Value::zero: off]
  virtual ~Net_Client_SSL_Connector_T ();

  // implement Net_Client_IConnector_T
  virtual enum Net_TransportLayerType transportLayer () const;
  inline virtual bool useReactor () const { return true; };

  // *NOTE*: handlers retrieve the configuration object with get ()
  inline virtual bool initialize (const HandlerConfigurationType& configuration_in) { configuration_ = const_cast<HandlerConfigurationType&> (configuration_in); return true; };
  inline virtual const HandlerConfigurationType& get () const { return configuration_; };

  virtual void abort ();
  virtual ACE_HANDLE connect (const AddressType&);

 protected:
  // override default activation strategy
  virtual int activate_svc_handler (HandlerType*);
  // override default instantiation strategy
  virtual int make_svc_handler (HandlerType*&);

 private:
  typedef ACE_SSL_SOCK_Connector inherited;

  ACE_UNIMPLEMENTED_FUNC (Net_Client_SSL_Connector_T ())
  ACE_UNIMPLEMENTED_FUNC (Net_Client_SSL_Connector_T (const Net_Client_SSL_Connector_T&))
  ACE_UNIMPLEMENTED_FUNC (Net_Client_SSL_Connector_T& operator= (const Net_Client_SSL_Connector_T&))

  typedef Net_Client_SSL_Connector_T<HandlerType,
                                     ConnectorType,
                                     AddressType,
                                     ConfigurationType,
                                     StateType,
                                     StatisticContainerType,
                                     HandlerConfigurationType,
                                     StreamType,
                                     UserDataType> OWN_TYPE_T;

  HandlerConfigurationType configuration_;

  ICONNECTION_MANAGER_T*   connectionManager_;
  ACE_Time_Value           statisticCollectionInterval_;
};

// include template definition
#include "net_client_ssl_connector.inl"

#endif
