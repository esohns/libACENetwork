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

#ifndef TEST_U_GTK_COMMON_H
#define TEST_U_GTK_COMMON_H

#include <deque>

#include "gtk/gtk.h"

#include "common_ui_gtk_common.h"

#include "stream_common.h"

enum Test_U_GTK_Event : int
{
  TEST_U_GTKEVENT_INVALID   = -1,
  TEST_U_GTKEVENT_CONNECT   = 0,
  TEST_U_GTKEVENT_DATA,
  TEST_U_GTKEVENT_DISCONNECT,
  TEST_U_GTKEVENT_STATISTIC,
  // -------------------------------------
  TEST_U_GTKEVENT_MAX
};
typedef std::deque<enum Test_U_GTK_Event> Test_U_GTK_Events_t;
typedef Test_U_GTK_Events_t::const_iterator Test_U_GTK_EventsIterator_t;

struct Test_U_GTK_ProgressData
{
  Test_U_GTK_ProgressData ()
   : /*cursorType (GDK_LAST_CURSOR)
   ,*/ GTKState (NULL)
   , size (0)
   , transferred (0)
   , statistic ()
  {};

  //GdkCursorType       cursorType;
  struct Common_UI_GTKState* GTKState;
  size_t                     size; // bytes
  size_t                     transferred; // bytes
  struct Stream_Statistic    statistic;
};

struct Test_U_GTK_CBData
 : Common_UI_GTKState
{
  Test_U_GTK_CBData ()
   : Common_UI_GTKState ()
   , allowUserRuntimeStatistic (true)
   , eventStack ()
   , progressData ()
   , progressEventSourceId (0)
  {};

  bool                           allowUserRuntimeStatistic;
  Test_U_GTK_Events_t            eventStack;
  struct Test_U_GTK_ProgressData progressData;
  guint                          progressEventSourceId;
};

#endif
