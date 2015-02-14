/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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
#include "stdafx.h"

#include "net_sockethandler_base.h"

//#include "ace/Log_Msg.h"

#include "net_macros.h"

Net_SocketHandlerBase::Net_SocketHandlerBase ()
 : allocator_ (NULL)
// , socketConfiguration_ ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_SocketHandlerBase::Net_SocketHandlerBase"));

  ACE_OS::memset (&socketConfiguration_, 0, sizeof (socketConfiguration_));
}

Net_SocketHandlerBase::~Net_SocketHandlerBase ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_SocketHandlerBase::~Net_SocketHandlerBase"));

}

void
Net_SocketHandlerBase::init (Net_SocketConfiguration_t socketConfiguration_in,
                             ACE_Allocator* allocator_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_SocketHandlerBase::init"));

  allocator_ = allocator_in;
  socketConfiguration_ = socketConfiguration_in;
}
