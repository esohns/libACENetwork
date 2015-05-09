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

#include "net_client_timeouthandler.h"

#include "ace/Log_Msg.h"

#include "net_defines.h"
#include "net_common.h"
#include "net_connection_manager_common.h"
#include "net_macros.h"

Net_Client_TimeoutHandler::Net_Client_TimeoutHandler (ActionMode_t mode_in,
                                                      unsigned int maxNumConnections_in,
                                                      const ACE_INET_Addr& remoteSAP_in,
                                                      Net_Client_IConnector_t* connector_in)
 : inherited (NULL,                           // default reactor
              ACE_Event_Handler::LO_PRIORITY) // priority
 , alternatingModeState_ (ALTERNATING_STATE_CONNECT)
 , connector_ (connector_in)
 , distribution_ (1, 100)
 , generator_ ()
 , lock_ ()
 , maxNumConnections_ (maxNumConnections_in)
 , mode_ (mode_in)
 , peerAddress_ (remoteSAP_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_TimeoutHandler::Net_Client_TimeoutHandler"));

}

Net_Client_TimeoutHandler::~Net_Client_TimeoutHandler ()
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_TimeoutHandler::~Net_Client_TimeoutHandler"));

}

void
Net_Client_TimeoutHandler::mode (ActionMode_t mode_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_TimeoutHandler::mode"));

  ACE_ASSERT (mode_in < ACTION_MAX);

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

    mode_ = mode_in;
  } // end lock scope
}

Net_Client_TimeoutHandler::ActionMode_t
Net_Client_TimeoutHandler::mode () const
{
  NETWORK_TRACE (ACE_TEXT ("Net_Client_TimeoutHandler::mode"));

  // initialize return value(s)
  ActionMode_t result = ACTION_INVALID;

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

    result = mode_;
  } // end lock scope

  return result;
}

int
Net_Client_TimeoutHandler::handle_timeout (const ACE_Time_Value& tv_in,
                                           const void* arg_in)
{
  NETWORK_TRACE(ACE_TEXT("Net_Client_TimeoutHandler::handle_timeout"));

  ACE_UNUSED_ARG (tv_in);
  ACE_UNUSED_ARG (arg_in);

  Net_InetConnectionManager_t::CONNECTION_T* abort_connection_p = NULL;
  Net_InetConnectionManager_t::CONNECTION_T* ping_connection_p = NULL;
  bool do_abort = false;
  bool do_abort_oldest = false;
  bool do_abort_youngest = false;
  bool do_connect = false;
  bool do_ping = false;
  NET_CONNECTIONMANAGER_SINGLETON::instance ()->lock ();
  unsigned int num_connections =
    NET_CONNECTIONMANAGER_SINGLETON::instance ()->numConnections ();

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

    switch (mode_)
    {
      case ACTION_NORMAL:
      {
        do_connect = true;

        break;
      }
      case ACTION_ALTERNATING:
      {
        switch (alternatingModeState_)
        {
          case ALTERNATING_STATE_CONNECT:
          {
            // sanity check: max num connections already reached ?
            // --> abort the oldest one first
            if (maxNumConnections_ &&
                (num_connections >= maxNumConnections_))
              do_abort_oldest = true;

            do_connect = true;

            break;
          }
          case ALTERNATING_STATE_ABORT:
          {
            // sanity check
            if (num_connections == 0)
              break; // nothing to do...

            // grab a (random) connection handler
            int index = 0;
            // *PORTABILITY*: outside glibc, this is not very portable...
  #if !defined (ACE_WIN32) && !defined (ACE_WIN64)
            index = ((::random () % num_connections) + 1);
  #else
            // *NOTE*: use ACE_OS::rand_r() for improved thread safety !
            //results_out.push_back((ACE_OS::rand() % range_in) + 1);
            unsigned int usecs =
              static_cast<unsigned int> (COMMON_TIME_NOW.usec ());
            index = ((ACE_OS::rand_r (&usecs) % num_connections) + 1);
  #endif

            abort_connection_p =
              NET_CONNECTIONMANAGER_SINGLETON::instance ()->operator [] (index - 1);
            if (!abort_connection_p)
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("failed to retrieve connection #%d/%d, aborting\n"),
                          index - 1, num_connections));

              // clean up
              NET_CONNECTIONMANAGER_SINGLETON::instance ()->unlock ();

              return -1;
            } // end IF

            do_abort = true;

            break;
          }
          default:
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("unknown/invalid alternating state (was: %d), aborting\n"),
                        alternatingModeState_));

            // clean up
            NET_CONNECTIONMANAGER_SINGLETON::instance ()->unlock ();

            return -1;
          }
        } // end SWITCH

        // cycle mode
        int temp = alternatingModeState_;
        alternatingModeState_ =
          static_cast<Net_Client_TimeoutHandler::AlternatingModeState_t> (++temp);
        if (alternatingModeState_ == ALTERNATING_STATE_MAX)
          alternatingModeState_ = ALTERNATING_STATE_CONNECT;

        break;
      }
      case ACTION_STRESS:
      {
        // allow some probability for closing connections in between
        auto dice = std::bind (distribution_, generator_);
        float dice_roll = static_cast<float> (dice ()) / 100.0F;

        if ((num_connections > 0) &&
            (dice_roll <= NET_CLIENT_U_TEST_ABORT_PROBABILITY))
          do_abort_youngest = true;

        // allow some probability for opening connections in between
        dice_roll = static_cast<float> (dice ()) / 100.0F;
        if (dice_roll <= NET_CLIENT_U_TEST_CONNECT_PROBABILITY)
          do_connect = true;

        // ping the server

        // sanity check
        if ((num_connections == 0) ||
            ((num_connections == 1) && do_abort_youngest))
          break;

        // grab a (random) connection handler
        std::uniform_int_distribution<int> distribution (0, num_connections - 1);
        int dice_roll_2 = distribution (generator_);
        ping_connection_p =
          NET_CONNECTIONMANAGER_SINGLETON::instance ()->operator [] (dice_roll_2);
        if (!ping_connection_p)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to retrieve connection #%d/%d, aborting\n"),
                      dice_roll_2, num_connections));

          // clean up
          NET_CONNECTIONMANAGER_SINGLETON::instance ()->unlock ();

          return -1;
        } // end IF

        do_ping = true;

        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unknown/invalid mode (was: %d), aborting\n"),
                    mode_));

        // clean up
        NET_CONNECTIONMANAGER_SINGLETON::instance ()->unlock ();

        return -1;
      }
    } // end SWITCH
  } // end lock scope
  NET_CONNECTIONMANAGER_SINGLETON::instance ()->unlock ();

  // ------------------------------------

  if (do_abort)
  {
    ACE_ASSERT (abort_connection_p);
    try
    {
      abort_connection_p->close ();
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Net_IConnection_T::close(), aborting\n")));

      // clean up
      abort_connection_p->decrease ();
      if (ping_connection_p)
        ping_connection_p->decrease ();

      return -1;
    }

    // clean up
    abort_connection_p->decrease ();
  } // end IF

  if (do_abort_oldest)
    NET_CONNECTIONMANAGER_SINGLETON::instance ()->abortOldestConnection ();

  if (do_abort_youngest)
    NET_CONNECTIONMANAGER_SINGLETON::instance ()->abortYoungestConnection ();

  if (do_connect)
  {
    ACE_ASSERT (connector_);
    bool result = false;
    try
    {
      result = connector_->connect (peerAddress_);
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Net_Client_IConnector_t::connect(), aborting\n")));

      // clean up
      if (ping_connection_p)
        ping_connection_p->decrease ();

      return -1;
    }
    if (!result)
    {
      ACE_TCHAR buffer[BUFSIZ];
      ACE_OS::memset (buffer, 0, sizeof (buffer));
      int result_2 = peerAddress_.addr_to_string (buffer, sizeof (buffer));
      if (result_2 == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_Client_IConnector::connect(\"%s\"), continuing\n"),
                  buffer));
    } // end IF
  } // end IF

  if (do_ping)
  {
    ACE_ASSERT (ping_connection_p);
    Net_ITransportLayer_t* transportlayer_p =
      dynamic_cast<Net_ITransportLayer_t*> (ping_connection_p);
    if (!transportlayer_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to dynamic_cast<Net_ITransportLayer_t*> (0x%@), aborting\n"),
                  ping_connection_p));

      // clean up
      ping_connection_p->decrease ();

      return -1;
    } // end IF

    try
    {
      transportlayer_p->ping ();
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Net_ITransportLayer_t::ping(), aborting\n")));

      // clean up
      ping_connection_p->decrease ();

      return -1;
    }

    // clean up
    ping_connection_p->decrease ();
  } // end IF

  return 0;
}
