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

#ifndef NET_SERVER_COMMON_H
#define NET_SERVER_COMMON_H

#include "common_istatistic.h"

#include "stream_common.h"
#include "stream_iallocator.h"

#include "net_configuration.h"
#include "net_connection_manager_common.h"

#include "net_server_ilistener.h"

struct Net_Server_ListenerConfiguration
{
  inline Net_Server_ListenerConfiguration ()
   : addressFamily (ACE_ADDRESS_FAMILY_INET)
   , connectionManager (NULL)
   , messageAllocator (NULL)
   , portNumber (0)
   , socketHandlerConfiguration (NULL)
   //, statisticCollectionInterval (0)
   , useLoopbackDevice (false)
  {};

  int                             addressFamily;
  Net_IInetConnectionManager_t*   connectionManager;
  Stream_IAllocator*              messageAllocator;
  unsigned short                  portNumber;
  Net_SocketHandlerConfiguration* socketHandlerConfiguration;
  //unsigned int                    statisticCollectionInterval; // statistics collecting interval (second(s))
  //                                                             // 0 --> DON'T collect statistics
  bool                            useLoopbackDevice;
};

typedef Net_Server_IListener_T<Net_Server_ListenerConfiguration> Net_Server_IListener_t;

typedef Common_IStatistic_T<Stream_Statistic> Net_Server_StatisticReportingHandler_t;

struct Net_Server_SignalHandlerConfiguration
{
  Net_Server_IListener_t*                 listener;
  Net_Server_StatisticReportingHandler_t* statisticReportingHandler;
  long                                    statisticReportingTimerID;
};

#endif
