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

#ifndef NET_ILISTENER_H
#define NET_ILISTENER_H

#include "common_icontrol.h"
#include "common_iget.h"
#include "common_iinitialize.h"

#include "net_configuration.h"

template <typename ConfigurationType>
class Net_IListener_T
 : public Common_IControl
 , public Common_IGet_T<ConfigurationType>
 , public Common_IInitialize_T<ConfigurationType>
{
 public:
  virtual ~Net_IListener_T () {};

  virtual bool useReactor () const = 0; // ? : uses proactor
};

/////////////////////////////////////////

typedef Net_IListener_T<Net_SocketHandlerConfiguration> Net_IListener_t;
//typedef Net_IListener_T<Net_Client_ListenerConfiguration> Net_IListener_t;

#endif
