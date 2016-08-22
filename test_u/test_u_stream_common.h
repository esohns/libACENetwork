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

//#include "ace/Synch_Traits.h"

//#include "common_time_common.h"

#include "stream_common.h"
//#include "stream_imodule.h"
#include "stream_inotify.h"
#include "stream_session_data.h"

// forward declarations
typedef Stream_Statistic Net_RuntimeStatistic_t;
struct Net_ConnectionState;
struct Test_U_UserData;

struct Test_U_StreamSessionData
 : Stream_SessionData
{
  inline Test_U_StreamSessionData ()
   : Stream_SessionData ()
   , connectionState (NULL)
   , currentStatistic ()
   , userData (NULL)
  {};

  Net_ConnectionState*   connectionState;

  Net_RuntimeStatistic_t currentStatistic;

  Test_U_UserData*       userData;
};
typedef Stream_SessionData_T<Test_U_StreamSessionData> Test_U_StreamSessionData_t;

struct Test_U_StreamState
 : Stream_State
{
  inline Test_U_StreamState ()
   : Stream_State ()
   , currentSessionData (NULL)
   , userData (NULL)
  {};

  Test_U_StreamSessionData* currentSessionData;
  Test_U_UserData*          userData;
};

//typedef Stream_IModuleHandler_T<Test_U_ModuleHandlerConfiguration> Test_U_IModuleHandler_t;
//typedef Stream_IModule_T<ACE_MT_SYNCH,
//                         Common_TimePolicy_t,
//                         Stream_ModuleConfiguration,
//                         Test_U_ModuleHandlerConfiguration> Test_U_IModule_t;

typedef Stream_INotify_T<Stream_SessionMessageType> Test_U_IStreamNotify_t;

#endif
