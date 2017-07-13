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

#include "ace/Synch.h"
#include "net_transportlayer_base.h"

/////////////////////////////////////////

#if defined (ACE_HAS_NETLINK)
Net_NetlinkTransportLayer_Base::Net_NetlinkTransportLayer_Base ()
 : dispatch_(COMMON_DISPATCH_INVALID)
 , role_(NET_ROLE_INVALID)
 , transportLayer_ (NET_TRANSPORTLAYER_NETLINK)
{
  NETWORK_TRACE (ACE_TEXT ("Net_NetlinkTransportLayer_Base::Net_NetlinkTransportLayer_Base"));

}

bool
Net_NetlinkTransportLayer_Base::initialize (enum Common_DispatchType dispatch_in,
                                            enum Net_ClientServerRole role_in,
                                            const struct Net_NetlinkSocketConfiguration& configuration_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_NetlinkTransportLayer_Base::initialize"));

  ACE_UNUSED_ARG (configuration_in);

  dispatch_ = dispatch_in;
  role_ = role_in;

  return true;
}
#endif
