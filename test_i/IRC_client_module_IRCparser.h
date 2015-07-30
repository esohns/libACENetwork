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

#ifndef IRC_CLIENT_MODULE_IRCPARSER_H
#define IRC_CLIENT_MODULE_IRCPARSER_H

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_common.h"
#include "stream_streammodule_base.h"
#include "stream_task_base_synch.h"

#include "IRC_client_defines.h"
#include "IRC_client_IRCparser_driver.h"
#include "IRC_client_message.h"
#include "IRC_client_sessionmessage.h"
#include "IRC_client_stream_common.h"

// forward declaration(s)
class Stream_IAllocator;

class IRC_Client_Module_IRCParser
 : public Stream_TaskBaseSynch_T<Common_TimePolicy_t,
                                 IRC_Client_SessionMessage,
                                 IRC_Client_Message>
{
 public:
  IRC_Client_Module_IRCParser ();
  virtual ~IRC_Client_Module_IRCParser ();

  // configuration / initialization
  bool initialize (Stream_IAllocator*,                    // message allocator
                   bool = IRC_CLIENT_DEF_CRUNCH_MESSAGES, // crunch messages ?
                   bool = IRC_CLIENT_DEF_LEX_TRACE,       // debug scanner ?
                   bool = IRC_CLIENT_DEF_YACC_TRACE);     // debug parser ?

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (IRC_Client_Message*&, // data message handle
                                  bool&);               // return value: pass message downstream ?

 private:
  typedef Stream_TaskBaseSynch_T<Common_TimePolicy_t,
                                 IRC_Client_SessionMessage,
                                 IRC_Client_Message> inherited;

  ACE_UNIMPLEMENTED_FUNC (IRC_Client_Module_IRCParser (const IRC_Client_Module_IRCParser&))
  ACE_UNIMPLEMENTED_FUNC (IRC_Client_Module_IRCParser& operator= (const IRC_Client_Module_IRCParser&))

  // helper methods
  IRC_Client_Message* allocateMessage (unsigned int); // requested size

  // message allocator
  Stream_IAllocator*         allocator_;

  // driver
  bool                       debugScanner_;
  bool                       debugParser_;
  IRC_Client_IRCParserDriver driver_;

  bool                       crunchMessages_;
  bool                       isInitialized_;
};

// declare module
DATASTREAM_MODULE_INPUT_ONLY (ACE_MT_SYNCH,                 // task synch type
                              Common_TimePolicy_t,          // time policy
                              Stream_ModuleConfiguration,   // module configuration type
                              IRC_Client_Configuration,     // module handler configuration type
                              IRC_Client_Module_IRCParser); // writer type

#endif
