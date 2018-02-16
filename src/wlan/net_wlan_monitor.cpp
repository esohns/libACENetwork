﻿/***************************************************************************
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

#include "ace/Synch.h"
#include "net_wlan_monitor.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if defined (DBUS_SUPPORT)
#include "NetworkManager/NetworkManager.h"
#endif // DBUS_SUPPORT

#include "net_configuration.h"
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if defined (DBUS_SUPPORT)
//void
//network_wlan_dbus_main_wakeup_cb (void* userData_in)
//{
//  //  NETWORK_TRACE (ACE_TEXT ("network_wlan_dbus_main_wakeup_cb "));

//  // sanity check(s)
//  ACE_ASSERT (userData_in);

//  struct DBusConnection* connection_p =
//      static_cast<struct DBusConnection*> (userData_in);
//  ACE_ASSERT (connection_p);

//  int socket_fd = -1;
//  if (!dbus_connection_get_socket (connection_p,
//                                   &socket_fd))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to dbus_connection_get_socket(0x%@): \"%m\", returning\n"),
//                connection_p));
//    return;
//  } // end IF
//  ACE_ASSERT (socket_fd != -1);

//  int result = ACE_OS::raise (SIGPOLL); // *TODO*: prefer SIGIO
//  if (result == -1)
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_OS::raise(SIGPOLL): \"%m\", continuing\n")));
//}

DBusHandlerResult
network_wlan_dbus_default_filter_cb (struct DBusConnection* connection_in,
                                     struct DBusMessage* message_in,
                                     void* userData_in)
{
//  NETWORK_TRACE (ACE_TEXT ("network_wlan_dbus_default_filter_cb"));

  // sanity check(s)
  ACE_ASSERT (connection_in);
  ACE_ASSERT (message_in);
  if (dbus_message_get_type (message_in) != DBUS_MESSAGE_TYPE_SIGNAL)
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  ACE_ASSERT (userData_in);
  Net_WLAN_IMonitorBase* imonitorbase_p =
      static_cast<Net_WLAN_IMonitorBase*> (userData_in);
  ACE_ASSERT (imonitorbase_p);

  bool result = false;
  struct DBusError error_s;
  dbus_error_init (&error_s);
  std::string object_path_string, device_identifier_string;
  const char* object_path_p = NULL;

  if (dbus_message_is_signal (message_in,
                              ACE_TEXT_ALWAYS_CHAR (NET_WLAN_MONITOR_DBUS_NETWORKMANAGER_INTERFACE),
                              ACE_TEXT_ALWAYS_CHAR ("DeviceAdded")))
  {
    if (!dbus_message_get_args (message_in,
                                &error_s,
                                DBUS_TYPE_OBJECT_PATH,
                                &object_path_p,
                                DBUS_TYPE_INVALID))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to dbus_message_get_args(): \"%s\", aborting\n"),
                  ACE_TEXT (error_s.message)));

      dbus_error_free (&error_s);

      goto continue_;
    } // end IF
    ACE_ASSERT (object_path_p);

    object_path_string = object_path_p;
    device_identifier_string =
        Net_WLAN_Tools::deviceDBusPathToIdentifier (connection_in,
                                                    object_path_string);
    if (device_identifier_string.empty ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_WLAN_Tools::deviceDBusPathToIdentifier(\"%s\"), aborting\n"),
                  ACE_TEXT (object_path_string.c_str ())));
      goto continue_;
    } // end IF

    // add to cache
    Common_ISet2R_T<std::string>* iset_p =
        dynamic_cast<Common_ISet2R_T<std::string>*> (imonitorbase_p);
    ACE_ASSERT (iset_p);
    iset_p->set2R (device_identifier_string, object_path_string);

    try {
      imonitorbase_p->onHotPlug (device_identifier_string,
                                     true);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Net_WLAN_IMonitorCB;;onHotPlug(), continuing\n")));
      goto continue_;
    }
  } // end IF
  else if (dbus_message_is_signal (message_in,
                                   ACE_TEXT_ALWAYS_CHAR (NET_WLAN_MONITOR_DBUS_NETWORKMANAGER_INTERFACE),
                                   ACE_TEXT_ALWAYS_CHAR ("DeviceRemoved")))
  {
    if (!dbus_message_get_args (message_in,
                                &error_s,
                                DBUS_TYPE_OBJECT_PATH,
                                &object_path_p,
                                DBUS_TYPE_INVALID))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to dbus_message_get_args(): \"%s\", aborting\n"),
                  ACE_TEXT (error_s.message)));

      dbus_error_free (&error_s);

      goto continue_;
    } // end IF
    ACE_ASSERT (object_path_p);

    object_path_string = object_path_p;
    // check cache first
    Common_IGet1R_T<std::string>* iget_p =
        dynamic_cast<Common_IGet1R_T<std::string>*> (imonitorbase_p);
    ACE_ASSERT (iget_p);
    // *NOTE*: this fails, the device has gone away
//    device_identifier_string =
//        Net_Common_Tools::deviceDBusPathToIdentifier (connection_in,
//                                                      object_path_string);
    device_identifier_string = iget_p->get1R (object_path_string);
    if (device_identifier_string.empty ())
    {
//      ACE_DEBUG ((LM_DEBUG,
//                  ACE_TEXT ("failed to Net_Common_Tools::deviceDBusPathToIdentifier(\"%s\"), aborting\n"),
//                  ACE_TEXT (object_path_string.c_str ())));
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("failed to retrieve device identifier (DBus object path was: \"%s\"), aborting\n"),
                  ACE_TEXT (object_path_string.c_str ())));
      goto continue_;
    } // end IF
    try {
      imonitorbase_p->onHotPlug (device_identifier_string,
                                     false);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Net_WLAN_IMonitorCB;;onHotPlug(), continuing\n")));
      goto continue_;
    }
  } // end ELSE IF
  ////////////////////////////////////////
  else if (dbus_message_is_signal (message_in,
                                   ACE_TEXT_ALWAYS_CHAR (NET_WLAN_MONITOR_DBUS_NETWORKMANAGER_DEVICE_INTERFACE),
                                   ACE_TEXT_ALWAYS_CHAR ("StateChanged")))
  {
    NMDeviceState state_previous, state_current;
    NMDeviceStateReason reason;
    std::string active_access_point_path_string, SSID_string;
    Common_IGet1R_T<std::string>* iget_p = NULL;

    if (!dbus_message_get_args (message_in,
                                &error_s,
                                DBUS_TYPE_UINT32,
                                &state_current,
                                DBUS_TYPE_UINT32,
                                &state_previous,
                                DBUS_TYPE_UINT32,
                                &reason,
                                DBUS_TYPE_INVALID))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to dbus_message_get_args(): \"%s\", aborting\n"),
                  ACE_TEXT (error_s.message)));

      dbus_error_free (&error_s);

      goto continue_;
    } // end IF
    switch (state_current)
    {
      case NM_DEVICE_STATE_IP_CONFIG:
      case NM_DEVICE_STATE_ACTIVATED:
      case NM_DEVICE_STATE_DEACTIVATING:
      case NM_DEVICE_STATE_DISCONNECTED:
        break;
      default:
        goto continue_;
    } // end SWITCH

    object_path_p = dbus_message_get_path (message_in);
    ACE_ASSERT (object_path_p);
    object_path_string = object_path_p;
    if (object_path_string.empty ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to dbus_message_get_path(), aborting\n")));
      goto continue_;
    } // end IF
    // check cache first
    iget_p =
        dynamic_cast<Common_IGet1R_T<std::string>*> (imonitorbase_p);
    ACE_ASSERT (iget_p);
    device_identifier_string = iget_p->get1R (object_path_string);
    if (device_identifier_string.empty ())
      device_identifier_string =
          Net_WLAN_Tools::deviceDBusPathToIdentifier (connection_in,
                                                      object_path_string);
    if (device_identifier_string.empty ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_WLAN_Tools::deviceDBusPathToIdentifier(\"%s\"), aborting\n"),
                  ACE_TEXT (object_path_string.c_str ())));
      goto continue_;
    } // end IF
    if (ACE_OS::strcmp (device_identifier_string.c_str (),
                        imonitorbase_p->interfaceIdentifier ().c_str ()))
      goto continue_; // --> not interested
    // *NOTE*: this may fail if the device has-/is- disconnect-ed/-ing
    if ((state_current == NM_DEVICE_STATE_IP_CONFIG) ||
        (state_current == NM_DEVICE_STATE_ACTIVATED))
    {
      active_access_point_path_string =
          Net_WLAN_Tools::deviceDBusPathToActiveAccessPointDBusPath (connection_in,
                                                                     object_path_string);
      if (active_access_point_path_string.empty ())
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Net_WLAN_Tools::deviceDBusPathToActiveAccessPointDBusPath(\"%s\"), aborting\n"),
                    ACE_TEXT (object_path_string.c_str ())));
        goto continue_;
      } // end IF
      SSID_string =
          Net_WLAN_Tools::accessPointDBusPathToSSID (connection_in,
                                                     active_access_point_path_string);
      if (SSID_string.empty ())
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Net_WLAN_Tools::accessPointDBusPathToSSID(\"%s\"), aborting\n"),
                    ACE_TEXT (active_access_point_path_string.c_str ())));
        goto continue_;
      } // end IF
    } // end IF
    switch (state_current)
    {
      case NM_DEVICE_STATE_IP_CONFIG:
      {
        try {
          imonitorbase_p->onAssociate (device_identifier_string,
                                       SSID_string,
                                       true);
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("caught exception in Net_WLAN_IMonitorCB::onAssociate(), continuing\n")));
          goto continue_;
        }
        break;
      }
      case NM_DEVICE_STATE_ACTIVATED:
      {
        try {
          imonitorbase_p->onConnect (device_identifier_string,
                                     SSID_string,
                                     true);
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("caught exception in Net_WLAN_IMonitorCB::onConnect(), continuing\n")));
          goto continue_;
        }
        break;
      }
      case NM_DEVICE_STATE_DEACTIVATING:
      {
        try {
          imonitorbase_p->onConnect (device_identifier_string,
                                     SSID_string,
                                     false);
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("caught exception in Net_WLAN_IMonitorCB::onConnect(), continuing\n")));
          goto continue_;
        }
        break;
      }
      case NM_DEVICE_STATE_DISCONNECTED:
      {
        try {
          imonitorbase_p->onAssociate (device_identifier_string,
                                       SSID_string,
                                       false);
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("caught exception in Net_WLAN_IMonitorCB::onAssociate(), continuing\n")));
          goto continue_;
        }
        break;
      }
      default:
        break;
    } // end SWITCH
  } // end ELSE IF
  ////////////////////////////////////////
  else if (dbus_message_is_signal (message_in,
                                   ACE_TEXT_ALWAYS_CHAR (NET_WLAN_MONITOR_DBUS_NETWORKMANAGER_CONNECTIONACTIVE_INTERFACE),
                                   ACE_TEXT_ALWAYS_CHAR ("StateChanged")))
  {
    NMActiveConnectionState state_current;
    ACE_UINT32 reason;
    std::string active_access_point_path_string, SSID_string;
    std::string device_path_string;

    if (!dbus_message_get_args (message_in,
                                &error_s,
                                DBUS_TYPE_UINT32,
                                &state_current,
                                DBUS_TYPE_UINT32,
                                &reason,
                                DBUS_TYPE_INVALID))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to dbus_message_get_args(): \"%s\", aborting\n"),
                  ACE_TEXT (error_s.message)));

      dbus_error_free (&error_s);

      goto continue_;
    } // end IF
    switch (state_current)
    {
//      case NM_ACTIVE_CONNECTION_STATE_ACTIVATING:
      case NM_ACTIVE_CONNECTION_STATE_ACTIVATED:
      case NM_ACTIVE_CONNECTION_STATE_DEACTIVATING:
//      case NM_ACTIVE_CONNECTION_STATE_DEACTIVATED:
        break;
      default:
        goto continue_;
    } // end SWITCH

    object_path_p = dbus_message_get_path (message_in);
    ACE_ASSERT (object_path_p);
    object_path_string = object_path_p;
    if (object_path_string.empty ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to dbus_message_get_path(), aborting\n")));
      goto continue_;
    } // end IF
    device_path_string =
        Net_WLAN_Tools::activeConnectionDBusPathToDeviceDBusPath (connection_in,
                                                                  object_path_string);
    if (device_path_string.empty ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_WLANs_Tools::activeConnectionDBusPathToDeviceDBusPath(\"%s\"), aborting\n"),
                  ACE_TEXT (object_path_string.c_str ())));
      goto continue_;
    } // end IF
    device_identifier_string =
        Net_WLAN_Tools::deviceDBusPathToIdentifier (connection_in,
                                                    device_path_string);
    if (device_identifier_string.empty ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_WLAN_Tools::deviceDBusPathToIdentifier(\"%s\"), aborting\n"),
                  ACE_TEXT (device_path_string.c_str ())));
      goto continue_;
    } // end IF
    active_access_point_path_string =
        Net_WLAN_Tools::deviceDBusPathToActiveAccessPointDBusPath (connection_in,
                                                                   device_path_string);
    if (active_access_point_path_string.empty ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_WLAN_Tools::deviceDBusPathToActiveAccessPointDBusPath(\"%s\"), aborting\n"),
                  ACE_TEXT (device_path_string.c_str ())));
      goto continue_;
    } // end IF
    SSID_string =
        Net_WLAN_Tools::accessPointDBusPathToSSID (connection_in,
                                                   active_access_point_path_string);
    if (SSID_string.empty ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_WLAN_Tools::accessPointDBusPathToSSID(\"%s\"), aborting\n"),
                  ACE_TEXT (active_access_point_path_string.c_str ())));
      goto continue_;
    } // end IF
    switch (state_current)
    {
//      case NM_ACTIVE_CONNECTION_STATE_ACTIVATING:
//      {
//        try {
//          imonitorbase_p->onAssociate (device_identifier_string,
//                                           SSID_string,
//                                           true);
//        } catch (...) {
//          ACE_DEBUG ((LM_ERROR,
//                      ACE_TEXT ("caught exception in Net_WLAN_IMonitorCB;;onAssociate(), continuing\n")));
//          goto continue_;
//        }
//        break;
//      }
      case NM_ACTIVE_CONNECTION_STATE_ACTIVATED:
      {
        try {
          imonitorbase_p->onConnect (device_identifier_string,
                                         SSID_string,
                                         true);
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("caught exception in Net_WLAN_IMonitorCB;;onConnect(), continuing\n")));
          goto continue_;
        }
        break;
      }
      case NM_ACTIVE_CONNECTION_STATE_DEACTIVATING:
      {
        try {
          imonitorbase_p->onConnect (device_identifier_string,
                                         SSID_string,
                                         false);
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("caught exception in Net_WLAN_IMonitorCB;;onConnect(), continuing\n")));
          goto continue_;
        }
        break;
      }
//      case NM_ACTIVE_CONNECTION_STATE_DEACTIVATED:
//      {
//        try {
//          imonitorbase_p->onAssociate (device_identifier_string,
//                                           SSID_string,
//                                           false);
//        } catch (...) {
//          ACE_DEBUG ((LM_ERROR,
//                      ACE_TEXT ("caught exception in Net_WLAN_IMonitorCB;;onAssociate(), continuing\n")));
//          goto continue_;
//        }
//        break;
//      }
      default:
        break;
    } // end SWITCH
  } // end ELSE IF
  ////////////////////////////////////////
  else if (dbus_message_is_signal (message_in,
                                   ACE_TEXT_ALWAYS_CHAR (NET_WLAN_MONITOR_DBUS_NETWORKMANAGER_DEVICEWIRELESS_INTERFACE),
                                   ACE_TEXT_ALWAYS_CHAR ("AccessPointAdded")))
  {
    object_path_p = dbus_message_get_path (message_in);
    ACE_ASSERT (object_path_p);
    object_path_string = object_path_p;
    if (object_path_string.empty ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to dbus_message_get_path(), aborting\n")));
      goto continue_;
    } // end IF
    device_identifier_string =
        Net_WLAN_Tools::deviceDBusPathToIdentifier (connection_in,
                                                    object_path_string);

    if (device_identifier_string.empty ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_WLAN_Tools::deviceDBusPathToIdentifier(\"%s\"), aborting\n"),
                  ACE_TEXT (object_path_string.c_str ())));
      goto continue_;
    } // end IF
    try {
      imonitorbase_p->onScanComplete (device_identifier_string);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: caught exception in Net_WLAN_IMonitorCB::onScanComplete(), continuing\n")));
    }
  } // end ELSE IF
//  else if (dbus_message_is_signal (message_in,
//                                   ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_WIRELESS_INTERFACE),
//                                   ACE_TEXT_ALWAYS_CHAR ("AccessPointRemoved")))
//  {
//    if (!dbus_message_get_args (message_in,
//                                &error_s,
//                                DBUS_TYPE_OBJECT_PATH,
//                                &object_path_p,
//                                DBUS_TYPE_INVALID))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to dbus_message_get_args(): \"%s\", aborting\n"),
//                  ACE_TEXT (error_s.message)));

//      dbus_error_free (&error_s);

//      goto continue_;
//    } // end IF
//    ACE_ASSERT (object_path_p);
//    SSID_string = Net_Common_Tools::accessPointDBusPathToSSID (connection_in,
//                                                               object_path_p);
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("lost access point (SSID was: %s)...\n"),
//                ACE_TEXT (SSID_string.c_str ())));
//  } // end ELSE IF
  else
  {
//    ACE_DEBUG ((LM_WARNING,
//                ACE_TEXT ("invalid/unknown dbus signal (was: \"%s\":\"%s\"), continuing\n"),
//                ACE_TEXT (dbus_message_get_interface (message_in)),
//                ACE_TEXT ("")));
    goto continue_;
  } // end IF

  result = true;

continue_:
  if (dbus_error_is_set (&error_s))
    dbus_error_free (&error_s);

  return (result ? DBUS_HANDLER_RESULT_HANDLED
                 : DBUS_HANDLER_RESULT_NOT_YET_HANDLED);
}
//void
//network_wlan_networkmanager_state_changed_cb (struct DBusGProxy* proxy_in,
//                                              guint32 state_in,
//                                              gpointer userData_in)
//{
//  //  NETWORK_TRACE (ACE_TEXT ("network_wlan_networkmanager_state_changed_cb"));

//  ACE_UNUSED_ARG (proxy_in);

//  // sanity check(s)
//  ACE_ASSERT (userData_in);

//  Net_WLAN_IMonitorCB* iwlan_cb_p = static_cast<Net_WLAN_IMonitorCB*> (userData_in);
//}
#endif // DBUS_SUPPORT

int
network_wlan_nl80211_error_cb (struct sockaddr_nl* address_in,
                               struct nlmsgerr* message_in,
                               void* argument_in)
{
  //  NETWORK_TRACE (ACE_TEXT ("network_wlan_nl80211_error_cb"));

  // sanity check(s)
  ACE_ASSERT (address_in);
  ACE_ASSERT (message_in);
  ACE_ASSERT (argument_in);

  Net_Netlink_Addr netlink_address;
  netlink_address.set_addr (address_in,
                            sizeof (struct sockaddr_nl));
  ACE_TCHAR buffer_a[BUFSIZ];
  int result = netlink_address.addr_to_string (buffer_a,
                                               sizeof (buffer_a),
                                               1);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Netlink_Addr::addr_to_string(): \"%m\", continuing\n")));
    ACE_OS::memset (buffer_a, 0, sizeof (buffer_a));
  } // end IF
  struct genlmsghdr* genlmsghdr_p =
      static_cast<struct genlmsghdr*> (nlmsg_data (&message_in->msg));
  ACE_ASSERT (genlmsghdr_p);
  int* result_p = static_cast<int*> (argument_in);
  struct nlattr* nlattr_a[NLMSGERR_ATTR_MAX + 1];
  nla_parse (nlattr_a,
             NLMSGERR_ATTR_MAX,
             genlmsg_attrdata (genlmsghdr_p, 0),
             genlmsg_attrlen (genlmsghdr_p, 0),
             NULL);
  ACE_ASSERT (nlattr_a[NLMSGERR_ATTR_MSG]);

  // *NOTE*: kernel returns negative errno
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("\"%s\": nl80211 error: \"%s\" (errno: \"%s\" (%d)), stopping\n"),
              buffer_a,
              ACE_TEXT (nla_get_string (nlattr_a[NLMSGERR_ATTR_MSG])),
              ACE_TEXT (ACE_OS::strerror ( -message_in->error)),
              message_in->error));

  *result_p = message_in->error;

  return NL_STOP;
}

int
network_wlan_nl80211_finish_cb (struct nl_msg* message_in,
                                void* argument_in)
{
  //  NETWORK_TRACE (ACE_TEXT ("network_wlan_nl80211_finish_cb"));

  // sanity check(s)
  ACE_ASSERT (message_in);
  ACE_ASSERT (argument_in);

  ACE_UNUSED_ARG (message_in);

  int* result_p = static_cast<int*> (argument_in);
  *result_p = 0;

  return NL_SKIP;
}

int
network_wlan_nl80211_ack_cb (struct nl_msg* message_in,
                             void* argument_in)
{
  //  NETWORK_TRACE (ACE_TEXT ("network_wlan_nl80211_ack_cb"));

  // sanity check(s)
  ACE_ASSERT (message_in);
  ACE_ASSERT (argument_in);

  ACE_UNUSED_ARG (message_in);

  int* result_p = static_cast<int*> (argument_in);
  *result_p = 0;

  return NL_STOP;
}

int
network_wlan_nl80211_no_seq_check_cb (struct nl_msg* message_in,
                                      void* argument_in)
{
  //  NETWORK_TRACE (ACE_TEXT ("network_wlan_nl80211_no_seq_check_cb"));

  // sanity check(s)
  ACE_ASSERT (message_in);
  ACE_ASSERT (argument_in);

  ACE_UNUSED_ARG (message_in);
  ACE_UNUSED_ARG (argument_in);

  return NL_OK;
}

int
network_wlan_nl80211_family_cb (struct nl_msg* message_in,
                                void* argument_in)
{
  //  NETWORK_TRACE (ACE_TEXT ("network_wlan_nl80211_family_cb"));

  // sanity check(s)
  ACE_ASSERT (message_in);
  ACE_ASSERT (argument_in);

  // Callback for NL_CB_VALID within nl_get_multicast_id(). From http://sourcecodebrowser.com/iw/0.9.14/genl_8c.html.
  struct nlattr* nlattr_a[CTRL_ATTR_MAX + 1];
  struct genlmsghdr* genlmsghdr_p =
      static_cast<struct genlmsghdr*> (nlmsg_data (nlmsg_hdr (message_in)));
  ACE_ASSERT (genlmsghdr_p);
  nla_parse (nlattr_a,
             CTRL_ATTR_MAX,
             genlmsg_attrdata (genlmsghdr_p, 0),
             genlmsg_attrlen (genlmsghdr_p, 0),
             NULL);
  if (!nlattr_a[CTRL_ATTR_MCAST_GROUPS])
    return NL_SKIP;

  struct nlattr* nlattr_p = NULL;
  int rem_mcgrp = 0;
  struct Net_WLAN_nl80211_MulticastHandlerArguments* arguments_p =
      static_cast<struct Net_WLAN_nl80211_MulticastHandlerArguments*> (argument_in);
  ACE_ASSERT (!arguments_p->group.empty ());
  nla_for_each_nested (nlattr_p, nlattr_a[CTRL_ATTR_MCAST_GROUPS], rem_mcgrp)
  { ACE_ASSERT (nlattr_p);
    struct nlattr* nlattr_2[CTRL_ATTR_MCAST_GRP_MAX + 1];
    nla_parse (nlattr_2,
               CTRL_ATTR_MCAST_GRP_MAX,
               static_cast<struct nlattr*> (nla_data (nlattr_p)),
               nla_len (nlattr_p),
               NULL);
    if (!nlattr_2[CTRL_ATTR_MCAST_GRP_NAME] ||
        !nlattr_2[CTRL_ATTR_MCAST_GRP_ID])
      continue;
    if (ACE_OS::strncmp (reinterpret_cast<char*> (nla_data (nlattr_2[CTRL_ATTR_MCAST_GRP_NAME])),
                         ACE_TEXT_ALWAYS_CHAR (arguments_p->group.c_str ()),
                         static_cast<size_t> (nla_len (nlattr_2[CTRL_ATTR_MCAST_GRP_NAME]))))
      continue;

    arguments_p->id = nla_get_u32 (nlattr_2[CTRL_ATTR_MCAST_GRP_ID]);
    break;
  } // end FOREACH

  return NL_SKIP;
}

int
network_wlan_nl80211_scan_result_cb (struct nl_msg* message_in,
                                     void* argument_in)
{
  //  NETWORK_TRACE (ACE_TEXT ("network_wlan_nl80211_scan_result_cb"));

  // sanity check(s)
  ACE_ASSERT (message_in);
  ACE_ASSERT (argument_in);

  // Called by the kernel when the scan is done or has been aborted.
#if defined (_DEBUG)
  nl_msg_dump (message_in, stdout);
#endif // _DEBUG

  struct genlmsghdr* genlmsghdr_p =
      static_cast<struct genlmsghdr*> (nlmsg_data (nlmsg_hdr (message_in)));
  ACE_ASSERT (genlmsghdr_p);
  struct Net_WLAN_nl80211_ScanResult* result_p =
      static_cast<struct Net_WLAN_nl80211_ScanResult*> (argument_in);

  switch (genlmsghdr_p->cmd)
  {
    case NL80211_CMD_SCAN_ABORTED:
    {
      result_p->aborted = true;
      result_p->done = true;
      break;
    }
    case NL80211_CMD_NEW_SCAN_RESULTS:
    {
//      results->aborted = false;
      result_p->done = true;
      break;
    }  // else probably an uninteresting multicast message.
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown nl80211 command (was: %d), continuing\n"),
                  genlmsghdr_p->cmd));
      break;
    }
  } // end SWITCH

  return NL_SKIP;
}
int
network_wlan_nl80211_scan_data_cb (struct nl_msg* message_in,
                                   void* argument_in)
{
  //  NETWORK_TRACE (ACE_TEXT ("network_wlan_nl80211_scan_data_cb"));

  // sanity check(s)
  ACE_ASSERT (message_in);
  ACE_ASSERT (argument_in);

  // Called by the kernel with a dump of the successful scan's data. Called for each SSID.

  struct genlmsghdr* genlmsghdr_p =
      static_cast<struct genlmsghdr*> (nlmsg_data (nlmsg_hdr (message_in)));
  ACE_ASSERT (genlmsghdr_p);
  struct nlattr* nlattr_a[NL80211_ATTR_MAX + 1];
  struct nlattr* nlattr_2[NL80211_BSS_MAX + 1];
  // *TODO*: this is bound to change; #include from somewhere else
  static struct nla_policy nla_policy_bss_a[NL80211_BSS_MAX + 1] =
  {
    {NLA_U8, 6, 6},     // NL80211_BSS_BSSID
    {NLA_U32, 0, 0},    // NL80211_BSS_FREQUENCY
    {NLA_U64, 0, 0},    // NL80211_BSS_TSF
    {NLA_U16, 0, 0},    // NL80211_BSS_BEACON_INTERVAL
    {NLA_U16, 0, 0},    // NL80211_BSS_CAPABILITY
    {NLA_BINARY, 0, 0}, // NL80211_BSS_INFORMATION_ELEMENTS
    {NLA_S32, 0, 0},    // NL80211_BSS_SIGNAL_MBM
    {NLA_U8, 0, 0},     // NL80211_BSS_SIGNAL_UNSPEC
    {NLA_U32, 0, 0},    // NL80211_BSS_STATUS
    {NLA_U32, 0, 0},    // NL80211_BSS_SEEN_MS_AGO
    {NLA_BINARY, 0, 0}, // NL80211_BSS_BEACON_IES
    {NLA_U32, 0, 0},    // NL80211_BSS_CHAN_WIDTH
    {NLA_U64, 0, 0},    // NL80211_BSS_BEACON_TSF
    {NLA_FLAG, 0, 0},   // NL80211_BSS_PRESP_DATA
    {NLA_U64, 0, 0},    // NL80211_BSS_LAST_SEEN_BOOTTIME
    {NLA_U64, 0, 0},    // NL80211_BSS_PAD
    {NLA_U64, 0, 0},    // NL80211_BSS_PARENT_TSF
    {NLA_U8, 6, 6},     // NL80211_BSS_PARENT_BSSID
  };
  nla_parse (nlattr_a,
             NL80211_ATTR_MAX,
             genlmsg_attrdata (genlmsghdr_p, 0),
             genlmsg_attrlen (genlmsghdr_p, 0),
             NULL);
  // sanity check(s)
  if (!nlattr_a[NL80211_ATTR_BSS])
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to parse nl80211 scan result data: bss info missing, returning\n")));
    return NL_SKIP;
  } // end IF
  if (nla_parse_nested (nlattr_2,
                        NL80211_BSS_MAX,
                        nlattr_a[NL80211_ATTR_BSS],
                        nla_policy_bss_a))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to parse nl80211 scan result data bss info, returning\n")));
    return NL_SKIP;
  } // end IF
  if (!nlattr_2[NL80211_BSS_BSSID] ||
      !nlattr_2[NL80211_BSS_INFORMATION_ELEMENTS])
    return NL_SKIP;

  struct ether_addr ap_mac_address_s;
  ACE_OS::memcpy (&ap_mac_address_s.ether_addr_octet,
                  nla_data (nlattr_2[NL80211_BSS_BSSID]),
                  ETH_ALEN);
  unsigned int frequency_mhz =
      nla_get_u32 (nlattr_2[NL80211_BSS_FREQUENCY]);

//  char buffer_a[IW_ESSID_MAX_SIZE];
  uint8_t len_i;
  uint8_t* data_p;
  int i;
  uint8_t ie_len_i =
      static_cast<uint8_t> (nla_len (nlattr_2[NL80211_BSS_INFORMATION_ELEMENTS]));
  uint8_t* ie_data_p =
      reinterpret_cast<uint8_t*> (nla_data (nlattr_2[NL80211_BSS_INFORMATION_ELEMENTS]));
  while (ie_len_i >= 2 && ie_len_i >= ie_data_p[1])
  {
    if (ie_data_p[0] == 0 &&
        ie_data_p[1] >= 0 && ie_data_p[1] <= 32)
    {
      len_i = ie_data_p[1];
      data_p = ie_data_p + 2;
      for (i = 0; i < len_i; i++)
      {
        if (isprint (data_p[i]) &&
            data_p[i] != ' ' &&
            data_p[i] != '\\')
          printf ("%c", data_p[i]);
        else if (data_p[i] == ' ' && (i != 0 && i != len_i -1))
          printf (" ");
        else
          printf ("\\x%.2x", data_p[i]);
      } // end FOR
      break;
    } // end IF
    ie_len_i -= ie_data_p[1] + 2;
    ie_data_p += ie_data_p[1] + 2;
  } // end WHILE

  return NL_SKIP;
}
#endif // ACE_WIN32 || ACE_WIN64
