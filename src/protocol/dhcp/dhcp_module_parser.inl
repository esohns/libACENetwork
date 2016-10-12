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

#include "common_timer_manager_common.h"

#include "net_macros.h"

#include "dhcp_common.h"
#include "dhcp_defines.h"

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType>
DHCP_Module_Parser_T<ACE_SYNCH_USE,
                     TimePolicyType,
                     ConfigurationType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType>::DHCP_Module_Parser_T ()
 : inherited ()
 , debugScanner_ (DHCP_DEFAULT_LEX_TRACE) // trace scanning ?
 , debugParser_ (DHCP_DEFAULT_YACC_TRACE) // trace parsing ?
 , driver_ (DHCP_DEFAULT_LEX_TRACE,  // trace scanning ?
            DHCP_DEFAULT_YACC_TRACE) // trace parsing ?
 , isDriverInitialized_ (false)
 , initialized_ (false)
{
  NETWORK_TRACE (ACE_TEXT ("DHCP_Module_Parser_T::DHCP_Module_Parser_T"));

}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType>
DHCP_Module_Parser_T<ACE_SYNCH_USE,
                     TimePolicyType,
                     ConfigurationType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType>::~DHCP_Module_Parser_T ()
{
  NETWORK_TRACE (ACE_TEXT ("DHCP_Module_Parser_T::~DHCP_Module_Parser_T"));

}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType>
bool
DHCP_Module_Parser_T<ACE_SYNCH_USE,
                     TimePolicyType,
                     ConfigurationType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType>::initialize (const ConfigurationType& configuration_in)
{
  NETWORK_TRACE (ACE_TEXT ("DHCP_Module_Parser_T::initialize"));

  if (initialized_)
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("re-initializing...\n")));

    debugScanner_ = DHCP_DEFAULT_LEX_TRACE;
    debugParser_ = DHCP_DEFAULT_YACC_TRACE;
    isDriverInitialized_ = false;

    initialized_ = false;
  } // end IF

  // *NOTE*: need to clean up timer beyond this point !

  debugScanner_ = configuration_in.traceScanning;
  debugParser_ = configuration_in.traceParsing;

  initialized_ = inherited::initialize (configuration_in);

  return initialized_;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType>
void
DHCP_Module_Parser_T<ACE_SYNCH_USE,
                     TimePolicyType,
                     ConfigurationType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType>::handleDataMessage (DataMessageType*& message_inout,
                                                             bool& passMessageDownstream_out)
{
  NETWORK_TRACE (ACE_TEXT ("DHCP_Module_Parser_T::handleDataMessage"));

  DHCP_Record* record_p = NULL;

  // append the "\0\0"-sequence, as required by flex
  ACE_ASSERT (message_inout->capacity () - message_inout->length () >= DHCP_FLEX_BUFFER_BOUNDARY_SIZE);
  *(message_inout->wr_ptr ()) = YY_END_OF_BUFFER_CHAR;
  *(message_inout->wr_ptr () + 1) = YY_END_OF_BUFFER_CHAR;
  // *NOTE*: DO NOT adjust the write pointer --> length() must stay as it was

//  DATA_CONTAINER_T& data_container_r =
//    const_cast<DATA_CONTAINER_T&> (message_inout->get ());
//  DATA_T& data_r = const_cast<DATA_T&> (data_container_r.get ());
  DATA_T& data_r = const_cast<DATA_T&> (message_inout->get ());
//  if (!data_r.DHCPRecord)
//  {
//    ACE_NEW_NORETURN (data_r.DHCPRecord,
//                      DHCP_Record ());
//    if (!data_r.DHCPRecord)
//    {
//      ACE_DEBUG ((LM_CRITICAL,
//                  ACE_TEXT ("failed to allocate memory, returning\n")));
//      return;
//    } // end IF
//  } // end IF
//  record_p = data_r.DHCPRecord;
  record_p = &data_r;

  // initialize driver ?
  if (!isDriverInitialized_)
  {
    driver_.initialize (*record_p,
                        debugScanner_,
                        debugParser_,
                        NULL,
                        DHCP_DEFAULT_USE_YY_SCAN_BUFFER);
    isDriverInitialized_ = true;
  } // end IF

  // OK: parse this message

//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("parsing message (ID:%u,%u byte(s))...\n"),
//              message_p->getID (),
//              message_p->length ()));

  if (!driver_.parse (message_inout))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to DHCP_ParserDriver::parse() (message ID: %d), returning\n"),
                message_inout->getID ()));
    return;
  } // end IF
  message_inout->rd_ptr (message_inout->base ());
  message_inout->initialize (*record_p,
                             NULL);
  isDriverInitialized_ = false;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType>
void
DHCP_Module_Parser_T<ACE_SYNCH_USE,
                     TimePolicyType,
                     ConfigurationType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                bool& passMessageDownstream_out)
{
  NETWORK_TRACE (ACE_TEXT ("DHCP_Module_Parser_T::handleSessionMessage"));

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (message_inout);

  switch (message_inout->type ())
  {
    default:
      break;
  } // end SWITCH
}

////////////////////////////////////////////////////////////////////////////////

template <typename LockType,
          typename TaskSynchType,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename StreamControlType,
          typename StreamNotificationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType>
DHCP_Module_ParserH_T<LockType,
                     TaskSynchType,
                     TimePolicyType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType,
                     ConfigurationType,
                     StreamControlType,
                     StreamNotificationType,
                     StreamStateType,
                     SessionDataType,
                     SessionDataContainerType,
                     StatisticContainerType>::DHCP_Module_ParserH_T (LockType* lock_in)
 : inherited (lock_in, // lock handle
              false,   // auto-start ?
              true)    // generate sesssion messages ?
 , debugScanner_ (DHCP_DEFAULT_LEX_TRACE) // trace scanning ?
 , debugParser_ (DHCP_DEFAULT_YACC_TRACE) // trace parsing ?
 , driver_ (DHCP_DEFAULT_LEX_TRACE,  // trace scanning ?
            DHCP_DEFAULT_YACC_TRACE) // trace parsing ?
 , isDriverInitialized_ (false)
{
  NETWORK_TRACE (ACE_TEXT ("DHCP_Module_ParserH_T::DHCP_Module_ParserH_T"));

}

template <typename LockType,
          typename TaskSynchType,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename StreamControlType,
          typename StreamNotificationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType>
DHCP_Module_ParserH_T<LockType,
                     TaskSynchType,
                     TimePolicyType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType,
                     ConfigurationType,
                     StreamControlType,
                     StreamNotificationType,
                     StreamStateType,
                     SessionDataType,
                     SessionDataContainerType,
                     StatisticContainerType>::~DHCP_Module_ParserH_T ()
{
  NETWORK_TRACE (ACE_TEXT ("DHCP_Module_ParserH_T::~DHCP_Module_ParserH_T"));

}

template <typename LockType,
          typename TaskSynchType,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename StreamControlType,
          typename StreamNotificationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType>
bool
DHCP_Module_ParserH_T<LockType,
                     TaskSynchType,
                     TimePolicyType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType,
                     ConfigurationType,
                     StreamControlType,
                     StreamNotificationType,
                     StreamStateType,
                     SessionDataType,
                     SessionDataContainerType,
                     StatisticContainerType>::initialize (const ConfigurationType& configuration_in)
{
  NETWORK_TRACE (ACE_TEXT ("DHCP_Module_ParserH_T::initialize"));

  bool result = false;

  if (inherited::initialized_)
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("re-initializing...\n")));

    debugScanner_ = DHCP_DEFAULT_LEX_TRACE;
    debugParser_ = DHCP_DEFAULT_YACC_TRACE;
    isDriverInitialized_ = false;
  } // end IF

  debugScanner_ = configuration_in.traceScanning;
  debugParser_ = configuration_in.traceParsing;

  // OK: all's well...
  result = inherited::initialize (configuration_in);
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_HeadModuleTaskBase_T::initialize(): \"%m\", aborting\n")));
    return false;
  } // end IF

  return result;
}

template <typename LockType,
          typename TaskSynchType,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename StreamControlType,
          typename StreamNotificationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType>
void
DHCP_Module_ParserH_T<LockType,
                     TaskSynchType,
                     TimePolicyType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType,
                     ConfigurationType,
                     StreamControlType,
                     StreamNotificationType,
                     StreamStateType,
                     SessionDataType,
                     SessionDataContainerType,
                     StatisticContainerType>::handleDataMessage (DataMessageType*& message_inout,
                                                                 bool& passMessageDownstream_out)
{
  NETWORK_TRACE (ACE_TEXT ("DHCP_Module_ParserH_T::handleDataMessage"));

  int result = -1;
  DHCP_Record* record_p = NULL;

  // append the "\0\0"-sequence, as required by flex
  ACE_ASSERT (message_inout->capacity () - message_inout->length () >= DHCP_FLEX_BUFFER_BOUNDARY_SIZE);
  *(message_inout->wr_ptr ()) = YY_END_OF_BUFFER_CHAR;
  *(message_inout->wr_ptr () + 1) = YY_END_OF_BUFFER_CHAR;
  // *NOTE*: DO NOT adjust the write pointer --> length() must stay as it was

//  DATA_CONTAINER_T& data_container_r =
//    const_cast<DATA_CONTAINER_T&> (message_inout->get ());
//  DATA_T& data_r = const_cast<DATA_T&> (data_container_r.get ());
  DATA_T& data_r = const_cast<DATA_T&> (message_inout->get ());
  if (!data_r.DHCPRecord)
  {
    ACE_NEW_NORETURN (data_r.DHCPRecord,
                      DHCP_Record ());
    if (!data_r.DHCPRecord)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, returning\n")));
      return;
    } // end IF
  } // end IF
  record_p = data_r.DHCPRecord;

  // initialize driver ?
  if (!isDriverInitialized_)
  {
    driver_.initialize (*record_p,
                        debugScanner_,
                        debugParser_);
    isDriverInitialized_ = true;
  } // end IF

    // OK: parse this message

    //  ACE_DEBUG ((LM_DEBUG,
    //              ACE_TEXT ("parsing message (ID:%u,%u byte(s))...\n"),
    //              message_p->getID (),
    //              message_p->length ()));

  if (!driver_.parse (message_inout))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to DHCP_ParserDriver::parse() (message ID: %d), returning\n"),
                message_inout->getID ()));
    return;
  } // end IF
}

template <typename LockType,
          typename TaskSynchType,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename StreamControlType,
          typename StreamNotificationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType>
void
DHCP_Module_ParserH_T<LockType,
                     TaskSynchType,
                     TimePolicyType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType,
                     ConfigurationType,
                     StreamControlType,
                     StreamNotificationType,
                     StreamStateType,
                     SessionDataType,
                     SessionDataContainerType,
                     StatisticContainerType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                    bool& passMessageDownstream_out)
{
  NETWORK_TRACE (ACE_TEXT ("DHCP_Module_ParserH_T::handleSessionMessage"));

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  switch (message_inout->type ())
  {
    case STREAM_SESSION_MESSAGE_BEGIN:
    {
      // retain session ID for reporting
      const SessionDataContainerType& session_data_container_r =
          message_inout->get ();
      const SessionDataType& session_data_r = session_data_container_r.get ();
      ACE_ASSERT (inherited::streamState_);
      ACE_ASSERT (inherited::streamState_->currentSessionData);
      ACE_Guard<ACE_SYNCH_MUTEX> aGuard (*(inherited::streamState_->currentSessionData->lock));
      inherited::streamState_->currentSessionData->sessionID =
          session_data_r.sessionID;

      // start profile timer...
      //profile_.start ();

      break;
    }
    case STREAM_SESSION_MESSAGE_END:
    default:
      break;
  } // end SWITCH
}

template <typename LockType,
          typename TaskSynchType,
          typename TimePolicyType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConfigurationType,
          typename StreamControlType,
          typename StreamNotificationType,
          typename StreamStateType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename StatisticContainerType>
bool
DHCP_Module_ParserH_T<LockType,
                     TaskSynchType,
                     TimePolicyType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType,
                     ConfigurationType,
                     StreamControlType,
                     StreamNotificationType,
                     StreamStateType,
                     SessionDataType,
                     SessionDataContainerType,
                     StatisticContainerType>::collect (StatisticContainerType& data_out)
{
  NETWORK_TRACE (ACE_TEXT ("DHCP_Module_ParserH_T::collect"));

  // step1: initialize info container POD
  data_out.bytes = 0.0;
  data_out.dataMessages = 0;
  data_out.droppedMessages = 0;
  data_out.timeStamp = COMMON_TIME_NOW;

  // *NOTE*: information is collected by the statistic module (if any)

  // step1: send the container downstream
  if (!inherited::putStatisticMessage (data_out)) // data container
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to putStatisticMessage(SESSION_STATISTICS), aborting\n")));
    return false;
  } // end IF

  return true;
}

//template <typename LockType,
//          typename TaskSynchType,
//          typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename StreamStateType,
//          typename SessionDataType,
//          typename SessionDataContainerType,
//          typename StatisticContainerType>
//void
//DHCP_Module_ParserH_T<LockType,
//                     TaskSynchType,
//                     TimePolicyType,
//                     SessionMessageType,
//                     ProtocolMessageType,
//                     ConfigurationType,
//                     StreamStateType,
//                     SessionDataType,
//                     SessionDataContainerType,
//                     StatisticContainerType>::report () const
//{
//  NETWORK_TRACE (ACE_TEXT ("DHCP_Module_ParserH_T::report"));
//
//  ACE_ASSERT (false);
//  ACE_NOTSUP;
//  ACE_NOTREACHED (return);
//}

//template <typename LockType,
//          typename TaskSynchType,
//          typename TimePolicyType,
//          typename SessionMessageType,
//          typename ProtocolMessageType,
//          typename ConfigurationType,
//          typename StreamStateType,
//          typename SessionDataType,
//          typename SessionDataContainerType,
//          typename StatisticContainerType>
//bool
//DHCP_Module_ParserH_T<LockType,
//                     TaskSynchType,
//                     TimePolicyType,
//                     SessionMessageType,
//                     ProtocolMessageType,
//                     ConfigurationType,
//                     StreamStateType,
//                     SessionDataType,
//                     SessionDataContainerType,
//                     StatisticContainerType>::putStatisticMessage (const StatisticContainerType& statisticData_in) const
//{
//  NETWORK_TRACE (ACE_TEXT ("DHCP_Module_ParserH_T::putStatisticMessage"));
//
//  // sanity check(s)
//  ACE_ASSERT (inherited::configuration_);
//  ACE_ASSERT (inherited::configuration_->streamConfiguration);
//
////  // step1: initialize session data
////  IRC_StreamSessionData* session_data_p = NULL;
////  ACE_NEW_NORETURN (session_data_p,
////                    IRC_StreamSessionData ());
////  if (!session_data_p)
////  {
////    ACE_DEBUG ((LM_CRITICAL,
////                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
////    return false;
////  } // end IF
////  //ACE_OS::memset (data_p, 0, sizeof (IRC_SessionData));
//  SessionDataType& session_data_r =
//      const_cast<SessionDataType&> (inherited::sessionData_->get ());
//  session_data_r.currentStatistic = statisticData_in;
//
////  // step2: allocate session data container
////  IRC_StreamSessionData_t* session_data_container_p = NULL;
////  // *NOTE*: fire-and-forget stream_session_data_p
////  ACE_NEW_NORETURN (session_data_container_p,
////                    IRC_StreamSessionData_t (stream_session_data_p,
////                                                    true));
////  if (!session_data_container_p)
////  {
////    ACE_DEBUG ((LM_CRITICAL,
////                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
//
////    // clean up
////    delete stream_session_data_p;
//
////    return false;
////  } // end IF
//
//  // step3: send the data downstream...
//  // *NOTE*: fire-and-forget session_data_container_p
//  return inherited::putSessionMessage (STREAM_SESSION_STATISTIC,
//                                       *inherited::sessionData_,
//                                       inherited::configuration_->streamConfiguration->messageAllocator);
//}
