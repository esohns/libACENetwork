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

#ifndef NET_CONNECTION_MANAGER_H
#define NET_CONNECTION_MANAGER_H

#include "ace/Containers_T.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"
#include "ace/Time_Value.h"

#include "common_idumpstate.h"
#include "common_istatistic.h"

#include "net_iconnection.h"
#include "net_iconnectionmanager.h"

template <typename AddressType,
          typename ConfigurationType,
          typename StateType,
          typename StatisticContainerType,
          typename StreamType,
          ///////////////////////////////
          typename UserDataType>
class Net_Connection_Manager_T
 : public Net_IConnectionManager_T<AddressType,
                                   ConfigurationType,
                                   StateType,
                                   StatisticContainerType,
                                   StreamType,
                                   //////
                                   UserDataType>
 , public Common_IStatistic_T<StatisticContainerType>
 , public Common_IDumpState
{
  // singleton has access to the ctor/dtors
  friend class ACE_Singleton<Net_Connection_Manager_T<AddressType,
                                                      ConfigurationType,
                                                      StateType,
                                                      StatisticContainerType,
                                                      StreamType,

                                                      UserDataType>,
                             ACE_SYNCH_MUTEX>;

 public:
  // convenience types
  typedef Net_IConnection_T<AddressType,
                            ConfigurationType,
                            StateType,
                            StatisticContainerType,
                            StreamType> CONNECTION_T;

  // configuration / initialization
  void initialize (unsigned int); // maximum number of concurrent connections

  // implement Net_IConnectionManager_T
  // *TODO*: clean this up
  virtual void set (const ConfigurationType&, // connection handler (default)
                                              // configuration
                    UserDataType*);           // (stream) user data
  // *IMPORTANT NOTE*: in terms of the (stream) user data, this works only once
  //                   (!) for every set() call
  virtual void get (ConfigurationType&, // return value: (default)
                                        // connection handler configuration
                    UserDataType*&);    // return value: (stream) user data

  virtual CONNECTION_T* operator[] (unsigned int) const; // index
  virtual CONNECTION_T* get (ACE_HANDLE) const; // socket handle
  virtual CONNECTION_T* get (const AddressType&) const; // peer address

  virtual bool registerc (CONNECTION_T*); // connection handle
  virtual void deregister (CONNECTION_T*); // connection handle

  virtual void abort ();
  virtual unsigned int numConnections () const;
  virtual void wait () const;

  // ---------------------------------------------------------------------------
  virtual void abortLeastRecent ();
  virtual void abortMostRecent ();
  // ---------------------------------------------------------------------------

  // implement Common_IControl
  virtual void start ();
  virtual void stop (bool = true,  // wait for completion ?
                     bool = true); // locked access ?
  virtual bool isRunning () const;

  // implement Common_ILock
  virtual void lock ();
  virtual void unlock ();

  // implement (part of) Common_IStatistic_T
  virtual void report () const;

  // implement Common_IDumpState
  virtual void dump_state () const;

 private:
  // convenience types
  typedef Net_Connection_Manager_T<AddressType,
                                   ConfigurationType,
                                   StateType,
                                   StatisticContainerType,
                                   StreamType,
                                   //////
                                   UserDataType> OWN_TYPE_T;

  typedef ACE_DLList<CONNECTION_T> CONNECTION_CONTAINER_T;
  typedef ACE_DLList_Iterator<CONNECTION_T> CONNECTION_CONTAINER_ITERATOR_T;
  typedef ACE_DLList_Reverse_Iterator<CONNECTION_T> CONNECTION_CONTAINER_REVERSEITERATOR_T;

  // implement (part of) Common_IStatistic_T
  // *WARNING*: this assumes lock_ is being held
  virtual bool collect (StatisticContainerType&); // return value: statistic data

  Net_Connection_Manager_T ();
  ACE_UNIMPLEMENTED_FUNC (Net_Connection_Manager_T (const Net_Connection_Manager_T&))
  ACE_UNIMPLEMENTED_FUNC (Net_Connection_Manager_T& operator= (const Net_Connection_Manager_T&))
  virtual ~Net_Connection_Manager_T ();

  // implement blocking wait...
  mutable ACE_SYNCH_RECURSIVE_CONDITION condition_;
  ConfigurationType                     configuration_; // default-
  CONNECTION_CONTAINER_T                connections_;
  bool                                  isActive_;
  bool                                  isInitialized_;
  // *NOTE*: MUST be recursive, otherwise asynchronous abort is not feasible
  mutable ACE_SYNCH_RECURSIVE_MUTEX     lock_;
  unsigned int                          maxNumConnections_;
  UserDataType*                         userData_;
};

// include template implementation
#include "net_connection_manager.inl"

#endif
