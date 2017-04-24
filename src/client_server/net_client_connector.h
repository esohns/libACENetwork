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

#ifndef NET_CLIENT_CONNECTOR_H
#define NET_CLIENT_CONNECTOR_H

#include <ace/Connector.h>
#include <ace/Global_Macros.h>
#include <ace/INET_Addr.h>
#include <ace/Time_Value.h>

#include "stream_statemachine_control.h"

#include "net_connection_manager.h"
#include "net_iconnectionmanager.h"

#include "net_iconnector.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "net_netlinksockethandler.h"
#endif
#include "net_udpconnection_base.h"

template <typename HandlerType,
          typename ConnectorType, // ACE_SOCK_CONNECTOR
          ////////////////////////////////
          typename AddressType,
          typename ConfigurationType, // connection-
          typename StateType, // connection-
          typename StatisticContainerType,
          ////////////////////////////////
          typename HandlerConfigurationType, // socket-
          ////////////////////////////////
          typename StreamType,
          ////////////////////////////////
          typename UserDataType>
class Net_Client_Connector_T
 : public ACE_Connector<HandlerType,
                        ConnectorType>
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

  Net_Client_Connector_T (ICONNECTION_MANAGER_T* = NULL,                 // connection manager handle
                          const ACE_Time_Value& = ACE_Time_Value::zero); // statistic collecting interval [ACE_Time_Value::zero: off]
  virtual ~Net_Client_Connector_T ();

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
  typedef ACE_Connector<HandlerType,
                        ConnectorType> inherited;

  typedef Net_Client_Connector_T<HandlerType,
                                 ConnectorType,
                                 AddressType,
                                 ConfigurationType,
                                 StateType,
                                 StatisticContainerType,
                                 HandlerConfigurationType,
                                 StreamType,
                                 UserDataType> OWN_TYPE_T;

  ACE_UNIMPLEMENTED_FUNC (Net_Client_Connector_T ())
  ACE_UNIMPLEMENTED_FUNC (Net_Client_Connector_T (const Net_Client_Connector_T&))
  ACE_UNIMPLEMENTED_FUNC (Net_Client_Connector_T& operator= (const Net_Client_Connector_T&))

  HandlerConfigurationType configuration_;

  ICONNECTION_MANAGER_T*   connectionManager_;
  ACE_Time_Value           statisticCollectionInterval_;
};

//////////////////////////////////////////

// (partial) specializations (for UDP)
template <typename HandlerType,
          typename ConnectorType, // ACE_SOCK_CONNECTOR
          ////////////////////////////////
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          ////////////////////////////////
          typename HandlerConfigurationType, // socket-
          ////////////////////////////////
          typename StreamType,
          ////////////////////////////////
          typename UserDataType>
class Net_Client_Connector_T<Net_UDPConnectionBase_T<HandlerType,
                                                     ConfigurationType,
                                                     StateType,
                                                     StatisticContainerType,
                                                     HandlerConfigurationType,
                                                     StreamType,
                                                     UserDataType>,
                             ConnectorType,
                             ACE_INET_Addr,
                             ConfigurationType,
                             StateType,
                             StatisticContainerType,
                             HandlerConfigurationType,
                             StreamType,
                             UserDataType>
 : public Net_IConnector_T<ACE_INET_Addr,
                           HandlerConfigurationType>
{
 public:
  typedef StreamType STREAM_T;

  typedef Net_IConnection_T<ACE_INET_Addr,
                            ConfigurationType,
                            StateType,
                            StatisticContainerType> ICONNECTION_T;
  typedef Net_UDPConnectionBase_T<HandlerType,
                                  ConfigurationType,
                                  StateType,
                                  StatisticContainerType,
                                  HandlerConfigurationType,
                                  StreamType,
                                  UserDataType> CONNECTION_T;
//  typedef Net_ISocketConnection_T<ACE_INET_Addr,
//                                  ConfigurationType,
//                                  StateType,
//                                  StatisticContainerType,
//                                  struct Net_SocketConfiguration,
//                                  HandlerConfigurationType> ISOCKET_CONNECTION_T;
  typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                  ConfigurationType,
                                  StateType,
                                  StatisticContainerType,
                                  struct Net_SocketConfiguration,
                                  HandlerConfigurationType,
                                  StreamType,
                                  enum Stream_StateMachine_ControlState> ISTREAM_CONNECTION_T;

  typedef Net_Connection_Manager_T<ACE_INET_Addr,
                                   ConfigurationType,
                                   StateType,
                                   StatisticContainerType,
                                   UserDataType> CONNECTION_MANAGER_T;
  typedef Net_IConnectionManager_T<ACE_INET_Addr,
                                   ConfigurationType,
                                   StateType,
                                   StatisticContainerType,
                                   UserDataType> ICONNECTION_MANAGER_T;

  typedef Net_IConnector_T<ACE_INET_Addr,
                           HandlerConfigurationType> ICONNECTOR_T;

  Net_Client_Connector_T (ICONNECTION_MANAGER_T*,                        // connection manager handle
                          const ACE_Time_Value& = ACE_Time_Value::zero); // statistic collecting interval [ACE_Time_Value::zero: off]
  virtual ~Net_Client_Connector_T ();

  // implement Net_Client_IConnector_T
  inline virtual enum Net_TransportLayerType transportLayer () const { return NET_TRANSPORTLAYER_UDP; };
  inline virtual bool useReactor () const { return true; };

  // *NOTE*: handlers retrieve the configuration object with get ()
  inline virtual bool initialize (const HandlerConfigurationType& configuration_in) { configuration_ = const_cast<HandlerConfigurationType&> (configuration_in); return true; };
  inline virtual const HandlerConfigurationType& get () const { return configuration_; };

  // *NOTE*: this is just a stub
  inline virtual void abort () { };
  // specialize (part of) Net_Client_IConnector_T
  virtual ACE_HANDLE connect (const ACE_INET_Addr&);

 protected:
  //typedef ACE_Connector<CONNECTION_T,
  //                      ACE_SOCK_CONNECTOR> ACE_CONNECTOR_T;

  // override default activation strategy
  virtual int activate_svc_handler (CONNECTION_T*);
  // override default instantiation strategy
  virtual int make_svc_handler (CONNECTION_T*&);
  //virtual int connect_svc_handler (CONNECTION_T*&,
  //                                 const ACE_SOCK_Connector::PEER_ADDR&,
  //                                 ACE_Time_Value*,
  //                                 const ACE_SOCK_Connector::PEER_ADDR&,
  //                                 int,
  //                                 int,
  //                                 int);
  //// *TODO*: it's not quite clear what this API does (see Connector.h:514),
  ////         needs overriding to please the compiler...
  //virtual int connect_svc_handler (CONNECTION_T*&,
  //                                 CONNECTION_T*&,
  //                                 const ACE_SOCK_Connector::PEER_ADDR&,
  //                                 ACE_Time_Value*,
  //                                 const ACE_SOCK_Connector::PEER_ADDR&,
  //                                 int,
  //                                 int,
  //                                 int);

 private:
  ACE_UNIMPLEMENTED_FUNC (Net_Client_Connector_T ())
  ACE_UNIMPLEMENTED_FUNC (Net_Client_Connector_T (const Net_Client_Connector_T&))
  ACE_UNIMPLEMENTED_FUNC (Net_Client_Connector_T& operator= (const Net_Client_Connector_T&))

  HandlerConfigurationType configuration_;

  ICONNECTION_MANAGER_T*   connectionManager_;
  ACE_Time_Value           statisticCollectionInterval_;
};

//////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
// partial specialization (for Netlink)
template <typename HandlerType,
          typename ConnectorType, // ACE_SOCK_CONNECTOR
          ////////////////////////////////
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          ////////////////////////////////
          typename HandlerConfigurationType, // socket-
          ////////////////////////////////
          typename StreamType,
          ////////////////////////////////
          typename UserDataType>
class Net_Client_Connector_T<HandlerType,
                             ConnectorType,
                             Net_Netlink_Addr,
                             ConfigurationType,
                             StateType,
                             StatisticContainerType,
                             HandlerConfigurationType,
                             StreamType,
                             UserDataType>
 : public Net_IConnector_T<Net_Netlink_Addr,
                           HandlerConfigurationType>
{
 public:
  typedef StreamType STREAM_T;

  typedef Net_IConnection_T<Net_Netlink_Addr,
                            ConfigurationType,
                            StateType,
                            StatisticContainerType> ICONNECTION_T;
//  typedef Net_ISocketConnection_T<Net_Netlink_Addr,
//                                  ConfigurationType,
//                                  StateType,
//                                  StatisticContainerType,
//                                  struct Net_SocketConfiguration,
//                                  HandlerConfigurationType> ISOCKET_CONNECTION_T;
  typedef Net_IStreamConnection_T<Net_Netlink_Addr,
                                  ConfigurationType,
                                  StateType,
                                  StatisticContainerType,
                                  struct Net_SocketConfiguration,
                                  HandlerConfigurationType,
                                  StreamType,
                                  enum Stream_StateMachine_ControlState> ISTREAM_CONNECTION_T;

  typedef Net_Connection_Manager_T<Net_Netlink_Addr,
                                   ConfigurationType,
                                   StateType,
                                   StatisticContainerType,
                                   UserDataType> CONNECTION_MANAGER_T;
  typedef Net_IConnectionManager_T<Net_Netlink_Addr,
                                   ConfigurationType,
                                   StateType,
                                   StatisticContainerType,
                                   UserDataType> ICONNECTION_MANAGER_T;

  typedef Net_IConnector_T<Net_Netlink_Addr,
                           HandlerConfigurationType> ICONNECTOR_T;

  Net_Client_Connector_T (ICONNECTION_MANAGER_T*,                        // connection manager handle
                          const ACE_Time_Value& = ACE_Time_Value::zero); // statistic collecting interval [ACE_Time_Value::zero: off]
  virtual ~Net_Client_Connector_T ();

  // implement Net_Client_IConnector_T
  inline virtual enum Net_TransportLayerType transportLayer () const { return NET_TRANSPORTLAYER_NETLINK; };
  inline virtual bool useReactor () const { return true; };

  // *NOTE*: handlers retrieve the configuration object with get ()
  inline virtual bool initialize (const HandlerConfigurationType& configuration_in) { configuration_ = const_cast<HandlerConfigurationType&> (configuration_in); return true; };
  inline virtual const HandlerConfigurationType& get () const { return configuration_; };

  // *NOTE*: this is just a stub
  inline virtual void abort () { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) };
  virtual ACE_HANDLE connect (const Net_Netlink_Addr&);

 protected:
  // override default instantiation strategy
  virtual int make_svc_handler (HandlerType*&);

 private:
  ACE_UNIMPLEMENTED_FUNC (Net_Client_Connector_T ())
  ACE_UNIMPLEMENTED_FUNC (Net_Client_Connector_T (const Net_Client_Connector_T&))
  ACE_UNIMPLEMENTED_FUNC (Net_Client_Connector_T& operator= (const Net_Client_Connector_T&))

  HandlerConfigurationType configuration_;

  ICONNECTION_MANAGER_T*   connectionManager_;
  ACE_Time_Value           statisticCollectionInterval_;
};
#endif

// include template definition
#include "net_client_connector.inl"

#endif
