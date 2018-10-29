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

#ifndef TEST_U_STREAM_COMMON_H
#define TEST_U_STREAM_COMMON_H

#include "ace/Synch_Traits.h"

#include "stream_common.h"
#include "stream_control_message.h"
//#include "stream_inotify.h"
#include "stream_messageallocatorheap_base.h"
#include "stream_session_data.h"

#include "net_common.h"
#include "net_configuration.h"

#include "test_u_common.h"

 // forward declarations
class Test_U_Message;
class Test_U_SessionMessage;

struct ClientServer_StreamSessionData
 : Stream_SessionData
{
  ClientServer_StreamSessionData ()
   : Stream_SessionData ()
   , connectionState (NULL)
   , statistic ()
   , userData (NULL)
  {};

  struct Net_ConnectionState* connectionState;
  Test_U_Statistic_t          statistic;

  struct Test_U_UserData*     userData;
};
typedef Stream_SessionData_T<struct ClientServer_StreamSessionData> ClientServer_StreamSessionData_t;

struct ClientServer_StreamState
 : Stream_State
{
  ClientServer_StreamState ()
   : Stream_State ()
   , sessionData (NULL)
   , userData (NULL)
  {};

  struct ClientServer_StreamSessionData* sessionData;

  struct Test_U_UserData*                userData;
};

//////////////////////////////////////////

typedef Stream_ControlMessage_T<enum Stream_ControlType,
                                enum Stream_ControlMessageType,
                                struct Net_AllocatorConfiguration> Test_U_ControlMessage_t;

typedef Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                          struct Net_AllocatorConfiguration,
                                          Test_U_ControlMessage_t,
                                          Test_U_Message,
                                          Test_U_SessionMessage> Test_U_MessageAllocator_t;

//typedef Stream_IModuleHandler_T<Test_U_ModuleHandlerConfiguration> Test_U_IModuleHandler_t;
//typedef Stream_IModule_T<ACE_MT_SYNCH,
//                         Common_TimePolicy_t,
//                         struct Stream_ModuleConfiguration,
//                         struct Test_U_ModuleHandlerConfiguration> Test_U_IModule_t;

//typedef Stream_INotify_T<enum Stream_SessionMessageType> Test_U_IStreamNotify_t;

//typedef Stream_ISessionDataNotify_T<Stream_SessionId_t,
//                                    struct Test_U_StreamSessionData,
//                                    enum Stream_SessionMessageType,
//                                    Test_U_Message,
//                                    Test_U_SessionMessage> Test_U_ISessionNotify_t;
//typedef std::list<Test_U_ISessionNotify_t*> Test_U_Subscribers_t;
//typedef Test_U_Subscribers_t::const_iterator Test_U_SubscribersIterator_t;

#endif