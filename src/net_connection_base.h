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

#ifndef NET_CONNECTION_BASE_H
#define NET_CONNECTION_BASE_H

#include "common_referencecounter_base.h"

#include "stream_common.h"
#include "stream_statistichandler.h"

#include "net_common.h"
#include "net_configuration.h"
#include "net_iconnection.h"
#include "net_iconnectionmanager.h"

template <typename AddressType,
          typename SocketConfigurationType,
          typename ConfigurationType,
          typename UserDataType,
          typename SessionDataType,
          typename StatisticContainerType>
class Net_ConnectionBase_T
 : public Common_ReferenceCounterBase
 , virtual public Net_IConnection_T<AddressType,
                                    ConfigurationType,
                                    StatisticContainerType>
{
 public:
  // implement (part of) Net_IConnection_T
  virtual bool initialize (const ConfigurationType&); // handler configuration

 protected:
  typedef Net_IConnectionManager_T<AddressType,
                                   SocketConfigurationType,
                                   ConfigurationType,
                                   UserDataType,
                                   StatisticContainerType> ICONNECTION_MANAGER_T;

  Net_ConnectionBase_T (ICONNECTION_MANAGER_T*, // connection manager handle
                        unsigned int = 0);      // statistics collecting interval (second(s))
                                                // 0 --> DON'T collect statistics
  virtual ~Net_ConnectionBase_T ();

  // *NOTE*: (de-)registers connection with the connection manager (if any)
  bool registerc ();
  void deregister ();

  ConfigurationType      configuration_;
  bool                   isRegistered_;
  ICONNECTION_MANAGER_T* manager_;
  SessionDataType*       sessionData_;

 private:
  typedef Common_ReferenceCounterBase inherited;
  typedef Net_IConnection_T<AddressType,
                            ConfigurationType,
                            StatisticContainerType> inherited2;

  ACE_UNIMPLEMENTED_FUNC (Net_ConnectionBase_T ());
  ACE_UNIMPLEMENTED_FUNC (Net_ConnectionBase_T (const Net_ConnectionBase_T&));
  ACE_UNIMPLEMENTED_FUNC (Net_ConnectionBase_T& operator= (const Net_ConnectionBase_T&));

  // timer
  unsigned int                      statisticCollectionInterval_; // seconds
  Stream_StatisticHandler_Reactor_t statisticCollectHandler_;
  long                              statisticCollectHandlerID_;
};

// include template implementation
#include "net_connection_base.inl"

#endif
