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

#ifndef TEST_I_STREAM_COMMON_H
#define TEST_I_STREAM_COMMON_H

#include "stream_common.h"
#include "stream_inotify.h"
#include "stream_session_data.h"

#include "test_i_common.h"

// forward declarations
struct Net_ConnectionState;

struct Test_I_StreamSessionData
 : Stream_SessionData
{
  Test_I_StreamSessionData ()
   : Stream_SessionData ()
   , connection (NULL)
   , statistic ()
  {};

  Net_IINETConnection_t* connection;

  Test_I_Statistic_t     statistic;
};
typedef Stream_SessionData_T<struct Test_I_StreamSessionData> Test_I_StreamSessionData_t;

struct Test_I_StreamState
 : Stream_State
{
  Test_I_StreamState ()
   : Stream_State ()
   , sessionData (NULL)
  {};

  struct Test_I_StreamSessionData* sessionData;
};

//typedef Stream_INotify_T<enum Stream_SessionMessageType> Test_I_IStreamNotify_t;

//enum Test_I_SAXParserStateBase
//{
//  SAXPARSER_STATE_INVALID = -1,
//  ////////////////////////////////////////
//  SAXPARSER_STATE_IN_HEAD = 0,
//  SAXPARSER_STATE_IN_HTML,
//  SAXPARSER_STATE_IN_BODY
//  ////////////////////////////////////////
//};
//struct Test_I_SAXParserContext
// : Stream_Module_HTMLParser_SAXParserContextBase
//{
//  inline Test_I_SAXParserContext ()
//   : Stream_Module_HTMLParser_SAXParserContextBase ()
//   , sessionData (NULL)
//   , state (SAXPARSER_STATE_INVALID)
//  {};
//
//  struct Test_I_StreamSessionData* sessionData;
//
//  enum Test_I_SAXParserStateBase   state;
//};

#endif
