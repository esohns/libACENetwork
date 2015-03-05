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

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"
#include "ace/Time_Value.h"

//#include "common.h"
#include "common_timer_manager.h"

#include "stream_message_base.h"
#include "stream_iallocator.h"

#include "net_common_tools.h"
#include "net_macros.h"

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
Net_Module_Statistic_WriterTask_T<TaskSynchType,
                                  TimePolicyType,
                                  SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::Net_Module_Statistic_WriterTask_T ()
 : inherited ()
 , isInitialized_ (false)
 , resetTimeoutHandler_ (this)
 , resetTimeoutHandlerID_ (-1)
 , localReportingHandler_ (this,
                           ACTION_REPORT)
 , localReportingHandlerID_ (-1)
 , reportingInterval_ (0)
 , sessionID_ (0)
 , numInboundMessages_ (0)
 , numOutboundMessages_ (0)
 , numSessionMessages_ (0)
 , messageCounter_ (0)
 , lastMessagesPerSecondCount_ (0)
 , numInboundBytes_ (0.0F)
 , numOutboundBytes_ (0.0F)
 , byteCounter_ (0)
 , lastBytesPerSecondCount_ (0)
// , messageTypeStatistics_.clear()
 , allocator_ (NULL)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Module_Statistic_WriterTask_T::Net_Module_Statistic_WriterTask_T"));

}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
Net_Module_Statistic_WriterTask_T<TaskSynchType,
                                  TimePolicyType,
                                  SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::~Net_Module_Statistic_WriterTask_T ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_Module_Statistic_WriterTask_T::~Net_Module_Statistic_WriterTask_T"));

  // clean up
  fini_timers (true);
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
bool
Net_Module_Statistic_WriterTask_T<TaskSynchType,
                                  TimePolicyType,
                                  SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::init (unsigned int reportingInterval_in,
                                                                  bool printFinalReport_in,
                                                                  const Stream_IAllocator* allocator_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Module_Statistic_WriterTask_T::init"));

  // sanity check(s)
  if (isInitialized_)
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("re-initializing...\n")));

    // stop timers
    fini_timers (true);

    reportingInterval_ = 0;
    printFinalReport_ = false;
    sessionID_ = 0;
    // reset various counters...
    {
      ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

      numInboundMessages_ = 0;
      numOutboundMessages_ = 0;
      numSessionMessages_ = 0;
      messageCounter_ = 0;
      lastMessagesPerSecondCount_ = 0;

      numInboundBytes_ = 0.0F;
      numOutboundBytes_ = 0.0F;
      byteCounter_ = 0;
      lastBytesPerSecondCount_ = 0;

      messageTypeStatistics_.clear ();
    } // end lock scope
    allocator_ = NULL;

    isInitialized_ = false;
  } // end IF

  reportingInterval_ = reportingInterval_in;
  if (reportingInterval_)
  {
    // schedule the second-granularity timer
    ACE_Time_Value interval (1, 0); // one second interval
    ACE_Event_Handler* eh = &resetTimeoutHandler_;
    resetTimeoutHandlerID_ =
      COMMON_TIMERMANAGER_SINGLETON::instance()->schedule (eh,                               // event handler
                                                           NULL,                             // ACT
                                                           COMMON_TIME_POLICY () + interval, // first wakeup time
                                                           interval);                        // interval
    if (resetTimeoutHandlerID_ == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Common_Timer_Manager::schedule(), aborting\n")));

      return false;
    } // end IF
//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("scheduled second-interval timer (ID: %d)...\n"),
//               resetTimeoutHandlerID_));
  } // end IF
  printFinalReport_ = printFinalReport_in;
  allocator_ = allocator_in;
//   // sanity check(s)
//   if (!allocator_)
//   {
//     ACE_DEBUG ((LM_ERROR,
//                 ACE_TEXT ("invalid argument (was NULL), aborting\n")));
//
//     return false;
//   } // end IF

  isInitialized_ = true;

  return true;
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
void
Net_Module_Statistic_WriterTask_T<TaskSynchType,
                                  TimePolicyType,
                                  SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::handleDataMessage (ProtocolMessageType*& message_inout,
                                                                               bool& passMessageDownstream_out)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Module_Statistic_WriterTask_T::handleDataMessage"));

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (message_inout);

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

    // update counters...
    numInboundMessages_++;
    numInboundBytes_ += message_inout->total_length ();
    byteCounter_ += message_inout->total_length ();

    messageCounter_++;

    // add message to statistic...
    messageTypeStatistics_[static_cast<ProtocolCommandType> (message_inout->getCommand ())]++;
  } // end lock scope
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
void
Net_Module_Statistic_WriterTask_T<TaskSynchType,
                                  TimePolicyType,
                                  SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                                  bool& passMessageDownstream_out)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Module_Statistic_WriterTask_T::handleSessionMessage"));

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (message_inout);
  ACE_ASSERT (isInitialized_);

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

    // update our counters...
    // *NOTE*: currently, session messages travel only downstream...
    //numInboundMessages_++;
    numSessionMessages_++;
    //messageCounter_++;
  } // end lock scope

  switch (message_inout->getType ())
  {
    case SESSION_BEGIN:
    {
      // retain session ID for reporting...
      sessionID_ = message_inout->getData ()->getState ()->sessionID;

      // statistics reporting
      if (reportingInterval_)
      {
        // schedule the reporting interval timer
        ACE_Time_Value interval (reportingInterval_, 0);
        ACE_ASSERT (localReportingHandlerID_ == -1);
        ACE_Event_Handler* eh = &localReportingHandler_;
        localReportingHandlerID_ =
          COMMON_TIMERMANAGER_SINGLETON::instance ()->schedule (eh,                               // event handler
                                                                NULL,                             // act
                                                                COMMON_TIME_POLICY () + interval, // first wakeup time
                                                                interval);                        // interval
        if (localReportingHandlerID_ == -1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to RPG_Common_Timer_Manager::schedule(), aborting\n")));

          return;
        } // end IF
        //     ACE_DEBUG ((LM_DEBUG,
        //                 ACE_TEXT ("scheduled (local) reporting timer (ID: %d) for intervals of %u second(s)...\n"),
        //                 localReportingHandlerID_,
        //                 reportingInterval_in));
      } // end IF
      else
      {
        // *NOTE*: even if this doesn't report, it might still be triggered from outside...
        //     ACE_DEBUG ((LM_DEBUG,
        //                 ACE_TEXT ("(local) statistics reporting has been disabled...\n")));
      } // end IF

      break;
    }
    case SESSION_END:
    {
      // stop reporting timer
      fini_timers (false);

      // session finished --> print overall statistics ?
      if (printFinalReport_)
        final_report ();

      break;
    }
    case SESSION_STATISTICS:
    {
//       // *NOTE*: protect access to statistics data
//       // from asynchronous API calls (as well as local reporting)...
//       {
//         ACE_Guard<ACE_Thread_Mutex> aGuard (statsLock_);
//
//         currentStats_ = message_inout->getConfiguration ()->getStats ();
//
//         // remember previous timestamp (so we can satisfy our asynchronous API)...
//         lastStatsTimestamp_ = currentStatsTimestamp_;
//
//         currentStatsTimestamp_ = message_inout->getConfiguration ()->getStatGenerationTime ();
//       } // end lock scope

      break;
    }
    default:
      break;
  } // end SWITCH
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
void
Net_Module_Statistic_WriterTask_T<TaskSynchType,
                                  TimePolicyType,
                                  SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::reset ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_Module_Statistic_WriterTask_T::reset"));

  // this should happen every second (roughly)...
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

    // remember this result (satisfies an asynchronous API)...
    lastMessagesPerSecondCount_ = messageCounter_;
    lastBytesPerSecondCount_ = byteCounter_;

    // reset counters
    messageCounter_ = 0;
    byteCounter_ = 0;
  } // end lock scope
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
bool
Net_Module_Statistic_WriterTask_T<TaskSynchType,
                                  TimePolicyType,
                                  SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::collect (StatisticsContainerType& data_out) const
{
  NETWORK_TRACE (ACE_TEXT ("Net_Module_Statistic_WriterTask_T::collect"));

  // *NOTE*: external call, fill the argument with meaningful values

  // init return value(s)
  ACE_OS::memset (&data_out, 0, sizeof (StatisticsContainerType));

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

    data_out.numDataMessages = (numInboundMessages_ + numOutboundMessages_);
    data_out.numBytes = (numInboundBytes_ + numOutboundBytes_);
  } // end lock scope

  return true;
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
void
Net_Module_Statistic_WriterTask_T<TaskSynchType,
                                  TimePolicyType,
                                  SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::report () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_Module_Statistic_WriterTask_T::report"));

  // compute cache usage...
//   unsigned int cache_used = 0;
//   unsigned int cache_size = 0;
//   double        cache_used_relative = 0.0;
  unsigned int numMessagesOnline = 0;
  unsigned int totalHeapBytesAllocated = 0;
  if (allocator_)
  {
    numMessagesOnline = allocator_->cache_depth ();
    totalHeapBytesAllocated = allocator_->cache_size ();
//    ACE_ASSERT (cache_size);
//     cache_used_relative = cache_used / ((cache_size ?
//                                          cache_size : 1) * 100.0);
  } // end IF

  // ...write some output
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("*** [session: %u] RUNTIME STATISTICS ***\n--> Stream Statistics <--\nmessages seen (last second): %u\nmessages seen (total [in/out]): %u/%u (data: %.2f %%)\n data seen (last second): %u bytes\n data seen (total): %.0f bytes\n current cache usage [%u messages / %u total allocated heap]\n*** RUNTIME STATISTICS ***\\END\n"),
                sessionID_,
                lastMessagesPerSecondCount_,
                numInboundMessages_, numOutboundMessages_,
                (((numInboundMessages_ + numOutboundMessages_) - numSessionMessages_) / 100.0),
                lastBytesPerSecondCount_,
                (numInboundBytes_ + numOutboundBytes_),
                numMessagesOnline,
                totalHeapBytesAllocated));
//                cache_used,
//                cache_size,
//                cache_used_relative));
  } // end lock scope
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
void
Net_Module_Statistic_WriterTask_T<TaskSynchType,
                                  TimePolicyType,
                                  SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::final_report () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_Module_Statistic_WriterTask_T::final_report"));

  {
    // synchronize access to statistics data
    ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

    if ((numInboundMessages_ + numOutboundMessages_))
    {
      // write some output
      ACE_DEBUG ((LM_INFO,
                  ACE_TEXT ("*** [session: %u] SESSION STATISTICS ***\ntotal # data message(s) (as seen [in/out]): %u/%u\n --> Protocol Info <--\n"),
                  sessionID_,
                  numInboundMessages_,
                  numOutboundMessages_));

      std::string protocol_string;
      for (Net_MessageStatisticIterator_t iterator = messageTypeStatistics_.begin ();
           iterator != messageTypeStatistics_.end ();
           iterator++)
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("\"%s\": %u --> %.2f %%\n"),
                    ACE_TEXT (ProtocolMessageType::CommandType2String (iterator->first).c_str ()),
                    iterator->second,
                    static_cast<double> (((iterator->second * 100.0) / (numInboundMessages_ + numOutboundMessages_)))));
    } // end IF

//     double messages_per_sec = double (message_count) / et.real_time;
//     ACE_DEBUG ((LM_DEBUG,
//                 ACE_TEXT ("\t\tmessages = %d\n\t\ttotal bytes = %d\n\t\tmbits/sec = %f\n\t\tusec-per-message = %f\n\t\tmessages-per-second = %0.00f\n"),
//                 message_count,
//                 total_bytes,
//                 (((double) total_bytes * 8) / et.real_time) / (double) (1024 * 1024),
//                 (et.real_time / (double) message_count) * 1000000,
//                 messages_per_sec < 0 ? 0 : messages_per_sec));
  } // end lock scope
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
void
Net_Module_Statistic_WriterTask_T<TaskSynchType,
                                  TimePolicyType,
                                  SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::fini_timers (bool cancelAllTimers_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Module_Statistic_WriterTask_T::fini_timers"));

  const void* act = NULL;
  if (cancelAllTimers_in)
  {
    if (resetTimeoutHandlerID_ != -1)
    {
      if (COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (resetTimeoutHandlerID_,
                                                              &act) == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                    resetTimeoutHandlerID_));
      resetTimeoutHandlerID_ = -1;
    } // end IF
  } // end IF

  if (localReportingHandlerID_ != -1)
  {
    act = NULL;
    if (COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (localReportingHandlerID_,
                                                            &act) == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                  localReportingHandlerID_));
    localReportingHandlerID_ = -1;
  } // end IF
}

// ----------------------------------------------------------------------------

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
Net_Module_Statistic_ReaderTask_T<TaskSynchType,
                                  TimePolicyType,
                                  SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::Net_Module_Statistic_ReaderTask_T ()
// : inherited ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_Module_Statistic_ReaderTask_T::Net_Module_Statistic_ReaderTask_T"));

  inherited::flags_ |= ACE_Task_Flags::ACE_READER;
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
Net_Module_Statistic_ReaderTask_T<TaskSynchType,
                                   TimePolicyType,
                                   SessionMessageType,
                                   ProtocolMessageType,
                                   ProtocolCommandType,
                                   StatisticsContainerType>::~Net_Module_Statistic_ReaderTask_T ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_Module_Statistic_ReaderTask_T::~Net_Module_Statistic_ReaderTask_T"));

}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
int
Net_Module_Statistic_ReaderTask_T<TaskSynchType,
                                  TimePolicyType,
                                  SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::put (ACE_Message_Block* mb_in,
                                                                 ACE_Time_Value* tv_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Module_Statistic_ReaderTask_T::put"));

  // pass the message to the sibling
  ACE_Task_Base* sibling_task = inherited::sibling ();
  if (!sibling_task)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("no sibling task: \"%m\", aborting\n")));

    return -1;
  } // end IF
  Net_Module_Statistic_WriterTask_t* stream_task =
      dynamic_cast<Net_Module_Statistic_WriterTask_t*> (sibling_task);
  if (!stream_task)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dynamic_cast<Net_Module_Statistic_WriterTask_t>: \"%m\", aborting\n")));

    return -1;
  } // end IF
  ProtocolMessageType* message = dynamic_cast<ProtocolMessageType*> (mb_in);
  if (!message)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dynamic_cast<ProtocolMessageType>: \"%m\", aborting\n")));

    return -1;
  } // end IF

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard (stream_task->lock_);

    // update counters...
    stream_task->numOutboundMessages_++;
    stream_task->numOutboundBytes_ += mb_in->total_length ();

    stream_task->byteCounter_ += mb_in->total_length ();

    stream_task->messageCounter_++;

    // add message to statistic...
    stream_task->messageTypeStatistics_[message->getCommand ()]++;
  } // end lock scope

  return inherited::put (mb_in, tv_in);
}
