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

//#include "ace/Synch.h"
#include "test_u_stream.h"

#include "ace/Log_Msg.h"

#include "stream_stat_defines.h"

#include "net_defines.h"
#include "net_macros.h"

#include "test_u_defines.h"
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
Test_U_Stream::load (Stream_ILayout* layout_inout,
                     bool& delete_out)
{
  NETWORK_TRACE (ACE_TEXT ("Test_U_Stream::load"));

  if (!inherited::load (layout_inout,
                        delete_out))
    return false;

  Stream_Module_t* module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  Test_U_Module_HeaderParser_Module (this,
                                                     ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_MODULE_HEADERPARSER_NAME)),
                  false);
  layout_inout->append (module_p, NULL, 0);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  Test_U_Module_StatisticReport_Module (this,
                                                        ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                  false);
  layout_inout->append (module_p, NULL, 0);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  Test_U_Module_ProtocolHandler_Module (this,
                                                        ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_MODULE_PROTOCOLHANDLER_NAME)),
                  false);
  layout_inout->append (module_p, NULL, 0);

  delete_out = true;

  return true;
}

bool
Test_U_Stream::initialize (const CONFIGURATION_T& configuration_in,
                           ACE_HANDLE handle_in)
{
  NETWORK_TRACE (ACE_TEXT ("Test_U_Stream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!inherited::isInitialized_);

//  bool result = false;
  bool setup_pipeline = configuration_in.configuration->setupPipeline;
  bool reset_setup_pipeline = false;
//  struct Test_U_StreamSessionData* session_data_p = NULL;
  Stream_Module_t* module_p = NULL;
  //Test_U_Module_TCPSocketHandler* socketHandler_impl_p = NULL;

  // allocate a new session state, reset stream
  const_cast<inherited::CONFIGURATION_T&> (configuration_in).configuration->setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in,
                              handle_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Module_Net_IO_Stream_T::initialize(), aborting\n"),
                ACE_TEXT (stream_name_string_)));
    goto error;
  } // end IF
  const_cast<inherited::CONFIGURATION_T&> (configuration_in).configuration->setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;
  ACE_ASSERT (inherited::sessionData_);

//  session_data_p =
//      &const_cast<struct Test_U_StreamSessionData&> (inherited::sessionData_->get ());
  //session_data_p->sessionID = configuration_in.sessionID;

  //  configuration_in.moduleConfiguration.streamState = &state_;

  // ---------------------------------------------------------------------------

  // ******************* Socket Handler ************************
  module_p =
    //const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (NET_STREAM_MODULE_SOCKETHANDLER_DEFAULT_NAME_STRING)));
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_IO_DEFAULT_NAME_STRING)));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (stream_name_string_),
                //ACE_TEXT (NET_STREAM_MODULE_SOCKETHANDLER_DEFAULT_NAME_STRING)));
                ACE_TEXT (MODULE_NET_IO_DEFAULT_NAME_STRING)));
      goto error;
  } // end IF
  //socketHandler_impl_p =
  //  //dynamic_cast<Test_U_Module_TCPSocketHandler*> (module_p->writer ());
  //  dynamic_cast<inherited::WRITER_T*> (module_p->writer ());
  //if (!socketHandler_impl_p)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              //ACE_TEXT ("%s: dynamic_cast<Test_U_Module_TCPSocketHandler> failed, aborting\n"),
  //              ACE_TEXT ("%s: dynamic_cast<Stream_Module_Net_IOWriter_T> failed, aborting\n"),
  //              ACE_TEXT (stream_name_string_)));
  //  goto error;
  //} // end IF
  //socketHandler_impl_p->setP (&(inherited::state_));
  //socketHandler_impl_p->initialize ();

  // *NOTE*: push()ing the module will open() it
  //         --> set the argument that is passed along (head module expects a
  //             handle to the session data)
  module_p->arg (inherited::sessionData_);

  if (configuration_in.configuration->setupPipeline)
    if (!inherited::setup (configuration_in.configuration->notificationStrategy))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (stream_name_string_)));
      goto error;
    } // end IF

  // -------------------------------------------------------------

  // set (session) message allocator
  //inherited::allocator_ = configuration_in.messageAllocator;

  inherited::isInitialized_ = true;

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<inherited::CONFIGURATION_T&> (configuration_in).configuration->setupPipeline =
      setup_pipeline;

  return false;
}

void
Test_U_Stream::ping ()
{
  NETWORK_TRACE (ACE_TEXT ("Test_U_Stream::ping"));

  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_MODULE_PROTOCOLHANDLER_NAME)));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve \"%s\" module handle, returning\n"),
                ACE_TEXT (TEST_U_STREAM_MODULE_PROTOCOLHANDLER_NAME)));
    return;
  } // end IF
  Common_ITimerHandler* itimer_handler_p =
    dynamic_cast<Common_ITimerHandler*> (module_p->writer ());
  if (!itimer_handler_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Common_ITimerHandler>(0x%@) failed, returning\n"),
                module_p->writer ()));
    return;
  } // end IF

  // delegate to this module
  itimer_handler_p->handle (NULL);
}
