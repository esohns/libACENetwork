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

#include "stream_dec_tools.h"

#include "net_macros.h"

#include "http_common.h"
#include "http_defines.h"
#include "http_tools.h"

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType>
HTTP_Module_Parser_T<ACE_SYNCH_USE,
                     TimePolicyType,
                     ConfigurationType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType>::HTTP_Module_Parser_T ()
 : inherited ()
 , inherited2 (NET_PROTOCOL_DEFAULT_LEX_TRACE,  // trace scanning ?
               NET_PROTOCOL_DEFAULT_YACC_TRACE) // trace parsing ?
 , headFragment_ (NULL)
 , crunch_ (HTTP_DEFAULT_CRUNCH_MESSAGES) // strip protocol data ?
 , debugScanner_ (NET_PROTOCOL_DEFAULT_LEX_TRACE) // trace scanning ?
 , debugParser_ (NET_PROTOCOL_DEFAULT_YACC_TRACE) // trace parsing ?
 //, lock_ ()
//, condtion_ (lock_)
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_Module_Parser_T::HTTP_Module_Parser_T"));

}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType>
HTTP_Module_Parser_T<ACE_SYNCH_USE,
                     TimePolicyType,
                     ConfigurationType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType>::~HTTP_Module_Parser_T ()
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_Module_Parser_T::~HTTP_Module_Parser_T"));

  if (headFragment_)
    headFragment_->release ();
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType>
bool
HTTP_Module_Parser_T<ACE_SYNCH_USE,
                     TimePolicyType,
                     ConfigurationType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType>::initialize (const ConfigurationType& configuration_in,
                                                      Stream_IAllocator* allocator_in)
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_Module_Parser_T::initialize"));

  int result = -1;

  if (inherited::isInitialized_)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("re-initializing...\n")));

    ACE_ASSERT (inherited::msg_queue_);
    result = inherited::msg_queue_->activate ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Message_Queue_Base::activate(): \"%m\", continuing\n")));

    crunch_ = HTTP_DEFAULT_CRUNCH_MESSAGES;

    debugScanner_ = NET_PROTOCOL_DEFAULT_LEX_TRACE;
    debugParser_ = NET_PROTOCOL_DEFAULT_YACC_TRACE;
    if (headFragment_)
    {
      headFragment_->release ();
      headFragment_ = NULL;
    } // end IF
  } // end IF

  crunch_ = configuration_in.crunchMessages;

  debugScanner_ = configuration_in.traceScanning;
  debugParser_ = configuration_in.traceParsing;

  inherited2::initialize (debugScanner_,
                          debugParser_,
                          inherited::msg_queue_,
//                          true,
                          true);

  return inherited::initialize (configuration_in,
                                allocator_in);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType>
void
HTTP_Module_Parser_T<ACE_SYNCH_USE,
                     TimePolicyType,
                     ConfigurationType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType>::handleDataMessage (DataMessageType*& message_inout,
                                                             bool& passMessageDownstream_out)
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_Module_Parser_T::handleDataMessage"));

  DataMessageType* message_p = NULL;
  int result = -1;
  bool release_inbound_message = true; // message_inout
  bool release_message = false; // message_p

  // initialize return value(s)
  passMessageDownstream_out = false;

  // sanity check(s)
  ACE_ASSERT (inherited::mod_);

  // append the "\0\0"-sequence, as required by flex
  ACE_ASSERT (message_inout->capacity () - message_inout->length () >=
              NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE);
  *(message_inout->wr_ptr ()) = YY_END_OF_BUFFER_CHAR;
  *(message_inout->wr_ptr () + 1) = YY_END_OF_BUFFER_CHAR;
  // *NOTE*: DO NOT adjust the write pointer --> length() must stay as it was

  {
    //ACE_Guard<ACE_SYNCH_MUTEX> aGuard (lock_);

    if (!headFragment_)
      headFragment_ = message_inout;
    else
    {
      for (message_p = headFragment_;
           message_p->cont ();
           message_p = dynamic_cast<DataMessageType*> (message_p->cont ()));
      message_p->cont (message_inout);

      //// just signal the parser (see below for an explanation)
      //result = condition_.broadcast ();
      //if (result == -1)
      //  ACE_DEBUG ((LM_ERROR,
      //              ACE_TEXT ("%s: failed to ACE_SYNCH_CONDITION::broadcast(): \"%s\", continuing\n"),
      //              inherited::mod_->name ()));
    } // end ELSE

    message_p = headFragment_;
  } // end lock scope
  ACE_ASSERT (message_p);
  message_inout = NULL;
  release_inbound_message = false;

  { // *NOTE*: protect scanner/parser state
    //ACE_Guard<ACE_SYNCH_MUTEX> aGuard (lock_);

    // OK: parse the message (fragment)

    //  ACE_DEBUG ((LM_DEBUG,
    //              ACE_TEXT ("parsing message (ID:%u,%u byte(s))...\n"),
    //              message_p->id (),
    //              message_p->length ()));

    if (!this->parse (message_p))
    { // *NOTE*: most probable reason: connection
      //         has been closed --> session end
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: failed to HTTP_IParser::parse() (message ID was: %d), returning\n"),
                  inherited::mod_->name (),
                  message_p->id ()));
      goto error;
    } // end IF
    // the message fragment has been parsed successfully

    if (!this->hasFinished ())
      goto continue_; // --> wait for more data to arrive

    // set session data format
    // sanity check(s)
    ACE_ASSERT (inherited::sessionData_);
    ACE_ASSERT (inherited2::record_);

    typename SessionMessageType::DATA_T::DATA_T& session_data_r =
      const_cast<typename SessionMessageType::DATA_T::DATA_T&> (inherited::sessionData_->get ());

    HTTP_HeadersIterator_t iterator =
      inherited2::record_->headers.find (ACE_TEXT_ALWAYS_CHAR (HTTP_PRT_HEADER_CONTENT_ENCODING_STRING));
    if (iterator != inherited2::record_->headers.end ())
    {
      session_data_r.format =
        HTTP_Tools::Encoding2CompressionFormat ((*iterator).second);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: set compression format: \"%s\"\n"),
                  inherited::mod_->name (),
                  ACE_TEXT (Stream_Module_Decoder_Tools::compressionFormatToString (session_data_r.format).c_str ())));
    } // end IF

    // make sure the whole fragment chain references the same data record
    // sanity check(s)
    ACE_ASSERT (headFragment_->isInitialized ());
    DATA_CONTAINER_T* data_container_p =
        &const_cast<DATA_CONTAINER_T&> (headFragment_->get ());
    DATA_CONTAINER_T* data_container_2 = NULL;
    message_p = dynamic_cast<DataMessageType*> (headFragment_->cont ());
    while (message_p)
    {
      data_container_p->increase ();
      data_container_2 = data_container_p;
      message_p->initialize (data_container_2,
                             NULL);
      message_p = dynamic_cast<DataMessageType*> (message_p->cont ());
    } // end WHILE
  } // end lock scope

  // *NOTE*: the message has been parsed successfully
  //         --> pass the data (chain) downstream
  {
    //ACE_Guard<ACE_SYNCH_MUTEX> aGuard (lock_);

    //// *NOTE*: new data fragments may have arrived by now
    ////         --> set the next head fragment ?
    //message_2 = dynamic_cast<DataMessageType*> (message_p->cont ());
    //if (message_2)
    //  message_p->cont (NULL);

    result = inherited::put_next (headFragment_, NULL);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_Task_T::put_next(): \"%m\", returning\n"),
                  inherited::mod_->name ()));

      // clean up
      headFragment_->release ();

      goto error;
    } // end IF
    headFragment_ = NULL;
  } // end lock scope

continue_:
error:
  if (release_inbound_message)
  {
    ACE_ASSERT (message_inout);
    message_inout->release ();
    message_inout = NULL;
  } // end IF
  if (release_message)
  {
    ACE_ASSERT (message_p);
    message_p->release ();
  } // end IF
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType>
void
HTTP_Module_Parser_T<ACE_SYNCH_USE,
                     TimePolicyType,
                     ConfigurationType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                bool& passMessageDownstream_out)
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_Module_Parser_T::handleSessionMessage"));

  //int result = -1;

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  switch (message_inout->type ())
  {
    case STREAM_SESSION_MESSAGE_END:
    {
      //// *NOTE*: (in a 'passive' scenario,) a parser thread may be waiting for
      ////         additional (entity) fragments to arrive
      ////         --> tell it to return
      //ACE_ASSERT (inherited::msg_queue_);
      //result = inherited::msg_queue_->pulse ();
      //if (result == -1)
      //  ACE_DEBUG ((LM_ERROR,
      //              ACE_TEXT ("failed to ACE_Message_Queue_Base::pulse(): \"%m\", continuing\n")));

      if (headFragment_)
      {
        headFragment_->release ();
        headFragment_ = NULL;
      } // end IF

      break;
    }
    default:
      break;
  } // end SWITCH
}

//template <ACE_SYNCH_DECL,
//          typename TimePolicyType,
//          typename ConfigurationType,
//          typename ControlMessageType,
//          typename DataMessageType,
//          typename SessionMessageType>
//DataMessageType*
//HTTP_Module_Parser_T<ACE_SYNCH_USE,
//                     TimePolicyType,
//                     ConfigurationType,
//                     ControlMessageType,
//                     DataMessageType,
//                     SessionMessageType>::allocateMessage (unsigned int requestedSize_in)
//{
//  NETWORK_TRACE (ACE_TEXT ("HTTP_Module_Parser_T::allocateMessage"));

//  // initialize return value(s)
//  DataMessageType* message_p = NULL;

//  if (allocator_)
//  {
//allocate:
//    try {
//      message_p =
//        static_cast<DataMessageType*> (allocator_->malloc (requestedSize_in));
//    } catch (...) {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("caught exception in Stream_IAllocator::malloc(%u), aborting\n"),
//                  requestedSize_in));
//      return NULL;
//    }

//    // keep retrying ?
//    if (!message_p && !allocator_->block ())
//      goto allocate;
//  } // end IF
//  else
//    ACE_NEW_NORETURN (message_p,
//                      DataMessageType (requestedSize_in));
//  if (!message_p)
//  {
//    if (allocator_)
//    {
//      if (allocator_->block ())
//        ACE_DEBUG ((LM_CRITICAL,
//                    ACE_TEXT ("failed to allocate DataMessageType: \"%m\", aborting\n")));
//    } // end IF
//    else
//      ACE_DEBUG ((LM_CRITICAL,
//                  ACE_TEXT ("failed to allocate DataMessageType: \"%m\", aborting\n")));
//  } // end IF

//  return message_p;
//}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType>
void
HTTP_Module_Parser_T<ACE_SYNCH_USE,
                     TimePolicyType,
                     ConfigurationType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType>::record (struct HTTP_Record*& record_inout)
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_Module_Parser_T::record"));

  // sanity check(s)
  ACE_ASSERT (record_inout);
  ACE_ASSERT (record_inout == inherited2::record_);
  ACE_ASSERT (!headFragment_->isInitialized ());

  // debug info
  if (inherited2::trace_)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("%s"),
                ACE_TEXT (HTTP_Tools::dump (*record_inout).c_str ())));

  DATA_CONTAINER_T& data_container_r =
      const_cast<DATA_CONTAINER_T&> (headFragment_->get ());
  data_container_r.set (record_inout);
  record_inout = NULL;

  inherited2::finished_ = true;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType>
void
HTTP_Module_Parser_T<ACE_SYNCH_USE,
                     TimePolicyType,
                     ConfigurationType,
                     ControlMessageType,
                     DataMessageType,
                     SessionMessageType>::encoding (const std::string& encoding_in)
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_Module_Parser_T::encoding"));

  ACE_UNUSED_ARG (encoding_in);
}

////////////////////////////////////////////////////////////////////////////////

template <ACE_SYNCH_DECL,
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
HTTP_Module_ParserH_T<ACE_SYNCH_USE,
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
                      StatisticContainerType>::HTTP_Module_ParserH_T ()
 : inherited (NULL,  // lock handle
              false, // auto-start ?
              true)  // generate sesssion messages ?
 , inherited2 (NET_PROTOCOL_DEFAULT_LEX_TRACE,  // trace scanning ?
               NET_PROTOCOL_DEFAULT_YACC_TRACE) // trace parsing ?
 , headFragment_ (NULL)
 , crunch_ (HTTP_DEFAULT_CRUNCH_MESSAGES) // strip protocol data ?
 , debugScanner_ (NET_PROTOCOL_DEFAULT_LEX_TRACE) // trace scanning ?
 , debugParser_ (NET_PROTOCOL_DEFAULT_YACC_TRACE) // trace parsing ?
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_Module_ParserH_T::HTTP_Module_ParserH_T"));

}

template <ACE_SYNCH_DECL,
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
HTTP_Module_ParserH_T<ACE_SYNCH_USE,
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
                      StatisticContainerType>::~HTTP_Module_ParserH_T ()
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_Module_ParserH_T::~HTTP_Module_ParserH_T"));

  if (headFragment_)
    headFragment_->release ();
}

template <ACE_SYNCH_DECL,
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
HTTP_Module_ParserH_T<ACE_SYNCH_USE,
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
                      StatisticContainerType>::initialize (const ConfigurationType& configuration_in,
                                                           Stream_IAllocator* allocator_in)
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_Module_ParserH_T::initialize"));

  // sanity check(s)
  ACE_ASSERT (configuration_in.streamConfiguration);

  if (inherited::isInitialized_)
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("re-initializing...\n")));

    crunch_ = HTTP_DEFAULT_CRUNCH_MESSAGES;

    debugScanner_ = NET_PROTOCOL_DEFAULT_LEX_TRACE;
    debugParser_ = NET_PROTOCOL_DEFAULT_YACC_TRACE;
    if (headFragment_)
    {
      headFragment_->release ();
      headFragment_ = NULL;
    } // end IF

    inherited::isInitialized_ = false;
  } // end IF

  crunch_ = configuration_in.crunchMessages;

  debugScanner_ = configuration_in.traceScanning;
  debugParser_ = configuration_in.traceParsing;

  inherited2::initialize (debugScanner_,
                          debugParser_,
                          inherited::msg_queue_,
//                          true,
                          true);

  return inherited::initialize (configuration_in,
                                allocator_in);
}

template <ACE_SYNCH_DECL,
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
HTTP_Module_ParserH_T<ACE_SYNCH_USE,
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
  NETWORK_TRACE (ACE_TEXT ("HTTP_Module_ParserH_T::handleDataMessage"));

  DataMessageType* message_p = NULL;
  int result = -1;
  bool release_inbound_message = true; // message_inout
  bool release_message = false; // message_p

  // initialize return value(s)
  passMessageDownstream_out = false;

  // sanity check(s)
  ACE_ASSERT (inherited::mod_);

  // append the "\0\0"-sequence, as required by flex
  ACE_ASSERT (message_inout->capacity () - message_inout->length () >=
              NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE);
  *(message_inout->wr_ptr ()) = YY_END_OF_BUFFER_CHAR;
  *(message_inout->wr_ptr () + 1) = YY_END_OF_BUFFER_CHAR;
  // *NOTE*: DO NOT adjust the write pointer --> length() must stay as it was

  {
    //ACE_Guard<ACE_SYNCH_MUTEX> aGuard (lock_);

    if (!headFragment_)
      headFragment_ = message_inout;
    else
    {
      for (message_p = headFragment_;
           message_p->cont ();
           message_p = dynamic_cast<DataMessageType*> (message_p->cont ()));
      message_p->cont (message_inout);

      //// just signal the parser (see below for an explanation)
      //result = condition_.broadcast ();
      //if (result == -1)
      //  ACE_DEBUG ((LM_ERROR,
      //              ACE_TEXT ("%s: failed to ACE_SYNCH_CONDITION::broadcast(): \"%s\", continuing\n"),
      //              inherited::mod_->name ()));
    } // end ELSE

    message_p = headFragment_;
  } // end lock scope
  ACE_ASSERT (message_p);
  message_inout = NULL;
  release_inbound_message = false;

  { // *NOTE*: protect scanner/parser state
    //ACE_Guard<ACE_SYNCH_MUTEX> aGuard (lock_);

    // OK: parse the message (fragment)

    //  ACE_DEBUG ((LM_DEBUG,
    //              ACE_TEXT ("parsing message (ID:%u,%u byte(s))...\n"),
    //              message_p->id (),
    //              message_p->length ()));

    if (!this->parse (message_p))
    { // *NOTE*: most probable reason: connection
      //         has been closed --> session end
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: failed to HTTP_ParserDriver::parse() (message ID was: %d), returning\n"),
                  inherited::mod_->name (),
                  message_p->id ()));
      goto error;
    } // end IF
    // the message fragment has been parsed successfully

    if (!this->hasFinished ())
      goto continue_; // --> wait for more data to arrive

    // set session data format
    // sanity check(s)
    ACE_ASSERT (inherited::sessionData_);
    ACE_ASSERT (inherited2::record_);

    typename SessionMessageType::DATA_T::DATA_T& session_data_r =
      const_cast<typename SessionMessageType::DATA_T::DATA_T&> (inherited::sessionData_->get ());

    HTTP_HeadersIterator_t iterator =
      inherited2::record_->headers.find (ACE_TEXT_ALWAYS_CHAR (HTTP_PRT_HEADER_CONTENT_ENCODING_STRING));
    if (iterator != inherited2::record_->headers.end ())
    {
      session_data_r.format =
        HTTP_Tools::Encoding2CompressionFormat ((*iterator).second);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: set compression format: \"%s\"\n"),
                  inherited::mod_->name (),
                  ACE_TEXT (Stream_Module_Decoder_Tools::compressionFormatToString (session_data_r.format).c_str ())));
    } // end IF

    // make sure the whole fragment chain references the same data record
    // sanity check(s)
    ACE_ASSERT (headFragment_->isInitialized ());
    DATA_CONTAINER_T* data_container_p =
        &const_cast<DATA_CONTAINER_T&> (headFragment_->get ());
    DATA_CONTAINER_T* data_container_2 = NULL;
    message_p = dynamic_cast<DataMessageType*> (headFragment_->cont ());
    while (message_p)
    {
      data_container_p->increase ();
      data_container_2 = data_container_p;
      message_p->initialize (data_container_2,
                             NULL);
      message_p = dynamic_cast<DataMessageType*> (message_p->cont ());
    } // end WHILE
  } // end lock scope

  // *NOTE*: the message has been parsed successfully
  //         --> pass the data (chain) downstream
  {
    //ACE_Guard<ACE_SYNCH_MUTEX> aGuard (lock_);

    //// *NOTE*: new data fragments may have arrived by now
    ////         --> set the next head fragment ?
    //message_2 = dynamic_cast<DataMessageType*> (message_p->cont ());
    //if (message_2)
    //  message_p->cont (NULL);

    result = inherited::put_next (headFragment_, NULL);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_Task_T::put_next(): \"%m\", returning\n"),
                  inherited::mod_->name ()));

      // clean up
      headFragment_->release ();

      goto error;
    } // end IF
    headFragment_ = NULL;
  } // end lock scope

continue_:
error:
  if (release_inbound_message)
  {
    ACE_ASSERT (message_inout);
    message_inout->release ();
    message_inout = NULL;
  } // end IF
  if (release_message)
  {
    ACE_ASSERT (message_p);
    message_p->release ();
  } // end IF
}

template <ACE_SYNCH_DECL,
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
HTTP_Module_ParserH_T<ACE_SYNCH_USE,
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
  NETWORK_TRACE (ACE_TEXT ("HTTP_Module_ParserH_T::handleSessionMessage"));

//  int result = -1;

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  switch (message_inout->type ())
  {
    case STREAM_SESSION_MESSAGE_BEGIN:
    {
      // sanity check(s)
      ACE_ASSERT (inherited::sessionData_);

      const SessionDataType& session_data_r = inherited::sessionData_->get ();

      // retain session ID for reporting
      ACE_ASSERT (inherited::streamState_);
      ACE_ASSERT (inherited::streamState_->sessionData);
      ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *(inherited::streamState_->sessionData->lock));
      inherited::streamState_->sessionData->sessionID =
          session_data_r.sessionID;

      // start profile timer...
      //profile_.start ();

      break;
    }
    //// *NOTE*: the stream has been link()ed, the message contains the (merged)
    ////         upstream session data --> retain a reference
    //case STREAM_SESSION_MESSAGE_LINK:
    //{
    //  // *NOTE*: relax the concurrency policy in this case
    //  // *TODO*: this isn't very reliable
    //  inherited::concurrent_ = true;

    //  break;
    //}
    case STREAM_SESSION_MESSAGE_END:
    {
      // *NOTE*: only process the first 'session end' message (see above: 2566)
      {
        ACE_Guard<ACE_SYNCH_MUTEX> aGuard (inherited::lock_);

        if (inherited::sessionEndProcessed_) break; // done
        inherited::sessionEndProcessed_ = true;
      } // end lock scope

      if (headFragment_)
      {
        headFragment_->release ();
        headFragment_ = NULL;
      } // end IF

      // *NOTE*: in passive 'concurrent' scenarios, there is no 'worker' thread
      //         running svc()
      //         --> do not signal completion in this case
      // *TODO*: remove type inference
      if (inherited::thr_count_ || inherited::runSvcOnStart_)
        this->inherited::TASK_BASE_T::stop (false, // wait ?
                                            true); // locked access ?

      break;
    }
    default:
      break;
  } // end SWITCH
}

//template <ACE_SYNCH_DECL,
//          typename TaskSynchType,
//          typename TimePolicyType,
//          typename SessionMessageType,
//          typename DataMessageType,
//          typename ConfigurationType,
//          typename StreamStateType,
//          typename SessionDataType,
//          typename SessionDataContainerType,
//          typename StatisticContainerType>
//DataMessageType*
//HTTP_Module_ParserH_T<ACE_SYNCH_USE,
//                      TimePolicyType,
//                     SessionMessageType,
//                     DataMessageType,
//                     ConfigurationType,
//                     StreamStateType,
//                     SessionDataType,
//                     SessionDataContainerType,
//                     StatisticContainerType>::allocateMessage (unsigned int requestedSize_in)
//{
//  NETWORK_TRACE (ACE_TEXT ("HTTP_Module_ParserH_T::allocateMessage"));
//
//  // sanity check(s)
//  ACE_ASSERT (inherited::configuration_);
//  ACE_ASSERT (inherited::configuration_->streamConfiguration);
//
//  // initialize return value(s)
//  DataMessageType* message_p = NULL;
//
//  if (inherited::configuration_->streamConfiguration->messageAllocator)
//  {
//allocate:
//    try {
//      message_p =
//        static_cast<DataMessageType*> (inherited::configuration_->streamConfiguration->messageAllocator->malloc (requestedSize_in));
//    } catch (...) {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("caught exception in Stream_IAllocator::malloc(%u), aborting\n"),
//                  requestedSize_in));
//      return NULL;
//    }
//
//    // keep retrying ?
//    if (!message_p && !inherited::configuration_->streamConfiguration->messageAllocator->block ())
//      goto allocate;
//  } // end IF
//  else
//    ACE_NEW_NORETURN (message_p,
//                      DataMessageType (requestedSize_in));
//  if (!message_p)
//  {
//    if (inherited::configuration_->streamConfiguration->messageAllocator)
//    {
//      if (inherited::configuration_->streamConfiguration->messageAllocator->block ())
//        ACE_DEBUG ((LM_CRITICAL,
//                    ACE_TEXT ("failed to allocate SessionMessageType: \"%m\", aborting\n")));
//    } // end IF
//    else
//      ACE_DEBUG ((LM_CRITICAL,
//                  ACE_TEXT ("failed to allocate SessionMessageType: \"%m\", aborting\n")));
//  } // end IF
//
//  return message_p;
//}

template <ACE_SYNCH_DECL,
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
HTTP_Module_ParserH_T<ACE_SYNCH_USE,
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
  NETWORK_TRACE (ACE_TEXT ("HTTP_Module_ParserH_T::collect"));

  // step1: initialize info container POD
  data_out.capturedFrames = 0;
  data_out.droppedFrames = 0;
  data_out.bytes = 0.0;
  data_out.dataMessages = 0;
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

template <ACE_SYNCH_DECL,
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
HTTP_Module_ParserH_T<ACE_SYNCH_USE,
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
                      StatisticContainerType>::record (struct HTTP_Record*& record_inout)
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_Module_ParserH_T::record"));

  // sanity check(s)
  ACE_ASSERT (record_inout);
  ACE_ASSERT (record_inout == inherited2::record_);
  ACE_ASSERT (!headFragment_->isInitialized ());

  // debug info
  if (inherited2::trace_)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("%s"),
                ACE_TEXT (HTTP_Tools::dump (*record_inout).c_str ())));

  DATA_CONTAINER_T* data_container_p = NULL;
  ACE_NEW_NORETURN (data_container_p,
                    DATA_CONTAINER_T ());
  if (!data_container_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate DATA_CONTAINER_T: \"%m\", returning\n")));

    record_inout = NULL;

    return;
  } // end IF
  data_container_p->set (record_inout);
  record_inout = NULL;

  headFragment_->initialize (data_container_p,
                             NULL);

  inherited2::finished_ = true;
}

template <ACE_SYNCH_DECL,
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
HTTP_Module_ParserH_T<ACE_SYNCH_USE,
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
                      StatisticContainerType>::encoding (const std::string& encoding_in)
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_Module_ParserH_T::encoding"));

  ACE_UNUSED_ARG (encoding_in);
}
