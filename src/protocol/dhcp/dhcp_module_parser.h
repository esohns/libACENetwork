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

#ifndef DHCP_MODULE_PARSER_H
#define DHCP_MODULE_PARSER_H

#include "ace/Global_Macros.h"

#include "common_istatistic.h"

#include "stream_headmoduletask_base.h"
#include "stream_statistichandler.h"
#include "stream_task_base_synch.h"

#include "dhcp_defines.h"
#include "dhcp_parser_driver.h"

// forward declaration(s)
class Stream_IAllocator;

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          ///////////////////////////////
          typename ConfigurationType>
class DHCP_Module_Parser_T
 : public Stream_TaskBaseSynch_T<TimePolicyType,
                                 SessionMessageType,
                                 ProtocolMessageType>
{
 public:
  DHCP_Module_Parser_T ();
  virtual ~DHCP_Module_Parser_T ();

  // override (part of) Stream_IModuleHandler_T
  virtual bool initialize (const ConfigurationType&);

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (ProtocolMessageType*&, // data message handle
                                  bool&);                // return value: pass message downstream ?

  // catch the session ID...
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

 private:
  typedef Stream_TaskBaseSynch_T<TimePolicyType,
                                 SessionMessageType,
                                 ProtocolMessageType> inherited;

  ACE_UNIMPLEMENTED_FUNC (DHCP_Module_Parser_T (const DHCP_Module_Parser_T&))
  ACE_UNIMPLEMENTED_FUNC (DHCP_Module_Parser_T& operator= (const DHCP_Module_Parser_T&))

  // convenient types
  typedef typename ProtocolMessageType::DATA_T DATA_T;
//  typedef typename ProtocolMessageType::DATA_T DATA_CONTAINER_T;
//  typedef typename ProtocolMessageType::DATA_T::DATA_T DATA_T;

  // driver
  bool              debugScanner_;
  bool              debugParser_;
  DHCP_ParserDriver driver_;
  bool              isDriverInitialized_;

  bool              initialized_;
};

////////////////////////////////////////////////////////////////////////////////

template <typename LockType,
          ///////////////////////////////
          typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          ///////////////////////////////
          typename ConfigurationType,
          ///////////////////////////////
          typename StreamStateType,
          ///////////////////////////////
          typename SessionDataType,          // session data
          typename SessionDataContainerType, // session message payload (reference counted)
          ///////////////////////////////
          typename StatisticContainerType>
class DHCP_Module_ParserH_T
 : public Stream_HeadModuleTaskBase_T<LockType,
                                      ///
                                      TaskSynchType,
                                      TimePolicyType,
                                      SessionMessageType,
                                      ProtocolMessageType,
                                      ///
                                      ConfigurationType,
                                      ///
                                      StreamStateType,
                                      ///
                                      SessionDataType,
                                      SessionDataContainerType>
// implement this to have a generic (timed) event handler to trigger
// periodic statistic collection
 , public Common_IStatistic_T<StatisticContainerType>
{
 public:
  DHCP_Module_ParserH_T ();
  virtual ~DHCP_Module_ParserH_T ();

  // *PORTABILITY*: for some reason, this base class member is not exposed
  //                (MSVC/gcc)
  using Stream_HeadModuleTaskBase_T<LockType,
                                    /////
                                    TaskSynchType,
                                    TimePolicyType,
                                    SessionMessageType,
                                    ProtocolMessageType,
                                    /////
                                    ConfigurationType,
                                    /////
                                    StreamStateType,
                                    /////
                                    SessionDataType,
                                    SessionDataContainerType>::initialize;

  // override (part of) Stream_IModuleHandler_T
  virtual bool initialize (const ConfigurationType&);

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (ProtocolMessageType*&, // data message handle
                                  bool&);                // return value: pass message downstream ?

  // catch the session ID...
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  // implement Common_IStatistic
  // *NOTE*: this reuses the interface to implement timer-based data collection
  virtual bool collect (StatisticContainerType&); // return value: (currently unused !)
  virtual void report () const;

 private:
  typedef Stream_HeadModuleTaskBase_T<LockType,
                                      ///
                                      TaskSynchType,
                                      TimePolicyType,
                                      SessionMessageType,
                                      ProtocolMessageType,
                                      ///
                                      ConfigurationType,
                                      ///
                                      StreamStateType,
                                      ///
                                      SessionDataType,
                                      SessionDataContainerType> inherited;

  ACE_UNIMPLEMENTED_FUNC (DHCP_Module_ParserH_T (const DHCP_Module_ParserH_T&))
  ACE_UNIMPLEMENTED_FUNC (DHCP_Module_ParserH_T& operator= (const DHCP_Module_ParserH_T&))

  // convenience types
  typedef Stream_StatisticHandler_Reactor_T<StatisticContainerType> STATISTICHANDLER_T;
  typedef typename ProtocolMessageType::DATA_T::DATA_T DATA_T;
//  typedef typename ProtocolMessageType::DATA_T DATA_CONTAINER_T;
//  typedef typename ProtocolMessageType::DATA_T::DATA_T DATA_T;

  // helper methods
  bool putStatisticMessage (const StatisticContainerType&) const;

  // timer
  STATISTICHANDLER_T statisticCollectHandler_;
  long               statisticCollectHandlerID_;

  // driver
  bool               debugScanner_;
  bool               debugParser_;
  DHCP_ParserDriver  driver_;
  bool               isDriverInitialized_;

  bool               initialized_;
};

// include template implementation
#include "dhcp_module_parser.inl"

#endif