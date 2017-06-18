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

#ifndef TEST_I_COMMON_H
#define TEST_I_COMMON_H

#include "stream_common.h"
#include "stream_configuration.h"
#include "stream_inotify.h"

#include "net_common.h"
#include "net_configuration.h"
#include "net_defines.h"

struct Net_Configuration;
struct Test_I_UserData
 : Net_UserData
{
  inline Test_I_UserData ()
   : Net_UserData ()
   , configuration (NULL)
  {};

  struct Net_Configuration* configuration;
};

struct Test_I_ConnectionState
 : Net_ConnectionState
{
  inline Test_I_ConnectionState ()
   : Net_ConnectionState ()
   //, configuration (NULL)
  {};

  // *TODO*: remove ASAP
  //struct Net_Configuration* configuration;
};

struct Test_I_AllocatorConfiguration
 : Stream_AllocatorConfiguration
{
  inline Test_I_AllocatorConfiguration ()
   : Stream_AllocatorConfiguration ()
  {
    // *NOTE*: this facilitates (message block) data buffers to be scanned with
    //         'flex's yy_scan_buffer() method
    paddingBytes = NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE;
  };
};

typedef Stream_Statistic Test_I_RuntimeStatistic_t;
typedef Common_IStatistic_T<Test_I_RuntimeStatistic_t> Test_I_StatisticReportingHandler_t;

struct Test_I_SignalHandlerConfiguration
  : Common_SignalHandlerConfiguration
{
  inline Test_I_SignalHandlerConfiguration ()
    : Common_SignalHandlerConfiguration ()
    , statisticReportingHandler (NULL)
    , statisticReportingTimerID (-1)
  {};

  Test_I_StatisticReportingHandler_t* statisticReportingHandler;
  long                                statisticReportingTimerID;
};

struct Test_I_Configuration
{
  inline Test_I_Configuration ()
   : connectionConfigurations ()
   , parserConfiguration ()
   //, streamConfiguration ()
   , signalHandlerConfiguration ()
   , useReactor (NET_EVENT_USE_REACTOR)
   , userData ()
  {};

  // **************************** socket data **********************************
  Net_ConnectionConfigurations_t           connectionConfigurations;
  // **************************** parser data **********************************
  struct Common_ParserConfiguration        parserConfiguration;
  //// **************************** stream data **********************************
  //struct Test_I_StreamConfiguration        streamConfiguration;
  // **************************** signal data **********************************
  struct Test_I_SignalHandlerConfiguration signalHandlerConfiguration;
  bool                                     useReactor;

  struct Test_I_UserData                   userData;
};

typedef Stream_INotify_T<enum Stream_SessionMessageType> Test_I_IStreamNotify_t;

#endif
