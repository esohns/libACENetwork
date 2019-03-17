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

#ifndef TEST_U_CONNECTION_STREAM_H
#define TEST_U_CONNECTION_STREAM_H

#include <string>

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_common.h"

#include "stream_net_io_stream.h"

#include "test_u_common.h"
#include "test_u_connection_manager_common.h"

#include "test_u_dhcp_client_common.h"
#include "test_u_message.h"

// forward declarations
class Stream_IAllocator;
class Test_U_SessionMessage;

extern const char stream_name_string_[];

class Test_U_InboundConnectionStream
 : public Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        stream_name_string_,
                                        enum Stream_ControlType,
                                        enum Stream_SessionMessageType,
                                        enum Stream_StateMachine_ControlState,
                                        struct DHCPClient_StreamState,
                                        struct DHCPClient_StreamConfiguration,
                                        DHCP_Statistic_t,
                                        Common_Timer_Manager_t,
                                        struct Common_FlexParserAllocatorConfiguration,
                                        struct Stream_ModuleConfiguration,
                                        struct DHCPClient_ModuleHandlerConfiguration,
                                        struct DHCPClient_SessionData, // session data
                                        DHCPClient_SessionData_t,      // session data container (reference counted)
                                        DHCPClient_ControlMessage_t,
                                        Test_U_Message,
                                        Test_U_SessionMessage,
                                        ACE_INET_Addr,
                                        DHCPClient_ConnectionManager_t,
                                        struct Net_UserData>
{
  typedef Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        stream_name_string_,
                                        enum Stream_ControlType,
                                        enum Stream_SessionMessageType,
                                        enum Stream_StateMachine_ControlState,
                                        struct DHCPClient_StreamState,
                                        struct DHCPClient_StreamConfiguration,
                                        DHCP_Statistic_t,
                                        Common_Timer_Manager_t,
                                        struct Common_FlexParserAllocatorConfiguration,
                                        struct Stream_ModuleConfiguration,
                                        struct DHCPClient_ModuleHandlerConfiguration,
                                        struct DHCPClient_SessionData,
                                        DHCPClient_SessionData_t,
                                        DHCPClient_ControlMessage_t,
                                        Test_U_Message,
                                        Test_U_SessionMessage,
                                        ACE_INET_Addr,
                                        DHCPClient_ConnectionManager_t,
                                        struct Net_UserData> inherited;

 public:
  Test_U_InboundConnectionStream ();
  virtual ~Test_U_InboundConnectionStream ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const inherited::CONFIGURATION_T&,
                           ACE_HANDLE);                       // socket handle

 private:
  ACE_UNIMPLEMENTED_FUNC (Test_U_InboundConnectionStream (const Test_U_InboundConnectionStream&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_InboundConnectionStream& operator= (const Test_U_InboundConnectionStream&))

  // *TODO*: re-consider this API
  inline void ping () { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
};

////////////////////////////////////////////////////////////////////////////////

class Test_U_OutboundConnectionStream
 : public Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        stream_name_string_,
                                        enum Stream_ControlType,
                                        enum Stream_SessionMessageType,
                                        enum Stream_StateMachine_ControlState,
                                        struct DHCPClient_StreamState,
                                        struct DHCPClient_StreamConfiguration,
                                        DHCP_Statistic_t,
                                        Common_Timer_Manager_t,
                                        struct Common_FlexParserAllocatorConfiguration,
                                        struct Stream_ModuleConfiguration,
                                        struct DHCPClient_ModuleHandlerConfiguration,
                                        struct DHCPClient_SessionData, // session data
                                        DHCPClient_SessionData_t,      // session data container (reference counted)
                                        DHCPClient_ControlMessage_t,
                                        Test_U_Message,
                                        Test_U_SessionMessage,
                                        ACE_INET_Addr,
                                        DHCPClient_ConnectionManager_t,
                                        struct Net_UserData>
{
  typedef Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        stream_name_string_,
                                        enum Stream_ControlType,
                                        enum Stream_SessionMessageType,
                                        enum Stream_StateMachine_ControlState,
                                        struct DHCPClient_StreamState,
                                        struct DHCPClient_StreamConfiguration,
                                        DHCP_Statistic_t,
                                        Common_Timer_Manager_t,
                                        struct Common_FlexParserAllocatorConfiguration,
                                        struct Stream_ModuleConfiguration,
                                        struct DHCPClient_ModuleHandlerConfiguration,
                                        struct DHCPClient_SessionData,
                                        DHCPClient_SessionData_t,
                                        DHCPClient_ControlMessage_t,
                                        Test_U_Message,
                                        Test_U_SessionMessage,
                                        ACE_INET_Addr,
                                        DHCPClient_ConnectionManager_t,
                                        struct Net_UserData> inherited;

 public:
  Test_U_OutboundConnectionStream ();
  virtual ~Test_U_OutboundConnectionStream ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const inherited::CONFIGURATION_T&,
                           ACE_HANDLE);                       // socket handle

 private:
  ACE_UNIMPLEMENTED_FUNC (Test_U_OutboundConnectionStream (const Test_U_OutboundConnectionStream&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_OutboundConnectionStream& operator= (const Test_U_OutboundConnectionStream&))

  // *TODO*: re-consider this API
  inline void ping () { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
};

#endif
