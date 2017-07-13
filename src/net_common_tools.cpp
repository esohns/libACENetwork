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

#include "ace/Synch.h"
#include "net_common_tools.h"

#include <regex>
#include <sstream>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <cguid.h>
#include <guiddef.h>
#include <iphlpapi.h>
#include <mstcpip.h>
#include <Wlanapi.h>
#else
#include <ifaddrs.h>
#include <iwlib.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <netinet/ether.h>
#endif

#include "ace/Dirent_Selector.h"
#include "ace/INET_Addr.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_defines.h"
#include "common_file_tools.h"
#include "common_tools.h"

#include "net_common.h"
#include "net_defines.h"
#include "net_macros.h"
#include "net_packet_headers.h"

//////////////////////////////////////////

enum Net_LinkLayerType&
operator++ (enum Net_LinkLayerType& lhs) // prefix-
{
  // roll over ?
  if (lhs == NET_LINKLAYER_MAX)
  {
    lhs = NET_LINKLAYER_ATM;
    return lhs;
  } // end IF

  int result = lhs << 1;
  lhs = static_cast<enum Net_LinkLayerType> (result);
  if (lhs >= NET_LINKLAYER_MAX) lhs = NET_LINKLAYER_MAX;

  return lhs;
}
enum Net_LinkLayerType
operator++ (enum Net_LinkLayerType& lhs, int) // postfix-
{
  enum Net_LinkLayerType result = lhs;
  ++lhs;
  return result;
}

//////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
void WINAPI
network_wlan_notification_cb (PWLAN_NOTIFICATION_DATA data_in,
                              PVOID context_in)
{
  NETWORK_TRACE (ACE_TEXT ("network_wlan_notification_cb"));

  // sanity check(s)
  ACE_ASSERT (data_in);
  ACE_ASSERT (context_in);

  ACE_SYNCH_CONDITION* condition_p =
    static_cast<ACE_SYNCH_CONDITION*> (context_in);

  std::string notification_string;
  switch (data_in->NotificationSource)
  {
    case WLAN_NOTIFICATION_SOURCE_ACM:
    {
      switch (data_in->NotificationCode)
      {
        case wlan_notification_acm_autoconf_enabled:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_autoconf_enabled");
          break;
        case wlan_notification_acm_autoconf_disabled:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_autoconf_disabled");
          break;
        case wlan_notification_acm_background_scan_enabled:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_background_scan_enabled");
          break;
        case wlan_notification_acm_background_scan_disabled:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_background_scan_disabled");
          break;
        case wlan_notification_acm_bss_type_change:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_bss_type_change");
          break;
        case wlan_notification_acm_power_setting_change:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_power_setting_change");
          break;
        case wlan_notification_acm_scan_complete:
        {
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_scan_complete");
          int result = condition_p->broadcast ();
          if (result == -1)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_Condition::broadcast(): \"%m\"\n")));
          break;
        }
        case wlan_notification_acm_scan_fail:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_scan_fail");
          break;
        case wlan_notification_acm_connection_start:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_connection_start");
          break;
        case wlan_notification_acm_connection_complete:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_connection_complete");
          break;
        case wlan_notification_acm_connection_attempt_fail:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_connection_attempt_fail");
          break;
        case wlan_notification_acm_filter_list_change:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_filter_list_change");
          break;
        case wlan_notification_acm_interface_arrival:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_interface_arrival");
          break;
        case wlan_notification_acm_interface_removal:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_interface_removal");
          break;
        case wlan_notification_acm_profile_change:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_profile_change");
          break;
        case wlan_notification_acm_profile_name_change:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_profile_name_change");
          break;
        case wlan_notification_acm_profiles_exhausted:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_profiles_exhausted");
          break;
        case wlan_notification_acm_network_not_available:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_network_not_available");
          break;
        case wlan_notification_acm_network_available:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_network_available");
          break;
        case wlan_notification_acm_disconnecting:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_disconnecting");
          break;
        case wlan_notification_acm_disconnected:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_disconnected");
          break;
        case wlan_notification_acm_adhoc_network_state_change:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_adhoc_network_state_change");
          break;
        case wlan_notification_acm_profile_unblocked:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_profile_unblocked");
          break;
        case wlan_notification_acm_screen_power_change:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_screen_power_change");
          break;
        case wlan_notification_acm_profile_blocked:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_profile_blocked");
          break;
        case wlan_notification_acm_scan_list_refresh:
          notification_string =
            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_scan_list_refresh");
          break;
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown notification code (was: %d), returning\n"),
                      data_in->NotificationCode));
          return;
        }
      } // end SWITCH

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown notification source (was: %d), returning\n"),
                  data_in->NotificationSource));
      return;
    }
  } // end SWITCH
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\"%s\": received notification \"%s\"\n"),
              ACE_TEXT (Net_Common_Tools::interfaceToString (data_in->InterfaceGuid).c_str ()),
              ACE_TEXT (notification_string.c_str ())));
}
#endif

//////////////////////////////////////////

#if defined (ACE_HAS_NETLINK)
std::string
Net_Common_Tools::NetlinkAddressToString (const Net_Netlink_Addr& NetlinkAddress_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::NetlinkAddressToString"));

  // initialize return value(s)
  std::string return_value;

  int result = -1;
  ACE_TCHAR buffer[BUFSIZ];
  ACE_OS::memset (&buffer, 0, sizeof (buffer));
  result = NetlinkAddress_in.addr_to_string (buffer,
                                             sizeof (buffer),
                                             1); // N/A
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Netlink_Addr::addr_to_string(): \"%m\", aborting\n")));
    return return_value;
  } // end IF

  // copy string from buffer
  return_value = buffer;

  return return_value;
}
#endif
std::string
Net_Common_Tools::IPAddressToString (unsigned short port_in,
                                     ACE_UINT32 IPAddress_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::IPAddressToString"));

  // initialize return value(s)
  std::string return_value;

  int result = -1;
  ACE_INET_Addr inet_addr;
  ACE_TCHAR buffer[32]; // "xxx.xxx.xxx.xxx:yyyyy\0"
  ACE_OS::memset (&buffer, 0, sizeof (buffer));
  result = inet_addr.set (port_in,
                          IPAddress_in,
                          0,  // no need to encode, data IS in network byte order !
                          0); // only needed for IPv6...
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::set(): \"%m\", aborting\n")));
    return return_value;
  } // end IF
  result = inet_addr.addr_to_string (buffer,
                                     sizeof (buffer),
                                     1); // want IP address, not hostname !
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Inet_Addr::addr_to_string(): \"%m\", aborting\n")));
    return return_value;
  } // end IF

  // copy string from buffer
  return_value = buffer;

  // clean up: if port number was 0, cut off the trailing ":0" !
  if (!port_in)
  {
    std::string::size_type last_colon_pos =
      return_value.find_last_of (':',
                                 std::string::npos); // begin searching at the end !
    if (last_colon_pos != std::string::npos)
      return_value = return_value.substr (0, last_colon_pos);
  } // end IF

  return return_value;
}

bool
Net_Common_Tools::matchIPAddress (std::string& address_in)
{
  NETWORK_TRACE ("Net_Common_Tools::matchIPAddress");

  std::string regex_string =
    //ACE_TEXT_ALWAYS_CHAR ("^([[:digit:]]{0,3})(\\.[[:digit:]]{0,3}){0,3}$");
    ACE_TEXT_ALWAYS_CHAR ("^([[:digit:]]{0,3})(\\.[[:digit:]]{0,3})?(\\.[[:digit:]]{0,3})?(\\.[[:digit:]]{0,3})?$");
  std::regex::flag_type flags = std::regex_constants::ECMAScript;
  std::regex regex;
  std::smatch match_results;
  //try {
    regex.assign (regex_string, flags);
  //} catch (std::regex_error exception_in) {
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("caught regex exception (was: \"%s\"), returning\n"),
  //              ACE_TEXT (exception_in.what ())));
  //  goto refuse;
  //}
  if (!std::regex_match (address_in,
                         match_results,
                         regex,
                         std::regex_constants::match_default))
  {
    //ACE_DEBUG ((LM_DEBUG,
    //            ACE_TEXT ("invalid address (was: \"%s\"), aborting\n"),
    //            ACE_TEXT (address_in.c_str ())));
    return false;
  } // end IF
  ACE_ASSERT (match_results.ready () && !match_results.empty ());

  // validate all groups
  std::stringstream converter;
  std::string group_string;
  unsigned int group = 0;
  std::string::size_type start_position;
  for (unsigned int i = 1; i < match_results.size (); ++i)
  {
    if (!match_results[i].matched) return false;

    group_string = match_results[i].str ();
    if (i > 1) group_string.erase (0, 1);

    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter.clear ();
    converter << group_string;
    converter >> group;
    if (group > 255) return false; // refuse groups > 255

    start_position = group_string.find ('0', 0);
    if ((start_position == 0) &&
        (group_string.size () > 1)) return false; // refuse leading 0s
  } // end FOR

  return true;
}

ACE_INET_Addr
Net_Common_Tools::stringToIPAddress (std::string& address_in)
{
  NETWORK_TRACE ("Net_Common_Tools::stringToIPAddress");

  // *NOTE*: ACE_INET_Addr::string_to_address() needs a trailing port number to
  //         function properly (see: ace/INET_Addr.h:237)
  //         --> append one if necessary
  std::string ip_address_string = address_in;
  std::string::size_type position = ip_address_string.find (':', 0);
  if (position == std::string::npos)
    ip_address_string += ACE_TEXT_ALWAYS_CHAR (":0");

  int result = -1;
  ACE_INET_Addr inet_addr;
  result = inet_addr.string_to_addr (ip_address_string.c_str (),
                                     AF_INET);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::string_to_addr(): \"%m\", aborting\n")));
    return ACE_INET_Addr ();
  } // end IF

  return inet_addr;
}

std::string
Net_Common_Tools::IPProtocolToString (unsigned char protocol_in)
{
  NETWORK_TRACE ("Net_Common_Tools::IPProtocolToString");

  // initialize return value(s)
  std::string result;

  switch (protocol_in)
  {
    case IPPROTO_IP: // OR case IPPROTO_HOPOPTS:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_IP/IPPROTO_HOPOPTS");
      break;
    case IPPROTO_ICMP:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_ICMP");
      break;
    case IPPROTO_IGMP:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_IGMP");
      break;
    case IPPROTO_IPIP:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_IPIP");
      break;
    case IPPROTO_TCP:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_TCP");
      break;
    case IPPROTO_EGP:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_EGP");
      break;
    case IPPROTO_PUP:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_PUP");
      break;
    case IPPROTO_UDP:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_UDP");
      break;
    case IPPROTO_IDP:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_IDP");
      break;
    case IPPROTO_TP:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_TP");
      break;
    case IPPROTO_IPV6:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_IPV6");
      break;
    case IPPROTO_ROUTING:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_ROUTING");
      break;
    case IPPROTO_FRAGMENT:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_FRAGMENT");
      break;
    case IPPROTO_RSVP:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_RSVP");
      break;
    case IPPROTO_GRE:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_GRE");
      break;
    case IPPROTO_ESP:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_ESP");
      break;
    case IPPROTO_AH:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_AH");
      break;
    case IPPROTO_ICMPV6:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_ICMPV6");
      break;
    case IPPROTO_NONE:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_NONE");
      break;
    case IPPROTO_DSTOPTS:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_DSTOPTS");
      break;
    case IPPROTO_MTP:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_MTP");
      break;
    case IPPROTO_ENCAP:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_ENCAP");
      break;
    case IPPROTO_PIM:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_PIM");
      break;
    case IPPROTO_COMP:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_COMP");
      break;
    case IPPROTO_SCTP:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_SCTP");
      break;
    case IPPROTO_RAW:
      result = ACE_TEXT_ALWAYS_CHAR ("IPPROTO_RAW");
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown IP protocol: %1u, continuing\n"),
                  protocol_in));
      break;
    }
  } // end SWITCH

  return result;
}

std::string
Net_Common_Tools::LinkLayerAddressToString (const unsigned char* const addressDataPtr_in,
                                            enum Net_LinkLayerType type_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::LinkLayerAddressToString"));

  // initialize return value(s)
  std::string result = ACE_TEXT_ALWAYS_CHAR ("NET_LINKLAYER_INVALID");

  switch (type_in)
  {
    case NET_LINKLAYER_802_3:
    case NET_LINKLAYER_802_11:
    {
      char buffer[NET_ADDRESS_LINK_ETHERNET_ADDRESS_STRING_SIZE];
      ACE_OS::memset (&buffer, 0, sizeof (buffer));

      // *IMPORTANT NOTE*: ether_ntoa_r is not portable
      // *TODO*: ether_ntoa_r is not portable...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      PSTR result_2 =
        RtlEthernetAddressToStringA (reinterpret_cast<const _DL_EUI48* const> (addressDataPtr_in),
                                     buffer);
      if (result_2 != (buffer + NET_ADDRESS_LINK_ETHERNET_ADDRESS_STRING_SIZE - 1))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to RtlEthernetAddressToStringA(), aborting\n")));
        return result;
      } // end IF
      //char buffer[BUFSIZ];
      //ACE_OS::memset (buffer, 0, sizeof (buffer));
      //int result_2 =
      //  ACE_OS::sprintf (buffer,
      //                   ACE_TEXT_ALWAYS_CHAR ("%02X:%02X:%02X:%02X:%02X:%02X"),
      //                   addressDataPtr_in[0], addressDataPtr_in[1],
      //                   addressDataPtr_in[2], addressDataPtr_in[3],
      //                   addressDataPtr_in[4], addressDataPtr_in[5]);
      //if (result_2 == -1)
      //{
      //  ACE_DEBUG ((LM_ERROR,
      //              ACE_TEXT ("failed to ACE_OS::sprintf(): \"%m\", aborting\n")));
      //  return std::string ();
      //} // end IF
      //result = buffer;
#else
      if (::ether_ntoa_r (reinterpret_cast<const ether_addr*> (addressDataPtr_in),
                          buffer) != buffer)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ::ether_ntoa_r(): \"%m\", aborting\n")));
        return result;
      } // end IF
#endif
      result = buffer;

      break;
    }
    case NET_LINKLAYER_PPP:
    {
      // *NOTE*: being point-to-point in nature, PPP does not support (link
      //         layer-) addressing
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("link layer type \"%s\" does not support addressing, continuing\n"),
                  ACE_TEXT (Net_Common_Tools::LinkLayerTypeToString (type_in).c_str ())));
      return ACE_TEXT_ALWAYS_CHAR ("");
    }
    case NET_LINKLAYER_ATM:
    case NET_LINKLAYER_FDDI:
    {
      ACE_ASSERT (false);
      ACE_NOTSUP_RETURN (result);

      ACE_NOTREACHED (break;)
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown link layer type (was: \"%s\"), aborting\n"),
                  ACE_TEXT (Net_Common_Tools::LinkLayerTypeToString (type_in).c_str ())));
      break;
    }
  } // end SWITCH

  return result;
}
std::string
Net_Common_Tools::LinkLayerTypeToString (enum Net_LinkLayerType type_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::LinkLayerTypeToString"));

  // initialize return value(s)
  std::string result = ACE_TEXT_ALWAYS_CHAR ("NET_LINKLAYER_INVALID");

  switch (type_in)
  {
    case NET_LINKLAYER_ATM:
      return ACE_TEXT_ALWAYS_CHAR ("NET_LINKLAYER_ATM");
    case NET_LINKLAYER_802_3:
      return ACE_TEXT_ALWAYS_CHAR ("NET_LINKLAYER_802_3");
    case NET_LINKLAYER_FDDI:
      return ACE_TEXT_ALWAYS_CHAR ("NET_LINKLAYER_FDDI");
    case NET_LINKLAYER_PPP:
      return ACE_TEXT_ALWAYS_CHAR ("NET_LINKLAYER_PPP");
    case NET_LINKLAYER_802_11:
      return ACE_TEXT_ALWAYS_CHAR ("NET_LINKLAYER_802_11");
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown link layer type (was: %d), aborting\n"),
                  type_in));
      break;
    }
  } // end SWITCH

  return result;
}

std::string
Net_Common_Tools::EthernetProtocolTypeIDToString (unsigned short frameType_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::EthernetProtocolTypeIDToString"));

  // initialize return value(s)
  std::string result;

  switch (ACE_NTOHS (frameType_in))
  {
    case ETH_P_LOOP:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_LOOP");
      break;
    case ETHERTYPE_GRE_ISO:
      result = ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_GRE_ISO");
      break;
    case ETH_P_PUP:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_PUP");
      break;
    case ETH_P_PUPAT:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_PUPAT");
      break;
    case ETHERTYPE_SPRITE:
      result = ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_SPRITE");
      break;
    case ETH_P_IP:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_IP");
      break;
    case ETH_P_X25:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_X25");
      break;
    case ETH_P_ARP:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_ARP");
      break;
    case ETH_P_BPQ:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_BPQ");
      break;
    case ETH_P_IEEEPUP:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_IEEEPUP");
      break;
    case ETH_P_IEEEPUPAT:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_IEEEPUPAT");
      break;
    case ETHERTYPE_NS:
      result = ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_NS");
      break;
    case ETHERTYPE_TRAIL:
      result = ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_TRAIL");
      break;
    case ETH_P_DEC:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_DEC");
      break;
    case ETH_P_DNA_DL:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_DNA_DL");
      break;
    case ETH_P_DNA_RC:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_DNA_RC");
      break;
    case ETH_P_DNA_RT:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_DNA_RT");
      break;
    case ETH_P_LAT:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_LAT");
      break;
    case ETH_P_DIAG:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_DIAG");
      break;
    case ETH_P_CUST:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_CUST");
      break;
    case ETH_P_SCA:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_SCA");
      break;
    case ETH_P_RARP:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_RARP");
      break;
    case ETHERTYPE_LANBRIDGE:
      result = ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_LANBRIDGE");
      break;
    case ETHERTYPE_DECDNS:
      result = ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_DECDNS");
      break;
    case ETHERTYPE_DECDTS:
      result = ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_DECDTS");
      break;
    case ETHERTYPE_VEXP:
      result = ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_VEXP");
      break;
    case ETHERTYPE_VPROD:
      result = ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_VPROD");
      break;
    case ETH_P_ATALK:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_ATALK");
      break;
    case ETH_P_AARP:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_AARP");
      break;
    case ETH_P_8021Q:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_8021Q");
      break;
    case ETH_P_IPX:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_IPX");
      break;
    case ETH_P_IPV6:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_IPV6");
      break;
    case ETHERTYPE_MPCP:
      result = ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_MPCP");
      break;
    case ETHERTYPE_SLOW:
      result = ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_SLOW");
      break;
    case ETHERTYPE_PPP:
      result = ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_PPP");
      break;
    case ETH_P_WCCP:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_WCCP");
      break;
    case ETH_P_MPLS_UC:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_MPLS_UC");
      break;
    case ETH_P_MPLS_MC:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_MPLS_MC");
      break;
    case ETH_P_ATMMPOA:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_ATMMPOA");
      break;
    case ETH_P_PPP_DISC:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_PPP_DISC");
      break;
    case ETH_P_PPP_SES:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_PPP_SES");
      break;
    case ETHERTYPE_JUMBO:
      result = ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_JUMBO");
      break;
    case ETH_P_ATMFATE:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_ATMFATE");
      break;
    case ETHERTYPE_EAPOL:
      result = ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_EAPOL");
      break;
    case ETH_P_AOE:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_AOE");
      break;
    case ETH_P_TIPC:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_TIPC");
      break;
    case ETHERTYPE_LOOPBACK:
      result = ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_LOOPBACK");
      break;
    case ETHERTYPE_VMAN:
      result = ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_VMAN");
      break;
    case ETHERTYPE_ISO:
      result = ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_ISO");
      break;
// ********************* Non DIX types ***********************
    case ETH_P_802_3:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_802_3");
      break;
    case ETH_P_AX25:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_AX25");
      break;
    case ETH_P_ALL:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_ALL");
      break;
    case ETH_P_802_2:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_802_2");
      break;
    case ETH_P_SNAP:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_SNAP");
      break;
    case ETH_P_DDCMP:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_DDCMP");
      break;
    case ETH_P_WAN_PPP:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_WAN_PPP");
      break;
    case ETH_P_PPP_MP:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_PPP_MP");
      break;
    case ETH_P_LOCALTALK:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_LOCALTALK");
      break;
    case ETH_P_PPPTALK:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_PPPTALK");
      break;
    case ETH_P_TR_802_2:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_TR_802_2");
      break;
    case ETH_P_MOBITEX:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_MOBITEX");
      break;
    case ETH_P_CONTROL:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_CONTROL");
      break;
    case ETH_P_IRDA:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_IRDA");
      break;
    case ETH_P_ECONET:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_ECONET");
      break;
    case ETH_P_HDLC:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_HDLC");
      break;
    case ETH_P_ARCNET:
      result = ACE_TEXT_ALWAYS_CHAR ("ETH_P_ARCNET");
      break;
    default:
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("invalid/unknown ethernet frame type: \"0x%x\", continuing\n"),
                  ACE_NTOHS (frameType_in)));

      // IEEE 802.3 ? --> change result string...
      if (ACE_NTOHS (frameType_in) <= ETH_DATA_LEN)
        result = ACE_TEXT_ALWAYS_CHAR ("UNKNOWN_IEEE_802_3_FRAME_TYPE");

      break;
    }
  } // end SWITCH

  return result;
}

bool
Net_Common_Tools::interfaceToExternalIPAddress (const std::string& interfaceIdentifier_in,
                                                ACE_INET_Addr& IPAddress_out)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::interfaceToExternalIPAddress"));

  // initialize return value(s)
  IPAddress_out.reset ();

  std::string interface_identifier_string = interfaceIdentifier_in;
  if (interface_identifier_string.empty ())
    interface_identifier_string =
      Net_Common_Tools::getDefaultInterface ();

  // step1: determine the 'internal' IP address
  ACE_INET_Addr internal_ip_address, gateway_ip_address;
  if (!Net_Common_Tools::interfaceToIPAddress (interface_identifier_string,
                                               internal_ip_address,
                                               gateway_ip_address))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(\"%s\"), aborting\n"),
                ACE_TEXT (interface_identifier_string.c_str ())));
    return false;
  } // end IF

  int result = -1;
  // *TODO*: this should work on most Linux/Windows systems, but is really a bad
  //         idea:
  //         - relies on local 'nslookup' tool
  //         - the 'opendns.com' domain resolution scheme
  //         - temporary files
  //         - system(3) call
  //         --> extremely inefficient; remove ASAP
  std::string filename_string =
      Common_File_Tools::getTempFilename (ACE_TEXT_ALWAYS_CHAR (""));
  std::string command_line_string =
      ACE_TEXT_ALWAYS_CHAR ("nslookup myip.opendns.com. resolver1.opendns.com >> ");
  command_line_string += filename_string;

  result = ACE_OS::system (ACE_TEXT (command_line_string.c_str ()));
//  result = execl ("/bin/sh", "sh", "-c", command, (char *) 0);
  if ((result == -1)      ||
      !WIFEXITED (result) ||
      WEXITSTATUS (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::system(\"%s\"): \"%m\" (result was: %d), aborting\n"),
                ACE_TEXT (command_line_string.c_str ()),
                WEXITSTATUS (result)));
    return false;
  } // end IF
  unsigned char* data_p = NULL;
  if (!Common_File_Tools::load (filename_string,
                                data_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::load(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (filename_string.c_str ())));
    return false;
  } // end IF
  if (!Common_File_Tools::deleteFile (filename_string))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::deleteFile(\"%s\"), continuing\n"),
                ACE_TEXT (filename_string.c_str ())));

  std::string resolution_record_string = reinterpret_cast<char*> (data_p);
  delete [] data_p;
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("nslookup data: \"%s\"\n"),
//              ACE_TEXT (resolution_record_string.c_str ())));

  std::string external_ip_address;
  std::istringstream converter;
  char buffer [BUFSIZ];
  std::string regex_string =
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_TEXT_ALWAYS_CHAR ("^([^:]+)(?::[[:blank:]]*)(.+)(?:\r)$");
#else
    ACE_TEXT_ALWAYS_CHAR ("^([^:]+)(?::[[:blank:]]*)(.+)$");
#endif
  std::regex regex (regex_string);
  std::smatch match_results;
  converter.str (resolution_record_string);
  bool is_first = true;
  std::string buffer_string;
  do
  {
    converter.getline (buffer, sizeof (buffer));
    buffer_string = buffer;
    if (!std::regex_match (buffer_string,
                           match_results,
                           regex,
                           std::regex_constants::match_default))
      continue;
    ACE_ASSERT (match_results.ready () && !match_results.empty ());

    if (match_results[1].matched &&
        (ACE_OS::strcmp (match_results[1].str ().c_str (),
                         ACE_TEXT_ALWAYS_CHAR (NET_ADDRESS_NSLOOKUP_RESULT_ADDRESS_KEY_STRING)) == 0))
    {
      if (is_first)
      {
        is_first = false;
        continue;
      } // end IF

      ACE_ASSERT (match_results[2].matched);
      external_ip_address = match_results[2];
      break;
    } // end IF
  } while (!converter.fail ());
  if (external_ip_address.empty ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to resolve IP address (was: %s), aborting\n"),
                ACE_TEXT (Net_Common_Tools::IPAddressToString (internal_ip_address).c_str ())));
    return false;
  } // end IF
  IPAddress_out = Net_Common_Tools::stringToIPAddress (external_ip_address);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("interface \"%s\" --> %s (--> %s)\n"),
              ACE_TEXT (interfaceIdentifier_in.c_str ()),
              ACE_TEXT (Net_Common_Tools::IPAddressToString (internal_ip_address).c_str ()),
              ACE_TEXT (Net_Common_Tools::IPAddressToString (IPAddress_out).c_str ())));

  return true;
}

bool
Net_Common_Tools::interfaceToMACAddress (const std::string& interfaceIdentifier_in,
                                         unsigned char MACAddress_out[])
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::interfaceToMACAddress"));

//  // sanity check(s)
//  ACE_ASSERT (sizeof (MACAddress_out) >= 6);

  // initialize return value(s)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  //  ACE_OS::memset (MACAddress_out, 0, sizeof (MACAddress_out));
    ACE_OS::memset (MACAddress_out, 0, 6);

  // sanity check(s)
  //ACE_ASSERT (sizeof (MACAddress_out) >= MAX_ADAPTER_ADDRESS_LENGTH);

  PIP_ADAPTER_INFO ip_adapter_info_p = NULL;
  ULONG buffer_length = 0;
  ULONG result = GetAdaptersInfo (ip_adapter_info_p, &buffer_length);
  if (result != ERROR_BUFFER_OVERFLOW)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::GetAdaptersInfo(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result).c_str ())));
    return false;
  } // end IF
  ACE_ASSERT (buffer_length);
  ip_adapter_info_p =
    static_cast<PIP_ADAPTER_INFO> (ACE_MALLOC_FUNC (buffer_length));
  if (!ip_adapter_info_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
    return false;
  } // end IF

  result = GetAdaptersInfo (ip_adapter_info_p,
                            &buffer_length);
  if (result != NO_ERROR)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::GetAdaptersInfo(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result).c_str ())));

    // clean up
    ACE_FREE_FUNC (ip_adapter_info_p);

    return false;
  } // end IF

  PIP_ADAPTER_INFO ip_adapter_info_2 = ip_adapter_info_p;
  do
  {
    //ACE_DEBUG ((LM_DEBUG,
    //            ACE_TEXT ("found network interface: \"%s\"...\n"),
    //            ACE_TEXT (Net_Common_Tools::MACAddress2String (ip_adapter_info_2->Address).c_str ())));

    if (ACE_OS::strcmp (interfaceIdentifier_in.c_str (),
                        ip_adapter_info_2->AdapterName))
      goto continue_;

    ACE_OS::memcpy (MACAddress_out, ip_adapter_info_2->Address,
                    6);
    break;

continue_:
    ip_adapter_info_2 = ip_adapter_info_2->Next;
  } while (ip_adapter_info_2);

  // clean up
  ACE_FREE_FUNC (ip_adapter_info_p);
#else
  //  ACE_OS::memset (MACAddress_out, 0, sizeof (MACAddress_out));
  ACE_OS::memset (MACAddress_out, 0, ETH_ALEN);

#if defined (ACE_HAS_GETIFADDRS)
  struct ifaddrs* ifaddrs_p = NULL;
  int result = ::getifaddrs (&ifaddrs_p);
  if (result == -1)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ::getifaddrs(): \"%m\", aborting\n")));
    return false;
  } // end IF
  ACE_ASSERT (ifaddrs_p);

  struct sockaddr_ll* sockaddr_ll_p = NULL;
  for (struct ifaddrs* ifaddrs_2 = ifaddrs_p;
       ifaddrs_2;
       ifaddrs_2 = ifaddrs_2->ifa_next)
  {
    if (ACE_OS::strcmp (interfaceIdentifier_in.c_str (),
                        ifaddrs_2->ifa_name))
      continue;

#if defined (ACE_LINUX)
    if (ifaddrs_2->ifa_addr->sa_family != AF_PACKET)
#else
    // *TODO*: this may work on BSD and APPLE systems
    if (ifaddrs_2->ifa_addr->sa_family != AF_LINK)
#endif
      continue;

    sockaddr_ll_p =
        reinterpret_cast<struct sockaddr_ll*> (ifaddrs_2->ifa_addr);
    ACE_OS::memcpy (MACAddress_out, sockaddr_ll_p->sll_addr,
                    ETH_ALEN);
    break;
  } // end FOR

  // clean up
  ::freeifaddrs (ifaddrs_p);
#else
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);
  ACE_NOTREACHED (return false;)
#endif /* ACE_HAS_GETIFADDRS */
#endif

  return true;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
std::string
Net_Common_Tools::interfaceToString (REFGUID GUID_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::interfaceToString"));

  // initialize return value(s)
  std::string result;

  HANDLE handle_client = NULL;
  // *TODO*: support WinXP
  DWORD maximum_client_version =
    WLAN_API_MAKE_VERSION (2, 0); // *NOTE*: 1 for <= WinXP_SP2
  DWORD current_version = 0;
  DWORD result_2 = 0;
  PWLAN_INTERFACE_INFO_LIST interface_list_p = NULL;
  PWLAN_INTERFACE_INFO interface_info_p = NULL;

  result_2 = WlanOpenHandle (maximum_client_version,
                              NULL,
                              &current_version,
                              &handle_client);
  if (result_2 != ERROR_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::WlanOpenHandle(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
    return result;
  } // end IF
  result_2 = WlanEnumInterfaces (handle_client,
                                 NULL,
                                 &interface_list_p);
  if (result_2 != ERROR_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::WlanEnumInterfaces(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF
  ACE_ASSERT (interface_list_p);

  for (DWORD i = 0;
       i < interface_list_p->dwNumberOfItems;
       ++i)
  {
    interface_info_p = &interface_list_p->InterfaceInfo[i];
    if (interface_info_p->InterfaceGuid != GUID_in)
      continue;

    result =
      ACE_TEXT_WCHAR_TO_TCHAR (interface_info_p->strInterfaceDescription);
  } // end FOR
  if (result.empty ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("interface not found (GUID was: %s), aborting\n"),
                ACE_TEXT (Common_Tools::GUIDToString (GUID_in).c_str ())));
    goto error;
  } // end IF

error:
  if (interface_list_p)
    WlanFreeMemory (interface_list_p);
  result_2 = WlanCloseHandle (handle_client,
                              NULL);
  if (result_2 != ERROR_SUCCESS)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::WlanCloseHandle(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));

  return result;
}
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
bool
Net_Common_Tools::interfaceIsWireless (const std::string& adapter_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::interfaceIsWireless"));

  // sanity check(s)
  ACE_ASSERT (!adapter_in.empty ());
  ACE_ASSERT (adapter_in.size () <= IFNAMSIZ);

  bool result = false;
  int socket_handle = -1;
  struct iwreq request_s;
  ACE_OS::memset (&request_s, 0, sizeof (struct iwreq));
  int result_2 = -1;

  ACE_OS::strncpy (request_s.ifr_name,
                   adapter_in.c_str (),
                   IFNAMSIZ);
  socket_handle = ACE_OS::socket (AF_INET,
                                  SOCK_STREAM,
                                  0);
  if (socket_handle == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::socket(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (adapter_in.c_str ())));
    return false;
  } // end IF
  result_2 = ACE_OS::ioctl (socket_handle,
                            SIOCGIWNAME,
                            &request_s);
  if (!result_2)
    result = true;

  result_2 = ACE_OS::close (socket_handle);
  if (socket_handle == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::close(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (adapter_in.c_str ())));

  return result;
}
#endif
bool
#if defined (ACE_WIN32) || defined (ACE_WIN64)
Net_Common_Tools::associateWithWLAN (REFGUID adapter_in,
#else
Net_Common_Tools::associateWithWLAN (const std::string& adapter_in,
#endif
                                     const std::string& SSID_in,
                                     ACE_INET_Addr& peerSAP_out,
                                     ACE_INET_Addr& localSAP_out,
                                     bool scanForNetworks_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::associateWithWLAN"));

  // sanity check(s)
  ACE_ASSERT (!SSID_in.empty ());

  // initialize return value(s)
  peerSAP_out.reset ();
  localSAP_out.reset ();

  bool result = false;
  unsigned int retries = 0;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  HANDLE handle_client = NULL;
  // *TODO*: support WinXP
  DWORD maximum_client_version =
    WLAN_API_MAKE_VERSION (2, 0); // *NOTE*: 1 for <= WinXP_SP2
  DWORD current_version = 0;
  DWORD result_2 = 0;
  PWLAN_INTERFACE_INFO_LIST interface_list_p = NULL;
  PWLAN_INTERFACE_INFO interface_info_p = NULL;
  //std::string interface_state_string;
  struct _DOT11_SSID ssid_s;
  ACE_OS::memset (&ssid_s, 0, sizeof (struct _DOT11_SSID));
  ACE_ASSERT (SSID_in.size () <= DOT11_SSID_MAX_LENGTH);
  ssid_s.uSSIDLength = SSID_in.size ();
  ACE_OS::memcpy (ssid_s.ucSSID,
                  SSID_in.c_str (),
                  SSID_in.size ());
  PWLAN_RAW_DATA raw_data_p = NULL;
  //DWORD notification_mask = WLAN_NOTIFICATION_SOURCE_ACM;
  DWORD notification_mask = WLAN_NOTIFICATION_SOURCE_ALL;
  DWORD previous_notification_mask = 0;
  //PWLAN_BSS_LIST wlan_bss_list_p = NULL;
  //PWLAN_BSS_ENTRY wlan_bss_entry_p = NULL;
  DWORD flags =
    (WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_ADHOC_PROFILES        |
     WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_MANUAL_HIDDEN_PROFILES);
  PWLAN_AVAILABLE_NETWORK_LIST wlan_network_list_p = NULL;
  PWLAN_AVAILABLE_NETWORK wlan_network_p = NULL;
  //std::string phy_type_string;
  //std::string SSID_string;
  //std::string bss_network_type_string;
  struct _WLAN_CONNECTION_PARAMETERS wlan_connection_parameters_s;
  ACE_SYNCH_MUTEX mutex;
  ACE_SYNCH_CONDITION condition (mutex);
  ACE_Time_Value wlan_ssid_scan_timeout (NET_PROTOCOL_WIN32_WLAN_SCAN_TIMEOUT,
                                         0);
  ACE_Time_Value timeout;
  int result_3 = -1;

  result_2 = WlanOpenHandle (maximum_client_version,
                             NULL,
                             &current_version,
                             &handle_client);
  if (result_2 != ERROR_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::WlanOpenHandle(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
    return false;
  } // end IF
  result_2 = WlanRegisterNotification (handle_client,
                                       notification_mask,
                                       FALSE,
                                       network_wlan_notification_cb,
                                       static_cast<PVOID> (&condition),
                                       NULL,
                                       &previous_notification_mask);
  if (result_2 != ERROR_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::WlanRegisterNotification(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF
  result_2 = WlanEnumInterfaces (handle_client,
                                 NULL,
                                 &interface_list_p);
  if (result_2 != ERROR_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::WlanEnumInterfaces(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF
  ACE_ASSERT (interface_list_p);

  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("found %u wireless adapter(s)\n"),
  //            interface_list_p->dwNumberOfItems));
  for (DWORD i = 0;
       i < interface_list_p->dwNumberOfItems;
       ++i)
  {
    interface_info_p = &interface_list_p->InterfaceInfo[i];
    //switch (interface_info_p->isState)
    //{
    //  case wlan_interface_state_not_ready:
    //    interface_state_string = ACE_TEXT_ALWAYS_CHAR ("not ready"); break;
    //  case wlan_interface_state_connected:
    //    interface_state_string = ACE_TEXT_ALWAYS_CHAR ("connected"); break;
    //  case wlan_interface_state_ad_hoc_network_formed:
    //    interface_state_string = ACE_TEXT_ALWAYS_CHAR ("first node in a ad hoc network"); break;
    //  case wlan_interface_state_disconnecting:
    //    interface_state_string = ACE_TEXT_ALWAYS_CHAR ("disconnecting"); break;
    //  case wlan_interface_state_disconnected:
    //    interface_state_string = ACE_TEXT_ALWAYS_CHAR ("not connected"); break;
    //  case wlan_interface_state_associating:
    //    interface_state_string = ACE_TEXT_ALWAYS_CHAR ("attempting to associate with a network"); break;
    //  case wlan_interface_state_discovering:
    //    interface_state_string = ACE_TEXT_ALWAYS_CHAR ("auto configuration is discovering settings for the network"); break;
    //  case wlan_interface_state_authenticating:
    //    interface_state_string = ACE_TEXT_ALWAYS_CHAR ("in process of authenticating"); break;
    //  default:
    //    interface_state_string = ACE_TEXT_ALWAYS_CHAR ("unknown state"); break;
    //} // end SWITCH
    //ACE_DEBUG ((LM_DEBUG,
    //            ACE_TEXT ("[#%u] %s: \"%s\": %s\n"),
    //            i + 1,
    //            ACE_TEXT (Common_Tools::GUIDToString (interface_info_p->InterfaceGuid).c_str ()),
    //            ACE_TEXT_WCHAR_TO_TCHAR (interface_info_p->strInterfaceDescription),
    //            ACE_TEXT (interface_state_string.c_str ())));

    if (!InlineIsEqualGUID (adapter_in, GUID_NULL) &&
        !InlineIsEqualGUID (adapter_in, interface_info_p->InterfaceGuid))
      continue;

check_networks:
    //result_2 = WlanGetNetworkBssList (handle_client,
    //                                  &interface_info_p->InterfaceGuid,
    //                                  &ssid_s,
    //                                  dot11_BSS_type_any,
    //                                  FALSE,
    //                                  NULL,
    //                                  &wlan_bss_list_p);
    result_2 = WlanGetAvailableNetworkList (handle_client,
                                            &interface_info_p->InterfaceGuid,
                                            flags,
                                            NULL,
                                            &wlan_network_list_p);
    if (result_2 != ERROR_SUCCESS)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("\"%s\": failed to ::WlanGetAvailableNetworkList(): \"%s\", aborting\n"),
                  ACE_TEXT_WCHAR_TO_TCHAR (interface_info_p->strInterfaceDescription),
                  ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
      goto error;
    } // end IF
    //ACE_ASSERT (wlan_bss_list_p);
    ACE_ASSERT (wlan_network_list_p);
    //ACE_DEBUG ((LM_DEBUG,
    //            ACE_TEXT ("found %u BSSs for wireless adapter \"%s\"\n"),
    //            wlan_bss_list_p->dwNumberOfItems,
    //            ACE_TEXT_WCHAR_TO_TCHAR (interface_info_p->strInterfaceDescription)));
    //ACE_DEBUG ((LM_DEBUG,
    //            ACE_TEXT ("found %u network(s) for wireless adapter \"%s\"\n"),
    //            wlan_network_list_p->dwNumberOfItems,
    //            ACE_TEXT_WCHAR_TO_TCHAR (interface_info_p->strInterfaceDescription)));

    for (DWORD j = 0;
         //j < wlan_bss_list_p->dwNumberOfItems;
         j < wlan_network_list_p->dwNumberOfItems;
         ++j)
    {
      //wlan_bss_entry_p = &wlan_bss_list_p->wlanBssEntries[j];
      wlan_network_p = &wlan_network_list_p->Network[j];
      //switch (wlan_bss_entry_p->dot11BssPhyType)
      //{
      //  case dot11_phy_type_fhss:
      //    phy_type_string = ACE_TEXT_ALWAYS_CHAR ("FHSS"); break;
      //  case dot11_phy_type_dsss:
      //    phy_type_string = ACE_TEXT_ALWAYS_CHAR ("DSSS"); break;
      //  case dot11_phy_type_irbaseband:
      //    phy_type_string = ACE_TEXT_ALWAYS_CHAR ("IR baseband"); break;
      //  case dot11_phy_type_ofdm:
      //    phy_type_string = ACE_TEXT_ALWAYS_CHAR ("OFDM"); break;
      //  case dot11_phy_type_hrdsss:
      //    phy_type_string = ACE_TEXT_ALWAYS_CHAR ("high-rate DSSS"); break;
      //  case dot11_phy_type_erp:
      //    phy_type_string = ACE_TEXT_ALWAYS_CHAR ("ERP"); break;
      //  case dot11_phy_type_ht:
      //    phy_type_string = ACE_TEXT_ALWAYS_CHAR ("802.11n PHY"); break;
      //  case dot11_phy_type_vht:
      //    phy_type_string = ACE_TEXT_ALWAYS_CHAR ("802.11ac PHY"); break;
      //  case dot11_phy_type_IHV_start:
      //  case dot11_phy_type_IHV_end:
      //  case dot11_phy_type_unknown:
      //  //case dot11_phy_type_any:
      //  default:
      //    phy_type_string = ACE_TEXT_ALWAYS_CHAR ("unknown PHY type"); break;
      //} // end SWITCH
      //switch (wlan_bss_entry_p->dot11BssType)
      //SSID_string.assign (reinterpret_cast<CHAR*> (wlan_network_p->dot11Ssid.ucSSID),
      //                    static_cast<std::string::size_type> (wlan_network_p->dot11Ssid.uSSIDLength));
      //switch (wlan_network_p->dot11BssType)
      //{
      //  case dot11_BSS_type_infrastructure:
      //    bss_network_type_string = ACE_TEXT_ALWAYS_CHAR ("infrastructure"); break;
      //  case dot11_BSS_type_independent:
      //    bss_network_type_string = ACE_TEXT_ALWAYS_CHAR ("ad-hoc"); break;
      //  case dot11_BSS_type_any:
      //  default:
      //    bss_network_type_string = ACE_TEXT_ALWAYS_CHAR ("unknown type"); break;
      //} // end SWITCH
      // *TODO*: report all available information here
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("[#%u] PHY %u; type: %s: AP MAC: %s; AP type: %s; RSSI: %d (dBm); link quality %u%%; in region domain: %s; beacon interval (us): %u; channel center frequency (kHz): %u\n"),
      //            j + 1,
      //            wlan_bss_entry_p->uPhyId,
      //            ACE_TEXT (phy_type_string.c_str ()),
      //            ACE_TEXT (Net_Common_Tools::LinkLayerAddressToString (wlan_bss_entry_p->dot11Bssid,
      //                                                                  NET_LINKLAYER_802_11).c_str ()),
      //            ACE_TEXT (bss_network_type_string.c_str ()),
      //            ACE_TEXT (Common_Tools::GUIDToString (interface_info_p->InterfaceGuid).c_str ()),
      //            ACE_TEXT_WCHAR_TO_TCHAR (interface_info_p->strInterfaceDescription),
      //            ACE_TEXT (interface_state_string.c_str ()),
      //            wlan_bss_entry_p->lRssi, wlan_bss_entry_p->uLinkQuality,
      //            (wlan_bss_entry_p->bInRegDomain ? ACE_TEXT ("true") : ACE_TEXT ("false")),
      //            wlan_bss_entry_p->usBeaconPeriod * 1024,
      //            wlan_bss_entry_p->ulChCenterFrequency));
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("[#%u] profile \"%s\"; SSID: \"%s\"; type: %s; connectable: \"%s\"%s; signal quality %d%% [RSSI: %d (dBm)]; security enabled: \"%s\"\n"),
      //            j + 1,
      //            (wlan_network_p->strProfileName ? ACE_TEXT_WCHAR_TO_TCHAR (wlan_network_p->strProfileName) : ACE_TEXT ("N/A")),
      //            ACE_TEXT (SSID_string.c_str ()),
      //            ACE_TEXT (bss_network_type_string.c_str ()),
      //            (wlan_network_p->bNetworkConnectable ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
      //            (wlan_network_p->bNetworkConnectable ? ACE_TEXT ("") : ACE_TEXT (" [reason]")),
      //            wlan_network_p->wlanSignalQuality, (-100 + static_cast<int> (static_cast<float> (wlan_network_p->wlanSignalQuality) * ::abs ((-100.0F - -50.0F) / 100.0F))),
      //            (wlan_network_p->bSecurityEnabled ? ACE_TEXT ("yes") : ACE_TEXT ("no"))));

      if (ACE_OS::memcmp (SSID_in.c_str (),
                          wlan_network_p->dot11Ssid.ucSSID,
                          //wlan_bss_entry_p->dot11Ssid.ucSSID,
                          SSID_in.size ()))
        continue;

      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("\"%s\": found SSID (was: \"%s\"), connecting...\n"),
                  ACE_TEXT_WCHAR_TO_TCHAR (interface_info_p->strInterfaceDescription),
                  ACE_TEXT (SSID_in.c_str ())));

      ACE_OS::memset (&wlan_connection_parameters_s,
                      0,
                      sizeof (struct _WLAN_CONNECTION_PARAMETERS));
      wlan_connection_parameters_s.dot11BssType = dot11_BSS_type_infrastructure;
      //wlan_connection_parameters_s.dwFlags = 0;
      //wlan_connection_parameters_s.pDesiredBssidList = NULL;
      wlan_connection_parameters_s.pDot11Ssid = &ssid_s;
      //wlan_connection_parameters_s.strProfile = NULL;
      wlan_connection_parameters_s.wlanConnectionMode =
        wlan_connection_mode_discovery_unsecure;
      result_2 = WlanConnect (handle_client,
                              &interface_info_p->InterfaceGuid,
                              &wlan_connection_parameters_s,
                              NULL);
      if (result_2 != ERROR_SUCCESS)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("\"%s\": failed to ::WlanConnect(\"%s\"): \"%s\", aborting\n"),
                    ACE_TEXT_WCHAR_TO_TCHAR (interface_info_p->strInterfaceDescription),
                    ACE_TEXT (SSID_in.c_str ()),
                    ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
        goto error;
      } // end IF
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("associated with SSID %s via WLAN interface %s\n"),
      //            ACE_TEXT (SSID_in.c_str ()),
      //            ACE_TEXT_WCHAR_TO_TCHAR (interface_info_p->strInterfaceDescription)));

      goto done;
    } // end FOR
    //WlanFreeMemory (wlan_bss_list_p);
    //wlan_bss_list_p = NULL;
    WlanFreeMemory (wlan_network_list_p);
    wlan_network_list_p = NULL;
  } // end FOR

  // SSID not found --> scan for networks ?
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("SSID (was: \"%s\") not found on WLAN interface \"%s\"%s\n"),
              ACE_TEXT (SSID_in.c_str ()),
              ACE_TEXT_WCHAR_TO_TCHAR (interface_info_p->strInterfaceDescription),
              (scanForNetworks_in ? ((retries == NET_PROTOCOL_WLAN_SCAN_RETRIES) ? ACE_TEXT (", giving up")
                                                                                 : ACE_TEXT (", scanning..."))
                                  : ACE_TEXT (", aborting"))));
  if (!scanForNetworks_in ||
      (retries == NET_PROTOCOL_WLAN_SCAN_RETRIES))
    goto error;
  ++retries;

  // *NOTE*: this function returns immediately
  result_2 = WlanScan (handle_client,
                       &interface_info_p->InterfaceGuid,
                       NULL, // *NOTE*: support WinXP
                       //&ssid_s,
                       raw_data_p,
                       NULL);
  if (result_2 != ERROR_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::WlanScan(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF
  // *NOTE*: on Win32 platforms, the network scan takes <=4 seconds ('Windows
  //         logo' requirement, see also:
  //         https://msdn.microsoft.com/en-us/library/windows/desktop/ms706783(v=vs.85).aspx)
  timeout = COMMON_TIME_NOW + wlan_ssid_scan_timeout;
  result_3 = condition.wait (&timeout);
  if (result_3 == -1)
  {
    int error = ACE_OS::last_error ();
    if (error != ETIME) // ETIME: 137
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Condition::wait(%#T): \"%m\", aborting\n"),
                  &wlan_ssid_scan_timeout));
    goto error;
  } // end IF
  goto check_networks;

done:
  // *NOTE*: that the DHCP transactions may not have completed at this stage, so
  //         the address information might not be up to date yet
  // *TODO*: sleep a meaningful amount of time here
  if (!Net_Common_Tools::interfaceToIPAddress (Common_Tools::GUIDToString (interface_info_p->InterfaceGuid),
                                               localSAP_out,
                                               peerSAP_out))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(\"%s\"), aborting\n"),
                ACE_TEXT_WCHAR_TO_TCHAR (interface_info_p->strInterfaceDescription)));
    goto error;
  } // end IF

  result = true;

error:
  if (interface_list_p)
    WlanFreeMemory (interface_list_p);
  //if (wlan_bss_list_p)
  //  WlanFreeMemory (wlan_bss_list_p);
  if (wlan_network_list_p)
    WlanFreeMemory (wlan_network_list_p);
  result_2 = WlanCloseHandle (handle_client,
                              NULL);
  if (result_2 != ERROR_SUCCESS)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::WlanCloseHandle(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));

  return result;
#else
  int socket_handle = -1;
  int result_2 = -1;
  struct iwreq iwreq_s;
  char essid[IW_ESSID_MAX_SIZE + 1];
  struct iw_range iw_range_s;
  struct wireless_scan_head wireless_scan_head_s;
  struct wireless_scan* wireless_scan_p = NULL;

  socket_handle = iw_sockets_open ();
  if (socket_handle == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to iw_sockets_open(): \"%m\", aborting\n")));
    goto clean;
  } // end IF

  ACE_OS::memset (&iwreq_s, 0, sizeof (struct iwreq));
  ACE_OS::memset (essid, 0, sizeof (IW_ESSID_MAX_SIZE + 1));
  iwreq_s.u.essid.pointer = essid;
  iwreq_s.u.essid.length = IW_ESSID_MAX_SIZE + 1;
  result_2 = iw_get_ext (socket_handle,
                         adapter_in.c_str (),
                         SIOCGIWESSID,
                         &iwreq_s);
  if (result_2 < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to iw_get_ext(%s,SIOCGIWESSID): \"%m\", aborting\n"),
                ACE_TEXT (adapter_in.c_str ())));
    goto clean;
  } // end IF
  if (!ACE_OS::memcmp (SSID_in.c_str (),
                       iwreq_s.u.essid.pointer,
                       iwreq_s.u.essid.length))
  { // --> already connected, nothing to do
    result = true;
    goto clean;
  } // end IF

  ACE_OS::memset (&iw_range_s, 0, sizeof (struct iw_range));
  result_2 = iw_get_range_info (socket_handle,
                                adapter_in.c_str (),
                                &iw_range_s);
  if (result_2 < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to iw_get_range_info(): \"%m\", aborting\n")));
    goto clean;
  } // end IF
  ACE_OS::memset (&wireless_scan_head_s, 0, sizeof (struct wireless_scan_head));

scan:
  result_2 = iw_scan (socket_handle,
                      const_cast<char*> (adapter_in.c_str ()),
                      iw_range_s.we_version_compiled,
                      &wireless_scan_head_s);
  if (result_2 < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to iw_scan(): \"%m\", aborting\n")));
    goto clean;
  } // end IF
  for (wireless_scan_p = wireless_scan_head_s.result;
       wireless_scan_p;
       wireless_scan_p = wireless_scan_p->next)
  {
    if (!wireless_scan_p->b.essid_on)
      continue;
    if (!ACE_OS::strncmp (SSID_in.c_str (),
                          wireless_scan_p->b.essid,
                          wireless_scan_p->b.essid_len))
    { // --> found SSID on adapter
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("\"%s\": found SSID (was: \"%s\"), connecting...\n"),
                  ACE_TEXT (adapter_in.c_str ()),
                  ACE_TEXT (SSID_in.c_str ())));

      result = true;
      goto clean;
    } // end IF
  } // end FOR
  // SSID not found --> scan for networks ?
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("SSID (was: \"%s\") not found on WLAN interface \"%s\"%s\n"),
              ACE_TEXT (SSID_in.c_str ()),
              ACE_TEXT (adapter_in.c_str ()),
              (scanForNetworks_in ? ((retries == NET_PROTOCOL_WLAN_SCAN_RETRIES) ? ACE_TEXT (", giving up")
                                                                                 : ACE_TEXT (", scanning..."))
                                  : ACE_TEXT (", aborting"))));
  if (!scanForNetworks_in ||
      (retries == NET_PROTOCOL_WLAN_SCAN_RETRIES))
    goto clean;
  ++retries;
  goto scan;

clean:
  iw_sockets_close (socket_handle);
//  if (result_2 == -1)
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to iw_sockets_close(): \"%m\", continuing\n")));

  return result;
#endif
}

bool
Net_Common_Tools::interfaceToIPAddress (const std::string& interfaceIdentifier_in,
                                        ACE_INET_Addr& IPAddress_out,
                                        ACE_INET_Addr& GatewayIPAddress_out)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::interfaceToIPAddress"));

  // initialize return value(s)
  IPAddress_out.reset ();
  GatewayIPAddress_out.reset ();

  std::string interface_identifier_string = interfaceIdentifier_in;
  if (interface_identifier_string.empty ())
    interface_identifier_string = Net_Common_Tools::getDefaultInterface ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ULONG flags = (GAA_FLAG_INCLUDE_PREFIX             |
                 GAA_FLAG_INCLUDE_WINS_INFO          |
                 GAA_FLAG_INCLUDE_GATEWAYS           |
                 GAA_FLAG_INCLUDE_ALL_INTERFACES     |
                 GAA_FLAG_INCLUDE_ALL_COMPARTMENTS   |
                 GAA_FLAG_INCLUDE_TUNNEL_BINDINGORDER);
  struct _IP_ADAPTER_ADDRESSES_LH* ip_adapter_addresses_p = NULL;
  ULONG buffer_length = 0;
  ULONG result =
    GetAdaptersAddresses (AF_UNSPEC,              // Family
                          flags,                  // Flags
                          NULL,                   // Reserved
                          ip_adapter_addresses_p, // AdapterAddresses
                          &buffer_length);        // SizePointer
  if (result != ERROR_BUFFER_OVERFLOW)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::GetAdaptersAddresses(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result).c_str ())));
    return false;
  } // end IF
  ACE_ASSERT (buffer_length);
  ip_adapter_addresses_p =
    static_cast<struct _IP_ADAPTER_ADDRESSES_LH*> (ACE_MALLOC_FUNC (buffer_length));
  if (!ip_adapter_addresses_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
    return false;
  } // end IF
  result =
    GetAdaptersAddresses (AF_UNSPEC,              // Family
                          flags,                  // Flags
                          NULL,                   // Reserved
                          ip_adapter_addresses_p, // AdapterAddresses
                          &buffer_length);        // SizePointer
  if (result != NO_ERROR)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::GetAdaptersAddresses(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result).c_str ())));

    // clean up
    ACE_FREE_FUNC (ip_adapter_addresses_p);

    return false;
  } // end IF

  struct _IP_ADAPTER_ADDRESSES_LH* ip_adapter_addresses_2 =
    ip_adapter_addresses_p;
  struct _IP_ADAPTER_UNICAST_ADDRESS_LH* unicast_address_p = NULL;
  struct _IP_ADAPTER_GATEWAY_ADDRESS_LH* gateway_address_p = NULL;
  struct _SOCKET_ADDRESS* socket_address_p = NULL;
  struct sockaddr_in* sockaddr_in_p = NULL;
  do
  {
    //ACE_DEBUG ((LM_DEBUG,
    //            ACE_TEXT ("found network interface: \"%s\"...\n"),
    //            ACE_TEXT (Net_Common_Tools::MACAddress2String (ip_adapter_info_2->Address).c_str ())));

//    if ((ip_adapter_addresses_2->OperStatus != IfOperStatusUp) ||
//        (!ip_adapter_addresses_2->FirstUnicastAddress))
//      continue;
    if (ACE_OS::strcmp (interface_identifier_string.c_str (),
                        ip_adapter_addresses_2->AdapterName))
      goto continue_;

    unicast_address_p = ip_adapter_addresses_2->FirstUnicastAddress;
    ACE_ASSERT (unicast_address_p);
    do
    {
      socket_address_p = &unicast_address_p->Address;
      ACE_ASSERT (socket_address_p->lpSockaddr);
      if (socket_address_p->lpSockaddr->sa_family == AF_INET)
        break;

      unicast_address_p = unicast_address_p->Next;
    } while (unicast_address_p);
    if (!unicast_address_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("adapter \"%s:\"\"%s\" does not currently have any unicast IPv4 address, aborting\n"),
                  ACE_TEXT_WCHAR_TO_TCHAR (ip_adapter_addresses_2->FriendlyName),
                  ACE_TEXT_WCHAR_TO_TCHAR (ip_adapter_addresses_2->Description)));

      // clean up
      ACE_FREE_FUNC (ip_adapter_addresses_p);

      return false;
    } // end IF
    sockaddr_in_p = (struct sockaddr_in*)socket_address_p->lpSockaddr;
    result = IPAddress_out.set (sockaddr_in_p,
                                socket_address_p->iSockaddrLength);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::set(): \"%m\", aborting\n")));

      // clean up
      ACE_FREE_FUNC (ip_adapter_addresses_p);

      return false;
    } // end IF

    gateway_address_p = ip_adapter_addresses_2->FirstGatewayAddress;
    if (!gateway_address_p)
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("adapter \"%s:\"\"%s\" does not currently have any gateway IPv4 address, continuing\n"),
                  ACE_TEXT_WCHAR_TO_TCHAR (ip_adapter_addresses_2->FriendlyName),
                  ACE_TEXT_WCHAR_TO_TCHAR (ip_adapter_addresses_2->Description)));
      break;
    } // end IF
    socket_address_p = &gateway_address_p->Address;
    sockaddr_in_p = (struct sockaddr_in*)socket_address_p->lpSockaddr;
    result = GatewayIPAddress_out.set (sockaddr_in_p,
                                       socket_address_p->iSockaddrLength);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::set(): \"%m\", aborting\n")));

      // clean up
      ACE_FREE_FUNC (ip_adapter_addresses_p);

      return false;
    } // end IF
    break;

continue_:
    ip_adapter_addresses_2 = ip_adapter_addresses_2->Next;
  } while (ip_adapter_addresses_2);

  // clean up
  ACE_FREE_FUNC (ip_adapter_addresses_p);
#else
#if defined (ACE_HAS_GETIFADDRS)
  struct ifaddrs* ifaddrs_p = NULL;
  int result = ::getifaddrs (&ifaddrs_p);
  if (result == -1)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ::getifaddrs(): \"%m\", aborting\n")));
    return false;
  } // end IF
  ACE_ASSERT (ifaddrs_p);

  struct sockaddr_in* sockaddr_in_p = NULL;
//  struct sockaddr_ll* sockaddr_ll_p = NULL;
  for (struct ifaddrs* ifaddrs_2 = ifaddrs_p;
       ifaddrs_2;
       ifaddrs_2 = ifaddrs_2->ifa_next)
  {
//    if (((ifaddrs_2->ifa_flags & IFF_UP) == 0) ||
//        (!ifaddrs_2->ifa_addr))
//      continue;
    if (ACE_OS::strcmp (interface_identifier_string.c_str (),
                        ifaddrs_2->ifa_name))
      continue;

    if (!ifaddrs_2->ifa_addr) continue;
    if (ifaddrs_2->ifa_addr->sa_family != AF_INET) continue;

    sockaddr_in_p = (struct sockaddr_in*)ifaddrs_2->ifa_addr;
    result = IPAddress_out.set (sockaddr_in_p,
                                sizeof (struct sockaddr_in));
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::set(): \"%m\", aborting\n")));

      // clean up
      ::freeifaddrs (ifaddrs_p);

      return false;
    } // end IF
    break;
  } // end FOR

  // clean up
  ::freeifaddrs (ifaddrs_p);
#else
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);
  ACE_NOTREACHED (return false;)
#endif /* ACE_HAS_GETIFADDRS */
#endif

//  result = IPAddress_out.addr_to_string (buffer,
//                                         sizeof (buffer),
//                                         1);
//  if (result == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", aborting\n")));
//    return false;
//  } // end IF
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("interface \"%s\" --> %s\n"),
//              ACE_TEXT (interfaceIdentifier_in.c_str ()),
//              buffer));

  return true;
}

bool
Net_Common_Tools::IPAddressToInterface (const ACE_INET_Addr& IPAddress_in,
                                        std::string& interfaceIdentifier_out)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::IPAddressToInterface"));

  // initialize return value(s)
  interfaceIdentifier_out.clear ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct _IP_ADAPTER_ADDRESSES_LH* ip_adapter_addresses_p = NULL;
  ULONG buffer_length = 0;
  ULONG result =
    GetAdaptersAddresses (AF_UNSPEC,              // Family
                          0,                      // Flags
                          NULL,                   // Reserved
                          ip_adapter_addresses_p, // AdapterAddresses
                          &buffer_length);        // SizePointer
  if (result != ERROR_BUFFER_OVERFLOW)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::GetAdaptersAddresses(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result).c_str ())));
    return false;
  } // end IF
  ACE_ASSERT (buffer_length);
  ip_adapter_addresses_p =
    static_cast<struct _IP_ADAPTER_ADDRESSES_LH*> (ACE_MALLOC_FUNC (buffer_length));
  if (!ip_adapter_addresses_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
    return false;
  } // end IF
  result =
    GetAdaptersAddresses (AF_UNSPEC,              // Family
                          0,                      // Flags
                          NULL,                   // Reserved
                          ip_adapter_addresses_p, // AdapterAddresses
                          &buffer_length);        // SizePointer
  if (result != NO_ERROR)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::GetAdaptersAddresses(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result).c_str ())));

    // clean up
    ACE_FREE_FUNC (ip_adapter_addresses_p);

    return false;
  } // end IF

  struct _IP_ADAPTER_ADDRESSES_LH* ip_adapter_addresses_2 =
    ip_adapter_addresses_p;
  struct _IP_ADAPTER_UNICAST_ADDRESS_LH* unicast_address_p = NULL;
  struct _SOCKET_ADDRESS* socket_address_p = NULL;
  struct sockaddr_in* sockaddr_in_p, *sockaddr_in_2 = NULL;
  ULONG network_mask = 0;
  sockaddr_in_2 = (struct sockaddr_in*)IPAddress_in.get_addr ();
  do
  {
    unicast_address_p = ip_adapter_addresses_2->FirstUnicastAddress;
    ACE_ASSERT (unicast_address_p);
    do
    {
      socket_address_p = &unicast_address_p->Address;
      ACE_ASSERT (socket_address_p->lpSockaddr);
      if (socket_address_p->lpSockaddr->sa_family == AF_INET)
        break;

      unicast_address_p = unicast_address_p->Next;
    } while (unicast_address_p);
    if (!unicast_address_p)
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("adapter \"%s:\"%s does not currently have any unicast IPv4 address, continuing\n"),
                  ACE_TEXT_WCHAR_TO_TCHAR (ip_adapter_addresses_2->FriendlyName),
                  ACE_TEXT_WCHAR_TO_TCHAR (ip_adapter_addresses_2->Description)));
      goto continue_;
    } // end IF

    // *NOTE*: this works for IPv4 addresses only
    ACE_ASSERT (unicast_address_p->OnLinkPrefixLength <= 32);
    network_mask = ~((1 << (32 - unicast_address_p->OnLinkPrefixLength)) - 1);
    sockaddr_in_p = (struct sockaddr_in*)socket_address_p->lpSockaddr;
    //ACE_DEBUG ((LM_DEBUG,
    //            ACE_TEXT ("found adapter \"%s\": \"%s\" on network %s...\n"),
    //            ACE_TEXT_WCHAR_TO_TCHAR (ip_adapter_addresses_2->FriendlyName),
    //            ACE_TEXT_WCHAR_TO_TCHAR (ip_adapter_addresses_2->Description),
    //            ACE_TEXT (Net_Common_Tools::IPAddressToString (ACE_INET_Addr (static_cast<u_short> (0),
    //                                                                         static_cast<ACE_UINT32> ((ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN) ? ACE_SWAP_LONG (sockaddr_in_p->sin_addr.S_un.S_addr) & network_mask
    //                                                                                                                                        : sockaddr_in_p->sin_addr.S_un.S_addr & network_mask)),
    //                                                          true).c_str ())));

    if (((ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN) ? ACE_SWAP_LONG (sockaddr_in_p->sin_addr.S_un.S_addr) & network_mask
                                               : sockaddr_in_p->sin_addr.S_un.S_addr & network_mask) !=
        ((ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN) ? ACE_SWAP_LONG (sockaddr_in_2->sin_addr.S_un.S_addr) & network_mask
                                               : sockaddr_in_2->sin_addr.S_un.S_addr & network_mask))
      goto continue_;

    interfaceIdentifier_out = ip_adapter_addresses_2->AdapterName;

    break;

continue_:
    ip_adapter_addresses_2 = ip_adapter_addresses_2->Next;
  } while (ip_adapter_addresses_2);

  // clean up
  ACE_FREE_FUNC (ip_adapter_addresses_p);
#else
#if defined (ACE_HAS_GETIFADDRS)
  struct ifaddrs* ifaddrs_p = NULL;
  int result = ::getifaddrs (&ifaddrs_p);
  if (result == -1)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ::getifaddrs(): \"%m\", aborting\n")));
    return false;
  } // end IF
  ACE_ASSERT (ifaddrs_p);

  struct sockaddr_in* sockaddr_in_p =
      static_cast<struct sockaddr_in*> (IPAddress_in.get_addr ());
//  struct sockaddr_ll* sockaddr_ll_p = NULL;
  struct sockaddr_in* sockaddr_in_2 = NULL;
  struct sockaddr_in* sockaddr_in_3 = NULL;
  in_addr_t inet_address_network = 0;
  for (struct ifaddrs* ifaddrs_2 = ifaddrs_p;
       ifaddrs_2;
       ifaddrs_2 = ifaddrs_2->ifa_next)
  {
    if (!ifaddrs_2->ifa_addr)
      continue;
    if (ifaddrs_2->ifa_addr->sa_family != AF_INET)
      continue;

    sockaddr_in_2 = reinterpret_cast<struct sockaddr_in*> (ifaddrs_2->ifa_addr);
    sockaddr_in_3 =
        reinterpret_cast<struct sockaddr_in*> (ifaddrs_2->ifa_netmask);
    inet_address_network =
        sockaddr_in_2->sin_addr.s_addr & sockaddr_in_3->sin_addr.s_addr;
    if (inet_address_network !=
        (sockaddr_in_p->sin_addr.s_addr & sockaddr_in_3->sin_addr.s_addr))
      continue;

    interfaceIdentifier_out = ifaddrs_2->ifa_name;
    break;
  } // end FOR

  // clean up
  ::freeifaddrs (ifaddrs_p);
#else
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);
  ACE_NOTREACHED (return false;)
#endif /* ACE_HAS_GETIFADDRS */
#endif

  return (!interfaceIdentifier_out.empty ());
}

std::string
Net_Common_Tools::getDefaultInterface (enum Net_LinkLayerType type_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::getDefaultInterface"));

  std::string result;

  switch (type_in)
  {
    case NET_LINKLAYER_802_3:
    case NET_LINKLAYER_PPP:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      struct _IP_ADAPTER_ADDRESSES_LH* ip_adapter_addresses_p = NULL;
      ULONG buffer_length = 0;
      ULONG result_2 =
        GetAdaptersAddresses (AF_UNSPEC,              // Family
                              0,                      // Flags
                              NULL,                   // Reserved
                              ip_adapter_addresses_p, // AdapterAddresses
                              &buffer_length);        // SizePointer
      if (result_2 != ERROR_BUFFER_OVERFLOW)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ::GetAdaptersAddresses(): \"%s\", aborting\n"),
                    ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
        return result;
      } // end IF
      ACE_ASSERT (buffer_length);
      ip_adapter_addresses_p =
        static_cast<struct _IP_ADAPTER_ADDRESSES_LH*> (ACE_MALLOC_FUNC (buffer_length));
      if (!ip_adapter_addresses_p)
      {
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
        return result;
      } // end IF
      result_2 =
        GetAdaptersAddresses (AF_UNSPEC,              // Family
                              0,                      // Flags
                              NULL,                   // Reserved
                              ip_adapter_addresses_p, // AdapterAddresses
                              &buffer_length);        // SizePointer
      if (result_2 != NO_ERROR)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ::GetAdaptersAddresses(): \"%s\", aborting\n"),
                    ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));

        // clean up
        ACE_FREE_FUNC (ip_adapter_addresses_p);

        return result;
      } // end IF

      // step1: retrieve 'connected' interfaces
      std::map<ULONG, std::string> connected_interfaces;
      struct _IP_ADAPTER_ADDRESSES_LH* ip_adapter_addresses_2 =
        ip_adapter_addresses_p;
      do
      {
        if (type_in == NET_LINKLAYER_802_3)
        {
          if ((ip_adapter_addresses_2->IfType != IF_TYPE_ETHERNET_CSMACD) ||
              (ip_adapter_addresses_2->IfType != IF_TYPE_IS088023_CSMACD))
            goto continue_;
        } // end IF
        else if (ip_adapter_addresses_2->IfType != IF_TYPE_PPP)
          goto continue_;
        if (ip_adapter_addresses_2->OperStatus == IfOperStatusUp)
          connected_interfaces.insert (std::make_pair (ip_adapter_addresses_2->Ipv4Metric,
                                                       ip_adapter_addresses_2->AdapterName));

        if (ip_adapter_addresses_2->FirstUnicastAddress)
        {
          // debug info
          ACE_INET_Addr inet_address, gateway_address;
          if (!Net_Common_Tools::interfaceToIPAddress (ip_adapter_addresses_2->AdapterName,
                                                       inet_address,
                                                       gateway_address))
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(\"%s\"), aborting\n"),
                        ACE_TEXT_WCHAR_TO_TCHAR (ip_adapter_addresses_2->FriendlyName)));
            return result;
          } // end IF
          if (type_in == NET_LINKLAYER_802_3)
          {
            ACE_ASSERT (ip_adapter_addresses_2->PhysicalAddressLength >= ETH_ALEN);
          } // end IF
          ACE_DEBUG ((LM_DEBUG,
                      ACE_TEXT ("found network interface \"%s\"[%s]: IP#: %s; MAC#: %s...\n"),
                      ACE_TEXT_WCHAR_TO_TCHAR (ip_adapter_addresses_2->FriendlyName),
                      ACE_TEXT (ip_adapter_addresses_2->AdapterName),
                      ACE_TEXT (Net_Common_Tools::IPAddressToString (inet_address).c_str ()),
                      ACE_TEXT (Net_Common_Tools::LinkLayerAddressToString (ip_adapter_addresses_2->PhysicalAddress,
                                                                            type_in).c_str ())));
        } // end IF

continue_:
        ip_adapter_addresses_2 = ip_adapter_addresses_2->Next;
      } while (ip_adapter_addresses_2);

      // clean up
      ACE_FREE_FUNC (ip_adapter_addresses_p);

      if (connected_interfaces.empty ())
        return result;

      result = connected_interfaces.begin ()->second;
#else
      // *TODO*: this should work on most Unixy systems, but is a really bad
      //         idea:
      //         - relies on local 'ip'
      //         - temporary files
      //         - system(3) call
      //         --> extremely inefficient; remove ASAP
      std::string filename_string =
          Common_File_Tools::getTempFilename (ACE_TEXT_ALWAYS_CHAR (""));
      std::string command_line_string = ACE_TEXT_ALWAYS_CHAR ("ip route >> ");
      command_line_string += filename_string;

      int result_2 = ACE_OS::system (ACE_TEXT (command_line_string.c_str ()));
    //  result = execl ("/bin/sh", "sh", "-c", command, (char *) 0);
      if ((result_2 == -1)      ||
          !WIFEXITED (result_2) ||
          WEXITSTATUS (result_2))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::system(\"%s\"): \"%m\" (result was: %d), aborting\n"),
                    ACE_TEXT (command_line_string.c_str ()),
                    WEXITSTATUS (result_2)));
        return result;
      } // end IF
      unsigned char* data_p = NULL;
      if (!Common_File_Tools::load (filename_string,
                                    data_p))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_File_Tools::load(\"%s\"): \"%m\", aborting\n"),
                    ACE_TEXT (filename_string.c_str ())));
        return result;
      } // end IF
      if (!Common_File_Tools::deleteFile (filename_string))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_File_Tools::deleteFile(\"%s\"), continuing\n"),
                    ACE_TEXT (filename_string.c_str ())));

      std::string route_record_string = reinterpret_cast<char*> (data_p);
      delete [] data_p;
    //  ACE_DEBUG ((LM_DEBUG,
    //              ACE_TEXT ("ip data: \"%s\"\n"),
    //              ACE_TEXT (route_record_string.c_str ())));

      std::istringstream converter;
      char buffer [BUFSIZ];
      std::string regex_string =
          ACE_TEXT_ALWAYS_CHAR ("^default via ([[:digit:].]+) dev ([[:alnum:]]+)(?:.*)$");
      std::regex regex (regex_string);
      std::smatch match_results;
      converter.str (route_record_string);
      do {
        converter.getline (buffer, sizeof (buffer));
        if (!std::regex_match (std::string (buffer),
                               match_results,
                               regex,
                               std::regex_constants::match_default))
          continue;
        ACE_ASSERT (match_results.ready () && !match_results.empty ());

        ACE_ASSERT (match_results[1].matched);
        ACE_ASSERT (match_results[2].matched);
        result = match_results[2];

        break;
      } while (!converter.fail ());
      if (result.empty ())
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to retrieve default interface from route data (was: \"%s\"), aborting\n"),
                    ACE_TEXT (route_record_string.c_str ())));
        return result;
      } // end IF

      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("default interface: \"%s\" (gateway: %s)\n"),
                  ACE_TEXT (result.c_str ()),
                  ACE_TEXT (match_results[1].str ().c_str ())));
#endif
      break;
    }
    case NET_LINKLAYER_ATM:
    case NET_LINKLAYER_FDDI:
    case NET_LINKLAYER_802_11:
    {
      ACE_ASSERT (false);
      ACE_NOTSUP_RETURN (result);

      ACE_NOTREACHED (break;)
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown link layer type (was: %d), aborting\n"),
                  type_in));
      break;
    }
  } // end SWITCH

  return result;
}
std::string
Net_Common_Tools::getDefaultInterface (int linkLayerType_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::getDefaultInterface"));

  std::string result;

  // step1: retrieve 'default' device for each link layer type specified
  std::vector<std::string> interfaces;
  std::string interface_identifier;
  for (enum Net_LinkLayerType i = NET_LINKLAYER_ATM;
       i < NET_LINKLAYER_MAX;
       ++i)
    if (linkLayerType_in & i)
    {
      interface_identifier = Net_Common_Tools::getDefaultInterface (i);
      if (interface_identifier.empty ())
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Net_Common_Tools::getDefaultInterface() (type was: \"%s\"), continuing\n"),
                    ACE_TEXT (Net_Common_Tools::LinkLayerTypeToString (i).c_str ())));
        continue;
      } // end IF
      interfaces.push_back (interface_identifier);
    } // end IF

  if (!interfaces.empty ())
    result = interfaces.front ();

  return result;
}

bool
Net_Common_Tools::getAddress (std::string& hostName_inout,
                              std::string& dottedDecimal_inout)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::getAddress"));

//  struct hostent host, *result_p;
//  ACE_OS::memset (&host, 0, sizeof (struct hostent));
//  ACE_HOSTENT_DATA buffer;
//  ACE_OS::memset (buffer, 0, sizeof (ACE_HOSTENT_DATA));
//  int error = 0;

  int result = -1;
  ACE_INET_Addr inet_address;
  ACE_TCHAR buffer[HOST_NAME_MAX];
  ACE_OS::memset (buffer, 0, sizeof (buffer));

  if (hostName_inout.empty ())
  {
    // sanity check
    if (dottedDecimal_inout.empty ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid arguments (hostname/address empty), aborting\n")));
      return false;
    } // end IF

    result = inet_address.set (dottedDecimal_inout.c_str (), AF_INET);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::set(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (dottedDecimal_inout.c_str ())));
      return false;
    } // end IF

    result = inet_address.get_host_name (buffer, sizeof (buffer));
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::get_host_name(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (dottedDecimal_inout.c_str ())));
      return false;
    } // end IF

    dottedDecimal_inout = buffer;
  } // end IF
  else
  {
    result = inet_address.set (0,
                               hostName_inout.c_str (),
                               1,
                               AF_INET);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::set(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (hostName_inout.c_str ())));
      return false;
    } // end IF

    const char* result_p = inet_address.get_host_addr (buffer, sizeof (buffer));
    if (!result_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::get_host_addr(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (hostName_inout.c_str ())));
      return false;
    } // end IF

    dottedDecimal_inout = buffer;
  } // end ELSE
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("\"%s\" <--> %s\n"),
  //            ACE_TEXT (hostName_inout.c_str ()),
  //            ACE_TEXT (dottedDecimal_inout.c_str ())));

  return true;
}

bool
Net_Common_Tools::getHostname (std::string& hostname_out)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::getHostname"));

  hostname_out = Common_Tools::getHostName ();

  return !hostname_out.empty ();
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
bool
Net_Common_Tools::setPathMTUDiscovery (ACE_HANDLE handle_in,
                                       int option_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::setPathMTUDiscovery"));

  // sanity check(s)
  ACE_ASSERT (handle_in != ACE_INVALID_HANDLE);

  int result = -1;

  int optval = option_in;
  int optlen = sizeof (optval);
  result = ACE_OS::setsockopt (handle_in,
                               IPPROTO_IP,
                               IP_MTU_DISCOVER,
                               reinterpret_cast<const char*> (&optval),
                               optlen);
  if (result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::setsockopt(%d,%s,%d): \"%m\", aborting\n"),
                handle_in,
                ACE_TEXT ("IP_MTU_DISCOVER"),
                option_in));
    return false;
  } // end IF

  return true;
}

bool
Net_Common_Tools::getPathMTU (const ACE_INET_Addr& destinationAddress_in,
                              unsigned int& MTU_out)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::getPathMTU"));

  // initialize return value(s)
  bool result = false;
  MTU_out = 0;

  // sanity check(s)
  ACE_ASSERT (destinationAddress_in.get_addr_size () ==
              sizeof (struct sockaddr));

  ACE_HANDLE socket_handle = ACE_OS::socket (ACE_ADDRESS_FAMILY_INET,
                                             SOCK_DGRAM,
                                             0);
  if (socket_handle == ACE_INVALID_HANDLE)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::socket(%d,%d,%d): \"%m\", aborting\n"),
                ACE_ADDRESS_FAMILY_INET, SOCK_DGRAM, 0));
    return false;
  } // end IF

  ACE_TCHAR buffer[BUFSIZ];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  int result_2 = destinationAddress_in.addr_to_string (buffer,
                                                       sizeof (buffer),
                                                       1);
  if (result_2 == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));

  bool do_disassociate = false;
  struct sockaddr* sockaddr_p =
      reinterpret_cast<struct sockaddr*> (destinationAddress_in.get_addr ());
  result_2 = ACE_OS::connect (socket_handle,
                              sockaddr_p,
                              destinationAddress_in.get_addr_size ());
  if (result_2 == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::connect(%d,%s): \"%m\", aborting\n"),
                socket_handle,
                buffer));
    return false;
  } // end IF
  do_disassociate = true;

  // *NOTE*: IP_MTU works only on connect()ed sockets (see man 7 ip)
  int optval = 0;
  int optlen = sizeof (optval);
  result = ACE_OS::getsockopt (socket_handle,
                               IPPROTO_IP,
                               IP_MTU,
                               reinterpret_cast<char*> (&optval),
                               &optlen);
  if (result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(%d, %s): \"%m\", aborting\n"),
                socket_handle,
                ACE_TEXT ("IP_MTU")));
    goto close;
  } // end IF
  result = true;

  MTU_out = optval;

close:
  if (do_disassociate)
  {
    // *NOTE*: see man connect(2)
    struct sockaddr socket_address = *sockaddr_p;
    socket_address.sa_family = AF_UNSPEC;
    result_2 = ACE_OS::connect (socket_handle,
                                &socket_address,
                                sizeof (socket_address));
    if (result_2 == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::connect(%d,%s): \"%m\", continuing\n"),
                  socket_handle,
                  buffer));
  } // end IF

  return result;
}
#endif

unsigned int
Net_Common_Tools::getMTU (ACE_HANDLE handle_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::getMTU"));

  int result = -1;

  int optval = 0;
  int optlen = sizeof (optval);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE_OS::getsockopt (handle_in,
                               SOL_SOCKET,
                               SO_MAX_MSG_SIZE,
                               reinterpret_cast<char*> (&optval),
                               &optlen);
  if (result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(0x%@,SO_MAX_MSG_SIZE): \"%m\", aborting\n"),
                handle_in));
    return 0;
  } // end IF
#else
  // *NOTE*: IP_MTU works only on connect()ed sockets (see man 7 ip)
  result = ACE_OS::getsockopt (handle_in,
                               IPPROTO_IP,
                               IP_MTU,
                               reinterpret_cast<char*> (&optval),
                               &optlen);
  if (result)
  {
    // *PORTABILITY*
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(0x%@,IP_MTU): \"%m\", aborting\n"),
                handle_in));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(%d,IP_MTU): \"%m\", aborting\n"),
                handle_in));
#endif
    return 0;
  } // end IF
#endif

  return static_cast<unsigned int> (optval);
}

bool
Net_Common_Tools::setSocketBuffer (ACE_HANDLE handle_in,
                                   int optname_in,
                                   int size_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::setSocketBuffer"));

  // sanity check(s)
  if ((optname_in != SO_RCVBUF) &&
      (optname_in != SO_SNDBUF))
  {
    // *PORTABILITY*
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("0x%@: invalid socket option (was: %d), aborting\n"),
                handle_in,
                optname_in));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%d: invalid socket option (was: %d), aborting\n"),
                handle_in,
                optname_in));
#endif
    return false;
  } // end IF

  int result = -1;

  int optval = size_in;
  int optlen = sizeof (optval);
  result = ACE_OS::setsockopt (handle_in,
                               SOL_SOCKET,
                               optname_in,
                               reinterpret_cast<const char*> (&optval),
                               optlen);
  if (result)
  {
    // *PORTABILITY*
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::setsockopt(0x%@,%s): \"%m\", aborting\n"),
                handle_in,
                ((optname_in == SO_SNDBUF) ? ACE_TEXT ("SO_SNDBUF")
                                           : ACE_TEXT ("SO_RCVBUF"))));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::setsockopt(%d,%s): \"%m\", aborting\n"),
                handle_in,
                ((optname_in == SO_SNDBUF) ? ACE_TEXT ("SO_SNDBUF")
                                           : ACE_TEXT ("SO_RCVBUF"))));
#endif
    return false;
  } // end IF

  // validate result
  optval = 0;
  result = ACE_OS::getsockopt (handle_in,
                               SOL_SOCKET,
                               optname_in,
                               reinterpret_cast<char*> (&optval),
                               &optlen);
  if (result)
  {
    // *PORTABILITY*
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(0x%@,%s): \"%m\", aborting\n"),
                handle_in,
                ((optname_in == SO_SNDBUF) ? ACE_TEXT ("SO_SNDBUF")
                                           : ACE_TEXT ("SO_RCVBUF"))));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(%d,%s): \"%m\", aborting\n"),
                handle_in,
                ((optname_in == SO_SNDBUF) ? ACE_TEXT ("SO_SNDBUF")
                                           : ACE_TEXT ("SO_RCVBUF"))));
#endif
    return false;
  } // end IF

  if (optval != size_in)
  {
    // *NOTE*: for some reason, Linux will actually set TWICE the size value
    if (Common_Tools::isLinux () && (optval == (size_in * 2)))
      return true;

    // *PORTABILITY*
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("ACE_OS::getsockopt(0x%@,%s) returned %d (expected: %d), aborting\n"),
                handle_in,
                ((optname_in == SO_SNDBUF) ? ACE_TEXT ("SO_SNDBUF")
                                           : ACE_TEXT ("SO_RCVBUF")),
                optval,
                size_in));
#else
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("ACE_OS::getsockopt(%d,%s) returned %d (expected: %d), aborting\n"),
                handle_in,
                ((optname_in == SO_SNDBUF) ? ACE_TEXT ("SO_SNDBUF")
                                           : ACE_TEXT ("SO_RCVBUF")),
                optval,
                size_in));
#endif
    return false;
  } // end IF

  //ACE_DEBUG((LM_DEBUG,
  //           ACE_TEXT("set \"%s\" option of socket %d to: %d\n"),
  //           ((optname_in == SO_RCVBUF) ? ACE_TEXT("SO_RCVBUF")
  //                                      : ACE_TEXT("SO_SNDBUF")),
  //           handle_in,
  //           size_in));

  return true;
}

bool
Net_Common_Tools::getNoDelay (ACE_HANDLE handle_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::getNoDelay"));

  int result = -1;
  int optval = 0;
  int optlen = sizeof (optval);
  result = ACE_OS::getsockopt (handle_in,
                               IPPROTO_TCP,
                               TCP_NODELAY,
                               reinterpret_cast<char*> (&optval),
                               &optlen);
  if (result)
  {
    // *PORTABILITY*
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(0x%@,TCP_NODELAY): \"%m\", aborting\n"),
                handle_in));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(%d,TCP_NODELAY): \"%m\", aborting\n"),
                handle_in));
#endif
    return false;
  } // end IF

  return (optval == 1);
}

bool
Net_Common_Tools::setNoDelay (ACE_HANDLE handle_in,
                              bool noDelay_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::setNoDelay"));

  int result = -1;
  int optval = (noDelay_in ? 1 : 0);
  int optlen = sizeof (optval);
  result = ACE_OS::setsockopt (handle_in,
                               IPPROTO_TCP,
                               TCP_NODELAY,
                               reinterpret_cast<const char*> (&optval),
                               optlen);
  if (result)
  {
    // *PORTABILITY*
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::setsockopt(0x%@,TCP_NODELAY): \"%m\", aborting\n"),
                handle_in));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::setsockopt(%d,TCP_NODELAY): \"%m\", aborting\n"),
                handle_in));
#endif
    return false;
  } // end IF

  // validate result
  optval = 0;
  result = ACE_OS::getsockopt (handle_in,
                               IPPROTO_TCP,
                               TCP_NODELAY,
                               reinterpret_cast<char*> (&optval),
                               &optlen);
  if (result)
  {
    // *PORTABILITY*
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(0x%@,TCP_NODELAY): \"%m\", aborting\n"),
                handle_in));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(%d,TCP_NODELAY): \"%m\", aborting\n"),
                handle_in));
#endif
    return false;
  } // end IF

  //ACE_DEBUG((LM_DEBUG,
  //           ACE_TEXT("setsockopt(%d,TCP_NODELAY): %s\n"),
  //           handle_in,
  //           (noDelay_in ? ((optval == 1) ? "on" : "off")
  //                       : ((optval == 0) ? "off" : "on"))));

  return (noDelay_in ? (optval == 1) : (optval == 0));
}

bool
Net_Common_Tools::setKeepAlive (ACE_HANDLE handle_in,
                                bool on_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::setKeepAlive"));

  int result = -1;
  int optval = (on_in ? 1 : 0);
  int optlen = sizeof (optval);
  result = ACE_OS::getsockopt (handle_in,
                               SOL_SOCKET,
                               SO_KEEPALIVE,
                               reinterpret_cast<char*> (&optval),
                               &optlen);
  if (result)
  {
    // *PORTABILITY*
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(0x%@,SO_KEEPALIVE): \"%m\", aborting\n"),
                handle_in));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(%d,SO_KEEPALIVE): \"%m\", aborting\n"),
                handle_in));
#endif
    return false;
  } // end IF

  optval = (on_in ? 1 : 0);
  result = ACE_OS::setsockopt (handle_in,
                               SOL_SOCKET,
                               SO_KEEPALIVE,
                               reinterpret_cast<const char*> (&optval),
                               optlen);
  if (result)
  {
    // *PORTABILITY*
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::setsockopt(0x%@,SO_KEEPALIVE): \"%m\", aborting\n"),
                handle_in));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::setsockopt(%d,SO_KEEPALIVE): \"%m\", aborting\n"),
                handle_in));
#endif
    return false;
  } // end IF
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("setsockopt(%d,SO_KEEPALIVE): %s\n"),
//              handle_in,
//              (on_in ? ACE_TEXT ("on") : ACE_TEXT ("off"))));

  return (result == 0);
}

bool
Net_Common_Tools::setLinger (ACE_HANDLE handle_in,
                             bool on_in,
                             unsigned short seconds_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::setLinger"));

  int result = -1;
  struct linger optval;
  ACE_OS::memset (&optval, 0, sizeof (optval));
  int optlen = sizeof (optval);
  result = ACE_OS::getsockopt (handle_in,
                               SOL_SOCKET,
                               SO_LINGER,
                               reinterpret_cast<char*> (&optval),
                               &optlen);
  if (result)
  {
    // *PORTABILITY*
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(0x%@,SO_LINGER): \"%m\", aborting\n"),
                handle_in));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(%d,SO_LINGER): \"%m\", aborting\n"),
                handle_in));
#endif
    return false;
  } // end IF

  optval.l_onoff = static_cast<unsigned short> (on_in ? 1 : 0);
  if (seconds_in)
    optval.l_linger = seconds_in;
  result = ACE_OS::setsockopt (handle_in,
                               SOL_SOCKET,
                               SO_LINGER,
                               reinterpret_cast<const char*> (&optval),
                               optlen);
  if (result)
  {
    // *PORTABILITY*
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::setsockopt(0x%@,SO_LINGER): \"%m\", aborting\n"),
                handle_in));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::setsockopt(%d,SO_LINGER): \"%m\", aborting\n"),
                handle_in));
#endif
    return false;
  } // end IF
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("setsockopt(%d,SO_LINGER): %s (%d second(s))\n"),
//              handle_in,
//              (optval.l_onoff ? ACE_TEXT ("on") : ACE_TEXT ("off")),
//              optval.l_linger));

  return (result == 0);
}

bool
Net_Common_Tools::setReuseAddress (ACE_HANDLE handle_in,
                                   bool reusePort_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::setReuseAddress"));

  int result = -1;
  int optval = 1;
  int optlen = sizeof (optval);
  result = ACE_OS::setsockopt (handle_in,
                               SOL_SOCKET,
                               SO_REUSEADDR,
                               reinterpret_cast<const char*> (&optval),
                               optlen);
  if (result)
  {
    // *PORTABILITY*
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::setsockopt(0x%@,SO_REUSEADDR): \"%m\", aborting\n"),
                handle_in));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::setsockopt(%d,SO_REUSEADDR): \"%m\", aborting\n"),
                handle_in));
#endif
    return false;
  } // end IF

  if (reusePort_in)
  {
    result = ACE_OS::setsockopt (handle_in,
                                 SOL_SOCKET,
                                 SO_REUSEPORT,
                                 reinterpret_cast<const char*> (&optval),
                                 optlen);
    if (result)
    {
      // *PORTABILITY*
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::setsockopt(0x%@,SO_REUSEPORT): \"%m\", aborting\n"),
                  handle_in));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::setsockopt(%d,SO_REUSEPORT): \"%m\", aborting\n"),
                  handle_in));
#endif
      return false;
    } // end IF
  } // end IF

  // validate result
  optval = 0;
  result = ACE_OS::getsockopt (handle_in,
                               SOL_SOCKET,
                               SO_REUSEADDR,
                               reinterpret_cast<char*> (&optval),
                               &optlen);
  if (result)
  {
    // *PORTABILITY*
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(0x%@,SO_REUSEADDR): \"%m\", aborting\n"),
                handle_in));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(%d,SO_REUSEADDR): \"%m\", aborting\n"),
                handle_in));
#endif
    return false;
  } // end IF

  //ACE_DEBUG((LM_DEBUG,
  //           ACE_TEXT("setsockopt(%d,SO_REUSEADDR): %s\n"),
  //           handle_in,
  //           (keepAlive_in ? ((optval == 1) ? "on" : "off")
  //                         : ((optval == 0) ? "off" : "on"))));

  return (optval == 1);
}

#if defined (ACE_LINUX)
bool
Net_Common_Tools::enableErrorQueue (ACE_HANDLE handle_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::enableErrorQueue"));

  int result = -1;

  int optval = 1;
  int optlen = sizeof (optval);
  result = ACE_OS::setsockopt (handle_in,
                               IPPROTO_IP,
                               IP_RECVERR,
                               reinterpret_cast<char*> (&optval),
                               optlen);
  if (result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::setsockopt(%d,IP_RECVERR): \"%m\", aborting\n"),
                handle_in));
    return false;
  } // end IF

  return true;
}
#endif

int
Net_Common_Tools::getProtocol (ACE_HANDLE handle_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::getProtocol"));

  int result = -1;
  int optval = 0;
  int optlen = sizeof (optval);
  result = ACE_OS::getsockopt (handle_in,
                               SOL_SOCKET,
                               SO_TYPE, // SO_STYLE
                               reinterpret_cast<char*> (&optval),
                               &optlen);
  if (result)
  {
    // *PORTABILITY*
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(0x%@,SO_TYPE): \"%m\", aborting\n"),
                handle_in));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(%d,SO_TYPE): \"%m\", aborting\n"),
                handle_in));
#endif
    return -1;
  } // end IF

  return optval;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
bool
Net_Common_Tools::setLoopBackFastPath (ACE_HANDLE handle_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::setLoopBackFastPath"));

  // *NOTE*: works for TCP connections only (see:
  // https://msdn.microsoft.com/en-us/library/windows/desktop/jj841212(v=vs.85).aspx)
  // sanity check(s)
  ACE_ASSERT (Net_Common_Tools::getProtocol (handle_in) == SOCK_STREAM);

  int optval = 1; // --> enable
  unsigned long number_of_bytes_returned = 0;
  int result =
    ACE_OS::ioctl (handle_in,                 // socket handle
                   SIO_LOOPBACK_FAST_PATH,    // control code
                   &optval,                   // input buffer
                   sizeof (optval),           // size of input buffer
                   NULL,                      // output buffer
                   0,                         // size of output buffer
                   &number_of_bytes_returned, // #bytes returned
                   NULL, NULL);               // overlapped / function
  if (result == SOCKET_ERROR)
  {
    DWORD error = ::GetLastError ();
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::ioctl(0x%@,SIO_LOOPBACK_FAST_PATH): \"%s\", aborting\n"),
                handle_in,
                ACE_TEXT (Common_Tools::errorToString (error).c_str ())));
    return false;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("enabled SIO_LOOPBACK_FAST_PATH on 0x%@\n"),
              handle_in));

  return true;
}
#endif

//Net_IInetConnectionManager_t*
//Net_Common_Tools::getConnectionManager ()
//{
//  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::getConnectionManager"));

//  return NET_CONNECTIONMANAGER_SINGLETON::instance ();
//}

std::string
Net_Common_Tools::URLToHostName (const std::string& URL_in,
                                 bool returnProtocol_in,
                                 bool returnPort_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::URLToHostName"));

  std::string result;

  std::string regex_string =
      ACE_TEXT_ALWAYS_CHAR ("^([[:alpha:]]+://)?([^\\/\\:]+)(\\:[[:digit:]]{1,5})?(.+)?$");
  std::regex regex (regex_string);
  std::smatch match_results;
  if (!std::regex_match (URL_in,
                         match_results,
                         regex,
                         std::regex_constants::match_default))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid URL string (was: \"%s\"), aborting\n"),
                ACE_TEXT (URL_in.c_str ())));
    return result;
  } // end IF
  ACE_ASSERT (match_results.ready () && !match_results.empty ());
  if (!match_results[2].matched)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid URL string (was: \"%s\"), aborting\n"),
                ACE_TEXT (URL_in.c_str ())));
    return result;
  } // end IF

  if (returnProtocol_in &&
      match_results[1].matched)
    result = match_results[1];
  result += match_results[2];
  if (returnPort_in &&
      match_results[3].matched)
    result += match_results[3];

  return result;
}
