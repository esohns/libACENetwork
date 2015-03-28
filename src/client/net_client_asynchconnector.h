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
#include "ace/Netlink_Addr.h"

#include "stream_common.h"

#include "net_client_iconnector.h"
#include "net_connection_manager_common.h"
#include "net_netlinkconnection.h"
#include "net_udpconnection.h"

template <typename AddressType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename ITransportLayerType,
          typename ConnectionType>
class Net_Client_AsynchConnector_T
 : public ACE_Asynch_Connector<ConnectionType>
 , public Net_Client_IConnector_T<AddressType,
                                  SocketHandlerConfigurationType>
{
 public:
   typedef Net_IConnectionManager_T<ConfigurationType,
                                    UserDataType,
                                    Stream_Statistic_t,
                                    ITransportLayerType> ICONNECTION_MANAGER_T;

  // *TODO*: pass SocketHandlerConfigurationType instead
   Net_Client_AsynchConnector_T (const ConfigurationType*, // configuration handle
                                 ICONNECTION_MANAGER_T*,   // connection manager handle
                                 unsigned int = 0);        // statistics collecting interval (second(s))
                                                           // 0 --> DON'T collect statistics
   virtual ~Net_Client_AsynchConnector_T ();

  // override default connect strategy
  virtual int validate_connection (const ACE_Asynch_Connect::Result&, // result
                                   const ACE_INET_Addr&,              // remote address
                                   const ACE_INET_Addr&);             // local address

  // implement Net_Client_IConnector_T
  virtual const SocketHandlerConfigurationType* getConfiguration () const;
  virtual void abort ();
  virtual bool connect (const AddressType&);

 protected:
  // override default creation strategy
  // *TODO*: why isn't this being picked up ?
  virtual ConnectionType* make_handler (void);

 private:
  typedef ACE_Asynch_Connector<ConnectionType> inherited;

  ACE_UNIMPLEMENTED_FUNC (Net_Client_AsynchConnector_T ());
  ACE_UNIMPLEMENTED_FUNC (Net_Client_AsynchConnector_T (const Net_Client_AsynchConnector_T&));
  ACE_UNIMPLEMENTED_FUNC (Net_Client_AsynchConnector_T& operator= (const Net_Client_AsynchConnector_T&));

  // *TODO*: retain SocketHandlerConfigurationType instead
  const ConfigurationType* configuration_;
  ICONNECTION_MANAGER_T*   interfaceHandle_;
  unsigned int             statisticCollectionInterval_; // seconds
};

/////////////////////////////////////////

// partial specialization (for UDP)
template <typename HandlerType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename ITransportLayerType>
class Net_Client_AsynchConnector_T<ACE_INET_Addr,
                                   ConfigurationType,
                                   SocketHandlerConfigurationType,
                                   UserDataType,
                                   SessionDataType,
                                   ITransportLayerType,
                                   Net_AsynchUDPConnection_T<UserDataType,
                                                             SessionDataType,
                                                             HandlerType> >
 : public ACE_Asynch_Connector<Net_AsynchUDPConnection_T<UserDataType,
                                                         SessionDataType,
                                                         HandlerType> >
 , public Net_Client_IConnector_T<ACE_INET_Addr,
                                  SocketHandlerConfigurationType>
{
 public:
  typedef Net_IConnectionManager_T<ConfigurationType,
                                   UserDataType,
                                   Stream_Statistic_t,
                                   ITransportLayerType> ICONNECTION_MANAGER_T;
  typedef Net_AsynchUDPConnection_T<UserDataType,
                                    SessionDataType,
                                    HandlerType> CONNECTION_T;

  // *TODO*: pass SocketHandlerConfigurationType instead
  Net_Client_AsynchConnector_T (const ConfigurationType*, // socket handler configuration handle
                                ICONNECTION_MANAGER_T*,   // connection manager handle
                                unsigned int = 0);        // statistics collecting interval (second(s))
                                                          // 0 --> DON'T collect statistics
  virtual ~Net_Client_AsynchConnector_T ();

  // override default connect strategy
  virtual int validate_connection (const ACE_Asynch_Connect::Result&, // result
                                   const ACE_INET_Addr&,              // remote address
                                   const ACE_INET_Addr&);             // local address

  // implement Net_Client_IConnector_T
  virtual const SocketHandlerConfigurationType* getConfiguration () const;
  virtual void abort ();
  virtual bool connect (const ACE_INET_Addr&);

 protected:
  // override default creation strategy
  // *TODO*: why isn't this being picked up ?
  //virtual CONNECTION_T* make_handler (void);
   virtual Net_AsynchUDPConnection_T<UserDataType,
                                     SessionDataType,
                                     HandlerType>* make_handler (void);

 private:
  typedef ACE_Asynch_Connector<Net_AsynchUDPConnection_T<UserDataType,
                                                         SessionDataType,
                                                         HandlerType> > inherited;

  ACE_UNIMPLEMENTED_FUNC (Net_Client_AsynchConnector_T ());
  ACE_UNIMPLEMENTED_FUNC (Net_Client_AsynchConnector_T (const Net_Client_AsynchConnector_T&));
  ACE_UNIMPLEMENTED_FUNC (Net_Client_AsynchConnector_T& operator= (const Net_Client_AsynchConnector_T&));

  // *TODO*: retain SocketHandlerConfigurationType instead
  const ConfigurationType* configuration_;
  ICONNECTION_MANAGER_T*   interfaceHandle_;
  unsigned int             statisticCollectionInterval_; // seconds
};

/////////////////////////////////////////

// partial specialization (for Netlink)
template <typename HandlerType,
          typename ConfigurationType,
          typename SocketHandlerConfigurationType,
          typename UserDataType,
          typename SessionDataType>
class Net_Client_AsynchConnector_T<ACE_Netlink_Addr,
                                   ConfigurationType,
                                   SocketHandlerConfigurationType,
                                   UserDataType,
                                   SessionDataType,
                                   Net_INetlinkTransportLayer_t,
                                   HandlerType>
 : public ACE_Asynch_Connector<HandlerType>
 , public Net_Client_IConnector_T<ACE_Netlink_Addr,
                                  SocketHandlerConfigurationType>
{
 public:
  typedef Net_IConnectionManager_T<ConfigurationType,
                                   UserDataType,
                                   Stream_Statistic_t,
                                   Net_INetlinkTransportLayer_t> ICONNECTION_MANAGER_T;

  // *TODO*: pass SocketHandlerConfigurationType instead
  Net_Client_AsynchConnector_T (const ConfigurationType*, // socket handler configuration handle
                                ICONNECTION_MANAGER_T*,   // connection manager handle
                                unsigned int = 0);        // statistics collecting interval (second(s))
                                                          // 0 --> DON'T collect statistics
  virtual ~Net_Client_AsynchConnector_T ();

  // override default connect strategy
  virtual int validate_connection (const ACE_Asynch_Connect::Result&, // result
                                   const ACE_Netlink_Addr&,           // remote address
                                   const ACE_Netlink_Addr&);          // local address

  // implement Net_Client_IConnector_T
  virtual const SocketHandlerConfigurationType* getConfiguration () const;
  virtual void abort ();
  virtual bool connect (const ACE_Netlink_Addr&);

 protected:
  // override default creation strategy
  // *TODO*: why isn't this being picked up ?
  virtual HandlerType* make_handler (void);

 private:
  typedef ACE_Asynch_Connector<HandlerType> inherited;

  ACE_UNIMPLEMENTED_FUNC (Net_Client_AsynchConnector_T ());
  ACE_UNIMPLEMENTED_FUNC (Net_Client_AsynchConnector_T (const Net_Client_AsynchConnector_T&));
  ACE_UNIMPLEMENTED_FUNC (Net_Client_AsynchConnector_T& operator= (const Net_Client_AsynchConnector_T&));

  // *TODO*: retain SocketHandlerConfigurationType instead
  const ConfigurationType* configuration_;
  ICONNECTION_MANAGER_T*   interfaceHandle_;
  unsigned int             statisticCollectionInterval_; // seconds
};

// include template implementation
#include "net_client_asynchconnector.inl"

#endif
