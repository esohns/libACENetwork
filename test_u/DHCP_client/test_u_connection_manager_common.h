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

#ifndef TEST_U_CONNECTION_MANAGER_COMMON_H
#define TEST_U_CONNECTION_MANAGER_COMMON_H

#include "ace/INET_Addr.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "net_connection_manager.h"
#include "net_iconnectionmanager.h"

#include "dhcp_common.h"

// forward declarations
struct Test_U_Configuration;
struct Test_U_ConnectionState;
struct Test_U_UserData;

typedef Net_IConnectionManager_T<ACE_INET_Addr,
                                 Test_U_Configuration,
                                 Test_U_ConnectionState,
                                 Test_U_RuntimeStatistic_t,
                                 ////////
                                 Test_U_UserData> Test_U_IConnectionManager_t;
typedef Net_Connection_Manager_T<ACE_INET_Addr,
                                 Test_U_Configuration,
                                 Test_U_ConnectionState,
                                 Test_U_RuntimeStatistic_t,
                                 ////////
                                 Test_U_UserData> Test_U_ConnectionManager_t;
typedef ACE_Singleton<Test_U_ConnectionManager_t,
                      ACE_SYNCH_MUTEX> TEST_U_CONNECTIONMANAGER_SINGLETON;

#endif
