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

#ifndef NET_MODULE_EVENTHANDLER_H
#define NET_MODULE_EVENTHANDLER_H

#include "ace/Global_Macros.h"
#include "ace/Synch.h"

#include "common.h"
#include "common_isubscribe.h"
#include "common_iclone.h"

#include "stream_common.h"
#include "stream_task_base_synch.h"
#include "stream_streammodule_base.h"

#include "net_common.h"
#include "net_message.h"
#include "net_sessionmessage.h"
#include "net_module_messagehandler.h"

// forward declaration(s)
class Net_SessionMessage;
class Net_Message;

class Net_Module_EventHandler
 : public Net_Module_MessageHandler_T<Stream_ModuleConfiguration,
                                      Net_SessionMessage,
                                      Net_Message>
{
 public:
  Net_Module_EventHandler ();
  virtual ~Net_Module_EventHandler ();

  // implement Common_IClone_T
  virtual Stream_Module_t* clone ();

 private:
  typedef Net_Module_MessageHandler_T<Stream_ModuleConfiguration,
                                      Net_SessionMessage,
                                      Net_Message> inherited;

  ACE_UNIMPLEMENTED_FUNC (Net_Module_EventHandler (const Net_Module_EventHandler&));
  ACE_UNIMPLEMENTED_FUNC (Net_Module_EventHandler& operator= (const Net_Module_EventHandler&));
};

// declare module
DATASTREAM_MODULE_INPUT_ONLY (ACE_MT_SYNCH,               // task synch type
                              Common_TimePolicy_t,        // time policy
                              Stream_ModuleConfiguration, // configuration type
                              Net_Module_EventHandler);   // writer type

#endif
