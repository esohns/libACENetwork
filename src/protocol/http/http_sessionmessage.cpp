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
#include "stdafx.h"

#include "http_sessionmessage.h"

#include "ace/Log_Msg.h"
#include "ace/Malloc_Base.h"

#include "net_macros.h"

HTTP_SessionMessage::HTTP_SessionMessage (Stream_SessionMessageType messageType_in,
                                          HTTP_Stream_SessionData_t*& sessionData_inout,
                                          HTTP_Stream_UserData* userData_in)
 : inherited (messageType_in,
              sessionData_inout,
              userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_SessionMessage::HTTP_SessionMessage"));

}

HTTP_SessionMessage::HTTP_SessionMessage (const HTTP_SessionMessage& message_in)
 : inherited (message_in)
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_SessionMessage::HTTP_SessionMessage"));

}

HTTP_SessionMessage::HTTP_SessionMessage (ACE_Allocator* messageAllocator_in)
 : inherited (messageAllocator_in)
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_SessionMessage::HTTP_SessionMessage"));

}

HTTP_SessionMessage::HTTP_SessionMessage (ACE_Data_Block* dataBlock_in,
                                          ACE_Allocator* messageAllocator_in)
 : inherited (dataBlock_in,
              messageAllocator_in)
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_SessionMessage::HTTP_SessionMessage"));

}

HTTP_SessionMessage::~HTTP_SessionMessage ()
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_SessionMessage::~HTTP_SessionMessage"));

}

ACE_Message_Block*
HTTP_SessionMessage::duplicate (void) const
{
  NETWORK_TRACE (ACE_TEXT ("HTTP_SessionMessage::duplicate"));

  HTTP_SessionMessage* message_p = NULL;

  // *NOTE*: create a new HTTP_SessionMessage that contains unique copies of
  // the message block fields, but a reference counted duplicate of
  // the ACE_Data_Block

  // if there is no allocator, use the standard new and delete calls.
  if (!inherited::message_block_allocator_)
    ACE_NEW_NORETURN (message_p,
                      HTTP_SessionMessage (*this));
  else
  {
    Stream_IAllocator* allocator_p =
      dynamic_cast<Stream_IAllocator*> (inherited::message_block_allocator_);
    ACE_ASSERT (allocator_p);
allocate:
    try
    {
      // *NOTE*: instruct the allocator to return a session message by passing 0 as
      //         argument to malloc()...
      // *IMPORTANT NOTE*: cached allocators require the object size as argument
      //                   to malloc() (instead of its internal "capacity()" !)
      // *TODO*: (depending on the allocator implementation) this senselessly
      // allocates a datablock anyway, only to immediately release it again...
      ACE_NEW_MALLOC_NORETURN (message_p,
                               static_cast<HTTP_SessionMessage*> (inherited::message_block_allocator_->calloc (0)),
                               HTTP_SessionMessage (*this));
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Stream_IAllocator::calloc(0), aborting\n")));
      return NULL;
    }

    // keep retrying ?
    if (!message_p &&
        !allocator_p->block ())
      goto allocate;
  } // end ELSE
  if (!message_p)
  {
    if (inherited::message_block_allocator_)
    {
      Stream_IAllocator* allocator_p =
        dynamic_cast<Stream_IAllocator*> (inherited::message_block_allocator_);
      ACE_ASSERT (allocator_p);

      if (allocator_p->block ())
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("failed to allocate HTTP_SessionMessage: \"%m\", aborting\n")));
    } // end IF
    else
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate HTTP_SessionMessage: \"%m\", aborting\n")));
    return NULL;
  } // end IF

  // *NOTE*: if "this" is initialized, so is the "clone" (and vice-versa)...

  return message_p;
}
