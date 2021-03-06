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

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"
#include "ace/Synch_Traits.h"

#include "common_parser_bencoding_tools.h"

#include "common_ui_common.h"

#include "net_macros.h"

#include "bittorrent_message.h"
#include "bittorrent_bencoding_parser_driver.h"
#include "bittorrent_sessionmessage.h"

template <typename SessionDataType,
          typename UserDataType,
          typename SessionInterfaceType
#if defined (GUI_SUPPORT)
          ,typename CBDataType>
#else
          >
#endif // GUI_SUPPORT
BitTorrent_Client_PeerStreamHandler_T<SessionDataType,
                                      UserDataType,
                                      SessionInterfaceType
#if defined (GUI_SUPPORT)
                                      ,CBDataType>::BitTorrent_Client_PeerStreamHandler_T (SessionInterfaceType* interfaceHandle_in,
                                                                                           CBDataType* CBData_in)
#else
                                      >::BitTorrent_Client_PeerStreamHandler_T (SessionInterfaceType* interfaceHandle_in)
#endif // GUI_SUPPORT
#if defined (GUI_SUPPORT)
 : CBData_ (CBData_in)
 , session_ (interfaceHandle_in)
#else
 : session_ (interfaceHandle_in)
#endif // GUI_SUPPORT
 , sessionData_ ()
{
  NETWORK_TRACE (ACE_TEXT ("BitTorrent_Client_PeerStreamHandler_T::BitTorrent_Client_PeerStreamHandler_T"));

  // sanity check(s)
  ACE_ASSERT (session_);
}

template <typename SessionDataType,
          typename UserDataType,
          typename SessionInterfaceType
#if defined (GUI_SUPPORT)
          ,typename CBDataType>
#else
          >
#endif // GUI_SUPPORT
void
BitTorrent_Client_PeerStreamHandler_T<SessionDataType,
                                      UserDataType,
                                      SessionInterfaceType
#if defined (GUI_SUPPORT)
                                      ,CBDataType>::start (Stream_SessionId_t sessionId_in,
#else
                                      >::start (Stream_SessionId_t sessionId_in,
#endif // GUI_SUPPORT
                                                           const SessionDataType& sessionData_in)
{
  NETWORK_TRACE (ACE_TEXT ("BitTorrent_Client_PeerStreamHandler_T::start"));

  SESSION_DATA_ITERATOR_T iterator = sessionData_.find (sessionId_in);

  // sanity check(s)
  ACE_ASSERT (session_);
  // *NOTE*: on Linux systems, file descriptors are reused immediately, so
  //         collisions can occur here (i.e. the original connection may not
  //         have finalized at this stage
  if (iterator != sessionData_.end ())
    sessionData_.erase (iterator);

  sessionData_.insert (std::make_pair (sessionId_in,
                                       &const_cast<SessionDataType&> (sessionData_in)));

  try {
    session_->connect (sessionId_in);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Net_ISession_T::connect(), continuing\n")));
  }

  if (CBData_)
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->lock);
    //  CBData_->progressData.transferred = 0;
    CBData_->eventStack.push (COMMON_UI_EVENT_STARTED);

//    guint event_source_id = g_idle_add (idle_start_UI_cb,
//                                        CBData_);
//    if (event_source_id == 0)
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to g_idle_add(idle_send_UI_cb): \"%m\", returning\n")));
//      return;
//    } // end IF
//    CBData_->eventSourceIds.insert (event_source_id);
  } // end lock scope
}

template <typename SessionDataType,
          typename UserDataType,
          typename SessionInterfaceType
#if defined (GUI_SUPPORT)
          ,typename CBDataType>
#else
          >
#endif // GUI_SUPPORT
void
BitTorrent_Client_PeerStreamHandler_T<SessionDataType,
                                      UserDataType,
                                      SessionInterfaceType
#if defined (GUI_SUPPORT)
                                      ,CBDataType>::notify (Stream_SessionId_t sessionId_in,
#else
                                      >::notify (Stream_SessionId_t sessionId_in,
#endif // GUI_SUPPORT
                                                            const enum Stream_SessionMessageType& sessionEvent_in)
{
  STREAM_TRACE (ACE_TEXT ("BitTorrent_Client_PeerStreamHandler_T::notify"));

  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (sessionEvent_in);

  ACE_ASSERT (false);
  ACE_NOTSUP;

  ACE_NOTREACHED (return;)
}

template <typename SessionDataType,
          typename UserDataType,
          typename SessionInterfaceType
#if defined (GUI_SUPPORT)
          ,typename CBDataType>
#else
          >
#endif // GUI_SUPPORT
void
BitTorrent_Client_PeerStreamHandler_T<SessionDataType,
                                      UserDataType,
                                      SessionInterfaceType
#if defined (GUI_SUPPORT)
                                      ,CBDataType>::end (Stream_SessionId_t sessionId_in)
#else
                                      >::end (Stream_SessionId_t sessionId_in)
#endif // GUI_SUPPORT
{
  NETWORK_TRACE (ACE_TEXT ("BitTorrent_Client_PeerStreamHandler_T::end"));

  SESSION_DATA_ITERATOR_T iterator = sessionData_.find (sessionId_in);

  // sanity check(s)
  ACE_ASSERT (session_);
  if (iterator != sessionData_.end ())
    sessionData_.erase (iterator);

  try {
    session_->disconnect (sessionId_in);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Net_ISession_T::disconnect(), continuing\n")));
  }

  if (CBData_)
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->lock);
    CBData_->eventStack.push (COMMON_UI_EVENT_FINISHED);

//    guint event_source_id = g_idle_add (idle_end_UI_cb,
//                                        CBData_);
//    if (event_source_id == 0)
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to g_idle_add(idle_end_UI_cb): \"%m\", returning\n")));
//      return;
//    } // end IF
//    CBData_->eventSourceIds.insert (event_source_id);
  } // end lock scope
}

template <typename SessionDataType,
          typename UserDataType,
          typename SessionInterfaceType
#if defined (GUI_SUPPORT)
          ,typename CBDataType>
#else
          >
#endif // GUI_SUPPORT
void
BitTorrent_Client_PeerStreamHandler_T<SessionDataType,
                                      UserDataType,
                                      SessionInterfaceType
#if defined (GUI_SUPPORT)
                                      ,CBDataType>::notify (Stream_SessionId_t sessionId_in,
#else
                                      >::notify (Stream_SessionId_t sessionId_in,
#endif // GUI_SUPPORT
                                                            const BitTorrent_Message_T<Stream_SessionData_T<SessionDataType>,
                                                                                       UserDataType>& message_in)
{
  NETWORK_TRACE (ACE_TEXT ("BitTorrent_Client_PeerStreamHandler_T::notify"));

  ACE_UNUSED_ARG (sessionId_in);

  // sanity check(s)
  ACE_ASSERT (session_);

  const typename BitTorrent_Message_T<Stream_SessionData_T<SessionDataType>,
                                      UserDataType>::DATA_T& data_container_r =
      message_in.getR ();
  const struct BitTorrent_PeerRecord& record_r = data_container_r.getR ();
  try {
    session_->notify (record_r,
                      (record_r.type == BITTORRENT_MESSAGETYPE_PIECE) ? &const_cast<BitTorrent_Message_T<Stream_SessionData_T<SessionDataType>,
                                                                                                         UserDataType>&> (message_in)
                                                                      : NULL);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Net_ISession_T::notify(), continuing\n")));
  }

  if (CBData_)
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->lock);
    CBData_->progressData.transferred += message_in.total_length ();
    CBData_->eventStack.push (COMMON_UI_EVENT_DATA);
  } // end lock scope
}
template <typename SessionDataType,
          typename UserDataType,
          typename SessionInterfaceType
#if defined (GUI_SUPPORT)
          ,typename CBDataType>
#else
          >
#endif // GUI_SUPPORT
void
BitTorrent_Client_PeerStreamHandler_T<SessionDataType,
                                      UserDataType,
                                      SessionInterfaceType
#if defined (GUI_SUPPORT)
                                      ,CBDataType>::notify (Stream_SessionId_t sessionId_in,
#else
                                      >::notify (Stream_SessionId_t sessionId_in,
#endif // GUI_SUPPORT
                                                            const BitTorrent_SessionMessage_T<SessionDataType,
                                                                                              UserDataType>& sessionMessage_in)
{
  NETWORK_TRACE (ACE_TEXT ("BitTorrent_Client_PeerStreamHandler_T::notify"));

  SESSION_DATA_ITERATOR_T iterator = sessionData_.find (sessionId_in);
  int result = -1;

  // sanity check(s)
  // *NOTE*: the current implementation isn't fully synchronized in the sense
  //         that messages may arrive 'out-of-session' (e.g.
  //         STREAM_SESSION_MESSAGE_DISCONNECT after STREAM_SESSION_MESSAGE_END)
  if (iterator == sessionData_.end ())
    return;

  if (CBData_)
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->lock);
    enum Common_UI_EventType event_e = COMMON_UI_EVENT_INVALID;
    switch (sessionMessage_in.type ())
    {
      case STREAM_SESSION_MESSAGE_STATISTIC:
      {
        if ((*iterator).second->lock)
        {
          result = (*iterator).second->lock->acquire ();
          if (result == -1)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", continuing\n")));
        } // end IF

        CBData_->progressData.statistic = (*iterator).second->statistic;

        if ((*iterator).second->lock)
        {
          result = (*iterator).second->lock->release ();
          if (result == -1)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n")));
        } // end IF

        event_e = COMMON_UI_EVENT_STATISTIC;
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown session message type (was: %d), returning\n"),
                    sessionMessage_in.type ()));
        return;
      }
    } // end SWITCH
    CBData_->eventStack.push (event_e);
  } // end IF
}

//////////////////////////////////////////

template <typename SessionDataType,
          typename UserDataType,
          typename SessionInterfaceType
#if defined (GUI_SUPPORT)
          ,typename CBDataType>
#else
          >
#endif // GUI_SUPPORT
BitTorrent_Client_TrackerStreamHandler_T<SessionDataType,
                                         UserDataType,
                                         SessionInterfaceType
#if defined (GUI_SUPPORT)
                                         ,CBDataType>::BitTorrent_Client_TrackerStreamHandler_T (SessionInterfaceType* interfaceHandle_in,
                                                                                                 CBDataType* CBData_in)
#else
                                         >::BitTorrent_Client_TrackerStreamHandler_T (SessionInterfaceType* interfaceHandle_in)
#endif // GUI_SUPPORT
#if defined (GUI_SUPPORT)
 : CBData_ (CBData_in)
 , session_ (interfaceHandle_in)
#else
 : session_ (interfaceHandle_in)
#endif // GUI_SUPPORT
 , sessionData_ ()
{
  NETWORK_TRACE (ACE_TEXT ("BitTorrent_Client_TrackerStreamHandler_T::BitTorrent_Client_TrackerStreamHandler_T"));

  // sanity check(s)
  ACE_ASSERT (session_);
}

template <typename SessionDataType,
          typename UserDataType,
          typename SessionInterfaceType
#if defined (GUI_SUPPORT)
          ,typename CBDataType>
#else
          >
#endif // GUI_SUPPORT
void
BitTorrent_Client_TrackerStreamHandler_T<SessionDataType,
                                         UserDataType,
                                         SessionInterfaceType
#if defined (GUI_SUPPORT)
                                         ,CBDataType>::start (Stream_SessionId_t sessionId_in,
#else
                                         >::start (Stream_SessionId_t sessionId_in,
#endif // GUI_SUPPORT
                                                              const SessionDataType& sessionData_in)
{
  NETWORK_TRACE (ACE_TEXT ("BitTorrent_Client_TrackerStreamHandler_T::start"));

  SESSION_DATA_ITERATOR_T iterator = sessionData_.find (sessionId_in);

  // sanity check(s)
  ACE_ASSERT (iterator == sessionData_.end ());
  ACE_ASSERT (session_);

  sessionData_.insert (std::make_pair (sessionId_in,
                                       &const_cast<SessionDataType&> (sessionData_in)));

  try {
    session_->trackerConnect (sessionId_in);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Net_ISession_T::trackerConnect(), continuing\n")));
  }

  if (CBData_)
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->lock);
    //  CBData_->progressData.transferred = 0;
    CBData_->eventStack.push (COMMON_UI_EVENT_STARTED);

//    guint event_source_id = g_idle_add (idle_start_UI_cb,
//                                        CBData_);
//    if (event_source_id == 0)
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to g_idle_add(idle_send_UI_cb): \"%m\", returning\n")));
//      return;
//    } // end IF
//    CBData_->eventSourceIds.insert (event_source_id);
  } // end lock scope
}

template <typename SessionDataType,
          typename UserDataType,
          typename SessionInterfaceType
#if defined (GUI_SUPPORT)
          ,typename CBDataType>
#else
          >
#endif // GUI_SUPPORT
void
BitTorrent_Client_TrackerStreamHandler_T<SessionDataType,
                                         UserDataType,
                                         SessionInterfaceType
#if defined (GUI_SUPPORT)
                                         ,CBDataType>::notify (Stream_SessionId_t sessionId_in,
#else
                                         >::notify (Stream_SessionId_t sessionId_in,
#endif // GUI_SUPPORT
                                                               const enum Stream_SessionMessageType& sessionEvent_in)
{
  STREAM_TRACE (ACE_TEXT ("BitTorrent_Client_TrackerStreamHandler_T::notify"));

  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (sessionEvent_in);

  ACE_ASSERT (false);
  ACE_NOTSUP;

  ACE_NOTREACHED (return;)
}

template <typename SessionDataType,
          typename UserDataType,
          typename SessionInterfaceType
#if defined (GUI_SUPPORT)
          ,typename CBDataType>
#else
          >
#endif // GUI_SUPPORT
void
BitTorrent_Client_TrackerStreamHandler_T<SessionDataType,
                                         UserDataType,
                                         SessionInterfaceType
#if defined (GUI_SUPPORT)
                                         ,CBDataType>::end (Stream_SessionId_t sessionId_in)
#else
                                         >::end (Stream_SessionId_t sessionId_in)
#endif // GUI_SUPPORT
{
  NETWORK_TRACE (ACE_TEXT ("BitTorrent_Client_TrackerStreamHandler_T::end"));

  SESSION_DATA_ITERATOR_T iterator = sessionData_.find (sessionId_in);

  // sanity check(s)
  ACE_ASSERT (iterator != sessionData_.end ());
  ACE_ASSERT (session_);

  sessionData_.erase (iterator);

  try {
    session_->trackerDisconnect (sessionId_in);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Net_ISession_T::trackerDisconnect(), continuing\n")));
  }

  if (CBData_)
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->lock);
    CBData_->eventStack.push (COMMON_UI_EVENT_FINISHED);

//    guint event_source_id = g_idle_add (idle_end_UI_cb,
//                                        CBData_);
//    if (event_source_id == 0)
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to g_idle_add(idle_end_UI_cb): \"%m\", returning\n")));
//      return;
//    } // end IF
//    CBData_->eventSourceIds.insert (event_source_id);
  } // end lock scope
}

template <typename SessionDataType,
          typename UserDataType,
          typename SessionInterfaceType
#if defined (GUI_SUPPORT)
          ,typename CBDataType>
#else
          >
#endif // GUI_SUPPORT
void
BitTorrent_Client_TrackerStreamHandler_T<SessionDataType,
                                         UserDataType,
                                         SessionInterfaceType
#if defined (GUI_SUPPORT)
                                         ,CBDataType>::notify (Stream_SessionId_t sessionId_in,
#else
                                         >::notify (Stream_SessionId_t sessionId_in,
#endif // GUI_SUPPORT
                                                               const BitTorrent_TrackerMessage_T<Stream_SessionData_T<SessionDataType>,
                                                                                                 UserDataType>& message_in)
{
  NETWORK_TRACE (ACE_TEXT ("BitTorrent_Client_TrackerStreamHandler_T::notify"));

  ACE_UNUSED_ARG (sessionId_in);

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);
  ACE_ASSERT (session_);

#if defined (_DEBUG)
  const typename MESSAGE_T::DATA_T& data_container_r = message_in.getR ();
  const struct HTTP_Record& record_r = data_container_r.getR ();
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s\n"),
              ACE_TEXT (HTTP_Tools::dump (record_r).c_str ())));
#endif

  inherited::configuration_->block = false;
  inherited::configuration_->messageQueue = NULL;

  // parse bencoded record
  PARSER_T parser (inherited::configuration_->debugScanner,
                   inherited::configuration_->debugParser);
  if (!parser.initialize (*inherited::configuration_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to BitTorrent_Bencoding_ParserDriver_T::initialize(), returning\n")));
    return;
  } // end IF

  if (!parser.parse (&const_cast<MESSAGE_T&> (message_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_ParserBase_T::parse(), returning\n")));
    return;
  } // end IF
  ACE_ASSERT (parser.bencoding_);
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s\n"),
              ACE_TEXT (Common_Parser_Bencoding_Tools::DictionaryToString (*parser.bencoding_).c_str ())));
#endif

  try {
    session_->notify (*parser.bencoding_);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Net_ISession_T::notify(), continuing\n")));
  }

  if (CBData_)
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->lock);
    CBData_->progressData.transferred += message_in.total_length ();
    CBData_->eventStack.push (COMMON_UI_EVENT_DATA);
  } // end lock scope
}
template <typename SessionDataType,
          typename UserDataType,
          typename SessionInterfaceType
#if defined (GUI_SUPPORT)
          ,typename CBDataType>
#else
          >
#endif // GUI_SUPPORT
void
BitTorrent_Client_TrackerStreamHandler_T<SessionDataType,
                                         UserDataType,
                                         SessionInterfaceType
#if defined (GUI_SUPPORT)
                                         ,CBDataType>::notify (Stream_SessionId_t sessionId_in,
#else
                                         >::notify (Stream_SessionId_t sessionId_in,
#endif // GUI_SUPPORT
                                                               const BitTorrent_SessionMessage_T<SessionDataType,
                                                                                                 UserDataType>& sessionMessage_in)
{
  NETWORK_TRACE (ACE_TEXT ("BitTorrent_Client_TrackerStreamHandler_T::notify"));

  SESSION_DATA_ITERATOR_T iterator = sessionData_.find (sessionId_in);
  int result = -1;

  // sanity check(s)
  ACE_ASSERT (iterator != sessionData_.end ());

  if (CBData_)
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->lock);
    enum Common_UI_EventType event_e = COMMON_UI_EVENT_INVALID;
    switch (sessionMessage_in.type ())
    {
      case STREAM_SESSION_MESSAGE_STATISTIC:
      {
        if ((*iterator).second->lock)
        {
          result = (*iterator).second->lock->acquire ();
          if (result == -1)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", continuing\n")));
        } // end IF

        CBData_->progressData.statistic = (*iterator).second->statistic;

        if ((*iterator).second->lock)
        {
          result = (*iterator).second->lock->release ();
          if (result == -1)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n")));
        } // end IF

        event_e = COMMON_UI_EVENT_STATISTIC;
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown session message type (was: %d), returning\n"),
                    sessionMessage_in.type ()));
        return;
      }
    } // end SWITCH
    CBData_->eventStack.push (event_e);
  } // end IF
}
