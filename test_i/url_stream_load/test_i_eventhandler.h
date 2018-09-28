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

#ifndef TEST_I_EVENTHANDLER_H
#define TEST_I_EVENTHANDLER_H

#include <map>

#include "ace/Global_Macros.h"

#include "common_inotify.h"

#include "test_i_stream_common.h"

#include "test_i_message.h"
#include "test_i_session_message.h"

class Test_I_EventHandler
 : public Test_I_ISessionNotify_t
{
 public:
#if defined (GUI_SUPPORT) && defined (GTK_USE)
  Test_I_EventHandler (struct Test_I_URLStreamLoad_GTK_CBData*); // GTK state
#else
  Test_I_EventHandler ();
#endif // GUI_SUPPORT && GTK_USE
  inline virtual ~Test_I_EventHandler () {}

  // implement Stream_ISessionDataNotify_T
  virtual void start (Stream_SessionId_t,                              // session id
                      const struct Test_I_URLStreamLoad_SessionData&); // session data
  virtual void notify (Stream_SessionId_t,
                       const enum Stream_SessionMessageType&);
  virtual void end (Stream_SessionId_t);                // session id
  virtual void notify (Stream_SessionId_t,              // session id
                       const Test_I_Message&);          // (protocol) message
  virtual void notify (Stream_SessionId_t,              // session id
                       const Test_I_SessionMessage&);   // session message

 private:
  ACE_UNIMPLEMENTED_FUNC (Test_I_EventHandler ())
  ACE_UNIMPLEMENTED_FUNC (Test_I_EventHandler (const Test_I_EventHandler&))
  ACE_UNIMPLEMENTED_FUNC (Test_I_EventHandler& operator= (const Test_I_EventHandler&))

  typedef std::map<unsigned int, struct Test_I_URLStreamLoad_SessionData*> SESSION_DATA_MAP_T;
  typedef SESSION_DATA_MAP_T::iterator SESSION_DATA_MAP_ITERATOR_T;

#if defined (GUI_SUPPORT)
  struct Test_I_URLStreamLoad_GTK_CBData* CBData_;
#endif // GUI_SUPPORT
  SESSION_DATA_MAP_T                      sessionDataMap_;
};

#endif
