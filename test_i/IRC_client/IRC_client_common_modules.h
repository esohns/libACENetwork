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

#ifndef IRC_CLIENT_COMMON_MODULES_H
#define IRC_CLIENT_COMMON_MODULES_H

#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "irc_common_modules.h"
#include "irc_module_bisector.h"
#include "irc_stream_common.h"

#include "IRC_client_configuration.h"

// forward declarations
class IRC_Message;
class IRC_Client_SessionMessage;

typedef IRC_Module_Parser_Module IRC_Client_Module_Parser_Module;

typedef IRC_Module_RuntimeStatistic_Module IRC_Client_Module_RuntimeStatistic_Module;

typedef IRC_Module_Bisector_T<ACE_SYNCH_MUTEX,
                              ///////////
                              ACE_MT_SYNCH,
                              Common_TimePolicy_t,
                              IRC_Client_SessionMessage,
                              IRC_Message,
                              ///////////
                              IRC_Client_ModuleHandlerConfiguration,
                              ///////////
                              IRC_StreamState,
                              ///////////
                              IRC_Client_SessionData,
                              IRC_Client_SessionData_t,
                              ///////////
                              IRC_RuntimeStatistic_t> IRC_Client_Module_Bisector_t;

DATASTREAM_MODULE_DUPLEX (ACE_MT_SYNCH,                          // task synch type
                          Common_TimePolicy_t,                   // time policy
                          Stream_ModuleConfiguration,            // module configuration type
                          IRC_Client_ModuleHandlerConfiguration, // module handler configuration type
                          IRC_Module_Streamer,                   // reader type
                          IRC_Client_Module_Bisector_t,          // writer type
                          IRC_Client_Module_Marshal);            // name

#endif
