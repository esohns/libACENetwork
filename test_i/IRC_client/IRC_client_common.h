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

#ifndef IRC_CLIENT_COMMON_H
#define IRC_CLIENT_COMMON_H

#include <bitset>
#include <deque>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "ace/Date_Time.h"
#include "ace/INET_Addr.h"
#include "ace/Synch_Traits.h"
#include "ace/Time_Value.h"

#include "common_inotify.h"

#include "irc_icontrol.h"

#include "stream_common.h"

#include "net_defines.h"
#include "net_iconnection.h"

#include "irc_stream_common.h"

#include "FILE_Stream.h"
#include "IOStream_alt_T.h"

// forward declarations
struct IRC_Client_Configuration;
struct IRC_Client_CursesState;
struct IRC_Client_ModuleHandlerConfiguration;
struct IRC_Client_SessionData;
class IRC_Client_SessionMessage;
struct IRC_Client_SessionState;
class IRC_Record;

typedef Common_INotify_T<unsigned int,
                         IRC_Client_SessionData,
                         IRC_Message,
                         IRC_Client_SessionMessage> IRC_Client_IStreamNotify_t;
//typedef IRC_IControl_T<IRC_Client_IStreamNotify_t> IRC_Client_IControl_t;

// phonebook
typedef std::set<std::string> IRC_Client_Networks_t;
typedef IRC_Client_Networks_t::const_iterator IRC_Client_NetworksIterator_t;
typedef std::pair<unsigned short, unsigned short> IRC_Client_PortRange_t;
typedef std::vector<IRC_Client_PortRange_t> IRC_Client_PortRanges_t;
typedef IRC_Client_PortRanges_t::const_iterator IRC_Client_PortRangesIterator_t;
struct IRC_Client_ConnectionEntry
{
  inline IRC_Client_ConnectionEntry ()
   : hostName ()
   , ports ()
   , netWork ()
  {};

  std::string             hostName;
  IRC_Client_PortRanges_t ports;
  std::string             netWork;
};
typedef std::vector<IRC_Client_ConnectionEntry> IRC_Client_Connections_t;
typedef IRC_Client_Connections_t::const_iterator IRC_Client_ConnectionsIterator_t;
typedef std::multimap<std::string,
                      IRC_Client_ConnectionEntry> IRC_Client_Servers_t;
typedef IRC_Client_Servers_t::const_iterator IRC_Client_ServersIterator_t;
struct IRC_Client_PhoneBook
{
  inline IRC_Client_PhoneBook ()
   : networks ()
   , servers ()
   , timeStamp ()
  {};

  // *NOTE*: this member is redundant (but still useful ?)...
  IRC_Client_Networks_t networks;
  IRC_Client_Servers_t  servers;
  ACE_Date_Time         timeStamp;
};

typedef std::deque<std::string> IRC_Client_MessageQueue_t;
typedef IRC_Client_MessageQueue_t::iterator IRC_Client_MessageQueueIterator_t;
typedef IRC_Client_MessageQueue_t::reverse_iterator IRC_Client_MessageQueueReverseIterator_t;

//  ACE_IOStream<ACE_FILE_Stream> output_;
typedef ACE_IOStream_alt_T<ACE_FILE_Stream> IRC_Client_IOStream_t;

struct IRC_Client_InputThreadData
{
  inline IRC_Client_InputThreadData ()
   : configuration (NULL)
   , cursesState (NULL)
   , groupID (-1)
   , moduleHandlerConfiguration (NULL)
   , useReactor (NET_EVENT_USE_REACTOR)
  {};

  IRC_Client_Configuration*              configuration;
  IRC_Client_CursesState*                cursesState;
  int                                    groupID;
  IRC_Client_ModuleHandlerConfiguration* moduleHandlerConfiguration;
  bool                                   useReactor;
};

struct IRC_Client_UserData
{
  inline IRC_Client_UserData ()
   : configuration (NULL)
   , moduleConfiguration (NULL)
   , moduleHandlerConfiguration (NULL)
  {};

  IRC_Client_Configuration*              configuration;

  // *TODO*: remove these ASAP
  Stream_ModuleConfiguration*            moduleConfiguration;
  IRC_Client_ModuleHandlerConfiguration* moduleHandlerConfiguration;
};

struct IRC_Client_SessionData
 : IRC_Stream_SessionData
{
  inline IRC_Client_SessionData ()
   : IRC_Stream_SessionData ()
   , connectionState (NULL)
   , userData (NULL)
  {};

  IRC_Client_SessionState* connectionState;

  IRC_Client_UserData*     userData;
};

typedef Stream_Statistic IRC_RuntimeStatistic_t;

struct IRC_Client_ConnectionState
 : IRC_ConnectionState
{
  inline IRC_Client_ConnectionState ()
   : IRC_ConnectionState ()
   , configuration (NULL)
   , controller (NULL)
   , userData (NULL)
  {};

  IRC_Client_Configuration* configuration;
  IRC_IControl*             controller;

  IRC_Client_UserData*      userData;
};

// *TODO*: remove this ASAP
struct IRC_Client_SessionState
 : IRC_Client_ConnectionState
{
  inline IRC_Client_SessionState ()
   : IRC_Client_ConnectionState ()
   , away (false)
   , channel ()
   , channelModes ()
   , isFirstMessage (false)
   , nickName ()
   , userModes ()
  {};

  // *TODO*: remove this
  bool               away;
  std::string        channel;
  IRC_ChannelModes_t channelModes;
  bool               isFirstMessage;
  std::string        nickName;
  IRC_UserModes_t    userModes;
};

#endif
