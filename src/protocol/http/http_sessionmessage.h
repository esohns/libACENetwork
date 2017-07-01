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

#ifndef HTTP_SessionMessage_H
#define HTTP_SessionMessage_H

#include "ace/Global_Macros.h"

#include "stream_session_message_base.h"

#include "http_common.h"
#include "http_stream_common.h"

// forward declarations
class ACE_Allocator;
class ACE_Data_Block;
class ACE_Message_Block;
//template <ACE_SYNCH_DECL,
//          typename AllocatorConfigurationType,
//          typename ControlMessageType,
//          typename DataMessageType,
//          typename SessionMessageType> class Stream_MessageAllocatorHeapBase_T;
//template <ACE_SYNCH_DECL,
//          typename AllocatorConfigurationType,
//          typename ControlMessageType,
//          typename DataMessageType,
//          typename SessionMessageType> class Stream_CachedMessageAllocator_T;

template <typename AllocatorType,
          ////////////////////////////////
          typename SessionDataType, // reference-counted
          ////////////////////////////////
          typename UserDataType>
class HTTP_SessionMessage_T
 : public Stream_SessionMessageBase_T<struct Stream_AllocatorConfiguration,
                                      enum Stream_SessionMessageType,
                                      SessionDataType,
                                      UserDataType>
{
  // enable access to specific private ctors
  friend AllocatorType;
  //friend class Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
  //                                               struct Stream_AllocatorConfiguration,
  //                                               ControlMessageType,
  //                                               DataMessageType,
  //                                               HTTP_SessionMessage_T<ControlMessageType,
  //                                                                     DataMessageType> >;
  //friend class Stream_CachedMessageAllocator_T<ACE_MT_SYNCH,
  //                                             struct Stream_AllocatorConfiguration,
  //                                             ControlMessageType,
  //                                             DataMessageType,
  //                                             HTTP_SessionMessage_T<ControlMessageType,
  //                                                                   DataMessageType> >;

 public:
  // convenient types
  typedef HTTP_SessionMessage_T<AllocatorType,
                                SessionDataType,
                                UserDataType> OWN_TYPE_T;

  // *NOTE*: assume lifetime responsibility for the second argument !
  HTTP_SessionMessage_T (enum Stream_SessionMessageType, // session message type
                         SessionDataType*&,              // session data container handle
                         UserDataType*);                 // user data handle
    // *NOTE*: to be used by message allocators
  HTTP_SessionMessage_T (ACE_Allocator*); // message allocator
  HTTP_SessionMessage_T (ACE_Data_Block*, // data block
                         ACE_Allocator*); // message allocator
  virtual ~HTTP_SessionMessage_T ();

  // override from ACE_Message_Block
  // *WARNING*: any children need to override this as well
  virtual ACE_Message_Block* duplicate (void) const;

 private:
  typedef Stream_SessionMessageBase_T<struct Stream_AllocatorConfiguration,
                                      enum Stream_SessionMessageType,
                                      SessionDataType,
                                      UserDataType> inherited;

  ACE_UNIMPLEMENTED_FUNC (HTTP_SessionMessage_T ())
  // copy ctor (to be used by duplicate())
  HTTP_SessionMessage_T (const HTTP_SessionMessage_T&);
  ACE_UNIMPLEMENTED_FUNC (HTTP_SessionMessage_T& operator= (const HTTP_SessionMessage_T&))
};

// include template definition
#include "http_sessionmessage.inl"

#endif
