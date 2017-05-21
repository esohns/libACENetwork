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
#include "IRC_client_stream.h"

#include "IRC_client_common_modules.h"
#include "IRC_client_module_IRChandler.h"

IRC_Client_Stream::IRC_Client_Stream (const std::string& name_in)
 : inherited (name_in)
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_Stream::IRC_Client_Stream"));

}

IRC_Client_Stream::~IRC_Client_Stream ()
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_Stream::~IRC_Client_Stream"));

  // *NOTE*: this implements an ordered shutdown on destruction...
  inherited::shutdown ();
}

bool
IRC_Client_Stream::load (Stream_ModuleList_t& modules_out,
                         bool& delete_out)
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_Stream::load"));

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);

  Stream_Module_t* module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  IRC_Client_Module_IRCHandler_Module (ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_HANDLER_MODULE_NAME),
                                                       NULL,
                                                       false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  IRC_Client_Module_StatisticReport_Module (ACE_TEXT_ALWAYS_CHAR ("StatisticReport"),
                                                            NULL,
                                                            false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  IRC_Client_Module_Parser_Module (ACE_TEXT_ALWAYS_CHAR ("Parser"),
                                                   NULL,
                                                   false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  IRC_Client_Module_Marshal_Module (ACE_TEXT_ALWAYS_CHAR ("Marshal"),
                                                    NULL,
                                                    false),
                  false);
  modules_out.push_back (module_p);

  delete_out = true;

  return true;
}

bool
IRC_Client_Stream::initialize (const struct IRC_Client_StreamConfiguration& configuration_in)
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_Stream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!inherited::isInitialized_);
  ACE_ASSERT (!isRunning ());

//  bool result = false;
  bool setup_pipeline = configuration_in.setupPipeline;
  bool reset_setup_pipeline = false;
  struct IRC_Client_SessionData* session_data_p = NULL;
  typename inherited::ISTREAM_T::MODULE_T* module_p = NULL;
  IRC_Client_Module_Bisector_t* bisector_impl_p = NULL;

  // allocate a new session state, reset stream
  const_cast<struct IRC_Client_StreamConfiguration&> (configuration_in).setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (inherited::name_.c_str ())));
    goto error;
  } // end IF
  const_cast<struct IRC_Client_StreamConfiguration&> (configuration_in).setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;
  ACE_ASSERT (inherited::sessionData_);

  // things to be done here:
  // - create modules (done for the ones "owned" by the stream itself)
  // - initialize modules
  // - push them onto the stream (tail-first) !
  session_data_p =
      &const_cast<struct IRC_Client_SessionData&> (inherited::sessionData_->get ());
  inherited::state_.currentSessionData = session_data_p;
  session_data_p->sessionID = configuration_in.sessionID;

//  ACE_ASSERT (configuration_in.moduleConfiguration);
//  configuration_in.moduleConfiguration->streamState = &inherited::state_;

  // ---------------------------------------------------------------------------
  // ******************* Marshal ************************
  module_p =
    const_cast<typename inherited::ISTREAM_T::MODULE_T*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("Marshal")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (inherited::name_.c_str ()),
                ACE_TEXT ("Marshal")));
    goto error;
  } // end IF
  bisector_impl_p =
    dynamic_cast<IRC_Client_Module_Bisector_t*> (module_p->writer ());
  if (!bisector_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<IRC_Module_Bisector_T> failed, aborting\n"),
                ACE_TEXT (inherited::name_.c_str ())));
    goto error;
  } // end IF
  bisector_impl_p->set (&(inherited::state_));

  // enqueue the module
  // *NOTE*: push()ing the module will open() it
  //         --> set the argument that is passed along (head module expects a
  //             handle to the session data)
  module_p->arg (inherited::sessionData_);

  // ---------------------------------------------------------------------------

  if (configuration_in.setupPipeline)
    if (!inherited::setup (NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (inherited::name_.c_str ())));
      goto error;
    } // end IF

  inherited::isInitialized_ = true;
  //   inherited::dump_state();

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<struct IRC_Client_StreamConfiguration&> (configuration_in).setupPipeline =
      setup_pipeline;

  return false;
}

void
IRC_Client_Stream::ping ()
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_Stream::ping"));

  // delegate to the head module, skip over ACE_Stream_Head
  typename inherited::ISTREAM_T::MODULE_T* module_p = inherited::head ();
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("no head module found, returning\n")));
    return;
  } // end IF
  module_p = module_p->next ();
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("no head module found, returning\n")));
    return;
  } // end IF

  // sanity check: head == tail ? --> no modules have been push()ed (yet) !
  if (module_p == inherited::tail ())
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("no modules have been enqueued yet --> nothing to do !, returning\n")));
    return;
  } // end IF

  typename inherited::ISTREAM_CONTROL_T* control_impl_p = NULL;
  control_impl_p =
    dynamic_cast<typename inherited::ISTREAM_CONTROL_T*> (module_p->reader ());
  if (!control_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<Stream_IStreamControl> failed, returning\n"),
                module_p->name ()));
    return;
  } // end IF

  // *TODO*
  try {
//    control_impl->stop ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Stream_IStreamControl::stop (module: \"%s\"), returning\n"),
                module_p->name ()));
    return;
  }
}
