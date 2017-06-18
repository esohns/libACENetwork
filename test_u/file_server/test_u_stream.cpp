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
#include "stdafx.h"

#include <ace/Synch.h>
#include "test_u_connection_common.h"
#include "test_u_stream.h"

#include "ace/Log_Msg.h"

#include "net_defines.h"
#include "net_macros.h"

#include "test_u_message.h"
#include "test_u_sessionmessage.h"
#include "test_u_common_modules.h"
#include "test_u_module_headerparser.h"
#include "test_u_module_protocolhandler.h"

Test_U_Stream::Test_U_Stream ()
 : inherited ()
{
  NETWORK_TRACE (ACE_TEXT ("Test_U_Stream::Test_U_Stream"));

}

Test_U_Stream::~Test_U_Stream ()
{
  NETWORK_TRACE (ACE_TEXT ("Test_U_Stream::~Test_U_Stream"));

  // *NOTE*: this implements an ordered shutdown on destruction
  inherited::shutdown ();
}

bool
Test_U_Stream::load (Stream_ModuleList_t& modules_out,
                     bool& delete_out)
{
  NETWORK_TRACE (ACE_TEXT ("Test_U_Stream::load"));

  Stream_Module_t* module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  Test_U_FileReader_Module (this,
                                            ACE_TEXT_ALWAYS_CHAR ("FileSource"),
                                            NULL,
                                            false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  Test_U_MPEG_TS_Decoder_Module (this,
                                                 ACE_TEXT_ALWAYS_CHAR ("MPEGTSDecoder"),
                                                 NULL,
                                                 false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  Test_U_StatisticReport_Module (this,
                                                 ACE_TEXT_ALWAYS_CHAR ("StatisticReport"),
                                                 NULL,
                                                 false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  Test_U_Net_IO_Module (this,
                                        ACE_TEXT_ALWAYS_CHAR ("NetworkIO"),
                                        NULL,
                                        false),
                  false);
  modules_out.push_back (module_p);

  delete_out = true;

  return true;
}

bool
Test_U_Stream::initialize (const typename inherited::CONFIGURATION_T& configuration_in)
{
  NETWORK_TRACE (ACE_TEXT ("Test_U_Stream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!inherited::isInitialized_);

  // update configuration
  typename inherited::MODULE_T* module_p = inherited::head ();
  ACE_ASSERT (module_p);
  typename inherited::TASK_T* task_p = module_p->reader ();
  ACE_ASSERT (task_p);
  typename inherited::CONFIGURATION_T::ITERATOR_T iterator =
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).end ());
  (*iterator).second.outboundQueue =
    dynamic_cast<Stream_IMessageQueue*> (task_p->msg_queue ());
  ACE_ASSERT ((*iterator).second.outboundQueue);

//  bool result = false;
  bool setup_pipeline = configuration_in.configuration_.setupPipeline;
  bool reset_setup_pipeline = false;
  struct FileServer_SessionData* session_data_p = NULL;
  Test_U_Module_Net_Writer_t* net_io_impl_p = NULL;

  // allocate a new session state, reset stream
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (inherited::configuration_.name_.c_str ())));
    goto error;
  } // end IF
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;
  ACE_ASSERT (inherited::sessionData_);

  session_data_p =
      &const_cast<struct FileServer_SessionData&> (inherited::sessionData_->get ());
  //session_data_p->sessionID = configuration_in.sessionID;

  //  configuration_in.moduleConfiguration.streamState = &state_;

  // ---------------------------------------------------------------------------

  // ******************* Socket Handler ************************
  module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("NetworkIO")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (inherited::configuration_.name_.c_str ()),
                ACE_TEXT ("NetworkIO")));
    goto error;
  } // end IF
  net_io_impl_p =
    dynamic_cast<Test_U_Module_Net_Writer_t*> (module_p->writer ());
  if (!net_io_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<Stream_Module_Net_IOWriter_T> failed, aborting\n"),
                ACE_TEXT (inherited::configuration_.name_.c_str ())));
    goto error;
  } // end IF
  net_io_impl_p->set (&(inherited::state_));

  // *NOTE*: push()ing the module will open() it
  //         --> set the argument that is passed along (head module expects a
  //             handle to the session data)
  module_p->arg (inherited::sessionData_);

  if (configuration_in.configuration_.setupPipeline)
    if (!inherited::setup (configuration_in.configuration_.notificationStrategy))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (inherited::configuration_.name_.c_str ())));
      goto error;
    } // end IF

  // -------------------------------------------------------------

  // set (session) message allocator
  //inherited::allocator_ = configuration_in.messageAllocator;

  inherited::isInitialized_ = true;

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
      setup_pipeline;

  return false;
}

bool
Test_U_Stream::collect (Net_RuntimeStatistic_t& data_out)
{
  NETWORK_TRACE (ACE_TEXT ("Test_U_Stream::collect"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);

  int result = -1;

  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("RuntimeStatistic")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT ("RuntimeStatistic")));
    return false;
  } // end IF
  Test_U_Module_StatisticReport_WriterTask_t* runtimeStatistic_impl =
    dynamic_cast<Test_U_Module_StatisticReport_WriterTask_t*> (module_p->writer ());
  if (!runtimeStatistic_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Test_U_Module_StatisticReport_WriterTask_t> failed, aborting\n")));
    return false;
  } // end IF

  // synch access
  struct FileServer_SessionData& session_data_r =
      const_cast<struct FileServer_SessionData&> (inherited::sessionData_->get ());
  if (session_data_r.lock)
  {
    result = session_data_r.lock->acquire ();
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", aborting\n")));
      return false;
    } // end IF
  } // end IF

  session_data_r.currentStatistic.timeStamp = COMMON_TIME_NOW;

  // delegate to the statistics module
  bool result_2 = false;
  try {
    result_2 = runtimeStatistic_impl->collect (data_out);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IStatistic_T::collect(), continuing\n")));
  }
  if (!result)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_IStatistic_T::collect(), aborting\n")));
  else
    session_data_r.currentStatistic = data_out;

  if (session_data_r.lock)
  {
    result = session_data_r.lock->release ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n")));
  } // end IF

  return result_2;
}

void
Test_U_Stream::report () const
{
  NETWORK_TRACE (ACE_TEXT ("Test_U_Stream::report"));

//   Net_Module_Statistic_ReaderTask_t* runtimeStatistic_impl = NULL;
//   runtimeStatistic_impl = dynamic_cast<Net_Module_Statistic_ReaderTask_t*> (//runtimeStatistic_.writer ());
//   if (!runtimeStatistic_impl)
//   {
//     ACE_DEBUG ((LM_ERROR,
//                 ACE_TEXT ("dynamic_cast<Net_Module_Statistic_ReaderTask_t> failed, returning\n")));
//
//     return;
//   } // end IF
//
//   // delegate to this module...
//   return (runtimeStatistic_impl->report ());

  // just a dummy
  ACE_ASSERT (false);

  ACE_NOTREACHED (return;)
}

//////////////////////////////////////////

Test_U_UDPStream::Test_U_UDPStream ()
 : inherited ()
{
  NETWORK_TRACE (ACE_TEXT ("Test_U_UDPStream::Test_U_UDPStream"));

}

Test_U_UDPStream::~Test_U_UDPStream ()
{
  NETWORK_TRACE (ACE_TEXT ("Test_U_UDPStream::~Test_U_UDPStream"));

  // *NOTE*: this implements an ordered shutdown on destruction
  inherited::shutdown ();
}

bool
Test_U_UDPStream::load (Stream_ModuleList_t& modules_out,
                        bool& delete_out)
{
  NETWORK_TRACE (ACE_TEXT ("Test_U_UDPStream::load"));

  Stream_Module_t* module_p = NULL;
  if (inherited::configuration_.configuration_.useReactor)
    ACE_NEW_RETURN (module_p,
                    Test_U_Module_Net_UDPTarget_Module (this,
                                                        ACE_TEXT_ALWAYS_CHAR ("NetworkTarget"),
                                                        NULL,
                                                        false),
                    false);
  else
    ACE_NEW_RETURN (module_p,
                  Test_U_Module_Net_AsynchUDPTarget_Module (this,
                                                            ACE_TEXT_ALWAYS_CHAR ("NetworkTarget"),
                                                            NULL,
                                                            false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  Test_U_MPEG_TS_Decoder_Module (this,
                                                 ACE_TEXT_ALWAYS_CHAR ("MPEGTSDecoder"),
                                                 NULL,
                                                 false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  Test_U_StatisticReport_Module (this,
                                                 ACE_TEXT_ALWAYS_CHAR ("StatisticReport"),
                                                 NULL,
                                                 false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  Test_U_FileReaderH_Module (this,
                                             ACE_TEXT_ALWAYS_CHAR ("FileSource"),
                                             NULL,
                                             false),
                  false);
  modules_out.push_back (module_p);

  delete_out = true;

  return true;
}

bool
Test_U_UDPStream::initialize (const typename inherited::CONFIGURATION_T& configuration_in)
{
  NETWORK_TRACE (ACE_TEXT ("Test_U_UDPStream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!inherited::isInitialized_);

  // update configuration
  typename inherited::CONFIGURATION_T::ITERATOR_T iterator =
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).end ());
  (*iterator).second.stream = this;

  bool result = false;
  bool setup_pipeline = configuration_in.configuration_.setupPipeline;
  bool reset_setup_pipeline = false;
  struct FileServer_SessionData* session_data_p = NULL;
  Stream_Module_t* module_p = NULL;
  Test_U_FileReaderH* file_source_impl_p = NULL;

  // allocate a new session state, reset stream
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (inherited::configuration_.name_.c_str ())));
    goto error;
  } // end IF
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;
  ACE_ASSERT (inherited::sessionData_);

  session_data_p =
      &const_cast<struct FileServer_SessionData&> (inherited::sessionData_->get ());
  //session_data_p->sessionID = configuration_in.sessionID;

  //  configuration_in.moduleConfiguration.streamState = &state_;

  // ---------------------------------------------------------------------------

  // ******************* File Reader ************************
  module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("FileSource")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (inherited::configuration_.name_.c_str ()),
                ACE_TEXT ("FileSource")));
    goto error;
  } // end IF
  file_source_impl_p =
    dynamic_cast<Test_U_FileReaderH*> (module_p->writer ());
  if (!file_source_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<Test_U_FileReaderH> failed, aborting\n"),
                ACE_TEXT (inherited::configuration_.name_.c_str ())));
    goto error;
  } // end IF
  file_source_impl_p->set (&(inherited::state_));

  // *NOTE*: push()ing the module will open() it
  //         --> set the argument that is passed along (head module expects a
  //             handle to the session data)
  module_p->arg (inherited::sessionData_);

  if (configuration_in.configuration_.setupPipeline)
    if (!inherited::setup (configuration_in.configuration_.notificationStrategy))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (inherited::configuration_.name_.c_str ())));
      goto error;
    } // end IF

  // -------------------------------------------------------------

  // set (session) message allocator
  //inherited::allocator_ = configuration_in.messageAllocator;

  inherited::isInitialized_ = true;

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
      setup_pipeline;

  return false;
}

bool
Test_U_UDPStream::collect (Net_RuntimeStatistic_t& data_out)
{
  NETWORK_TRACE (ACE_TEXT ("Test_U_UDPStream::collect"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);

  int result = -1;

  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("RuntimeStatistic")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT ("RuntimeStatistic")));
    return false;
  } // end IF
  Test_U_Module_StatisticReport_WriterTask_t* runtimeStatistic_impl =
    dynamic_cast<Test_U_Module_StatisticReport_WriterTask_t*> (module_p->writer ());
  if (!runtimeStatistic_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Test_U_Module_StatisticReport_WriterTask_t> failed, aborting\n")));
    return false;
  } // end IF

  // synch access
  struct FileServer_SessionData& session_data_r =
      const_cast<struct FileServer_SessionData&> (inherited::sessionData_->get ());
  if (session_data_r.lock)
  {
    result = session_data_r.lock->acquire ();
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", aborting\n")));
      return false;
    } // end IF
  } // end IF

  session_data_r.currentStatistic.timeStamp = COMMON_TIME_NOW;

  // delegate to the statistics module
  bool result_2 = false;
  try {
    result_2 = runtimeStatistic_impl->collect (data_out);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IStatistic_T::collect(), continuing\n")));
  }
  if (!result)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_IStatistic_T::collect(), aborting\n")));
  else
    session_data_r.currentStatistic = data_out;

  if (session_data_r.lock)
  {
    result = session_data_r.lock->release ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n")));
  } // end IF

  return result_2;
}

void
Test_U_UDPStream::report () const
{
  NETWORK_TRACE (ACE_TEXT ("Test_U_UDPStream::report"));

//   Net_Module_Statistic_ReaderTask_t* runtimeStatistic_impl = NULL;
//   runtimeStatistic_impl = dynamic_cast<Net_Module_Statistic_ReaderTask_t*> (//runtimeStatistic_.writer ());
//   if (!runtimeStatistic_impl)
//   {
//     ACE_DEBUG ((LM_ERROR,
//                 ACE_TEXT ("dynamic_cast<Net_Module_Statistic_ReaderTask_t> failed, returning\n")));
//
//     return;
//   } // end IF
//
//   // delegate to this module
//   return (runtimeStatistic_impl->report ());

  // just a dummy
  ACE_ASSERT (false);

  ACE_NOTREACHED (return;)
}
