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

#ifndef NET_CLIENT_ASYNCHCONNECTOR_H
#define NET_CLIENT_ASYNCHCONNECTOR_H

#include "ace/Asynch_Connector.h"
#include "ace/Global_Macros.h"
#include "ace/INET_Addr.h"
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#include "ace/Netlink_Addr.h"
#endif

#include "net_iconnectionmanager.h"
#include "net_iconnector.h"
//#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
//#include "net_netlinkconnection_base.h"
//#endif
#include "net_udpconnection_base.h"

template <typename HandlerType,
          ///////////////////////////////
          typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          ///////////////////////////////
          typename StreamType,
          ///////////////////////////////
          typename HandlerConfigurationType,
          ///////////////////////////////
          typename UserDataType>
class Net_Client_AsynchConnector_T
 : public ACE_Asynch_Connector<HandlerType>
 , public Net_IConnector_T<AddressType,
                           HandlerConfigurationType>
{
 public:
  typedef StreamType STREAM_T;
  typedef Net_IConnection_T<AddressType,
                            ConfigurationType,
                            StateType,
                            StatisticContainerType> ICONNECTION_T;
  typedef Net_ISocketConnection_T<AddressType,
                                  ConfigurationType,
                                  StateType,
                                  StatisticContainerType,
                                  StreamType,
                                  ///////
                                  Net_SocketConfiguration,
                                  ///////
                                  HandlerConfigurationType> ISOCKET_CONNECTION_T;
  typedef Net_IConnectionManager_T<AddressType,
                                   ConfigurationType,
                                   StateType,
                                   StatisticContainerType,
                                   //////
                                   UserDataType> ICONNECTION_MANAGER_T;

  Net_Client_AsynchConnector_T (ICONNECTION_MANAGER_T*, // connection manager handle
                                unsigned int = 0);      // statistic collecting interval (second(s)) [0: off]
  virtual ~Net_Client_AsynchConnector_T ();

  // override default connect strategy
  virtual int validate_connection (const ACE_Asynch_Connect::Result&, // result
                                   const ACE_INET_Addr&,              // remote address
                                   const ACE_INET_Addr&);             // local address

  // implement Net_Client_IConnector_T

  // *NOTE*: handlers receive the configuration object via
  //         ACE_Service_Handler::act ()
  virtual const HandlerConfigurationType& get () const;
  virtual bool initialize (const HandlerConfigurationType&);

  virtual bool useReactor () const; // ? : uses proactor

  virtual void abort ();
  virtual ACE_HANDLE connect (const AddressType&);

 protected:
  // override default connect strategy
  virtual void handle_connect (const ACE_Asynch_Connect::Result&);
  // override default creation strategy
  virtual HandlerType* make_handler (void);

 private:
  typedef ACE_Asynch_Connector<HandlerType> inherited;

  typedef Net_IConnector_T<AddressType,
                           HandlerConfigurationType> ICONNECTOR_T;

  ACE_UNIMPLEMENTED_FUNC (Net_Client_AsynchConnector_T (const Net_Client_AsynchConnector_T&))
  ACE_UNIMPLEMENTED_FUNC (Net_Client_AsynchConnector_T& operator= (const Net_Client_AsynchConnector_T&))

  HandlerConfigurationType configuration_;

  ICONNECTION_MANAGER_T*   connectionManager_;
  unsigned int             statisticCollectionInterval_;
};

/////////////////////////////////////////

// partial specialization (for UDP)
template <typename HandlerType,
          ///////////////////////////////
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          ///////////////////////////////
          typename HandlerConfigurationType,
          ///////////////////////////////
          typename UserDataType>
class Net_Client_AsynchConnector_T<Net_AsynchUDPConnectionBase_T<HandlerType,

                                                                 ConfigurationType,
                                                                 StateType,
                                                                 StatisticContainerType,
                                                                 StreamType,

                                                                 HandlerConfigurationType,

                                                                 UserDataType>,
                                   //////
                                   ACE_INET_Addr,
                                   ConfigurationType,
                                   StateType,
                                   StatisticContainerType,
                                   StreamType,
                                   //////
                                   HandlerConfigurationType,
                                   //////
                                   UserDataType>
 : public ACE_Asynch_Connector<Net_AsynchUDPConnectionBase_T<HandlerType,

                                                             ConfigurationType,
                                                             StateType,
                                                             StatisticContainerType,
                                                             StreamType,

                                                             HandlerConfigurationType,

                                                             UserDataType> >
 , public Net_IConnector_T<ACE_INET_Addr,
                           HandlerConfigurationType>
{
 public:
  typedef StreamType STREAM_T;
  typedef Net_IConnection_T<ACE_INET_Addr,
                            ConfigurationType,
                            StateType,
                            StatisticContainerType> ICONNECTION_T;
  typedef Net_ISocketConnection_T<ACE_INET_Addr,
                                  ConfigurationType,
                                  StateType,
                                  StatisticContainerType,
                                  StreamType,
                                  ///////
                                  Net_SocketConfiguration,
                                  ///////
                                  HandlerConfigurationType> ISOCKET_CONNECTION_T;
  typedef Net_IConnectionManager_T<ACE_INET_Addr,
                                   ConfigurationType,
                                   StateType,
                                   StatisticContainerType,
                                   //////
                                   UserDataType> ICONNECTION_MANAGER_T;
  typedef Net_AsynchUDPConnectionBase_T<HandlerType,

                                        ConfigurationType,
                                        StateType,
                                        StatisticContainerType,
                                        StreamType,

                                        HandlerConfigurationType,

                                        UserDataType> CONNECTION_T;

  Net_Client_AsynchConnector_T (ICONNECTION_MANAGER_T*, // connection manager handle
                                unsigned int = 0);      // statistic collecting interval (second(s)) [0: off]
  virtual ~Net_Client_AsynchConnector_T ();

  // override default connect strategy
  virtual int validate_connection (const ACE_Asynch_Connect::Result&, // result
                                   const ACE_INET_Addr&,              // remote address
                                   const ACE_INET_Addr&);             // local address

  // implement Net_Client_IConnector_T

  // *NOTE*: handlers receive the configuration object via
  //         ACE_Service_Handler::act ()
  virtual const HandlerConfigurationType& get () const;
  virtual bool initialize (const HandlerConfigurationType&);

  virtual bool useReactor () const; // ? : uses proactor

  virtual void abort ();
  virtual ACE_HANDLE connect (const ACE_INET_Addr&);

 protected:
  // override default creation strategy
  virtual Net_AsynchUDPConnectionBase_T<HandlerType,

                                        ConfigurationType,
                                        StateType,
                                        StatisticContainerType,
                                        StreamType,

                                        HandlerConfigurationType,

                                        UserDataType>* make_handler (void);

 private:
  typedef ACE_Asynch_Connector<Net_AsynchUDPConnectionBase_T<HandlerType,

                                                             ConfigurationType,
                                                             StateType,
                                                             StatisticContainerType,
                                                             StreamType,

                                                             HandlerConfigurationType,

                                                             UserDataType> > inherited;

  typedef Net_IConnector_T<ACE_INET_Addr,
                           HandlerConfigurationType> ICONNECTOR_T;

  ACE_UNIMPLEMENTED_FUNC (Net_Client_AsynchConnector_T (const Net_Client_AsynchConnector_T&))
  ACE_UNIMPLEMENTED_FUNC (Net_Client_AsynchConnector_T& operator= (const Net_Client_AsynchConnector_T&))

  HandlerConfigurationType configuration_;

  ICONNECTION_MANAGER_T*   connectionManager_;
  unsigned int             statisticCollectionInterval_;
};

/////////////////////////////////////////

#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
// partial specialization (for Netlink)
template <typename HandlerType,
          ///////////////////////////////
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          ///////////////////////////////
          typename HandlerConfigurationType,
          ///////////////////////////////
          typename UserDataType>
class Net_Client_AsynchConnector_T<HandlerType,
                                   //////
                                   ACE_Netlink_Addr,
                                   ConfigurationType,
                                   StateType,
                                   StatisticContainerType,
                                   StreamType,
                                   //////
                                   HandlerConfigurationType,
                                   //////
                                   UserDataType>
 : public ACE_Asynch_Connector<HandlerType>
 , public Net_IConnector_T<ACE_Netlink_Addr,
                           HandlerConfigurationType>
{
 public:
  typedef StreamType STREAM_T;
  typedef Net_IConnection_T<ACE_Netlink_Addr,
                            ConfigurationType,
                            StateType,
                            StatisticContainerType> ICONNECTION_T;
  typedef Net_ISocketConnection_T<ACE_Netlink_Addr,
                                  ConfigurationType,
                                  StateType,
                                  StatisticContainerType,
                                  StreamType,
                                  ///////
                                  Net_SocketConfiguration,
                                  ///////
                                  HandlerConfigurationType> ISOCKET_CONNECTION_T;
  typedef Net_IConnectionManager_T<ACE_Netlink_Addr,
                                   ConfigurationType,
                                   StateType,
                                   StatisticContainerType,
                                   //////
                                   UserDataType> ICONNECTION_MANAGER_T;

  Net_Client_AsynchConnector_T (ICONNECTION_MANAGER_T*, // connection manager handle
                                unsigned int = 0);      // statistic collecting interval (second(s)) [0: off]
  virtual ~Net_Client_AsynchConnector_T ();

  // override default connect strategy
  virtual int validate_connection (const ACE_Asynch_Connect::Result&, // result
                                   const ACE_Netlink_Addr&,           // remote address
                                   const ACE_Netlink_Addr&);          // local address

  // implement Net_Client_IConnector_T

  // *NOTE*: handlers receive the configuration object via
  //         ACE_Service_Handler::act ()
  virtual const HandlerConfigurationType& get () const;
  virtual bool initialize (const HandlerConfigurationType&);

  //virtual bool useReactor () const; // ? : uses proactor

  virtual void abort ();
  virtual ACE_HANDLE connect (const ACE_Netlink_Addr&);

 protected:
  // override default creation strategy
  virtual HandlerType* make_handler (void);

 private:
  typedef ACE_Asynch_Connector<HandlerType> inherited;

  typedef Net_IConnector_T<ACE_Netlink_Addr,
                           HandlerConfigurationType> ICONNECTOR_T;

  ACE_UNIMPLEMENTED_FUNC (Net_Client_AsynchConnector_T ())
  ACE_UNIMPLEMENTED_FUNC (Net_Client_AsynchConnector_T (const Net_Client_AsynchConnector_T&))
  ACE_UNIMPLEMENTED_FUNC (Net_Client_AsynchConnector_T& operator= (const Net_Client_AsynchConnector_T&))

  HandlerConfigurationType configuration_;

  ICONNECTION_MANAGER_T*   connectionManager_;
  unsigned int             statisticCollectionInterval_;
};
#endif

// include template implementation
#include "net_client_asynchconnector.inl"

#endif
