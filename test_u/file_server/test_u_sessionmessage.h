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

#ifndef TEST_U_SESSIONMESSAGE_H
#define TEST_U_SESSIONMESSAGE_H

#include <ace/Global_Macros.h>

#include "stream_common.h"
#include "stream_messageallocatorheap_base.h"
#include "stream_session_message_base.h"

#include "file_server_common.h"
#include "file_server_stream_common.h"

// forward declarations
class ACE_Allocator;
class ACE_Data_Block;
class ACE_Message_Block;
class Test_U_Message;

class Test_U_SessionMessage
 : public Stream_SessionMessageBase_T<struct Stream_AllocatorConfiguration,
                                      enum Stream_SessionMessageType,
                                      FileServer_SessionData_t,
                                      struct FileServer_UserData>
{
//  // enable access to private ctor(s)
//  friend class Net_StreamMessageAllocator;
  friend class Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                                 struct Stream_AllocatorConfiguration,
                                                 Test_U_ControlMessage_t,
                                                 Test_U_Message,
                                                 Test_U_SessionMessage>;

 public:
  // *NOTE*: assumes responsibility for the second argument !
  Test_U_SessionMessage (enum Stream_SessionMessageType, // session message type
                         FileServer_SessionData_t*&,     // session data handle
                         struct FileServer_UserData*);   // user data handle
  // *NOTE*: to be used by message allocators
  Test_U_SessionMessage (ACE_Allocator*); // message allocator
  Test_U_SessionMessage (ACE_Data_Block*, // data block
                         ACE_Allocator*); // message allocator
  virtual ~Test_U_SessionMessage ();

  // override from ACE_Message_Block
  // *WARNING*: any children need to override this as well
  virtual ACE_Message_Block* duplicate (void) const;

 private:
  typedef Stream_SessionMessageBase_T<struct Stream_AllocatorConfiguration,
                                      enum Stream_SessionMessageType,
                                      FileServer_SessionData_t,
                                      struct FileServer_UserData> inherited;

  ACE_UNIMPLEMENTED_FUNC (Test_U_SessionMessage ())
  // copy ctor (to be used by duplicate())
  Test_U_SessionMessage (const Test_U_SessionMessage&);
  ACE_UNIMPLEMENTED_FUNC (Test_U_SessionMessage& operator= (const Test_U_SessionMessage&))
};

#endif
