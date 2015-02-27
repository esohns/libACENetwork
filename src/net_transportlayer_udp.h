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

#ifndef Net_TRANSPORTLAYER_UDP_H
#define Net_TRANSPORTLAYER_UDP_H

#include "ace/INET_Addr.h"

#include "net_common.h"
#include "net_exports.h"
#include "net_transportlayer_base.h"

class Net_Export Net_TransportLayer_UDP
 : public Net_InetTransportLayer_Base
 //, public ACE_SOCK_Dgram
{
 public:
  Net_TransportLayer_UDP ();
  virtual ~Net_TransportLayer_UDP ();

 protected:
  ACE_INET_Addr address_;

 private:
  typedef Net_InetTransportLayer_Base inherited;
  //typedef ACE_SOCK_Dgram inherited2;

  ACE_UNIMPLEMENTED_FUNC (Net_TransportLayer_UDP (const Net_TransportLayer_UDP&));
  ACE_UNIMPLEMENTED_FUNC (Net_TransportLayer_UDP& operator= (const Net_TransportLayer_UDP&));
};

#endif
