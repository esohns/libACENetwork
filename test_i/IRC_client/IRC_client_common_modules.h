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

#ifndef IRC_CLIENT_COMMON_MODULES_H
#define IRC_CLIENT_COMMON_MODULES_H

#include "ace/Synch_Traits.h"

#include "common_time_common.h"
#include "common_timer_manager_common.h"

#include "stream_stat_statistic_report.h"

#include "irc_common.h"
//#include "irc_common_modules.h"
#include "irc_module_bisector.h"
#include "irc_stream_common.h"

#include "IRC_client_configuration.h"

// forward declarations
class IRC_Message;
class IRC_Client_SessionMessage;

typedef IRC_Module_Streamer_T<ACE_MT_SYNCH,
                              Common_TimePolicy_t,
                              struct IRC_Client_ModuleHandlerConfiguration,
                              IRC_Client_ControlMessage_t,
                              IRC_Message,
                              IRC_Client_SessionMessage,
                              struct IRC_Client_UserData> IRC_Client_Module_Streamer_t;
typedef IRC_Module_Bisector_T<ACE_MT_SYNCH,
                              Common_TimePolicy_t,
                              IRC_Client_ControlMessage_t,
                              IRC_Message,
                              IRC_Client_SessionMessage,
                              struct IRC_Client_ModuleHandlerConfiguration,
                              enum Stream_ControlType,
                              enum Stream_SessionMessageType,
                              struct IRC_Client_StreamState,
                              struct IRC_Client_SessionData,
                              IRC_Client_SessionData_t,
                              IRC_Statistic_t,
                              Common_Timer_Manager_t,
                              struct IRC_Client_UserData> IRC_Client_Module_Bisector_t;
//typedef IRC_Module_Bisector_T<ACE_MT_SYNCH,
//                              Common_TimePolicy_t,
//                              IRC_Client_ControlMessage_t,
//                              IRC_Message,
//                              IRC_Client_SessionMessage,
//                              struct IRC_Client_ModuleHandlerConfiguration,
//                              enum Stream_ControlType,
//                              enum Stream_SessionMessageType,
//                              struct IRC_Client_StreamState,
//                              struct IRC_Client_SessionData,
//                              IRC_Client_SessionData_t,
//                              IRC_Statistic_t,
//                              IRC_StatisticHandler_t,
//                              struct IRC_Client_UserData> IRC_Client_Module_AsynchBisector_t;

typedef IRC_Module_Parser_T<ACE_MT_SYNCH,
                            Common_TimePolicy_t,
                            struct IRC_Client_ModuleHandlerConfiguration,
                            IRC_Client_ControlMessage_t,
                            IRC_Message,
                            IRC_Client_SessionMessage> IRC_Client_Module_Parser;

typedef Stream_Statistic_StatisticReport_ReaderTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct IRC_Client_ModuleHandlerConfiguration,
                                                      IRC_Client_ControlMessage_t,
                                                      IRC_Message,
                                                      IRC_Client_SessionMessage,
                                                      IRC_CommandType_t,
                                                      IRC_Statistic_t,
                                                      Common_Timer_Manager_t,
                                                      struct IRC_Client_SessionData,
                                                      IRC_Client_SessionData_t> IRC_Client_Module_Statistic_ReaderTask_t;
typedef Stream_Statistic_StatisticReport_WriterTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct IRC_Client_ModuleHandlerConfiguration,
                                                      IRC_Client_ControlMessage_t,
                                                      IRC_Message,
                                                      IRC_Client_SessionMessage,
                                                      IRC_CommandType_t,
                                                      IRC_Statistic_t,
                                                      Common_Timer_Manager_t,
                                                      struct IRC_Client_SessionData,
                                                      IRC_Client_SessionData_t> IRC_Client_Module_Statistic_WriterTask_t;
//typedef Stream_Statistic_StatisticReport_ReaderTask_T<ACE_MT_SYNCH,
//                                                      Common_TimePolicy_t,
//                                                      struct IRC_Client_ModuleHandlerConfiguration,
//                                                      IRC_Client_ControlMessage_t,
//                                                      IRC_Message,
//                                                      IRC_Client_SessionMessage,
//                                                      IRC_CommandType_t,
//                                                      IRC_Statistic_t,
//                                                      IRC_StatisticHandler_t,
//                                                      struct IRC_Client_SessionData,
//                                                      IRC_Client_SessionData_t> IRC_Client_Module_Statistic_AsynchReaderTask_t;
//typedef Stream_Statistic_StatisticReport_WriterTask_T<ACE_MT_SYNCH,
//                                                      Common_TimePolicy_t,
//                                                      struct IRC_Client_ModuleHandlerConfiguration,
//                                                      IRC_Client_ControlMessage_t,
//                                                      IRC_Message,
//                                                      IRC_Client_SessionMessage,
//                                                      IRC_CommandType_t,
//                                                      IRC_Statistic_t,
//                                                      IRC_StatisticHandler_t,
//                                                      struct IRC_Client_SessionData,
//                                                      IRC_Client_SessionData_t> IRC_Client_Module_Statistic_AsynchWriterTask_t;

DATASTREAM_MODULE_DUPLEX (struct IRC_Client_SessionData,                // session data type
                          enum Stream_SessionMessageType,               // session event type
                          struct IRC_Client_ModuleHandlerConfiguration, // module handler configuration type
                          IRC_Client_IStreamNotify_t,                   // stream notification interface type
                          IRC_Client_Module_Streamer_t,                 // reader type
                          IRC_Client_Module_Bisector_t,                 // writer type
                          IRC_Client_Module_Marshal);                   // name
//DATASTREAM_MODULE_DUPLEX (struct IRC_Client_SessionData,                // session data type
//                          enum Stream_SessionMessageType,               // session event type
//                          struct IRC_Client_ModuleHandlerConfiguration, // module handler configuration type
//                          IRC_Client_IStreamNotify_t,                   // stream notification interface type
//                          IRC_Client_Module_Streamer_t,                 // reader type
//                          IRC_Client_Module_AsynchBisector_t,           // writer type
//                          IRC_Client_Module_AsynchMarshal);             // name
DATASTREAM_MODULE_INPUT_ONLY (struct IRC_Client_SessionData,                // session data type
                              enum Stream_SessionMessageType,               // session event type
                              struct IRC_Client_ModuleHandlerConfiguration, // module handler configuration type
                              IRC_Client_IStreamNotify_t,                   // stream notification interface type
                              IRC_Client_Module_Parser);                    // writer type
DATASTREAM_MODULE_DUPLEX (struct IRC_Client_SessionData,                // session data type
                          enum Stream_SessionMessageType,               // session event type
                          struct IRC_Client_ModuleHandlerConfiguration, // module handler configuration type
                          IRC_Client_IStreamNotify_t,                   // stream notification interface type
                          IRC_Client_Module_Statistic_ReaderTask_t,     // reader type
                          IRC_Client_Module_Statistic_WriterTask_t,     // writer type
                          IRC_Client_Module_StatisticReport);           // name
//DATASTREAM_MODULE_DUPLEX (struct IRC_Client_SessionData,                  // session data type
//                          enum Stream_SessionMessageType,                 // session event type
//                          struct IRC_Client_ModuleHandlerConfiguration,   // module handler configuration type
//                          IRC_Client_IStreamNotify_t,                     // stream notification interface type
//                          IRC_Client_Module_Statistic_AsynchReaderTask_t, // reader type
//                          IRC_Client_Module_Statistic_AsynchWriterTask_t, // writer type
//                          IRC_Client_Module_AsynchStatisticReport);       // name

#endif
