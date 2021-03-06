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

#include "ace/Log_Msg.h"

#include "net_macros.h"

#include "bittorrent_defines.h"

template <typename StreamStateType,
          typename ConfigurationType,
          typename StatisticContainerType,
          typename StatisticHandlerType,
          typename ModuleHandlerConfigurationType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConnectionConfigurationType,
          typename ConnectionStateType,
          typename HandlerConfigurationType,
          typename SessionStateType,
          typename ConnectionManagerType,
          typename UserDataType>
BitTorrent_TrackerStream_T<StreamStateType,
                           ConfigurationType,
                           StatisticContainerType,
                           StatisticHandlerType,
                           ModuleHandlerConfigurationType,
                           SessionDataType,
                           SessionDataContainerType,
                           ControlMessageType,
                           DataMessageType,
                           SessionMessageType,
                           ConnectionConfigurationType,
                           ConnectionStateType,
                           HandlerConfigurationType,
                           SessionStateType,
                           ConnectionManagerType,
                           UserDataType>::BitTorrent_TrackerStream_T ()
 : inherited ()
{
  NETWORK_TRACE (ACE_TEXT ("BitTorrent_TrackerStream_T::BitTorrent_TrackerStream_T"));

}

template <typename StreamStateType,
          typename ConfigurationType,
          typename StatisticContainerType,
          typename StatisticHandlerType,
          typename ModuleHandlerConfigurationType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConnectionConfigurationType,
          typename ConnectionStateType,
          typename HandlerConfigurationType,
          typename SessionStateType,
          typename ConnectionManagerType,
          typename UserDataType>
bool
BitTorrent_TrackerStream_T<StreamStateType,
                           ConfigurationType,
                           StatisticContainerType,
                           StatisticHandlerType,
                           ModuleHandlerConfigurationType,
                           SessionDataType,
                           SessionDataContainerType,
                           ControlMessageType,
                           DataMessageType,
                           SessionMessageType,
                           ConnectionConfigurationType,
                           ConnectionStateType,
                           HandlerConfigurationType,
                           SessionStateType,
                           ConnectionManagerType,
                           UserDataType>::load (Stream_ILayout* layout_inout,
                                                bool& deleteModules_out)
{
  NETWORK_TRACE (ACE_TEXT ("BitTorrent_TrackerStream_T::load"));

  // initialize return value(s)
  deleteModules_out = true;

  inherited::load (layout_inout,
                   deleteModules_out);

  //Stream_Module_t* module_p = NULL;
  //ACE_NEW_RETURN (module_p,
  //                MODULE_PARSER_T (this,
  //                                 ACE_TEXT_ALWAYS_CHAR (BITTORRENT_DEFAULT_TRACKER_PARSER_MODULE_NAME)),
  //                false);
  //layout_inout->append (module_p, NULL, 0);

  return true;
}

template <typename StreamStateType,
          typename ConfigurationType,
          typename StatisticContainerType,
          typename StatisticHandlerType,
          typename ModuleHandlerConfigurationType,
          typename SessionDataType,
          typename SessionDataContainerType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename ConnectionConfigurationType,
          typename ConnectionStateType,
          typename HandlerConfigurationType,
          typename SessionStateType,
          typename ConnectionManagerType,
          typename UserDataType>
bool
BitTorrent_TrackerStream_T<StreamStateType,
                           ConfigurationType,
                           StatisticContainerType,
                           StatisticHandlerType,
                           ModuleHandlerConfigurationType,
                           SessionDataType,
                           SessionDataContainerType,
                           ControlMessageType,
                           DataMessageType,
                           SessionMessageType,
                           ConnectionConfigurationType,
                           ConnectionStateType,
                           HandlerConfigurationType,
                           SessionStateType,
                           ConnectionManagerType,
// *TODO*: on MSVC 2015u3 the accurate declaration does not compile
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                           UserDataType>::initialize (const CONFIGURATION_T& configuration_in,
#else
                           UserDataType>::initialize (const typename inherited::CONFIGURATION_T& configuration_in,
#endif
                                                      ACE_HANDLE handle_in)
{
  NETWORK_TRACE (ACE_TEXT ("BitTorrent_TrackerStream_T::initialize"));

  // sanity check(s)
  ACE_ASSERT (!inherited::isInitialized_);
  ACE_ASSERT (!inherited::isRunning ());

//  int result = -1;
//  SessionDataType* session_data_p = NULL;
//  typename inherited::MODULE_T* module_p = NULL;
//  PARSER_T* parser_impl_p = NULL;

//  bool result = false;
  bool setup_pipeline = configuration_in.configuration->setupPipeline;
  bool reset_setup_pipeline = false;

  // allocate a new session state, reset stream
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration->setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in,
                              handle_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to HTTP_Stream_T::initialize(), aborting\n"),
                ACE_TEXT (inherited::name_.c_str ())));
    goto error;
  } // end IF
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration->setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;
  ACE_ASSERT (inherited::sessionData_);
//  session_data_p =
//    &const_cast<SessionDataType&> (inherited::sessionData_->get ());

//  session_data_p->sessionID = configuration_in.sessionID;

//  // ---------------------------------------------------------------------------
//  // sanity check(s)
//  ACE_ASSERT (configuration_in.moduleConfiguration);

  //  configuration_in.moduleConfiguration.streamState = &state_;

  // ---------------------------------------------------------------------------

//  // ******************* Handler ************************
//  IRC_Module_Handler* handler_impl = NULL;
//  handler_impl = dynamic_cast<IRC_Module_Handler*> (handler_.writer ());
//  if (!handler_impl)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("dynamic_cast<IRC_Module_Handler> failed, aborting\n")));
//    return false;
//  } // end IF
//  if (!handler_impl->initialize (configuration_in.messageAllocator,
//                                 (configuration_in.clientPingInterval ? false // servers shouldn't receive "pings" in the first place
//                                  : NET_DEF_PING_PONG), // auto-answer "ping" as a client ?...
//                                 (configuration_in.clientPingInterval == 0))) // clients print ('.') dots for received "pings"...
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
//                handler_.name ()));
//    return false;
//  } // end IF

  // ******************* Statistic Report ************************
  //STATISTIC_WRITER_T* runtimeStatistic_impl_p =
  //  dynamic_cast<STATISTIC_WRITER_T*> (runtimeStatistic_.writer ());
  //if (!runtimeStatistic_impl_p)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("dynamic_cast<Net_Module_Statistic_WriterTask_T> failed, aborting\n")));
  //  return false;
  //} // end IF
  //if (!runtimeStatistic_impl_p->initialize (configuration_in.statisticReportingInterval,
  //                                          configuration_in.messageAllocator))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
  //              runtimeStatistic_.name ()));
  //  return false;
  //} // end IF

  //// ******************* Parser ************************
  //PARSER_T* parser_impl_p = NULL;
  //parser_impl_p =
  //  dynamic_cast<PARSER_T*> (parser_.writer ());
  //if (!parser_impl_p)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("dynamic_cast<IRC_Module_Parser_T*> failed, aborting\n")));
  //  return false;
  //} // end IF
  //if (!parser_impl_p->initialize (configuration_in.messageAllocator,                            // message allocator
  //                                configuration_in.moduleHandlerConfiguration_2.crunchMessages, // "crunch" messages ?
  //                                configuration_in.moduleHandlerConfiguration_2.traceScanning,  // debug scanner ?
  //                                configuration_in.moduleHandlerConfiguration_2.traceParsing))  // debug parser ?
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
  //              parser_.name ()));
  //  return false;
  //} // end IF

//  // ******************* Marshal ************************
//  module_p =
//      const_cast<typename inherited::MODULE_T*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("Marshal")));
//  ACE_ASSERT (module_p);
//  parser_impl_p = dynamic_cast<PARSER_T*> (module_p->writer ());
//  if (!parser_impl_p)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("dynamic_cast<BitTorrent_Module_Parser_T*> failed, aborting\n")));
//    goto error;
//  } // end IF
////  if (!parser_impl_p->initialize (*configuration_in.moduleHandlerConfiguration))
////  {
////    ACE_DEBUG ((LM_ERROR,
////                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
////                marshal_.name ()));
////    return false;
////  } // end IF
//  if (!parser_impl_p->initialize (inherited::state_))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("%s: failed to initialize writer task, aborting\n"),
//                module_p->name ()));
//    goto error;
//  } // end IF

//  // *NOTE*: push()ing the module will open() it
//  //         --> set the argument that is passed along (head module expects a
//  //             handle to the session data)
//  module_p->arg (inherited::sessionData_);

  if (configuration_in.configuration->setupPipeline)
    if (!inherited::setup (configuration_in.configuration->notificationStrategy))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (inherited::name_.c_str ())));
      goto error;
    } // end IF

  inherited::isInitialized_ = true;

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration->setupPipeline =
      setup_pipeline;

  return false;
}
