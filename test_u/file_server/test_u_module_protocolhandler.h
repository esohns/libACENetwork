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

#ifndef TEST_U_MODULE_PROTOCOLHANDLER_H
#define TEST_U_MODULE_PROTOCOLHANDLER_H

#include <ace/Global_Macros.h>
#include <ace/Synch_Traits.h>

#include "common_time_common.h"

#include "stream_common.h"
#include "stream_streammodule_base.h"
#include "stream_task_base_synch.h"

#include "test_u_stream_common.h"

// forward declaration(s)
class Stream_IAllocator;
class Test_U_Message;
class Test_U_SessionMessage;

class Test_U_Module_ProtocolHandler
 : public Stream_TaskBaseSynch_T<ACE_MT_SYNCH,
                                 Common_TimePolicy_t,
                                 struct Stream_ModuleHandlerConfiguration,
                                 ACE_Message_Block,
                                 Test_U_Message,
                                 Test_U_SessionMessage,
                                 Stream_SessionId_t,
                                 enum Stream_ControlType,
                                 enum Stream_SessionMessageType,
                                 struct Stream_UserData>
{
 public:
  Test_U_Module_ProtocolHandler ();
  virtual ~Test_U_Module_ProtocolHandler ();

  // override (part of) Stream_IModuleHandler_T
  virtual bool initialize (const struct Stream_ModuleHandlerConfiguration&,
                           Stream_IAllocator*);

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (Test_U_Message*&, // data message handle
                                  bool&);           // return value: pass message downstream ?
  virtual void handleSessionMessage (Test_U_SessionMessage*&, // session message handle
                                     bool&);                  // return value: pass message downstream ?

  // implement Common_IDumpState
  virtual void dump_state () const;

 private:
  typedef Stream_TaskBaseSynch_T<ACE_MT_SYNCH,
                                 Common_TimePolicy_t,
                                 struct Stream_ModuleHandlerConfiguration,
                                 ACE_Message_Block,
                                 Test_U_Message,
                                 Test_U_SessionMessage,
                                 Stream_SessionId_t,
                                 enum Stream_ControlType,
                                 enum Stream_SessionMessageType,
                                 struct Stream_UserData> inherited;

  ACE_UNIMPLEMENTED_FUNC (Test_U_Module_ProtocolHandler (const Test_U_Module_ProtocolHandler&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_Module_ProtocolHandler& operator= (const Test_U_Module_ProtocolHandler&))

  // helper methods
  Test_U_Message* allocateMessage (unsigned int); // requested size

  Stream_IAllocator* allocator_;
  unsigned int       counter_;
  unsigned int       sessionID_;
};

// declare module
DATASTREAM_MODULE_INPUT_ONLY (struct Test_U_StreamSessionData,          // session data type
                              enum Stream_SessionMessageType,           // session event type
                              struct Stream_ModuleHandlerConfiguration, // module handler configuration type
                              Test_U_IStreamNotify_t,                   // stream notification interface type
                              Test_U_Module_ProtocolHandler);           // writer type

#endif