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
#include "net_transportlayer_ip_cast.h"

#include "net_macros.h"

Net_TransportLayer_IP_Broadcast::Net_TransportLayer_IP_Broadcast ()
 : inherited (NET_TRANSPORTLAYER_IP_CAST)
{
  NETWORK_TRACE (ACE_TEXT ("Net_TransportLayer_IP_Broadcast::Net_TransportLayer_IP_Broadcast"));

}

/////////////////////////////////////////

Net_TransportLayer_IP_Multicast::Net_TransportLayer_IP_Multicast ()
 : inherited (NET_TRANSPORTLAYER_IP_CAST)
// , joined_ (false)
{
  NETWORK_TRACE (ACE_TEXT ("Net_TransportLayer_IP_Multicast::Net_TransportLayer_IP_Multicast"));

}
