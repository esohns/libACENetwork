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

#ifndef NET_CONNECTION_COMMON_H
#define NET_CONNECTION_COMMON_H

#include "stream_common.h"

#include "net_configuration.h"
#include "net_iconnection.h"
#include "net_itransportlayer.h"

typedef Net_ITransportLayer_T<Net_SocketConfiguration> Net_ITransportLayer_t;
typedef Stream_Statistic Net_RuntimeStatistic_t;

struct Net_ConnectionState
{
  inline Net_ConnectionState ()
   : configuration ()
   , status (NET_CONNECTION_STATUS_INVALID)
   , currentStatistic ()
   , userData (NULL)
  {};

  // *TODO*: consider making this a separate entity (i.e. a pointer)
  Net_Configuration      configuration;

  Net_Connection_Status  status;

  Net_RuntimeStatistic_t currentStatistic;

  Net_StreamUserData*    userData;
};


#endif
