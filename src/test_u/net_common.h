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

#ifndef TEST_U_NET_COMMON_H
#define TEST_U_NET_COMMON_H

#include <deque>
#include <vector>

#include "ace/Synch.h"

#include "common.h"

#include "common_ui_common.h"

#include "net_configuration.h"
#include "net_stream_common.h"

#include "net_client_common.h"
#include "net_client_timeouthandler.h"

#include "net_server_common.h"

extern unsigned int random_seed;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
extern struct random_data random_data;
extern char random_state_buffer[];
#endif

enum Net_GTK_Event_t
{
  NET_GTKEVENT_CONNECT = 0,
  NET_GTKEVENT_DATA,
  NET_GTKEVENT_DISCONNECT,
  NET_GTKEVENT_STATISTICS,
  // ----------------------
  NET_GTKEVENT_MAX,
  NET_GKTEVENT_INVALID
};
typedef std::deque<Net_GTK_Event_t> Net_GTK_Events_t;
typedef Net_GTK_Events_t::const_iterator Net_GTK_EventsIterator_t;

struct Net_GTK_CBData_t
{
  inline Net_GTK_CBData_t ()
   : allowUserRuntimeStatistic (true)
   , eventStack ()
   , GTKState ()
   , listenerHandle (NULL)
   , logStack ()
   , signalHandlerConfiguration (NULL)
   , stackLock ()
   , subscribers ()
   , subscribersLock ()
   , timeoutHandler (NULL)
 { };

  bool                                     allowUserRuntimeStatistic;
  Net_GTK_Events_t                         eventStack;
  Common_UI_GTKState                       GTKState;
  Net_Server_IListener_t*                  listenerHandle;             // *NOTE*: server only !
  Common_MessageStack_t                    logStack;
  Net_Client_SignalHandlerConfiguration_t* signalHandlerConfiguration; // *NOTE*: client only !
  ACE_Recursive_Thread_Mutex               stackLock;
  Net_Subscribers_t                        subscribers;
  ACE_Recursive_Thread_Mutex               subscribersLock;
  Net_Client_TimeoutHandler*               timeoutHandler;             // *NOTE*: client only !
};

#endif
