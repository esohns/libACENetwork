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

#ifndef HTTP_STREAM_COMMON_H
#define HTTP_STREAM_COMMON_H

#include "ace/Synch_Traits.h"

#include "common_inotify.h"
#include "common_time_common.h"

#include "stream_configuration.h"
#include "stream_imodule.h"
#include "stream_session_data.h"

#include "net_defines.h"

#include "http_common.h"
#include "http_message.h"
//#include "http_stream.h"

// forward declarations
struct HTTP_ConnectionState;
struct HTTP_ModuleHandlerConfiguration;
struct HTTP_StreamConfiguration;

struct HTTP_AllocatorConfiguration
 : Stream_AllocatorConfiguration
{
  inline HTTP_AllocatorConfiguration ()
   : Stream_AllocatorConfiguration ()
  {
    // *NOTE*: this facilitates (message block) data buffers to be scanned with
    //         'flex's yy_scan_buffer() method
    paddingBytes = NET_PROTOCOL_PARSER_FLEX_BUFFER_BOUNDARY_SIZE;
  };
};

typedef HTTP_Message_T<struct HTTP_AllocatorConfiguration,
                       HTTP_MessageData_t> HTTP_Message_t;
//typedef Stream_ControlMessage_T<enum Stream_ControlType,
//                                struct Stream_AllocatorConfiguration,
//                                HTTP_Message_t,
//                                HTTP_SessionMessage> HTTP_ControlMessage_t;

struct HTTP_Stream_SessionData
 : Stream_SessionData
{
  inline HTTP_Stream_SessionData ()
   : Stream_SessionData ()
   , connectionState (NULL)
   //, currentStatistic ()
  {};

  struct HTTP_ConnectionState*   connectionState;

  //HTTP_RuntimeStatistic_t currentStatistic;
};
typedef Stream_SessionData_T<struct HTTP_Stream_SessionData> HTTP_Stream_SessionData_t;

struct HTTP_Stream_UserData
 : Stream_UserData
{
  inline HTTP_Stream_UserData ()
   : Stream_UserData ()
   //, moduleConfiguration (NULL)
   //, moduleHandlerConfiguration (NULL)
  {};

  // *TODO*: remove these ASAP
  //struct Stream_ModuleConfiguration*      moduleConfiguration;
  //struct HTTP_ModuleHandlerConfiguration* moduleHandlerConfiguration;
};

struct HTTP_StreamState
 : Stream_State
{
  inline HTTP_StreamState ()
   : Stream_State ()
   , currentSessionData (NULL)
   , userData (NULL)
  {};

  struct HTTP_Stream_SessionData* currentSessionData;
  struct HTTP_Stream_UserData*    userData;
};

//struct HTTP_ProtocolConfiguration;
struct HTTP_ModuleHandlerConfiguration
 : public Stream_ModuleHandlerConfiguration
{
  inline HTTP_ModuleHandlerConfiguration ()
   : Stream_ModuleHandlerConfiguration ()
   //////////////////////////////////////
   , crunchMessages (HTTP_DEFAULT_CRUNCH_MESSAGES)
   , printProgressDot (false)
   , pushStatisticMessages (true)
   //, protocolConfiguration (NULL)
   , URL ()
  {
    printFinalReport = true;
  };

  bool                               crunchMessages; // http parser module
  bool                               printProgressDot; // file writer module
  bool                               pushStatisticMessages;

  //struct HTTP_ProtocolConfiguration* protocolConfiguration;
  std::string                        URL;
};

//struct HTTP_ProtocolConfiguration;
struct HTTP_StreamConfiguration
 : Stream_Configuration
{
  inline HTTP_StreamConfiguration ()
   : Stream_Configuration ()
   //, protocolConfiguration (NULL)
   , userData (NULL)
  {};

  //struct HTTP_ProtocolConfiguration* protocolConfiguration;       // protocol configuration

  struct HTTP_Stream_UserData*       userData;
};

//typedef Common_INotify_T<unsigned int,
//                         HTTP_Stream_SessionData,
//                         HTTP_Record,
//                         HTTP_SessionMessage> HTTP_IStreamNotify_t;
typedef Stream_INotify_T<enum Stream_SessionMessageType> HTTP_Stream_INotify_t;

#endif
