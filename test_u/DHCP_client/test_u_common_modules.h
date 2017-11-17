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

#ifndef TEST_U_COMMON_MODULES_H
#define TEST_U_COMMON_MODULES_H

#include "ace/INET_Addr.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"
#include "common_timer_manager_common.h"

#include "stream_common.h"
#include "stream_streammodule_base.h"

#include "stream_misc_dump.h"

#include "stream_stat_statistic_report.h"

#include "stream_net_io.h"

#include "net_connection_manager.h"

#include "dhcp_common.h"
#include "dhcp_configuration.h"
//#include "dhcp_module_bisector.h"
#include "dhcp_module_discover.h"
#include "dhcp_module_parser.h"
#include "dhcp_module_streamer.h"
//#include "dhcp_stream_common.h"

#include "test_u_common.h"
#include "test_u_connection_common.h"

// forward declarations
class Test_U_SessionMessage;
class Test_U_Message;
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 struct Test_U_ConnectionConfiguration,
                                 struct Test_U_ConnectionState,
                                 DHCP_Statistic_t,
                                 struct Test_U_UserData> Test_U_ConnectionManager_t;

// declare module(s)
typedef Stream_Module_Net_IOWriter_T<ACE_MT_SYNCH,
                                     Test_U_ControlMessage_t,
                                     Test_U_Message,
                                     Test_U_SessionMessage,
                                     struct Test_U_StreamModuleHandlerConfiguration,
                                     enum Stream_ControlType,
                                     enum Stream_SessionMessageType,
                                     struct Test_U_StreamState,
                                     struct Test_U_DHCPClient_SessionData,
                                     Test_U_DHCPClient_SessionData_t,
                                     DHCP_Statistic_t,
                                     Common_Timer_Manager_t,
                                     ACE_INET_Addr,
                                     Test_U_ConnectionManager_t,
                                     struct Test_U_UserData> Test_U_Module_Net_Writer_t;
typedef Stream_Module_Net_IOReader_T<ACE_MT_SYNCH,
                                     Test_U_ControlMessage_t,
                                     Test_U_Message,
                                     Test_U_SessionMessage,
                                     struct Test_U_StreamModuleHandlerConfiguration,
                                     enum Stream_ControlType,
                                     enum Stream_SessionMessageType,
                                     struct Test_U_StreamState,
                                     struct Test_U_DHCPClient_SessionData,
                                     Test_U_DHCPClient_SessionData_t,
                                     DHCP_Statistic_t,
                                     Common_Timer_Manager_t,
                                     ACE_INET_Addr,
                                     Test_U_ConnectionManager_t,
                                     struct Test_U_UserData> Test_U_Module_Net_Reader_t;

typedef DHCP_Module_Discover_T<ACE_MT_SYNCH,
                               Common_TimePolicy_t,
                               Test_U_ControlMessage_t,
                               Test_U_Message,
                               Test_U_SessionMessage,
                               struct Test_U_StreamModuleHandlerConfiguration,
                               Test_U_ConnectionConfigurationIterator_t,
                               Test_U_ConnectionManager_t,
                               Test_U_OutboundConnectorBcast_t,
                               Test_U_OutboundConnector_t> Test_U_Module_DHCPDiscover;
typedef DHCP_Module_DiscoverH_T<ACE_MT_SYNCH,
                                Common_TimePolicy_t,
                                Test_U_ControlMessage_t,
                                Test_U_Message,
                                Test_U_SessionMessage,
                                struct Test_U_StreamModuleHandlerConfiguration,
                                enum Stream_ControlType,
                                enum Stream_SessionMessageType,
                                struct Test_U_StreamState,
                                struct Test_U_DHCPClient_SessionData,
                                Test_U_DHCPClient_SessionData_t,
                                DHCP_Statistic_t,
                                DHCP_StatisticHandler_t> Test_U_Module_DHCPDiscoverH;

typedef DHCP_Module_Streamer_T<ACE_MT_SYNCH,
                               Common_TimePolicy_t,
                               struct Test_U_StreamModuleHandlerConfiguration,
                               Test_U_ControlMessage_t,
                               Test_U_Message,
                               Test_U_SessionMessage> Test_U_Module_Streamer;

typedef DHCP_Module_Parser_T<ACE_MT_SYNCH,
                             Common_TimePolicy_t,
                             struct Test_U_StreamModuleHandlerConfiguration,
                             Test_U_ControlMessage_t,
                             Test_U_Message,
                             Test_U_SessionMessage> Test_U_Module_Parser;

typedef Stream_Statistic_StatisticReport_ReaderTask_T<ACE_MT_SYNCH,
                                                   Common_TimePolicy_t,
                                                   struct Test_U_StreamModuleHandlerConfiguration,
                                                   Test_U_ControlMessage_t,
                                                   Test_U_Message,
                                                   Test_U_SessionMessage,
                                                   DHCP_MessageType_t,
                                                   DHCP_Statistic_t,
                                                   Common_Timer_Manager_t,
                                                   struct Test_U_DHCPClient_SessionData,
                                                   Test_U_DHCPClient_SessionData_t> Test_U_Module_StatisticReport_ReaderTask_t;
typedef Stream_Statistic_StatisticReport_WriterTask_T<ACE_MT_SYNCH,
                                                   Common_TimePolicy_t,
                                                   struct Test_U_StreamModuleHandlerConfiguration,
                                                   Test_U_ControlMessage_t,
                                                   Test_U_Message,
                                                   Test_U_SessionMessage,
                                                   DHCP_MessageType_t,
                                                   DHCP_Statistic_t,
                                                   Common_Timer_Manager_t,
                                                   struct Test_U_DHCPClient_SessionData,
                                                   Test_U_DHCPClient_SessionData_t> Test_U_Module_StatisticReport_WriterTask_t;

typedef Stream_Module_Dump_T<ACE_MT_SYNCH,
                             Common_TimePolicy_t,
                             struct Test_U_StreamModuleHandlerConfiguration,
                             Test_U_ControlMessage_t,
                             Test_U_Message,
                             Test_U_SessionMessage,
                             Test_U_DHCPClient_SessionData_t,
                             struct Test_U_UserData> Test_U_Module_Dump;

// declare module(s)
DATASTREAM_MODULE_DUPLEX (struct Test_U_DHCPClient_SessionData,           // session data type
                          enum Stream_SessionMessageType,                 // session event type
                          struct Test_U_StreamModuleHandlerConfiguration, // module handler configuration type
                          libacestream_default_net_io_module_name_string,
                          Test_U_IStreamNotify_t,                         // stream notification interface type
                          Test_U_Module_Net_Reader_t,                     // reader type
                          Test_U_Module_Net_Writer_t,                     // writer type
                          Test_U_Module_Net_IO);                          // name

DATASTREAM_MODULE_INPUT_ONLY (struct Test_U_DHCPClient_SessionData,           // session data type
                              enum Stream_SessionMessageType,                 // session event type
                              struct Test_U_StreamModuleHandlerConfiguration, // module handler configuration type
                              libacenetwork_protocol_default_dhcp_discover_module_name_string,
                              Test_U_IStreamNotify_t,                         // stream notification interface type
                              Test_U_Module_DHCPDiscover);                    // writer type
DATASTREAM_MODULE_INPUT_ONLY (struct Test_U_DHCPClient_SessionData,           // session data type
                              enum Stream_SessionMessageType,                 // session event type
                              struct Test_U_StreamModuleHandlerConfiguration, // module handler configuration type
                              libacenetwork_protocol_default_dhcp_discover_module_name_string,
                              Test_U_IStreamNotify_t,                         // stream notification interface type
                              Test_U_Module_DHCPDiscoverH);                   // writer type
DATASTREAM_MODULE_OUTPUT_ONLY (struct Test_U_DHCPClient_SessionData,           // session data type
                               enum Stream_SessionMessageType,                 // session event type
                               struct Test_U_StreamModuleHandlerConfiguration, // module handler configuration type
                               libacenetwork_protocol_default_dhcp_streamer_module_name_string,
                               Test_U_IStreamNotify_t,                         // stream notification interface type
                               Test_U_Module_Streamer);                        // reader type
DATASTREAM_MODULE_DUPLEX (struct Test_U_DHCPClient_SessionData,           // session data type
                          enum Stream_SessionMessageType,                 // session event type
                          struct Test_U_StreamModuleHandlerConfiguration, // module handler configuration type
                          libacenetwork_protocol_default_dhcp_parser_module_name_string,
                          Test_U_IStreamNotify_t,                         // stream notification interface type
                          Test_U_Module_Streamer,                         // reader type
                          Test_U_Module_Parser,                           // writer type
                          Test_U_Module_Marshal);                         // name

DATASTREAM_MODULE_DUPLEX (struct Test_U_DHCPClient_SessionData,           // session data type
                          enum Stream_SessionMessageType,                 // session event type
                          struct Test_U_StreamModuleHandlerConfiguration, // module handler configuration type
                          libacestream_default_stat_report_module_name_string,
                          Test_U_IStreamNotify_t,                         // stream notification interface type
                          Test_U_Module_StatisticReport_ReaderTask_t,     // reader type
                          Test_U_Module_StatisticReport_WriterTask_t,     // writer type
                          Test_U_Module_StatisticReport);                   // name

DATASTREAM_MODULE_INPUT_ONLY (struct Test_U_DHCPClient_SessionData,           // session data type
                              enum Stream_SessionMessageType,                 // session event type
                              struct Test_U_StreamModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_dump_module_name_string,
                              Test_U_IStreamNotify_t,                         // stream notification interface type
                              Test_U_Module_Dump);                            // writer type

#endif
