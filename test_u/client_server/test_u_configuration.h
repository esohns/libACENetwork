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

#ifndef TEST_U_CONFIGURATION_H
#define TEST_U_CONFIGURATION_H

#include "ace/INET_Addr.h"
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#include "ace/Netlink_Addr.h"
#endif
#include "ace/Time_Value.h"

#include "stream_common.h"
#include "stream_iallocator.h"

#include "net_configuration.h"
#include "net_defines.h"
#include "net_iconnectionmanager.h"

#include "net_server_defines.h"

#include "test_u_common.h"

// forward declarations
struct Test_U_Configuration;
struct Test_U_ConnectionState;
typedef Net_IConnectionManager_T<ACE_INET_Addr,
                                 Test_U_Configuration,
                                 Test_U_ConnectionState,
                                 Net_RuntimeStatistic_t,
                                 Test_U_UserData> Test_U_IInetConnectionManager_t;

struct Test_U_ProtocolConfiguration
{
  inline Test_U_ProtocolConfiguration ()
   : PDUSize (NET_STREAM_MESSAGE_DATA_BUFFER_SIZE)
   , pingInterval (ACE_Time_Value::zero)
   , pingAutoAnswer (true)
   , printPongMessages (true)
  {};

  unsigned int   PDUSize; // pdu size (if fixed)
  ACE_Time_Value pingInterval; // {0: off}
  bool           pingAutoAnswer;
  bool           printPongMessages;
};

struct Test_U_ModuleHandlerConfiguration
 : public Stream_ModuleHandlerConfiguration
{
  inline Test_U_ModuleHandlerConfiguration ()
   : Stream_ModuleHandlerConfiguration ()
   , inbound (false)
   , printFinalReport (true)
   , printProgressDot (false)
   , pushStatisticMessages (true)
   , sessionData (NULL)
  {};

  bool                inbound; // statistic/IO module
  bool                printFinalReport; // statistic module
  bool                printProgressDot; // file writer module
  bool                pushStatisticMessages; // statistic module

  // *TODO*: remove this (--> session message data)
  Stream_SessionData* sessionData;
};

struct Test_U_StreamConfiguration
 : Stream_Configuration
{
  inline Test_U_StreamConfiguration ()
   : Stream_Configuration ()
   , moduleConfiguration_2 ()
   , moduleHandlerConfiguration_2 ()
   , protocolConfiguration (NULL)
   , userData (NULL)
  {
    bufferSize = NET_STREAM_MESSAGE_DATA_BUFFER_SIZE;
  };

  Stream_ModuleConfiguration        moduleConfiguration_2;        // module configuration
  Test_U_ModuleHandlerConfiguration moduleHandlerConfiguration_2; // module handler configuration
  Test_U_ProtocolConfiguration*     protocolConfiguration;        // protocol configuration

  Test_U_UserData*                  userData;                     // user data
};

struct Test_U_Configuration
{
  inline Test_U_Configuration ()
   : socketConfiguration ()
   , socketHandlerConfiguration ()
   , streamConfiguration ()
   , protocolConfiguration ()
   , userData ()
  {};

  // **************************** socket data **********************************
  Net_SocketConfiguration           socketConfiguration;
  Test_U_SocketHandlerConfiguration socketHandlerConfiguration;
  // **************************** stream data **********************************
  Test_U_StreamConfiguration        streamConfiguration;
  // *************************** protocol data *********************************
  Test_U_ProtocolConfiguration      protocolConfiguration;

  Test_U_UserData                   userData;
};

#endif
