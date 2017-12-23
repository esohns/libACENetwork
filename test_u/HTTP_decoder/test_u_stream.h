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

#ifndef TEST_U_STREAM_H
#define TEST_U_STREAM_H

#include <string>

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_statistic_handler.h"
#include "common_time_common.h"

#include "common_timer_manager_common.h"

#include "stream_base.h"
#include "stream_common.h"
#include "stream_statemachine_control.h"

#include "stream_net_io.h"
#include "stream_net_io_stream.h"

#include "http_common.h"

#include "test_u_common.h"

#include "test_u_connection_manager_common.h"
#include "test_u_HTTP_decoder_common.h"
#include "test_u_HTTP_decoder_stream_common.h"

// forward declarations
class ACE_Message_Block;
class Test_U_Message;
class Test_U_SessionMessage;

extern const char stream_name_string_[];

template <typename TimerManagerType> // implements Common_ITimer
class Test_U_Stream_T
 : public Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        stream_name_string_,
                                        enum Stream_ControlType,
                                        enum Stream_SessionMessageType,
                                        enum Stream_StateMachine_ControlState,
                                        struct Test_U_HTTPDecoder_StreamState,
                                        struct Test_U_StreamConfiguration,
                                        HTTP_Statistic_t,
                                        TimerManagerType,
                                        struct Test_U_AllocatorConfiguration,
                                        struct Stream_ModuleConfiguration,
                                        struct Test_U_ModuleHandlerConfiguration,
                                        struct Test_U_HTTPDecoder_SessionData,
                                        Test_U_HTTPDecoder_SessionData_t,
                                        Test_U_ControlMessage_t,
                                        Test_U_Message,
                                        Test_U_SessionMessage,
                                        ACE_INET_Addr,
                                        Test_U_ConnectionManager_t,
                                        struct Test_U_UserData>
{
  typedef Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        stream_name_string_,
                                        enum Stream_ControlType,
                                        enum Stream_SessionMessageType,
                                        enum Stream_StateMachine_ControlState,
                                        struct Test_U_HTTPDecoder_StreamState,
                                        struct Test_U_StreamConfiguration,
                                        HTTP_Statistic_t,
                                        TimerManagerType,
                                        struct Test_U_AllocatorConfiguration,
                                        struct Stream_ModuleConfiguration,
                                        struct Test_U_ModuleHandlerConfiguration,
                                        struct Test_U_HTTPDecoder_SessionData,
                                        Test_U_HTTPDecoder_SessionData_t,
                                        Test_U_ControlMessage_t,
                                        Test_U_Message,
                                        Test_U_SessionMessage,
                                        ACE_INET_Addr,
                                        Test_U_ConnectionManager_t,
                                        struct Test_U_UserData> inherited;

 public:
  Test_U_Stream_T ();
  virtual ~Test_U_Stream_T ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // *TODO*: on MSVC 2015u3 the accurate declaration does not compile
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  virtual bool initialize (const CONFIGURATION_T&,
#else
  virtual bool initialize (const typename inherited::CONFIGURATION_T&,
#endif
                           ACE_HANDLE);

  // implement Common_IStatistic_T
  // *NOTE*: these delegate to runtimeStatistic_
  virtual bool collect (HTTP_Statistic_t&); // return value: statistic data
  inline virtual void report () const { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) };

 private:
  typedef Stream_Module_Net_IOWriter_T<ACE_MT_SYNCH,
                                       Test_U_ControlMessage_t,
                                       Test_U_Message,
                                       Test_U_SessionMessage,
                                       struct Test_U_ModuleHandlerConfiguration,
                                       enum Stream_ControlType,
                                       enum Stream_SessionMessageType,
                                       struct Test_U_HTTPDecoder_StreamState,
                                       struct Test_U_HTTPDecoder_SessionData,
                                       Test_U_HTTPDecoder_SessionData_t,
                                       HTTP_Statistic_t,
                                       TimerManagerType,
                                       ACE_INET_Addr,
                                       Test_U_ConnectionManager_t,
                                       struct Test_U_UserData> WRITER_T;
  typedef Stream_Module_Net_IOReader_T<ACE_MT_SYNCH,
                                       Test_U_ControlMessage_t,
                                       Test_U_Message,
                                       Test_U_SessionMessage,
                                       struct Test_U_ModuleHandlerConfiguration,
                                       enum Stream_ControlType,
                                       enum Stream_SessionMessageType,
                                       struct Test_U_HTTPDecoder_StreamState,
                                       struct Test_U_HTTPDecoder_SessionData,
                                       Test_U_HTTPDecoder_SessionData_t,
                                       HTTP_Statistic_t,
                                       TimerManagerType,
                                       ACE_INET_Addr,
                                       Test_U_ConnectionManager_t,
                                       struct Test_U_UserData> READER_T;
  typedef Stream_StreamModule_T<ACE_MT_SYNCH,                             // task synch type
                                Common_TimePolicy_t,                      // time policy
                                Stream_SessionId_t,                       // session id type
                                struct Test_U_HTTPDecoder_SessionData,    // session data type
                                enum Stream_SessionMessageType,           // session event type
                                struct Stream_ModuleConfiguration,        // module configuration type
                                struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                                libacestream_default_net_io_module_name_string,
                                Test_U_IStreamNotify_t,                   // stream notification interface type
                                READER_T,                                 // reader type
                                WRITER_T> IO_MODULE_T;                    // writer type

  ACE_UNIMPLEMENTED_FUNC (Test_U_Stream_T (const Test_U_Stream_T&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_Stream_T& operator= (const Test_U_Stream_T&))

  // *TODO*: re-consider this API
  inline void ping () { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
};

// include template definition
#include "test_u_stream.inl"

//////////////////////////////////////////

typedef Test_U_Stream_T<Common_Timer_Manager_t> Test_U_Stream_t;

#endif
