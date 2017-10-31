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
#include <wlanapi.h>
#else
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netinet/ether.h>
#include "ifaddrs.h"
#include "iwlib.h"
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
operator++ (enum Net_LinkLayerType& lhs_inout) // prefix-
{ ACE_ASSERT (lhs_inout < NET_LINKLAYER_MAX);
  int result = lhs_inout + 1;
  if (unlikely (result == NET_LINKLAYER_MAX))
    lhs_inout = NET_LINKLAYER_ATM;
  else
    lhs_inout = static_cast<enum Net_LinkLayerType> (result);

  return lhs_inout;
}
enum Net_LinkLayerType
operator++ (enum Net_LinkLayerType& lhs_in, int) // postfix-
{
  enum Net_LinkLayerType result = lhs_in;
  ++lhs_in;

  return result;
}

//////////////////////////////////////////

//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//void WINAPI
//network_wlan_notification_cb (PWLAN_NOTIFICATION_DATA data_in,
//                              PVOID context_in)
//{
//  NETWORK_TRACE (ACE_TEXT ("network_wlan_notification_cb"));
//
//  // sanity check(s)
//  ACE_ASSERT (data_in);
//  ACE_ASSERT (context_in);
//
//  ACE_SYNCH_CONDITION* condition_p =
//    static_cast<ACE_SYNCH_CONDITION*> (context_in);
//
//  std::string notification_string;
//  switch (data_in->NotificationSource)
//  {
//    case WLAN_NOTIFICATION_SOURCE_ACM:
//    {
//      switch (data_in->NotificationCode)
//      {
//        case wlan_notification_acm_autoconf_enabled:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_autoconf_enabled");
//          break;
//        case wlan_notification_acm_autoconf_disabled:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_autoconf_disabled");
//          break;
//        case wlan_notification_acm_background_scan_enabled:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_background_scan_enabled");
//          break;
//        case wlan_notification_acm_background_scan_disabled:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_background_scan_disabled");
//          break;
//        case wlan_notification_acm_bss_type_change:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_bss_type_change");
//          break;
//        case wlan_notification_acm_power_setting_change:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_power_setting_change");
//          break;
//        case wlan_notification_acm_scan_complete:
//        {
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_scan_complete");
//          int result = condition_p->broadcast ();
//          if (result == -1)
//            ACE_DEBUG ((LM_ERROR,
//                        ACE_TEXT ("failed to ACE_Condition::broadcast(): \"%m\"\n")));
//          break;
//        }
//        case wlan_notification_acm_scan_fail:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_scan_fail");
//          break;
//        case wlan_notification_acm_connection_start:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_connection_start");
//          break;
//        case wlan_notification_acm_connection_complete:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_connection_complete");
//          break;
//        case wlan_notification_acm_connection_attempt_fail:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_connection_attempt_fail");
//          break;
//        case wlan_notification_acm_filter_list_change:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_filter_list_change");
//          break;
//        case wlan_notification_acm_interface_arrival:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_interface_arrival");
//          break;
//        case wlan_notification_acm_interface_removal:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_interface_removal");
//          break;
//        case wlan_notification_acm_profile_change:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_profile_change");
//          break;
//        case wlan_notification_acm_profile_name_change:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_profile_name_change");
//          break;
//        case wlan_notification_acm_profiles_exhausted:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_profiles_exhausted");
//          break;
//        case wlan_notification_acm_network_not_available:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_network_not_available");
//          break;
//        case wlan_notification_acm_network_available:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_network_available");
//          break;
//        case wlan_notification_acm_disconnecting:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_disconnecting");
//          break;
//        case wlan_notification_acm_disconnected:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_disconnected");
//          break;
//        case wlan_notification_acm_adhoc_network_state_change:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_adhoc_network_state_change");
//          break;
//        case wlan_notification_acm_profile_unblocked:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_profile_unblocked");
//          break;
//        case wlan_notification_acm_screen_power_change:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_screen_power_change");
//          break;
//        case wlan_notification_acm_profile_blocked:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_profile_blocked");
//          break;
//        case wlan_notification_acm_scan_list_refresh:
//          notification_string =
//            ACE_TEXT_ALWAYS_CHAR ("wlan_notification_acm_scan_list_refresh");
//          break;
//        default:
//        {
//          ACE_DEBUG ((LM_ERROR,
//                      ACE_TEXT ("invalid/unknown notification code (was: %d), returning\n"),
//                      data_in->NotificationCode));
//          return;
//        }
//      } // end SWITCH
//
//      break;
//    }
//    case WLAN_NOTIFICATION_SOURCE_MSM:
//    case WLAN_NOTIFICATION_SOURCE_SECURITY:
//    case WLAN_NOTIFICATION_SOURCE_IHV:
//    case WLAN_NOTIFICATION_SOURCE_HNWK:
//    case WLAN_NOTIFICATION_SOURCE_ONEX:
//      break;
//    default:
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("invalid/unknown notification source (was: %d), returning\n"),
//                  data_in->NotificationSource));
//      return;
//    }
//  } // end SWITCH
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("\"%s\": received notification \"%s\"\n"),
//              ACE_TEXT (Net_Common_Tools::interfaceToString (data_in->InterfaceGuid).c_str ()),
//              ACE_TEXT (notification_string.c_str ())));
//}
//#endif
//
////////////////////////////////////////////
//
//#if defined (ACE_HAS_NETLINK)
//std::string
//Net_Common_Tools::NetlinkAddressToString (const Net_Netlink_Addr& NetlinkAddress_in)
//{
//  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::NetlinkAddressToString"));
//
//  // initialize return value(s)
//  std::string return_value;
//
//  int result = -1;
//  ACE_TCHAR buffer[BUFSIZ];
//  ACE_OS::memset (&buffer, 0, sizeof (buffer));
//  result = NetlinkAddress_in.addr_to_string (buffer,
//                                             sizeof (buffer),
//                                             1); // N/A
//  if (result == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Net_Netlink_Addr::addr_to_string(): \"%m\", aborting\n")));
//    return return_value;
//  } // end IF
//
//  // copy string from buffer
//  return_value = buffer;
//
//  return return_value;
//}
//#endif

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
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::set(): \"%m\", aborting\n")));
    return return_value;
  } // end IF
  result = inet_addr.addr_to_string (buffer,
                                     sizeof (buffer),
                                     1); // want IP address, not hostname !
  if (unlikely (result == -1))
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
    if (likely (last_colon_pos != std::string::npos))
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
  if (unlikely (!std::regex_match (address_in,
                                   match_results,
                                   regex,
                                   std::regex_constants::match_default)))
    return false;
  ACE_ASSERT (match_results.ready () && !match_results.empty ());

  // validate all groups
  std::stringstream converter;
  std::string group_string;
  unsigned int group = 0;
  std::string::size_type start_position;
  for (unsigned int i = 1; i < match_results.size (); ++i)
  {
    if (unlikely (!match_results[i].matched))
      return false;

    group_string = match_results[i].str ();
    if (likely (i > 1))
      group_string.erase (0, 1);
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter.clear ();
    converter << group_string;
    converter >> group;
    if (unlikely (group > 255))
      return false; // refuse groups > 255

    start_position = group_string.find ('0', 0);
    if (unlikely ((start_position == 0) &&
                  (group_string.size () > 1)))
      return false; // refuse leading 0s
  } // end FOR

  return true;
}

bool
Net_Common_Tools::isLocal (const ACE_INET_Addr& address_in)
{
  NETWORK_TRACE ("Net_Common_Tools::isLocal");

  // sanity check(s)
  if (unlikely (address_in.is_any () ||
                address_in.is_loopback ()))
    return true;

  // retrieve all assigned local addresses
  ACE_INET_Addr local_ip_address;
  //ACE_INET_Addr network_address;
  std::vector<ACE_INET_Addr> local_ip_addresses_a;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  //ULONG flags = (GAA_FLAG_INCLUDE_PREFIX             |
  //               GAA_FLAG_INCLUDE_WINS_INFO          |
  //               GAA_FLAG_INCLUDE_GATEWAYS           |
  //               GAA_FLAG_INCLUDE_ALL_INTERFACES     |
  //               GAA_FLAG_INCLUDE_ALL_COMPARTMENTS   |
  //               GAA_FLAG_INCLUDE_TUNNEL_BINDINGORDER);
  //struct _IP_ADAPTER_ADDRESSES_LH* ip_adapter_addresses_p = NULL;
  //ULONG buffer_length = 0;
  //ULONG result =
  //  GetAdaptersAddresses (AF_UNSPEC,              // Family
  //                        flags,                  // Flags
  //                        NULL,                   // Reserved
  //                        ip_adapter_addresses_p, // AdapterAddresses
  //                        &buffer_length);        // SizePointer
  //if (result != ERROR_BUFFER_OVERFLOW)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to ::GetAdaptersAddresses(): \"%s\", aborting\n"),
  //              ACE_TEXT (Common_Tools::errorToString (result).c_str ())));
  //  return false;
  //} // end IF
  //ACE_ASSERT (buffer_length);
  //ip_adapter_addresses_p =
  //  static_cast<struct _IP_ADAPTER_ADDRESSES_LH*> (ACE_MALLOC_FUNC (buffer_length));
  //if (!ip_adapter_addresses_p)
  //{
  //  ACE_DEBUG ((LM_CRITICAL,
  //              ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
  //  return false;
  //} // end IF
  //result =
  //  GetAdaptersAddresses (AF_UNSPEC,              // Family
  //                        flags,                  // Flags
  //                        NULL,                   // Reserved
  //                        ip_adapter_addresses_p, // AdapterAddresses
  //                        &buffer_length);        // SizePointer
  //if (result != NO_ERROR)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to ::GetAdaptersAddresses(): \"%s\", aborting\n"),
  //              ACE_TEXT (Common_Tools::errorToString (result).c_str ())));

  //  // clean up
  //  ACE_FREE_FUNC (ip_adapter_addresses_p);

  //  return false;
  //} // end IF

  //struct _IP_ADAPTER_ADDRESSES_LH* ip_adapter_addresses_2 =
  //  ip_adapter_addresses_p;
  //struct _IP_ADAPTER_UNICAST_ADDRESS_LH* unicast_address_p = NULL;
  //struct _IP_ADAPTER_GATEWAY_ADDRESS_LH* gateway_address_p = NULL;
  //struct _SOCKET_ADDRESS* socket_address_p = NULL;
  //struct sockaddr_in* sockaddr_in_p = NULL;
  //do
  //{
  //  unicast_address_p = ip_adapter_addresses_2->FirstUnicastAddress;
  //  ACE_ASSERT (unicast_address_p);
  //  do
  //  {
  //    socket_address_p = &unicast_address_p->Address;
  //    ACE_ASSERT (socket_address_p->lpSockaddr);
  //    if (socket_address_p->lpSockaddr->sa_family != AF_INET)
  //      continue;

  //    sockaddr_in_p = (struct sockaddr_in*)socket_address_p->lpSockaddr;
  //    netmask_mask = std::numeric_limits<ACE_UINT32>::max ();
  //    netmask_mask = netmask_mask << ((4 * 8) - unicast_address_p->OnLinkPrefixLength);
  //    netmask = sockaddr_in_p->sin_addr.S_un.S_addr & netmask_mask;
  //    adapter_ip_netmasks_a.push_back (netmask);

  //    result = network_address.set (static_cast<u_short> (0),
  //                                  netmask,
  //                                  1,
  //                                  0);
  //    if (result == -1)
  //    {
  //      ACE_DEBUG ((LM_ERROR,
  //                  ACE_TEXT ("failed to ACE_INET_Addr::set(0,%u): \"%m\", aborting\n"),
  //                  netmask));

  //      // clean up
  //      ACE_FREE_FUNC (ip_adapter_addresses_p);

  //      return false;
  //    } // end IF
  //    ACE_DEBUG ((LM_DEBUG,
  //                ACE_TEXT ("found ip address on network %s\n"),
  //                ACE_TEXT (Net_Common_Tools::IPAddressToString (network_address).c_str ())));

  //    unicast_address_p = unicast_address_p->Next;
  //  } while (unicast_address_p);
  //  ip_adapter_addresses_2 = ip_adapter_addresses_2->Next;
  //} while (ip_adapter_addresses_2);

  //// clean up
  //ACE_FREE_FUNC (ip_adapter_addresses_p);

  struct _MIB_IPADDRTABLE* table_p = NULL;
  DWORD table_size = 0;
  DWORD result = 0;
  //IN_ADDR ip_address;

  table_p =
    static_cast<struct _MIB_IPADDRTABLE*> (HeapAlloc (GetProcessHeap (),
                                                      0,
                                                      sizeof (struct _MIB_IPADDRTABLE)));
  if (unlikely (!table_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::HeapAlloc(%u): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (::GetLastError ()).c_str ())));
    return false; // *TODO*: this could lead to false negatives
  } // end IF
  // step1: determine size of the table
  result = GetIpAddrTable (table_p, &table_size, FALSE);
  if (unlikely (!table_p ||
                (result != ERROR_INSUFFICIENT_BUFFER)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::GetIpAddrTable(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (::GetLastError ()).c_str ())));

    HeapFree (GetProcessHeap (), 0, table_p);

    return false; // *TODO*: this could lead to false negatives
  } // end IF
  HeapFree (GetProcessHeap (), 0, table_p);
  table_p = NULL;
  table_p =
    static_cast<struct _MIB_IPADDRTABLE*> (HeapAlloc (GetProcessHeap (),
                                                      0,
                                                      table_size));
  if (unlikely (!table_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::HeapAlloc(%u): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (::GetLastError ()).c_str ())));
    return false; // *TODO*: this could lead to false negatives
  } // end IF
  // step2: get the actual table data
  result = GetIpAddrTable (table_p, &table_size, FALSE);
  if (unlikely (result != NO_ERROR))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::GetIpAddrTable(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result).c_str ())));

    HeapFree (GetProcessHeap (), 0, table_p);

    return false; // *TODO*: this could lead to false negatives
  } // end IF

  for (DWORD i = 0;
       i < table_p->dwNumEntries;
       ++i)
  {
    //printf("\tType and State[%d]:", i);
    //if (pIPAddrTable->table[i].wType & MIB_IPADDR_PRIMARY)
    //    printf("\tPrimary IP Address");
    //if (pIPAddrTable->table[i].wType & MIB_IPADDR_DYNAMIC)
    //    printf("\tDynamic IP Address");
    //if (pIPAddrTable->table[i].wType & MIB_IPADDR_DISCONNECTED)
    //    printf("\tAddress is on disconnected interface");
    //if (pIPAddrTable->table[i].wType & MIB_IPADDR_DELETED)
    //    printf("\tAddress is being deleted");
    //if (pIPAddrTable->table[i].wType & MIB_IPADDR_TRANSIENT)
    //    printf("\tTransient address");

    result =
      local_ip_address.set (static_cast<u_short> (0),
                            table_p->table[i].dwAddr,
                            0, // already in network byte order
                            0);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::set(0,%u): \"%m\", aborting\n"),
                  table_p->table[i].dwAddr));

      // clean up
      HeapFree (GetProcessHeap (), 0, table_p);

      return false;
    } // end IF
    //ACE_DEBUG ((LM_DEBUG,
    //            ACE_TEXT ("found ip address %s on network %s\n"),
    //            ACE_TEXT (ACE_OS::inet_ntoa (ip_address)),
    //            ACE_TEXT (Net_Common_Tools::IPAddressToString (network_address, true).c_str ())));

    local_ip_addresses_a.push_back (local_ip_address);
  } // end FOR

  HeapFree (GetProcessHeap (), 0, table_p);
#else
#if defined (ACE_HAS_GETIFADDRS)
  struct ifaddrs* ifaddrs_p = NULL;
  struct in_addr in_addr_s;
  int result = ::getifaddrs (&ifaddrs_p);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ::getifaddrs(): \"%m\", aborting\n")));
    return false;
  } // end IF
  ACE_ASSERT (ifaddrs_p);

  struct sockaddr_in* sockaddr_in_p, *sockaddr_in_2 = NULL;
  for (struct ifaddrs* ifaddrs_2 = ifaddrs_p;
       ifaddrs_2;
       ifaddrs_2 = ifaddrs_2->ifa_next)
  {
    if (unlikely (!ifaddrs_2->ifa_addr))
      continue;
    if (unlikely (ifaddrs_2->ifa_addr->sa_family != AF_INET))
      continue;

    sockaddr_in_p = reinterpret_cast<struct sockaddr_in*> (ifaddrs_2->ifa_addr);
    sockaddr_in_2 =
        reinterpret_cast<struct sockaddr_in*> (ifaddrs_2->ifa_netmask);
    in_addr_s = sockaddr_in_p->sin_addr;
    in_addr_s.s_addr &= sockaddr_in_2->sin_addr.s_addr;
    sockaddr_in_2 =
        reinterpret_cast<struct sockaddr_in*> (ifaddrs_2->ifa_netmask);
    result =
      local_ip_address.set (static_cast<u_short> (0),
                            sockaddr_in_p->sin_addr.s_addr,
                            0, // already in network byte order
                            0);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::set(0,%u): \"%m\", aborting\n"),
                  sockaddr_in_p->sin_addr.s_addr));
      return false;
    } // end IF
    //ACE_DEBUG ((LM_DEBUG,
    //            ACE_TEXT ("found ip address %s on network %s\n"),
    //            ACE_TEXT (ACE_OS::inet_ntoa (in_addr_s)),
    //            ACE_TEXT (Net_Common_Tools::IPAddressToString (local_ip_address, true).c_str ())));

    local_ip_addresses_a.push_back (local_ip_address);
  } // end FOR

  // clean up
  ::freeifaddrs (ifaddrs_p);
#else
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);

  ACE_NOTREACHED (return false;)
#endif /* ACE_HAS_GETIFADDRS */
#endif

  for (std::vector<ACE_INET_Addr>::const_iterator iterator = local_ip_addresses_a.begin ();
       iterator != local_ip_addresses_a.end ();
       ++iterator)
    if (address_in.get_ip_address () == (*iterator).get_ip_address ())
      return true;

  return false;
}

ACE_INET_Addr
Net_Common_Tools::stringToIPAddress (std::string& address_in)
{
  NETWORK_TRACE ("Net_Common_Tools::stringToIPAddress");

  // sanity check(s)
  // *NOTE*: ACE_INET_Addr::string_to_address() needs a trailing port number to
  //         function properly (see: ace/INET_Addr.h:237)
  //         --> append one if necessary
  std::string ip_address_string = address_in;
  std::string::size_type position = ip_address_string.find (':', 0);
  if (likely (position == std::string::npos))
    ip_address_string += ACE_TEXT_ALWAYS_CHAR (":0");

  int result = -1;
  ACE_INET_Addr inet_address;
  result = inet_address.string_to_addr (ip_address_string.c_str (),
                                        AF_INET);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::string_to_addr(): \"%m\", aborting\n")));
    return ACE_INET_Addr ();
  } // end IF

  return inet_address;
}

std::string
Net_Common_Tools::IPProtocolToString (unsigned char protocol_in)
{
  NETWORK_TRACE ("Net_Common_Tools::IPProtocolToString");

  switch (protocol_in)
  {
    case IPPROTO_IP: // OR case IPPROTO_HOPOPTS:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_IP/IPPROTO_HOPOPTS");
    case IPPROTO_ICMP:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_ICMP");
    case IPPROTO_IGMP:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_IGMP");
    case IPPROTO_IPIP:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_IPIP");
    case IPPROTO_TCP:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_TCP");
    case IPPROTO_EGP:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_EGP");
    case IPPROTO_PUP:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_PUP");
    case IPPROTO_UDP:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_UDP");
    case IPPROTO_IDP:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_IDP");
    case IPPROTO_TP:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_TP");
    case IPPROTO_IPV6:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_IPV6");
    case IPPROTO_ROUTING:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_ROUTING");
    case IPPROTO_FRAGMENT:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_FRAGMENT");
    case IPPROTO_RSVP:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_RSVP");
    case IPPROTO_GRE:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_GRE");
    case IPPROTO_ESP:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_ESP");
    case IPPROTO_AH:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_AH");
    case IPPROTO_ICMPV6:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_ICMPV6");
    case IPPROTO_NONE:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_NONE");
    case IPPROTO_DSTOPTS:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_DSTOPTS");
    case IPPROTO_MTP:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_MTP");
    case IPPROTO_ENCAP:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_ENCAP");
    case IPPROTO_PIM:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_PIM");
    case IPPROTO_COMP:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_COMP");
    case IPPROTO_SCTP:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_SCTP");
    case IPPROTO_RAW:
      return ACE_TEXT_ALWAYS_CHAR ("IPPROTO_RAW");
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown IP protocol (was: %1u), aborting\n"),
                  protocol_in));
      break;
    }
  } // end SWITCH

  return ACE_TEXT_ALWAYS_CHAR ("");
}

std::string
Net_Common_Tools::TransportLayerTypeToString (enum Net_TransportLayerType type_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::TransportLayerTypeToString"));

  switch (type_in)
  {
    case NET_TRANSPORTLAYER_IP_CAST:
      return ACE_TEXT_ALWAYS_CHAR ("IP");
#if defined (ACE_LINUX)
    case NET_TRANSPORTLAYER_NETLINK:
      return ACE_TEXT_ALWAYS_CHAR ("Netlink");
#endif
    case NET_TRANSPORTLAYER_TCP:
      return ACE_TEXT_ALWAYS_CHAR ("TCP");
    case NET_TRANSPORTLAYER_UDP:
      return ACE_TEXT_ALWAYS_CHAR ("UDP");
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown transport layer type (was: %d), aborting\n"),
                  type_in));
      break;
    }
  } // end SWITCH

  return ACE_TEXT_ALWAYS_CHAR ("");
}

std::string
Net_Common_Tools::LinkLayerAddressToString (const unsigned char* const addressDataPtr_in,
                                            enum Net_LinkLayerType type_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::LinkLayerAddressToString"));

  switch (type_in)
  {
    case NET_LINKLAYER_802_3:
    case NET_LINKLAYER_802_11:
    {
      ACE_TCHAR buffer[NET_ADDRESS_LINK_ETHERNET_ADDRESS_STRING_SIZE];
      ACE_OS::memset (&buffer, 0, sizeof (buffer));

      // *IMPORTANT NOTE*: ether_ntoa_r is not portable
      // *TODO*: ether_ntoa_r is not portable...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (ACE_USES_WCHAR)
      PWSTR result_2 =
        RtlEthernetAddressToStringW (reinterpret_cast<const _DL_EUI48* const> (addressDataPtr_in),
                                     buffer);
#else
      PSTR result_2 =
        RtlEthernetAddressToStringA (reinterpret_cast<const _DL_EUI48* const> (addressDataPtr_in),
                                     buffer);
#endif
      if (unlikely (result_2 != (buffer + NET_ADDRESS_LINK_ETHERNET_ADDRESS_STRING_SIZE - 1)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ::RtlEthernetAddressToString(), aborting\n")));
        break;
      } // end IF
#else
      if (unlikely (::ether_ntoa_r (reinterpret_cast<const ether_addr*> (addressDataPtr_in),
                                    buffer) != buffer))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ::ether_ntoa_r(): \"%m\", aborting\n")));
        break;
      } // end IF
#endif
      return ACE_TEXT_ALWAYS_CHAR (buffer);
    }
    case NET_LINKLAYER_PPP:
    {
      // *NOTE*: being point-to-point in nature, PPP does not support (link
      //         layer-) addressing
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("link layer type \"%s\" does not support addressing, aborting\n"),
                  ACE_TEXT (Net_Common_Tools::LinkLayerTypeToString (type_in).c_str ())));
      break;
    }
    case NET_LINKLAYER_ATM:
    case NET_LINKLAYER_FDDI:
    {
      ACE_ASSERT (false);
      ACE_NOTSUP_RETURN (ACE_TEXT_ALWAYS_CHAR (""));

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

  return ACE_TEXT_ALWAYS_CHAR ("");
}
std::string
Net_Common_Tools::LinkLayerTypeToString (enum Net_LinkLayerType type_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::LinkLayerTypeToString"));

  switch (type_in)
  {
    case NET_LINKLAYER_802_3:
      return ACE_TEXT_ALWAYS_CHAR ("IEEE 802.3");
    case NET_LINKLAYER_802_11:
      return ACE_TEXT_ALWAYS_CHAR ("IEEE 802.11");
    case NET_LINKLAYER_ATM:
      return ACE_TEXT_ALWAYS_CHAR ("ATM");
    case NET_LINKLAYER_FDDI:
      return ACE_TEXT_ALWAYS_CHAR ("FDDI");
    case NET_LINKLAYER_PPP:
      return ACE_TEXT_ALWAYS_CHAR ("PPP");
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown link layer type (was: %d), aborting\n"),
                  type_in));
      break;
    }
  } // end SWITCH

  return ACE_TEXT_ALWAYS_CHAR ("");
}

std::string
Net_Common_Tools::EthernetProtocolTypeIdToString (unsigned short frameType_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::EthernetProtocolTypeIdToString"));

  switch (ACE_NTOHS (frameType_in))
  {
    case ETH_P_LOOP:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_LOOP");
    case ETHERTYPE_GRE_ISO:
      return ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_GRE_ISO");
    case ETH_P_PUP:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_PUP");
    case ETH_P_PUPAT:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_PUPAT");
    case ETHERTYPE_SPRITE:
      return ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_SPRITE");
    case ETH_P_IP:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_IP");
    case ETH_P_X25:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_X25");
    case ETH_P_ARP:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_ARP");
    case ETH_P_BPQ:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_BPQ");
    case ETH_P_IEEEPUP:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_IEEEPUP");
    case ETH_P_IEEEPUPAT:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_IEEEPUPAT");
    case ETHERTYPE_NS:
      return ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_NS");
    case ETHERTYPE_TRAIL:
      return ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_TRAIL");
    case ETH_P_DEC:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_DEC");
    case ETH_P_DNA_DL:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_DNA_DL");
    case ETH_P_DNA_RC:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_DNA_RC");
    case ETH_P_DNA_RT:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_DNA_RT");
    case ETH_P_LAT:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_LAT");
    case ETH_P_DIAG:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_DIAG");
    case ETH_P_CUST:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_CUST");
    case ETH_P_SCA:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_SCA");
    case ETH_P_RARP:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_RARP");
    case ETHERTYPE_LANBRIDGE:
      return ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_LANBRIDGE");
    case ETHERTYPE_DECDNS:
      return ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_DECDNS");
    case ETHERTYPE_DECDTS:
      return ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_DECDTS");
    case ETHERTYPE_VEXP:
      return ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_VEXP");
    case ETHERTYPE_VPROD:
      return ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_VPROD");
    case ETH_P_ATALK:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_ATALK");
    case ETH_P_AARP:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_AARP");
    case ETH_P_8021Q:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_8021Q");
    case ETH_P_IPX:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_IPX");
    case ETH_P_IPV6:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_IPV6");
    case ETHERTYPE_MPCP:
      return ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_MPCP");
    case ETHERTYPE_SLOW:
      return ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_SLOW");
    case ETHERTYPE_PPP:
      return ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_PPP");
    case ETH_P_WCCP:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_WCCP");
    case ETH_P_MPLS_UC:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_MPLS_UC");
    case ETH_P_MPLS_MC:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_MPLS_MC");
    case ETH_P_ATMMPOA:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_ATMMPOA");
    case ETH_P_PPP_DISC:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_PPP_DISC");
    case ETH_P_PPP_SES:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_PPP_SES");
    case ETHERTYPE_JUMBO:
      return ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_JUMBO");
    case ETH_P_ATMFATE:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_ATMFATE");
    case ETHERTYPE_EAPOL:
      return ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_EAPOL");
    case ETH_P_AOE:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_AOE");
    case ETH_P_TIPC:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_TIPC");
    case ETHERTYPE_LOOPBACK:
      return ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_LOOPBACK");
    case ETHERTYPE_VMAN:
      return ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_VMAN");
    case ETHERTYPE_ISO:
      return ACE_TEXT_ALWAYS_CHAR ("ETHERTYPE_ISO");
// ********************* non-DIX types ***********************
    case ETH_P_802_3:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_802_3");
    case ETH_P_AX25:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_AX25");
    case ETH_P_ALL:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_ALL");
    case ETH_P_802_2:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_802_2");
    case ETH_P_SNAP:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_SNAP");
    case ETH_P_DDCMP:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_DDCMP");
    case ETH_P_WAN_PPP:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_WAN_PPP");
    case ETH_P_PPP_MP:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_PPP_MP");
    case ETH_P_LOCALTALK:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_LOCALTALK");
    case ETH_P_PPPTALK:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_PPPTALK");
    case ETH_P_TR_802_2:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_TR_802_2");
    case ETH_P_MOBITEX:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_MOBITEX");
    case ETH_P_CONTROL:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_CONTROL");
    case ETH_P_IRDA:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_IRDA");
    case ETH_P_ECONET:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_ECONET");
    case ETH_P_HDLC:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_HDLC");
    case ETH_P_ARCNET:
      return ACE_TEXT_ALWAYS_CHAR ("ETH_P_ARCNET");
    default:
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("invalid/unknown ethernet frame type (was: 0x%x), continuing\n"),
                  ACE_NTOHS (frameType_in)));

      // IEEE 802.3 ? --> adjust result string
      if (likely (ACE_NTOHS (frameType_in) <= ETH_DATA_LEN))
        return ACE_TEXT_ALWAYS_CHAR ("UNKNOWN_IEEE_802_3_FRAME_TYPE");

      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown ethernet frame type (was: 0x%x), aborting\n"),
                  ACE_NTOHS (frameType_in)));

      break;
    }
  } // end SWITCH

  return ACE_TEXT_ALWAYS_CHAR ("");
}

bool
#if defined (ACE_WIN32) || defined (ACE_WIN64)
Net_Common_Tools::interfaceToExternalIPAddress (REFGUID interfaceIdentifier_in,
#else
Net_Common_Tools::interfaceToExternalIPAddress (const std::string& interfaceIdentifier_in,
#endif
                                                ACE_INET_Addr& IPAddress_out)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::interfaceToExternalIPAddress"));

  // initialize return value(s)
  IPAddress_out.reset ();

  // *TODO*: this implementation is broken; it does not consider the specific
  //         interface, but returns the external IP of the interface that
  //         happens to route the DNS resolution query (see below)

  // debug info
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct _GUID interface_identifier = interfaceIdentifier_in;
  if (InlineIsEqualGUID (interface_identifier, GUID_NULL))
#else
  std::string interface_identifier = interfaceIdentifier_in;
  if (interface_identifier.empty ())
#endif
    interface_identifier = Net_Common_Tools::getDefaultInterface ();

  // step1: determine the 'internal' IP address
  ACE_INET_Addr internal_ip_address, gateway_ip_address;
  if (unlikely (!Net_Common_Tools::interfaceToIPAddress (interface_identifier,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
                                                         NULL,
#endif
                                                         internal_ip_address,
                                                         gateway_ip_address)))
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(\"%s\"), aborting\n"),
                ACE_TEXT (Net_Common_Tools::interfaceToString (interface_identifier).c_str ())));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(\"%s\"), aborting\n"),
                ACE_TEXT (interface_identifier.c_str ())));
#endif
    return false;
  } // end IF

  int result = -1;
  // *TODO*: this should work on most Linux/Windows systems, but is really a bad
  //         idea, as it relies on:
  //         - local 'nslookup[.exe]' tool
  //         - the 'opendns.com' domain resolution service
  //         - local temporary files
  //         - system(3) call
  //         and is totally inefficient
  //         --> remove ASAP
  std::string filename_string =
      Common_File_Tools::getTempFilename (ACE_TEXT_ALWAYS_CHAR (""));
  std::string command_line_string =
      ACE_TEXT_ALWAYS_CHAR ("nslookup myip.opendns.com. resolver1.opendns.com >> ");
  command_line_string += filename_string;

  result = ACE_OS::system (ACE_TEXT (command_line_string.c_str ()));
//  result = execl ("/bin/sh", "sh", "-c", command, (char *) 0);
  if (unlikely ((result == -1)      ||
                !WIFEXITED (result) ||
                WEXITSTATUS (result)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::system(\"%s\"): \"%m\" (result was: %d), aborting\n"),
                ACE_TEXT (command_line_string.c_str ()),
                WEXITSTATUS (result)));
    return false;
  } // end IF
  unsigned char* data_p = NULL;
  if (unlikely (!Common_File_Tools::load (filename_string,
                                          data_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::load(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (filename_string.c_str ())));
    return false;
  } // end IF
  if (unlikely (!Common_File_Tools::deleteFile (filename_string)))
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
        !ACE_OS::strcmp (match_results[1].str ().c_str (),
                         ACE_TEXT_ALWAYS_CHAR (NET_ADDRESS_NSLOOKUP_RESULT_ADDRESS_KEY_STRING)))
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
  if (unlikely (external_ip_address.empty ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to resolve IP address (was: %s), aborting\n"),
                ACE_TEXT (Net_Common_Tools::IPAddressToString (internal_ip_address).c_str ())));
    return false;
  } // end IF
  IPAddress_out = Net_Common_Tools::stringToIPAddress (external_ip_address);

  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("interface \"%s\" --> %s (--> %s)\n"),
  //            ACE_TEXT (interfaceIdentifier_in.c_str ()),
  //            ACE_TEXT (Net_Common_Tools::IPAddressToString (internal_ip_address).c_str ()),
  //            ACE_TEXT (Net_Common_Tools::IPAddressToString (IPAddress_out).c_str ())));

  return true;
}

bool
#if defined (ACE_WIN32) || defined (ACE_WIN64)
Net_Common_Tools::interfaceToMACAddress (REFGUID interfaceIdentifier_in,
#else
Net_Common_Tools::interfaceToMACAddress (const std::string& interfaceIdentifier_in,
#endif
                                         unsigned char MACAddress_out[])
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::interfaceToMACAddress"));

//  // sanity check(s)
//  ACE_ASSERT (sizeof (MACAddress_out) >= 6);

  // initialize return value(s)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  //  ACE_OS::memset (MACAddress_out, 0, sizeof (MACAddress_out));
    ACE_OS::memset (MACAddress_out, 0, ETH_ALEN);

  // sanity check(s)
  ACE_ASSERT (!InlineIsEqualGUID (interfaceIdentifier_in, GUID_NULL));
  //ACE_ASSERT (sizeof (MACAddress_out) >= MAX_ADAPTER_ADDRESS_LENGTH);

  NET_IFINDEX interface_index = 0;
  struct _IP_interfaceIdentifier_inFO* ip_interfaceIdentifier_info_p = NULL;
  struct _IP_interfaceIdentifier_inFO* ip_interfaceIdentifier_info_2 = NULL;
  ULONG buffer_length = 0;
  ULONG result = 0;

  interface_index =
    Net_Common_Tools::interfaceToIndex (interfaceIdentifier_in);
  if (unlikely (!interface_index))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::interfaceToIndex(%s), aborting\n"),
                ACE_TEXT (Common_Tools::GUIDToString (interfaceIdentifier_in).c_str ())));
    return false;
  } // end IF

  result = GetAdaptersInfo (ip_interfaceIdentifier_info_p, &buffer_length);
  if (unlikely (result != ERROR_BUFFER_OVERFLOW))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::GetAdaptersInfo(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result).c_str ())));
    return false;
  } // end IF
  ACE_ASSERT (buffer_length);
  ip_interfaceIdentifier_info_p =
    static_cast<struct _IP_interfaceIdentifier_inFO*> (ACE_MALLOC_FUNC (buffer_length));
  if (unlikely (!ip_interfaceIdentifier_info_p))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
    return false;
  } // end IF
  result = GetAdaptersInfo (ip_interfaceIdentifier_info_p,
                            &buffer_length);
  if (unlikely (result != NO_ERROR))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::GetAdaptersInfo(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result).c_str ())));

    // clean up
    ACE_FREE_FUNC (ip_interfaceIdentifier_info_p);

    return false;
  } // end IF

  ip_interfaceIdentifier_info_2 = ip_interfaceIdentifier_info_p;
  do
  {
    if (static_cast<DWORD> (interface_index) != ip_interfaceIdentifier_info_2->Index)
      goto continue_;

    ACE_OS::memcpy (MACAddress_out, ip_interfaceIdentifier_info_2->Address,
                    ETH_ALEN);
    break;

continue_:
    ip_interfaceIdentifier_info_2 = ip_interfaceIdentifier_info_2->Next;
  } while (ip_interfaceIdentifier_info_2);

  // clean up
  ACE_FREE_FUNC (ip_interfaceIdentifier_info_p);
#else
  //  ACE_OS::memset (MACAddress_out, 0, sizeof (MACAddress_out));
  ACE_OS::memset (MACAddress_out, 0, ETH_ALEN);

#if defined (ACE_HAS_GETIFADDRS)
  struct ifaddrs* ifaddrs_p = NULL;
  int result = ::getifaddrs (&ifaddrs_p);
  if (unlikely (result == -1))
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
//std::string
//Net_Common_Tools::WLANInterfaceToString (HANDLE clientHandle_in,
//                                         REFGUID interfaceIdentifier_in)
//{
//  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::WLANInterfaceToString"));
//
//  // initialize return value(s)
//  std::string result;
//
//  // sanity check(s)
//  if  (unlikely ((clientHandle_in == ACE_INVALID_HANDLE) ||
//                 InlineIsEqualGUID (interfaceIdentifier_in, GUID_NULL)))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("invalid argument, aborting\n")));
//    return result;
//  } // end IF
//
//  struct _WLAN_INTERFACE_INFO_LIST* interface_list_p = NULL;
//  DWORD result_2 = WlanEnumInterfaces (clientHandle_in,
//                                       NULL,
//                                       &interface_list_p);
//  if (unlikely (result_2 != ERROR_SUCCESS))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ::WlanEnumInterfaces(0x%@): \"%s\", aborting\n"),
//                clientHandle_in,
//                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
//    return result;
//  } // end IF
//  ACE_ASSERT (interface_list_p);
//
//  for (DWORD i = 0;
//       i < interface_list_p->dwNumberOfItems;
//       ++i)
//  {
//    if (!InlineIsEqualGUID (interface_list_p->InterfaceInfo[i].InterfaceGuid,
//                            interfaceIdentifier_in))
//      continue;
//
//    result =
//      ACE_TEXT_WCHAR_TO_TCHAR (interface_list_p->InterfaceInfo[i].strInterfaceDescription);
//    break;
//  } // end FOR
//  if (unlikely (result.empty ()))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("device not found (id was: %s), aborting\n"),
//                ACE_TEXT (Common_Tools::GUIDToString (interfaceIdentifier_in).c_str ())));
//    goto error;
//  } // end IF
//
//error:
//  if (interface_list_p)
//    WlanFreeMemory (interface_list_p);
//
//  return result;
//}

std::string
Net_Common_Tools::interfaceToString (REFGUID interfaceIdentifier_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::interfaceToString"));

  // initialize return value(s)
  std::string result;

  // sanity check(s)
  if  (unlikely (InlineIsEqualGUID (interfaceIdentifier_in, GUID_NULL)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument, aborting\n")));
    return ACE_TEXT_ALWAYS_CHAR ("");
  } // end IF

  NET_IFINDEX interface_index = 0;
  struct _IP_interfaceIdentifier_inFO* ip_interfaceIdentifier_info_p = NULL;
  struct _IP_interfaceIdentifier_inFO* ip_interfaceIdentifier_info_2 = NULL;
  ULONG buffer_length = 0;
  ULONG result_2 = 0;

  interface_index = Net_Common_Tools::interfaceToIndex (interfaceIdentifier_in);
  if (unlikely (!interface_index))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::interfaceToIndex(%s), aborting\n"),
                ACE_TEXT (Common_Tools::GUIDToString (interfaceIdentifier_in).c_str ())));
    return ACE_TEXT_ALWAYS_CHAR ("");
  } // end IF

  result_2 = GetAdaptersInfo (ip_interfaceIdentifier_info_p, &buffer_length);
  if (unlikely (result_2 != ERROR_BUFFER_OVERFLOW))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::GetAdaptersInfo(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
    return ACE_TEXT_ALWAYS_CHAR ("");
  } // end IF
  ACE_ASSERT (buffer_length);
  ip_interfaceIdentifier_info_p =
    static_cast<struct _IP_interfaceIdentifier_inFO*> (ACE_MALLOC_FUNC (buffer_length));
  if (unlikely (!ip_interfaceIdentifier_info_p))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
    goto error;
  } // end IF
  result_2 = GetAdaptersInfo (ip_interfaceIdentifier_info_p,
                              &buffer_length);
  if (unlikely (result_2 != NO_ERROR))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::GetAdaptersInfo(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF

  ip_interfaceIdentifier_info_2 = ip_interfaceIdentifier_info_p;
  do
  {
    if (static_cast<DWORD> (interface_index) != ip_interfaceIdentifier_info_2->Index)
      goto continue_;

    result = ip_interfaceIdentifier_info_2->AdapterName;

    break;

continue_:
    ip_interfaceIdentifier_info_2 = ip_interfaceIdentifier_info_2->Next;
  } while (ip_interfaceIdentifier_info_2);

error:
  // clean up
  ACE_FREE_FUNC (ip_interfaceIdentifier_info_p);

  return result;
}

bool
Net_Common_Tools::getDeviceSettingBool (HANDLE clientHandle_in,
                                        REFGUID interfaceIdentifier_in,
                                        enum _WLAN_INTF_OPCODE parameter_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::getDeviceSettingBool"));

  // sanity check(s)
  if  (unlikely ((clientHandle_in == ACE_INVALID_HANDLE) ||
                 InlineIsEqualGUID (interfaceIdentifier_in, GUID_NULL)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument, aborting\n")));
    return false;
  } // end IF

  enum _WLAN_OPCODE_VALUE_TYPE value_type = wlan_opcode_value_type_invalid;
  DWORD data_size = 0;
  PVOID data_p = NULL;
  DWORD result = WlanQueryInterface (clientHandle_in,
                                     &interfaceIdentifier_in,
                                     parameter_in,
                                     NULL,
                                     &data_size,
                                     &data_p,
                                     &value_type);
  if (unlikely (result != ERROR_SUCCESS))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("\"%s\": failed to ::WlanQueryInterface(0x%@,%d): \"%s\", aborting\n"),
                ACE_TEXT (Net_Common_Tools::interfaceToString (interfaceIdentifier_in).c_str ()),
                clientHandle_in, parameter_in,
                ACE_TEXT (Common_Tools::errorToString (result).c_str ())));
    return false;
  } // end IF
  ACE_ASSERT (data_p && (data_size == sizeof (BOOL)));

  return *static_cast<BOOL*> (data_p);
}
bool
Net_Common_Tools::setDeviceSettingBool (HANDLE clientHandle_in,
                                        REFGUID interfaceIdentifier_in,
                                        enum _WLAN_INTF_OPCODE parameter_in,
                                        bool enable_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::setDeviceSettingBool"));

  // sanity check(s)
  if  (unlikely ((clientHandle_in == ACE_INVALID_HANDLE) ||
                 InlineIsEqualGUID (interfaceIdentifier_in, GUID_NULL)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument, aborting\n")));
    return false;
  } // end IF

  BOOL data_b = (enable_in ? TRUE : FALSE);
  PVOID data_p = &data_b;
  DWORD result = WlanSetInterface (clientHandle_in,
                                   &interfaceIdentifier_in,
                                   parameter_in,
                                   sizeof (BOOL),
                                   data_p,
                                   NULL);
  if (unlikely (result != ERROR_SUCCESS))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("\"%s\": failed to ::WlanSetInterface(0x%@,%d): \"%s\", aborting\n"),
                ACE_TEXT (Net_Common_Tools::interfaceToString (interfaceIdentifier_in).c_str ()),
                clientHandle_in, parameter_in,
                ACE_TEXT (Common_Tools::errorToString (result).c_str ())));
    return false;
  } // end IF
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("\"%s\": %s setting (was: %d)\n"),
  //            ACE_TEXT (Net_Common_Tools::interfaceToString (clientHandle_in, interfaceIdentifier_in).c_str ()),
  //            (enable_in ? ACE_TEXT ("enabled") : ACE_TEXT ("disabled")),
  //            parameter_in));

  return true;
}
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
ULONG
Net_Common_Tools::interfaceToIndex (REFGUID interfaceIdentifier_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::interfaceToIndex"));

  // initialize return value(s)
  NET_IFINDEX result = 0;

  // sanity check(s)
  ACE_ASSERT (!InlineIsEqualGUID (interfaceIdentifier_in, GUID_NULL));

  DWORD result_2 = 0;
  union _NET_LUID_LH interface_luid_u;
  interface_luid_u.Value = 0;
  NET_IFINDEX interface_index = 0;
  result_2 = ConvertInterfaceGuidToLuid (&interfaceIdentifier_in,
                                         &interface_luid_u);
  if (unlikely (result_2 != ERROR_SUCCESS))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::ConvertInterfaceGuidToLuid(%s): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::GUIDToString (interfaceIdentifier_in).c_str ()),
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
    return result;
  } // end IF
  result_2 = ConvertInterfaceLuidToIndex (&interface_luid_u,
                                          &result);
  if (unlikely (result_2 != ERROR_SUCCESS))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::ConvertInterfaceLuidToIndex(%q): \"%s\", aborting\n"),
                interface_luid_u.Value,
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
    return result;
  } // end IF

  return result;
}
struct _GUID
Net_Common_Tools::indexToInterface (ULONG interfaceIndex_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::indexToInterface"));

  // initialize return value(s)
  struct _GUID result = GUID_NULL;

  // sanity check(s)
  ACE_ASSERT (interfaceIndex_in);

  union _NET_LUID_LH interface_luid_u;
  interface_luid_u.Value = 0;
  DWORD result_2 = ConvertInterfaceIndexToLuid (interfaceIndex_in,
                                                &interface_luid_u);
  if (unlikely (result_2 != NO_ERROR))
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ConvertInterfaceIndexToLuid(%u), aborting\n"),
                interfaceIndex_in,
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
    return GUID_NULL;
  } // end IF
  result_2 = ConvertInterfaceLuidToGuid (&interface_luid_u,
                                         &result);
  if (unlikely (result_2 != NO_ERROR))
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ConvertInterfaceLuidToGuid(%q), aborting\n"),
                interface_luid_u.Value,
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
    return GUID_NULL;
  } // end IF

  return result;
}
#else
bool
Net_Common_Tools::interfaceIsWLAN (const std::string& interfaceIdentifier_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::interfaceIsWLAN"));

  // sanity check(s)
  ACE_ASSERT (!interfaceIdentifier_in.empty ());
  ACE_ASSERT (interfaceIdentifier_in.size () <= IFNAMSIZ);

  bool result = false;
  int socket_handle = -1;
  struct iwreq request_s;
  ACE_OS::memset (&request_s, 0, sizeof (struct iwreq));
  int result_2 = -1;

  ACE_OS::strncpy (request_s.ifr_name,
                   interfaceIdentifier_in.c_str (),
                   IFNAMSIZ);
  socket_handle = ACE_OS::socket (AF_INET,
                                  SOCK_STREAM,
                                  0);
  if (unlikely (socket_handle == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::socket(AF_INET): \"%m\", aborting\n")));
    return false;
  } // end IF
  // verify the presence of Wireless Extensions
  result_2 = ACE_OS::ioctl (socket_handle,
                            SIOCGIWNAME,
                            &request_s);
  if (!result_2)
    result = true;

  result_2 = ACE_OS::close (socket_handle);
  if (unlikely (socket_handle == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::close(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (interfaceIdentifier_in.c_str ())));

  return result;
}

void
Net_Common_Tools::scan (const std::string& interfaceIdentifier_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::scan"));

  // sanity check(s)
  ACE_ASSERT (!interfaceIdentifier_in.empty ());
  ACE_ASSERT (interfaceIdentifier_in.size () <= IFNAMSIZ);

  int socket_handle = -1;
  struct iwreq request_s;
  ACE_OS::memset (&request_s, 0, sizeof (struct iwreq));
  int result_2 = -1;

  ACE_OS::strncpy (request_s.ifr_name,
                   interfaceIdentifier_in.c_str (),
                   IFNAMSIZ);
  socket_handle = ACE_OS::socket (AF_INET,
                                  SOCK_STREAM,
                                  0);
  if (unlikely (socket_handle == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::socket(AF_INET): \"%m\", returning\n")));
    return;
  } // end IF
  result_2 = ACE_OS::ioctl (socket_handle,
                            SIOCSIWSCAN,
                            &request_s);
  if (unlikely (result_2 == 1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::ioctl(\"%s\",SIOCSIWSCAN): \"%m\", continuing\n"),
                ACE_TEXT (interfaceIdentifier_in.c_str ())));

  result_2 = ACE_OS::close (socket_handle);
  if (unlikely (socket_handle == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::close(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (interfaceIdentifier_in.c_str ())));
}
#endif
std::string
#if defined (ACE_WIN32) || defined (ACE_WIN64)
Net_Common_Tools::associatedSSID (HANDLE clientHandle_in,
                                  REFGUID interfaceIdentifier_in)
#else
Net_Common_Tools::associatedSSID (const std::string& interfaceIdentifier_in)
#endif
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::associatedSSID"));

  // initialize return value(s)
  std::string result;

  // sanity check(s)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (unlikely ((clientHandle_in == ACE_INVALID_HANDLE) ||
                InlineIsEqualGUID (interfaceIdentifier_in, GUID_NULL)))
#else
  if (unlikely (interfaceIdentifier_in.empty ()))
#endif
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument, aborting\n")));
    return result;
  } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  DWORD data_size = 0;
  PVOID data_p = NULL;
  struct _WLAN_CONNECTION_ATTRIBUTES* wlan_connection_attributes_p = NULL;
  DWORD result_2 =
    WlanQueryInterface (clientHandle_in,
                        &interfaceIdentifier_in,
                        wlan_intf_opcode_current_connection,
                        NULL,
                        &data_size,
                        &data_p,
                        NULL);
  if (unlikely ((result_2 != ERROR_SUCCESS) &&
                (result_2 != ERROR_INVALID_STATE)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::WlanQueryInterface(\"%s\",wlan_intf_opcode_current_connection): \"%s\", aborting\n"),
                ACE_TEXT (Net_Common_Tools::interfaceToString (interfaceIdentifier_in).c_str ()),
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
    return result;
  } // end IF
  if (result_2 == ERROR_INVALID_STATE) // <-- not connected
    return result;
  ACE_ASSERT (data_p && (data_size == sizeof (struct _WLAN_CONNECTION_ATTRIBUTES)));
  wlan_connection_attributes_p =
    static_cast<struct _WLAN_CONNECTION_ATTRIBUTES*> (data_p);
  result.assign (reinterpret_cast<char*> (wlan_connection_attributes_p->wlanAssociationAttributes.dot11Ssid.ucSSID),
                 wlan_connection_attributes_p->wlanAssociationAttributes.dot11Ssid.uSSIDLength);

  // clean up
  WlanFreeMemory (data_p);
#else
//  ACE_ASSERT (connection_in);

//  std::string device_path_string =
//      Net_Common_Tools::deviceToDBusPath (connection_in,
//                                          interfaceIdentifier_in);
//  ACE_ASSERT (!device_path_string.empty ());
//  std::string access_point_path_string =
//      Net_Common_Tools::deviceDBusPathToAccessPointDBusPath (connection_in,
//                                                             device_path_string);
//  if (access_point_path_string.empty ())
//    goto continue_;

//  result =
//      Net_Common_Tools::accessPointDBusPathToSSID (connection_in,
//                                                   access_point_path_string);

  int socket_handle = -1;
  int result_2 = -1;
  struct iwreq iwreq_s;
  char essid[IW_ESSID_MAX_SIZE + 1];

  socket_handle = iw_sockets_open ();
  if (unlikely (socket_handle == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to iw_sockets_open(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  ACE_OS::memset (&iwreq_s, 0, sizeof (struct iwreq));
  ACE_OS::memset (essid, 0, sizeof (IW_ESSID_MAX_SIZE + 1));
  iwreq_s.u.essid.pointer = essid;
  iwreq_s.u.essid.length = IW_ESSID_MAX_SIZE + 1;
  result_2 = iw_get_ext (socket_handle,
                         interfaceIdentifier_in.c_str (),
                         SIOCGIWESSID,
                         &iwreq_s);
  if (unlikely (result_2 < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to iw_get_ext(\"%s\",SIOCGIWESSID): \"%m\", aborting\n"),
                ACE_TEXT (interfaceIdentifier_in.c_str ())));
    goto error;
  } // end IF
  // *NOTE*: the length iwreq_s.u.essid.length is wrong
//  result.assign (essid, iwreq_s.u.essid.length);
  result = essid;

error:
  if (likely (socket_handle != -1))
    iw_sockets_close (socket_handle);
#endif

//continue_:
  return result;
}

bool
#if defined (ACE_WIN32) || defined (ACE_WIN64)
Net_Common_Tools::hasSSID (HANDLE clientHandle_in,
                           REFGUID interfaceIdentifier_in,
#else
Net_Common_Tools::hasSSID (const std::string& interfaceIdentifier_in,
#endif
                           const std::string& SSID_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::associatedSSID"));

  // initialize return value(s)
  bool result = false;

  // sanity check(s)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (unlikely ((clientHandle_in == ACE_INVALID_HANDLE) ||
                InlineIsEqualGUID (interfaceIdentifier_in, GUID_NULL)))
#else
  if (unlikely (interfaceIdentifier_in.empty ()))
#endif
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument, aborting\n")));
    return result;
  } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);

  ACE_NOTREACHED (return false;)
#else
  int result_2 = -1;
  std::vector<std::string> wireless_device_identifiers_a;
  int socket_handle = -1;
  struct iw_range iw_range_s;
  struct wireless_scan_head wireless_scan_head_s;
  struct wireless_scan* wireless_scan_p = NULL;
  bool done = false;
#if defined (ACE_HAS_GETIFADDRS)
  struct ifaddrs* ifaddrs_p = NULL;
#endif

  // step1: retrieve all wireless interfaces
  if (likely (!interfaceIdentifier_in.empty ()))
  {
    wireless_device_identifiers_a.push_back (interfaceIdentifier_in);
    goto continue_;
  } // end IF
#if defined (ACE_HAS_GETIFADDRS)
  result_2 = ::getifaddrs (&ifaddrs_p);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ::getifaddrs(): \"%m\", aborting\n")));
    return result;
  } // end IF
  ACE_ASSERT (ifaddrs_p);

  for (struct ifaddrs* ifaddrs_2 = ifaddrs_p;
       ifaddrs_2;
       ifaddrs_2 = ifaddrs_2->ifa_next)
  {
//    if ((ifaddrs_2->ifa_flags & IFF_UP) == 0)
//      continue;
    if (!ifaddrs_2->ifa_addr)
      continue;
    if (ifaddrs_2->ifa_addr->sa_family != AF_INET)
      continue;
    if (!Net_Common_Tools::interfaceIsWLAN (ifaddrs_2->ifa_name))
      continue;

    wireless_device_identifiers_a.push_back (ifaddrs_2->ifa_name);
  } // end FOR

  // clean up
  ::freeifaddrs (ifaddrs_p);
#else
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (result);

  ACE_NOTREACHED (return result;)
#endif /* ACE_HAS_GETIFADDRS */
continue_:
  socket_handle = iw_sockets_open ();
  if (unlikely (socket_handle == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to iw_sockets_open(): \"%m\", aborting\n")));
    return false;
  } // end IF

  for (std::vector<std::string>::const_iterator iterator = wireless_device_identifiers_a.begin ();
       (iterator != wireless_device_identifiers_a.end ()) && !done;
       ++iterator)
  {
    ACE_OS::memset (&iw_range_s, 0, sizeof (struct iw_range));
    result_2 = iw_get_range_info (socket_handle,
                                  (*iterator).c_str (),
                                  &iw_range_s);
    if (unlikely (result_2 < 0))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to iw_get_range_info(): \"%m\", aborting\n")));
      goto error;
    } // end IF

    ACE_OS::memset (&wireless_scan_head_s,
                    0,
                    sizeof (struct wireless_scan_head));
    // *TODO*: do not use iwlib; talk to the driver directly
    result_2 = iw_scan (socket_handle,
                        const_cast<char*> ((*iterator).c_str ()),
                        iw_range_s.we_version_compiled,
                        &wireless_scan_head_s);
    if (unlikely (result_2 < 0))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to iw_scan(): \"%m\", aborting\n")));
      goto error;
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
        result = true;
        done = true;
        break;
      } // end IF
    } // end FOR
  } // end FOR

error:
  // clean up
  iw_sockets_close (socket_handle);
#endif

  return result;
}

bool
#if defined (ACE_WIN32) || defined (ACE_WIN64)
Net_Common_Tools::interfaceToIPAddress (REFGUID interfaceIdentifier_in,
#else
Net_Common_Tools::interfaceToIPAddress (const std::string& interfaceIdentifier_in,
                                        struct DBusConnection* connection_in,
#endif
                                        ACE_INET_Addr& IPAddress_out,
                                        ACE_INET_Addr& gatewayIPAddress_out)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::interfaceToIPAddress"));

  // initialize return value(s)
  IPAddress_out.reset ();
  gatewayIPAddress_out.reset ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct _GUID interface_identifier = interfaceIdentifier_in;
  if (unlikely (InlineIsEqualGUID (interface_identifier, GUID_NULL)))
    interface_identifier = Net_Common_Tools::getDefaultInterface ();
#else
  std::string interface_identifier_string = interfaceIdentifier_in;
  if (unlikely (interface_identifier_string.empty ()))
    interface_identifier_string = Net_Common_Tools::getDefaultInterface ();
#endif
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // sanity check(s)
  ACE_ASSERT (!InlineIsEqualGUID (interface_identifier, GUID_NULL));

  NET_IFINDEX interface_index =
    Net_Common_Tools::interfaceToIndex (interface_identifier);
  if (unlikely (!interface_index))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::interfaceToIndex(%s), aborting\n"),
                ACE_TEXT (Common_Tools::GUIDToString (interface_identifier).c_str ())));
    return false;
  } // end IF

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
  if (unlikely (result != ERROR_BUFFER_OVERFLOW))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::GetAdaptersAddresses(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result).c_str ())));
    return false;
  } // end IF
  ACE_ASSERT (buffer_length);
  ip_adapter_addresses_p =
    static_cast<struct _IP_ADAPTER_ADDRESSES_LH*> (ACE_MALLOC_FUNC (buffer_length));
  if (unlikely (!ip_adapter_addresses_p))
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
  if (unlikely (result != NO_ERROR))
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
//    if ((ip_adapter_addresses_2->OperStatus != IfOperStatusUp) ||
//        (!ip_adapter_addresses_2->FirstUnicastAddress))
//      continue;
    if (ip_adapter_addresses_2->IfIndex != interface_index)
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
                  ACE_TEXT ("adapter \"%s\":\"%s\" does not currently have any unicast IPv4 address, aborting\n"),
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
                  ACE_TEXT ("adapter \"%s\":\"%s\" does not currently have any gateway IPv4 address, continuing\n"),
                  ACE_TEXT_WCHAR_TO_TCHAR (ip_adapter_addresses_2->FriendlyName),
                  ACE_TEXT_WCHAR_TO_TCHAR (ip_adapter_addresses_2->Description)));
      break;
    } // end IF
    socket_address_p = &gateway_address_p->Address;
    sockaddr_in_p = (struct sockaddr_in*)socket_address_p->lpSockaddr;
    result = gatewayIPAddress_out.set (sockaddr_in_p,
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
  if (unlikely (result == -1))
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

    if (!ifaddrs_2->ifa_addr)
      continue;
    if (ifaddrs_2->ifa_addr->sa_family != AF_INET)
      continue;

    sockaddr_in_p = reinterpret_cast<struct sockaddr_in*> (ifaddrs_2->ifa_addr);
    result = IPAddress_out.set (sockaddr_in_p,
                                sizeof (struct sockaddr_in));
    if (unlikely (result == -1))
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

  gatewayIPAddress_out =
      Net_Common_Tools::getGateway (interfaceIdentifier_in,
                                    connection_in);
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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                        struct _GUID& interfaceIdentifier_out)
#else
                                        std::string& interfaceIdentifier_out)
#endif
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::IPAddressToInterface"));

  // initialize return value(s)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  interfaceIdentifier_out = GUID_NULL;
#else
  interfaceIdentifier_out.clear ();
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (Net_Common_Tools::isLocal (IPAddress_in))
  {
    struct _IP_ADAPTER_ADDRESSES_LH* ip_adapter_addresses_p = NULL;
    ULONG buffer_length = 0;
    ULONG result =
      GetAdaptersAddresses (AF_UNSPEC,              // Family
                            0,                      // Flags
                            NULL,                   // Reserved
                            ip_adapter_addresses_p, // AdapterAddresses
                            &buffer_length);        // SizePointer
    if (unlikely (result != ERROR_BUFFER_OVERFLOW))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::GetAdaptersAddresses(): \"%s\", aborting\n"),
                  ACE_TEXT (Common_Tools::errorToString (result).c_str ())));
      return false;
    } // end IF
    ACE_ASSERT (buffer_length);
    ip_adapter_addresses_p =
      static_cast<struct _IP_ADAPTER_ADDRESSES_LH*> (ACE_MALLOC_FUNC (buffer_length));
    if (unlikely (!ip_adapter_addresses_p))
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
    if (unlikely (result != NO_ERROR))
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
    union _NET_LUID_LH interface_luid_u;
    interface_luid_u.Value = 0;
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
      if (unlikely (!unicast_address_p))
      {
        //ACE_DEBUG ((LM_DEBUG,
        //            ACE_TEXT ("adapter \"%s:\"%s does not currently have any unicast IPv4 address, continuing\n"),
        //            ACE_TEXT_WCHAR_TO_TCHAR (ip_adapter_addresses_2->FriendlyName),
        //            ACE_TEXT_WCHAR_TO_TCHAR (ip_adapter_addresses_2->Description)));
        goto continue_;
      } // end IF

      // *NOTE*: this works for IPv4 addresses only
      ACE_ASSERT (unicast_address_p->OnLinkPrefixLength <= 32);
      network_mask = ~((1 << (32 - unicast_address_p->OnLinkPrefixLength)) - 1);
      sockaddr_in_p = (struct sockaddr_in*)socket_address_p->lpSockaddr;
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("found adapter \"%s\": \"%s\" on network %s\n"),
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

      interfaceIdentifier_out =
        Net_Common_Tools::indexToInterface (ip_adapter_addresses_2->IfIndex);
      if (unlikely (InlineIsEqualGUID (interfaceIdentifier_out, GUID_NULL)))
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("failed to Net_Common_Tools::indexToInterface(%u), aborting\n"),
                    ip_adapter_addresses_2->IfIndex));

      break;

continue_:
      ip_adapter_addresses_2 = ip_adapter_addresses_2->Next;
    } while (ip_adapter_addresses_2);

    // clean up
    ACE_FREE_FUNC (ip_adapter_addresses_p);
  } // end IF
  else
  {
    DWORD result = NO_ERROR;
    struct _MIB_IPFORWARDROW route_s;

    ACE_OS::memset (&route_s, 0, sizeof (struct _MIB_IPFORWARDROW));
    result = GetBestRoute (IPAddress_in.get_ip_address (),
                           0,
                           &route_s);
    if (unlikely (result != NO_ERROR))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::GetBestRoute(%s): \"%s\", aborting\n"),
                  ACE_TEXT (Net_Common_Tools::IPAddressToString (IPAddress_in).c_str ()),
                  ACE_TEXT (Common_Tools::errorToString (result).c_str ())));
      return false;
    } // end IF

    interfaceIdentifier_out =
      Net_Common_Tools::indexToInterface (route_s.dwForwardIfIndex);
    if (unlikely (InlineIsEqualGUID (interfaceIdentifier_out, GUID_NULL)))
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("failed to Net_Common_Tools::indexToInterface(%u), aborting\n"),
                  route_s.dwForwardIfIndex));
  } // end ELSE
#else
#if defined (ACE_HAS_GETIFADDRS)
  struct ifaddrs* ifaddrs_p = NULL;
  int result = ::getifaddrs (&ifaddrs_p);
  if (unlikely (result == -1))
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

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  return (!InlineIsEqualGUID (interfaceIdentifier_out, GUID_NULL));
#else
  return (!interfaceIdentifier_out.empty ());
#endif
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct _GUID
#else
std::string
#endif
Net_Common_Tools::getDefaultInterface (enum Net_LinkLayerType type_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::getDefaultInterface"));

  switch (type_in)
  {
    case NET_LINKLAYER_802_3:
    case NET_LINKLAYER_PPP:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      struct _IP_ADAPTER_ADDRESSES_LH* ip_adapter_addresses_p = NULL;
      struct _IP_ADAPTER_ADDRESSES_LH* ip_adapter_addresses_2 = NULL;
      ULONG buffer_length = 0;
      ULONG result_2 = 0;
      struct _GUID interface_identifier = GUID_NULL;
      std::map<ULONG, struct _GUID> connected_interfaces;
      ACE_INET_Addr inet_address, gateway_address;

      result_2 =
        GetAdaptersAddresses (AF_UNSPEC,              // Family
                              0,                      // Flags
                              NULL,                   // Reserved
                              ip_adapter_addresses_p, // AdapterAddresses
                              &buffer_length);        // SizePointer
      if (unlikely (result_2 != ERROR_BUFFER_OVERFLOW))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ::GetAdaptersAddresses(): \"%s\", aborting\n"),
                    ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
        return GUID_NULL;
      } // end IF
      ACE_ASSERT (buffer_length);
      ip_adapter_addresses_p =
        static_cast<struct _IP_ADAPTER_ADDRESSES_LH*> (ACE_MALLOC_FUNC (buffer_length));
      if (unlikely (!ip_adapter_addresses_p))
      {
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
        return GUID_NULL;
      } // end IF
      result_2 =
        GetAdaptersAddresses (AF_UNSPEC,              // Family
                              0,                      // Flags
                              NULL,                   // Reserved
                              ip_adapter_addresses_p, // AdapterAddresses
                              &buffer_length);        // SizePointer
      if (unlikely (result_2 != NO_ERROR))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ::GetAdaptersAddresses(): \"%s\", aborting\n"),
                    ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
        goto error;
      } // end IF

      // step1: retrieve 'connected' interfaces
      ip_adapter_addresses_2 = ip_adapter_addresses_p;
      do
      { // *TODO*: encapsulate this translation
        if ((type_in == NET_LINKLAYER_802_3) &&
            ((ip_adapter_addresses_2->IfType != IF_TYPE_ETHERNET_CSMACD) ||
             (ip_adapter_addresses_2->IfType != IF_TYPE_IS088023_CSMACD)))
          goto continue_;
        else if ((type_in == NET_LINKLAYER_PPP) &&
                 (ip_adapter_addresses_2->IfType != IF_TYPE_PPP))
          goto continue_;
        if ((ip_adapter_addresses_2->OperStatus != IfOperStatusUp) ||
            !ip_adapter_addresses_2->FirstUnicastAddress)
          goto continue_;

        interface_identifier =
          Net_Common_Tools::indexToInterface (ip_adapter_addresses_2->IfIndex);
        if (unlikely (InlineIsEqualGUID (interface_identifier, GUID_NULL)))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Net_Common_Tools::indexToInterface(%u), aborting\n"),
                      ip_adapter_addresses_2->IfIndex));
          goto error;
        } // end IF
        
        connected_interfaces.insert (std::make_pair (ip_adapter_addresses_2->Ipv4Metric,
                                                     interface_identifier));

        //// debug info
        //if (unlikely (!Net_Common_Tools::interfaceToIPAddress (interface_identifier,
        //                                                       inet_address,
        //                                                       gateway_address)))
        //{
        //  ACE_DEBUG ((LM_ERROR,
        //              ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(\"%s\"), aborting\n"),
        //              ACE_TEXT (Net_Common_Tools::interfaceToString (interface_identifier).c_str ())));
        //  goto error;
        //} // end IF
        //if (likely (type_in == NET_LINKLAYER_802_3))
        //{
        //  ACE_ASSERT (ip_adapter_addresses_2->PhysicalAddressLength >= ETH_ALEN);
        //} // end IF
        //ACE_DEBUG ((LM_DEBUG,
        //            ACE_TEXT ("found network interface \"%s\"[%s]: IP#: %s; MAC#: %s\n"),
        //            ACE_TEXT_WCHAR_TO_TCHAR (ip_adapter_addresses_2->FriendlyName),
        //            ACE_TEXT (ip_adapter_addresses_2->AdapterName),
        //            ACE_TEXT (Net_Common_Tools::IPAddressToString (inet_address).c_str ()),
        //            ACE_TEXT (Net_Common_Tools::LinkLayerAddressToString (ip_adapter_addresses_2->PhysicalAddress,
        //                                                                  type_in).c_str ())));

continue_:
        ip_adapter_addresses_2 = ip_adapter_addresses_2->Next;
      } while (ip_adapter_addresses_2);

error:
      // clean up
      ACE_FREE_FUNC (ip_adapter_addresses_p);

      if (unlikely (connected_interfaces.empty ()))
        return GUID_NULL;

      return connected_interfaces.begin ()->second;
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
      if (unlikely ((result_2 == -1)      ||
                    !WIFEXITED (result_2) ||
                    WEXITSTATUS (result_2)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::system(\"%s\"): \"%m\" (result was: %d), aborting\n"),
                    ACE_TEXT (command_line_string.c_str ()),
                    WEXITSTATUS (result_2)));
        return ACE_TEXT_ALWAYS_CHAR ("");
      } // end IF
      unsigned char* data_p = NULL;
      if (unlikely (!Common_File_Tools::load (filename_string,
                                              data_p)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_File_Tools::load(\"%s\"): \"%m\", aborting\n"),
                    ACE_TEXT (filename_string.c_str ())));
        return ACE_TEXT_ALWAYS_CHAR ("");
      } // end IF
      if (unlikely (!Common_File_Tools::deleteFile (filename_string)))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_File_Tools::deleteFile(\"%s\"), continuing\n"),
                    ACE_TEXT (filename_string.c_str ())));

      std::string route_record_string = reinterpret_cast<char*> (data_p);
      delete [] data_p;
    //  ACE_DEBUG ((LM_DEBUG,
    //              ACE_TEXT ("ip data: \"%s\"\n"),
    //              ACE_TEXT (route_record_string.c_str ())));

      std::string result;
      std::istringstream converter;
      char buffer [BUFSIZ];
      std::string regex_string =
          ACE_TEXT_ALWAYS_CHAR ("^default via ([[:digit:].]+) dev ([[:alnum:]]+)(?:.*)$");
      std::regex regex (regex_string);
      std::cmatch match_results;
      converter.str (route_record_string);
      do
      {
        converter.getline (buffer, sizeof (buffer));
        if (unlikely (!std::regex_match (buffer,
                                         match_results,
                                         regex,
                                         std::regex_constants::match_default)))
          continue;
        ACE_ASSERT (match_results.ready () && !match_results.empty ());

        ACE_ASSERT (match_results[1].matched);
        ACE_ASSERT (match_results[2].matched);
        result = match_results[2];

        break;
      } while (!converter.fail ());
      if (unlikely (result.empty ()))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to retrieve default interface from route data (was: \"%s\"), aborting\n"),
                    ACE_TEXT (route_record_string.c_str ())));
        return ACE_TEXT_ALWAYS_CHAR ("");
      } // end IF
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("default interface: \"%s\" (gateway: %s)\n"),
      //            ACE_TEXT (result.c_str ()),
      //            ACE_TEXT (match_results[1].str ().c_str ())));
      return result;
#endif
    }
    case NET_LINKLAYER_ATM:
    case NET_LINKLAYER_FDDI:
    case NET_LINKLAYER_802_11:
    {
      ACE_ASSERT (false);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_NOTSUP_RETURN (GUID_NULL);
#else
      ACE_NOTSUP_RETURN (ACE_TEXT_ALWAYS_CHAR (""));
#endif
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

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  return GUID_NULL;
#else
  return ACE_TEXT_ALWAYS_CHAR ("");
#endif
}
#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct _GUID
#else
std::string
#endif
Net_Common_Tools::getDefaultInterface (int linkLayerType_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::getDefaultInterface"));

  // step1: retrieve 'default' device for each link layer type specified
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  std::vector<struct _GUID> interfaces;
  struct _GUID interface_identifier;
#else
  std::vector<std::string> interfaces;
  std::string interface_identifier;
#endif
  for (enum Net_LinkLayerType i = NET_LINKLAYER_ATM;
       i < NET_LINKLAYER_MAX;
       ++i)
    if (linkLayerType_in & i)
    {
      interface_identifier = Net_Common_Tools::getDefaultInterface (i);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      if (unlikely (InlineIsEqualGUID (interface_identifier, GUID_NULL)))
#else
      if (unlikely (interface_identifier.empty ()))
#endif
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Net_Common_Tools::getDefaultInterface() (type was: \"%s\"), continuing\n"),
                    ACE_TEXT (Net_Common_Tools::LinkLayerTypeToString (i).c_str ())));
        continue;
      } // end IF
      interfaces.push_back (interface_identifier);
    } // end IF

  if (likely (!interfaces.empty ()))
    return interfaces.front ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  return GUID_NULL;
#else
  return ACE_TEXT_ALWAYS_CHAR ("");
#endif
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
    if (unlikely (dottedDecimal_inout.empty ()))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid arguments (hostname/address empty), aborting\n")));
      return false;
    } // end IF

    result = inet_address.set (dottedDecimal_inout.c_str (), AF_INET);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::set(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (dottedDecimal_inout.c_str ())));
      return false;
    } // end IF

    result = inet_address.get_host_name (buffer, sizeof (buffer));
    if (unlikely (result == -1))
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
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::set(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (hostName_inout.c_str ())));
      return false;
    } // end IF

    const char* result_p = inet_address.get_host_addr (buffer, sizeof (buffer));
    if (unlikely (!result_p))
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
  if (unlikely (result))
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
  if (unlikely (socket_handle == ACE_INVALID_HANDLE))
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
  if (unlikely (result_2 == -1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));

  bool do_disassociate = false;
  struct sockaddr* sockaddr_p =
      reinterpret_cast<struct sockaddr*> (destinationAddress_in.get_addr ());
  result_2 = ACE_OS::connect (socket_handle,
                              sockaddr_p,
                              destinationAddress_in.get_addr_size ());
  if (unlikely (result_2 == -1))
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
  if (unlikely (result))
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
  if (likely (do_disassociate))
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
  if (unlikely (result))
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
  if (unlikely (result))
  {
    int error = ACE_OS::last_error ();
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_UNUSED_ARG (error);
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockopt(0x%@,SO_MAX_MSG_SIZE): \"%m\", aborting\n"),
                handle_in));
#else
    if (error != ENOTCONN) // 107
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
  if (unlikely (result))
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
  if (unlikely (result))
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

  if (unlikely (optval != size_in))
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
  if (unlikely (result))
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
  if (unlikely (result))
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
  if (unlikely (result))
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
  if (unlikely (result))
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
  if (unlikely (result))
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
  if (unlikely (result))
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
  if (unlikely (result))
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
Net_Common_Tools::setReuseAddress (ACE_HANDLE handle_in
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                   )
#else
                                   , bool reusePort_in)
#endif
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
  if (unlikely (result))
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

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if (reusePort_in)
  {
    result = ACE_OS::setsockopt (handle_in,
                                 SOL_SOCKET,
                                 SO_REUSEPORT,
                                 reinterpret_cast<const char*> (&optval),
                                 optlen);
    if (unlikely (result))
    {
      // *PORTABILITY*
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::setsockopt(%d,SO_REUSEPORT): \"%m\", aborting\n"),
                  handle_in));
      return false;
    } // end IF
  } // end IF
#endif

  return true;
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
  if (unlikely (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::setsockopt(%d,IP_RECVERR): \"%m\", aborting\n"),
                handle_in));
    return false;
  } // end IF

  return true;
}
#endif

ACE_INET_Addr
Net_Common_Tools::getBoundAddress (ACE_HANDLE handle_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::getBoundAddress"));

  ACE_INET_Addr result;
  int result_2 = -1;
  struct sockaddr_in socket_address_s;
  ACE_OS::memset (&socket_address_s, 0, sizeof (struct sockaddr));
  int socket_address_length = sizeof (struct sockaddr_in);

  result_2 =
    ACE_OS::getsockname (handle_in,
                         reinterpret_cast<struct sockaddr*> (&socket_address_s),
                         &socket_address_length);
  if (unlikely (result_2 == -1))
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockname(0x%@): \"%m\", aborting\n"),
                handle_in));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getsockname(%d): \"%m\", continuing\n"),
                handle_in));
#endif
    return result;
  } // end IF

  result_2 = result.set (&socket_address_s,
                         socket_address_length);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::set(): \"%m\", aborting\n")));
    return result;
  } // end IF

  return result;
}

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
  if (unlikely (result))
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

bool
Net_Common_Tools::sendDatagram (const ACE_INET_Addr& localSAP_in,
                                const ACE_INET_Addr& remoteSAP_in,
                                ACE_Message_Block* messageBlock_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::sendDatagram"));

  bool result = false;
  int result_2 = -1;
  ssize_t result_3 = -1;
  ACE_HANDLE handle_h = ACE_INVALID_HANDLE;
  size_t bytes_to_send = messageBlock_in->length ();
    
  handle_h = ACE_OS::socket (AF_INET,    // family
                             SOCK_DGRAM, // type
                             0);         // protocol
  if (unlikely (handle_h == ACE_INVALID_HANDLE))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::socket(%d,%d,0): \"%m\", aborting\n"),
                AF_INET, SOCK_DGRAM));
    return false;
  } // end IF

  // set source address ?
  if (unlikely (!localSAP_in.is_any ()))
  {
    int one = 1;
    result_2 = ACE_OS::setsockopt (handle_h,
                                   SOL_SOCKET,
                                   SO_REUSEADDR,
                                   reinterpret_cast<char*> (&one),
                                   sizeof (int));
    if (unlikely (result_2 == -1))
    {
      int error = ACE_OS::last_error ();
      ACE_UNUSED_ARG (error);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::setsockopt(0x%@,SO_REUSEADDR): \"%m\", aborting\n"),
                  handle_h));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::setsockopt(%d,SO_REUSEADDR): \"%m\", aborting\n"),
                  handle_h));
#endif
      goto error;
    } // end IF

    result_2 =
      ACE_OS::bind (handle_h,
                    reinterpret_cast<struct sockaddr*> (localSAP_in.get_addr ()),
                    localSAP_in.get_addr_size ());
    if (unlikely (result_2 == -1))
    {
      int error = ACE_OS::last_error ();
      ACE_UNUSED_ARG (error);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::bind(0x%@,%s): \"%m\", aborting\n"),
                  handle_h,
                  ACE_TEXT (Net_Common_Tools::IPAddressToString (localSAP_in).c_str ())));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::bind(%d,%s): \"%m\", aborting\n"),
                  handle_h,
                  ACE_TEXT (Net_Common_Tools::IPAddressToString (localSAP_in).c_str ())));
#endif
      goto error;
    } // end IF
  } // end IF

  result_3 =
    ACE_OS::sendto (handle_h,
                    messageBlock_in->rd_ptr (),
                    bytes_to_send,
                    0,
                    reinterpret_cast<struct sockaddr*> (remoteSAP_in.get_addr ()),
                    remoteSAP_in.get_addr_size ());
  if (unlikely (static_cast<size_t> (result_3) != bytes_to_send))
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::sendto(0x%@,%s,%u) (result was: %d): \"%m\", aborting\n"),
                handle_h,
                ACE_TEXT (Net_Common_Tools::IPAddressToString (remoteSAP_in).c_str ()),
                bytes_to_send, result_3));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::sendto(%d,%s,%u) (result was: %d): \"%m\", aborting\n"),
                handle_h,
                ACE_TEXT (Net_Common_Tools::IPAddressToString (remoteSAP_in).c_str ()),
                bytes_to_send, result_3));
#endif
    goto error;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("dispatched %u byte(s): %s --> %s\n"),
              bytes_to_send,
              ACE_TEXT (Net_Common_Tools::IPAddressToString (localSAP_in).c_str ()),
              ACE_TEXT (Net_Common_Tools::IPAddressToString (remoteSAP_in).c_str ())));

  // *TODO*: shutdown() first ?

  result = true;

error:
  result_2 = ACE_OS::closesocket (handle_h);
  if (unlikely (result_2 == -1))
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::closesocket(0x%@): \"%m\", continuing\n"),
                handle_h));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::closesocket(%d): \"%m\", continuing\n"),
                handle_h));
#endif

  return result;
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
  if (unlikely (result == SOCKET_ERROR))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::ioctl(0x%@,SIO_LOOPBACK_FAST_PATH): \"%s\", aborting\n"),
                handle_in,
                ACE_TEXT (Common_Tools::errorToString (::GetLastError ()).c_str ())));
    return false;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("enabled SIO_LOOPBACK_FAST_PATH on 0x%@\n"),
              handle_in));

  return true;
}
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
bool
Net_Common_Tools::activateConnection (struct DBusConnection* connection_in,
                                      const std::string& connectionObjectPath_in,
                                      const std::string& deviceObjectPath_in,
                                      const std::string& accessPointObjectPath_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::activateConnection"));

  // initialize return value(s)
  bool result = false;

  // sanity check(s)
  ACE_ASSERT (connection_in);
  ACE_ASSERT (!connectionObjectPath_in.empty ());
  ACE_ASSERT (!deviceObjectPath_in.empty ());
  ACE_ASSERT (!accessPointObjectPath_in.empty ());

  struct DBusMessage* message_p =
      dbus_message_new_method_call (ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_SERVICE),
                                    ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_OBJECT_PATH),
                                    ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_INTERFACE),
                                    ACE_TEXT_ALWAYS_CHAR ("ActivateConnection"));
  if (unlikely (!message_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_new_method_call(ActivateConnection): \"%m\", aborting\n")));
    return result;
  } // end IF
  struct DBusMessage* reply_p = NULL;
  struct DBusMessageIter iterator;
  char* object_path_p = NULL;
  const char* argument_string_p = connectionObjectPath_in.c_str ();
  dbus_message_iter_init_append (message_p, &iterator);
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_OBJECT_PATH,
                                                 &argument_string_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  argument_string_p = deviceObjectPath_in.c_str ();
  dbus_message_iter_init_append (message_p, &iterator);
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_OBJECT_PATH,
                                                 &argument_string_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  argument_string_p = accessPointObjectPath_in.c_str ();
  dbus_message_iter_init_append (message_p, &iterator);
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_OBJECT_PATH,
                                                 &argument_string_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  reply_p = Net_Common_Tools::dBusMessageExchange (connection_in,
                                                   message_p,
                                                   -1); // timeout (ms)
  ACE_ASSERT (!message_p);
  if (unlikely (!reply_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::dBusMessageExchange(-1): \"%m\", aborting\n")));
    goto error;
  } // end IF
  if (unlikely (!dbus_message_iter_init (reply_p, &iterator)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_init(), aborting\n")));
    goto error;
  } // end IF

  ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator) == DBUS_TYPE_OBJECT_PATH);
  dbus_message_iter_get_basic (&iterator, &object_path_p);
  ACE_ASSERT (object_path_p);
  dbus_message_unref (reply_p); reply_p = NULL;
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("%s: activated connection profile \"%s\" (active connection is: \"%s\")\n"),
//              ACE_TEXT (Net_Common_Tools::deviceDBusPathToIdentifier (connection_in, deviceObjectPath_in).c_str ()),
//              ACE_TEXT (connectionObjectPath_in.c_str ()),
//              ACE_TEXT (object_path_p)));

  result = true;

  goto continue_;

error:
  if (message_p)
    dbus_message_unref (message_p);
  if (reply_p)
    dbus_message_unref (reply_p);

  return false;

continue_:
  return result;
}

struct DBusMessage*
Net_Common_Tools::dBusMessageExchange (struct DBusConnection* connection_in,
                                       struct DBusMessage*& message_inout,
                                       int timeout_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::dBusMessageExchange"));

  // initialize return value(s)
  struct DBusMessage* result = NULL;

  // sanity check(s)
  ACE_ASSERT (connection_in);
  ACE_ASSERT (message_inout);

  struct DBusPendingCall* pending_p = NULL;
  if (unlikely (!dbus_connection_send_with_reply (connection_in,
                                                  message_inout,
                                                  &pending_p,
                                                  timeout_in))) // timeout (ms)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_connection_send_with_reply(0x%@,%d), aborting\n"),
                connection_in,
                timeout_in));
    goto error;
  } // end IF
  if (unlikely (!pending_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_connection_send_with_reply(0x%@,%d), aborting\n"),
                connection_in,
                timeout_in));
    goto error;
  } // end IF
//  dbus_connection_flush (connection_in);
  dbus_message_unref (message_inout); message_inout = NULL;
  dbus_pending_call_block (pending_p);
  result = dbus_pending_call_steal_reply (pending_p);
  if (unlikely (!result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_pending_call_steal_reply(0x%@), aborting\n"),
                pending_p));
    goto error;
  } // end IF
  dbus_pending_call_unref (pending_p); pending_p = NULL;

  goto continue_;

error:
  if (pending_p)
    dbus_pending_call_unref (pending_p);
  if (message_inout)
  {
    dbus_message_unref (message_inout);
    message_inout = NULL;
  } // end IF

continue_:
  return result;
}

bool
Net_Common_Tools::dBusMessageValidate (struct DBusMessageIter& iterator_in,
                                       int expectedType_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::dBusMessageValidate"));

  if (likely (dbus_message_iter_get_arg_type (&iterator_in) == expectedType_in))
    return true;

  char* signature_string_p = dbus_message_iter_get_signature (&iterator_in);
  ACE_ASSERT (signature_string_p);
  bool is_error = !ACE_OS::strcmp (ACE_TEXT_ALWAYS_CHAR ("s"),
                                   signature_string_p);
  char* error_string_p = NULL;
  if (is_error)
  {
    dbus_message_iter_get_basic (&iterator_in, &error_string_p);
    ACE_ASSERT (error_string_p);
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("unexpected dbus message argument type (was: %c, expected: %c%s%s%s%s%s, aborting\n"),
              dbus_message_iter_get_arg_type (&iterator_in), expectedType_in,
              (is_error ? ACE_TEXT ("; message signature: ") : ACE_TEXT (")")),
              (is_error ? ACE_TEXT (signature_string_p) : ACE_TEXT ("")),
              (is_error ? ACE_TEXT ("): \"") : ACE_TEXT ("")),
              (is_error ? ACE_TEXT (error_string_p) : ACE_TEXT ("")),
              (is_error ? ACE_TEXT ("\"") : ACE_TEXT (""))));

  // clean up
  dbus_free (signature_string_p);

  return false;
}

std::string
Net_Common_Tools::activeConnectionDBusPathToIp4ConfigDBusPath (struct DBusConnection* connection_in,
                                                               const std::string& activeConnectionObjectPath_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::activeConnectionDBusPathToIp4ConfigDBusPath"));

  // initialize return value(s)
  std::string result;

  // sanity check(s)
  ACE_ASSERT (connection_in);
  ACE_ASSERT (!activeConnectionObjectPath_in.empty ());

  struct DBusMessage* message_p =
      dbus_message_new_method_call (ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_SERVICE),
                                    activeConnectionObjectPath_in.c_str (),
                                    ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_PROPERTIES_INTERFACE),
                                    ACE_TEXT_ALWAYS_CHAR ("Get"));
  if (unlikely (!message_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_new_method_call(IPv4Config): \"%m\", aborting\n")));
    return result;
  } // end IF
  struct DBusMessage* reply_p = NULL;
  struct DBusMessageIter iterator;
  char* object_path_p = NULL;
  const char* argument_string_p =
      ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_CONNECTIONACTIVE_INTERFACE);
  dbus_message_iter_init_append (message_p, &iterator);
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_STRING,
                                                 &argument_string_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  argument_string_p = ACE_TEXT_ALWAYS_CHAR ("Ip4Config");
  dbus_message_iter_init_append (message_p, &iterator);
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_STRING,
                                                 &argument_string_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  reply_p = Net_Common_Tools::dBusMessageExchange (connection_in,
                                                   message_p,
                                                   -1); // timeout (ms)
  ACE_ASSERT (!message_p);
  if (unlikely (!reply_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::dBusMessageExchange(-1): \"%m\", aborting\n")));
    goto error;
  } // end IF
  if (unlikely (!dbus_message_iter_init (reply_p, &iterator)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_init(), aborting\n")));
    goto error;
  } // end IF

  ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator) == DBUS_TYPE_OBJECT_PATH);
  dbus_message_iter_get_basic (&iterator, &object_path_p);
  ACE_ASSERT (object_path_p);
  result = object_path_p;
  dbus_message_unref (reply_p); reply_p = NULL;

  goto continue_;

error:
  if (message_p)
    dbus_message_unref (message_p);
  if (reply_p)
    dbus_message_unref (reply_p);

continue_:
  return result;
}

std::string
Net_Common_Tools::activeConnectionDBusPathToDeviceDBusPath (struct DBusConnection* connection_in,
                                                            const std::string& activeConnectionObjectPath_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::activeConnectionDBusPathToDeviceDBusPath"));

  // initialize return value(s)
  std::string result;

  // sanity check(s)
  ACE_ASSERT (connection_in);
  ACE_ASSERT (!activeConnectionObjectPath_in.empty ());

  struct DBusMessage* message_p =
      dbus_message_new_method_call (ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_SERVICE),
                                    activeConnectionObjectPath_in.c_str (),
                                    ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_PROPERTIES_INTERFACE),
                                    ACE_TEXT_ALWAYS_CHAR ("Get"));
  if (unlikely (!message_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_new_method_call(Devices): \"%m\", aborting\n")));
    return result;
  } // end IF
  struct DBusMessage* reply_p = NULL;
  struct DBusMessageIter iterator, iterator_2;
  char* object_path_p = NULL;
  const char* argument_string_p =
      ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_CONNECTIONACTIVE_INTERFACE);
  dbus_message_iter_init_append (message_p, &iterator);
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_STRING,
                                                 &argument_string_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  argument_string_p = ACE_TEXT_ALWAYS_CHAR ("Devices");
  dbus_message_iter_init_append (message_p, &iterator);
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_STRING,
                                                 &argument_string_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  reply_p = Net_Common_Tools::dBusMessageExchange (connection_in,
                                                   message_p,
                                                   -1); // timeout (ms)
  ACE_ASSERT (!message_p);
  if (unlikely (!reply_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::dBusMessageExchange(-1): \"%m\", aborting\n")));
    goto error;
  } // end IF
  if (unlikely (!dbus_message_iter_init (reply_p, &iterator)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_init(), aborting\n")));
    goto error;
  } // end IF
  ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator) == DBUS_TYPE_ARRAY);
  dbus_message_iter_recurse (&iterator, &iterator_2);
  do {
    ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator_2) == DBUS_TYPE_OBJECT_PATH);
    dbus_message_iter_get_basic (&iterator_2, &object_path_p);
    ACE_ASSERT (object_path_p);
    if (unlikely (!result.empty ()))
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("active connection \"%s\" uses several devices, returning the first one\n"),
                  ACE_TEXT (activeConnectionObjectPath_in.c_str ())));
      break;
    } // end IF
    result = object_path_p;
  } while (dbus_message_iter_next (&iterator));
  dbus_message_unref (reply_p); reply_p = NULL;

  goto continue_;

error:
  if (message_p)
    dbus_message_unref (message_p);
  if (reply_p)
    dbus_message_unref (reply_p);

continue_:
  return result;
}

std::string
Net_Common_Tools::deviceToDBusPath (struct DBusConnection* connection_in,
                                    const std::string& interfaceIdentifier_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::deviceToDBusPath"));

  // initialize return value(s)
  std::string result;

  // sanity check(s)
  ACE_ASSERT (connection_in);
  ACE_ASSERT (!interfaceIdentifier_in.empty ());

  struct DBusMessage* reply_p = NULL;
  struct DBusMessage* message_p =
      dbus_message_new_method_call (ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_SERVICE),
                                    ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_OBJECT_PATH),
                                    ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_INTERFACE),
                                    ACE_TEXT_ALWAYS_CHAR ("GetDeviceByIpIface"));
  if (unlikely (!message_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_new_method_call(GetDeviceByIpIface): \"%m\", aborting\n")));
    return result;
  } // end IF
  struct DBusMessageIter iterator;
  char* object_path_p = NULL;
  dbus_message_iter_init_append (message_p, &iterator);
  const char* device_identifier_p = interfaceIdentifier_in.c_str ();
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_STRING,
                                                 &device_identifier_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  reply_p = Net_Common_Tools::dBusMessageExchange (connection_in,
                                                   message_p,
                                                   -1); // timeout (ms)
  ACE_ASSERT (!message_p);
  if (unlikely (!reply_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::dBusMessageExchange(-1): \"%m\", aborting\n")));
    goto error;
  } // end IF
  if (unlikely (!dbus_message_iter_init (reply_p, &iterator)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_init(), aborting\n")));
    goto error;
  } // end IF
  ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator) == DBUS_TYPE_OBJECT_PATH);
  dbus_message_iter_get_basic (&iterator, &object_path_p);
  ACE_ASSERT (object_path_p);
  result = object_path_p;
  dbus_message_unref (reply_p); reply_p = NULL;

  goto continue_;

error:
  if (message_p)
    dbus_message_unref (message_p);
  if (reply_p)
    dbus_message_unref (reply_p);

continue_:
  return result;
}

std::string
Net_Common_Tools::Ip4ConfigDBusPathToGateway (struct DBusConnection* connection_in,
                                              const std::string& Ip4ConfigObjectPath_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::Ip4ConfigDBusPathToGateway"));

  // initialize return value(s)
  std::string result;

  // sanity check(s)
  ACE_ASSERT (connection_in);
  ACE_ASSERT (!Ip4ConfigObjectPath_in.empty ());

  struct DBusMessage* message_p =
  dbus_message_new_method_call (ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_SERVICE),
                                Ip4ConfigObjectPath_in.c_str (),
                                ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_PROPERTIES_INTERFACE),
                                ACE_TEXT_ALWAYS_CHAR ("Get"));
  if (unlikely (!message_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_new_method_call(Gateway): \"%m\", aborting\n")));
    return result;
  } // end IF
  struct DBusMessage* reply_p = NULL;
  struct DBusMessageIter iterator, iterator_2;
  char* string_p = NULL;
//  char character_c = 0;
  const char* argument_string_p =
      ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_IP4CONFIG_INTERFACE);
  dbus_message_iter_init_append (message_p, &iterator);
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_STRING,
                                                 &argument_string_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  argument_string_p = ACE_TEXT_ALWAYS_CHAR ("Gateway");
  dbus_message_iter_init_append (message_p, &iterator);
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_STRING,
                                                 &argument_string_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  reply_p = Net_Common_Tools::dBusMessageExchange (connection_in,
                                                   message_p,
                                                   -1); // timeout (ms)
  ACE_ASSERT (!message_p);
  if (unlikely (!reply_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::dBusMessageExchange(-1): \"%m\", aborting\n")));
    goto error;
  } // end IF
  if (unlikely (!dbus_message_iter_init (reply_p, &iterator)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_init(), aborting\n")));
    goto error;
  } // end IF

//  character_c = dbus_message_iter_get_arg_type (&iterator);
  ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator) == DBUS_TYPE_VARIANT);
  dbus_message_iter_recurse (&iterator, &iterator_2);
  ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator_2) == DBUS_TYPE_STRING);
  dbus_message_iter_get_basic (&iterator_2, &string_p);
  ACE_ASSERT (string_p);
  result = string_p;
  dbus_message_unref (reply_p); reply_p = NULL;

  goto continue_;

error:
  if (message_p)
    dbus_message_unref (message_p);
  if (reply_p)
    dbus_message_unref (reply_p);

continue_:
  return result;
}

std::string
Net_Common_Tools::deviceDBusPathToActiveAccessPointDBusPath (struct DBusConnection* connection_in,
                                                             const std::string& deviceObjectPath_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::deviceDBusPathToActiveAccessPointDBusPath"));

  // initialize return value(s)
  std::string result;

  // sanity check(s)
  ACE_ASSERT (connection_in);
  ACE_ASSERT (!deviceObjectPath_in.empty ());

  struct DBusMessage* reply_p = NULL;
  struct DBusMessage* message_p =
      dbus_message_new_method_call (ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_SERVICE),
                                    deviceObjectPath_in.c_str (),
                                    ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_PROPERTIES_INTERFACE),
                                    ACE_TEXT_ALWAYS_CHAR ("Get"));
  if (unlikely (!message_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_new_method_call(Get): \"%m\", aborting\n")));
    return result;
  } // end IF
  struct DBusMessageIter iterator, iterator_2;
  char* object_path_p = NULL;
  const char* argument_string_p =
      ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_DEVICEWIRELESS_INTERFACE);
  dbus_message_iter_init_append (message_p, &iterator);
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_STRING,
                                                 &argument_string_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  argument_string_p = ACE_TEXT_ALWAYS_CHAR ("ActiveAccessPoint");
  dbus_message_iter_init_append (message_p, &iterator);
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_STRING,
                                                 &argument_string_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  reply_p = Net_Common_Tools::dBusMessageExchange (connection_in,
                                                   message_p,
                                                   -1); // timeout (ms)
  ACE_ASSERT (!message_p);
  if (unlikely (!reply_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::dBusMessageExchange(-1): \"%m\", aborting\n")));
    goto error;
  } // end IF
  if (unlikely (!dbus_message_iter_init (reply_p, &iterator)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_init(), aborting\n")));
    goto error;
  } // end IF
  if (unlikely (!Net_Common_Tools::dBusMessageValidate (iterator,
                                                        DBUS_TYPE_VARIANT)))
    goto error;
  dbus_message_iter_recurse (&iterator, &iterator_2);
  ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator_2) == DBUS_TYPE_OBJECT_PATH);
  dbus_message_iter_get_basic (&iterator_2, &object_path_p);
  ACE_ASSERT (object_path_p);
  result = object_path_p;
  dbus_message_unref (reply_p); reply_p = NULL;

  goto continue_;

error:
  if (message_p)
    dbus_message_unref (message_p);
  if (reply_p)
    dbus_message_unref (reply_p);

continue_:
  // *NOTE*: the D-Bus protocol defines "/" to be an invalid/unkown object path
  if (unlikely (!ACE_OS::strcmp (result.c_str (), ACE_TEXT_ALWAYS_CHAR ("/"))))
    result.resize (0);

  return result;
}

std::string
Net_Common_Tools::deviceDBusPathToIp4ConfigDBusPath (struct DBusConnection* connection_in,
                                                      const std::string& deviceObjectPath_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::deviceDBusPathToIp4ConfigDBusPath"));

  // initialize return value(s)
  std::string result;

  // sanity check(s)
  ACE_ASSERT (connection_in);
  ACE_ASSERT (!deviceObjectPath_in.empty ());

  struct DBusMessage* reply_p = NULL;
  struct DBusMessage* message_p =
      dbus_message_new_method_call (ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_SERVICE),
                                    deviceObjectPath_in.c_str (),
                                    ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_PROPERTIES_INTERFACE),
                                    ACE_TEXT_ALWAYS_CHAR ("Get"));
  if (unlikely (!message_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_new_method_call(Get): \"%m\", aborting\n")));
    return result;
  } // end IF
  struct DBusMessageIter iterator, iterator_2;
  char* object_path_p = NULL;
  const char* argument_string_p =
      ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_DEVICE_INTERFACE);
  dbus_message_iter_init_append (message_p, &iterator);
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_STRING,
                                                 &argument_string_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  argument_string_p = ACE_TEXT_ALWAYS_CHAR ("Ip4Config");
  dbus_message_iter_init_append (message_p, &iterator);
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_STRING,
                                                 &argument_string_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  reply_p = Net_Common_Tools::dBusMessageExchange (connection_in,
                                                   message_p,
                                                   -1); // timeout (ms)
  ACE_ASSERT (!message_p);
  if (unlikely (!reply_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::dBusMessageExchange(-1): \"%m\", aborting\n")));
    goto error;
  } // end IF
  if (unlikely (!dbus_message_iter_init (reply_p, &iterator)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_init(), aborting\n")));
    goto error;
  } // end IF
  // *NOTE*: contrary to the documentation, the type is 'vo', not 'o'
  ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator) == DBUS_TYPE_VARIANT);
  dbus_message_iter_recurse (&iterator, &iterator_2);
  ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator_2) == DBUS_TYPE_OBJECT_PATH);
  dbus_message_iter_get_basic (&iterator_2, &object_path_p);
  ACE_ASSERT (object_path_p);
  result = object_path_p;
  dbus_message_unref (reply_p); reply_p = NULL;

  goto continue_;

error:
  if (message_p)
    dbus_message_unref (message_p);
  if (reply_p)
    dbus_message_unref (reply_p);

continue_:
  // *NOTE*: the D-Bus protocol defines "/" to be an invalid/unkown object path
  if (unlikely (!ACE_OS::strcmp (result.c_str (), ACE_TEXT_ALWAYS_CHAR ("/"))))
    result.resize (0);

  return result;
}

std::string
Net_Common_Tools::deviceDBusPathToIdentifier (struct DBusConnection* connection_in,
                                              const std::string& deviceObjectPath_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::deviceDBusPathToIdentifier"));

  // initialize return value(s)
  std::string result;

  // sanity check(s)
  ACE_ASSERT (connection_in);
  ACE_ASSERT (!deviceObjectPath_in.empty ());

  struct DBusMessage* reply_p = NULL;
  struct DBusMessage* message_p =
      dbus_message_new_method_call (ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_SERVICE),
                                    deviceObjectPath_in.c_str (),
                                    ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_PROPERTIES_INTERFACE),
                                    ACE_TEXT_ALWAYS_CHAR ("Get"));
  if (unlikely (!message_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_new_method_call(Get): \"%m\", aborting\n")));
    return result;
  } // end IF
  struct DBusMessageIter iterator, iterator_2;
  char* device_identifier_p = NULL;
  const char* argument_string_p =
      ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_DEVICE_INTERFACE);
  dbus_message_iter_init_append (message_p, &iterator);
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_STRING,
                                                 &argument_string_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  argument_string_p = ACE_TEXT_ALWAYS_CHAR ("Interface");
  dbus_message_iter_init_append (message_p, &iterator);
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_STRING,
                                                 &argument_string_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  reply_p = Net_Common_Tools::dBusMessageExchange (connection_in,
                                                   message_p,
                                                   -1); // timeout (ms)
  ACE_ASSERT (!message_p);
  if (unlikely (!reply_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::dBusMessageExchange(-1): \"%m\", aborting\n")));
    goto error;
  } // end IF
  if (unlikely (!dbus_message_iter_init (reply_p, &iterator)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_init(), aborting\n")));
    goto error;
  } // end IF
  if (unlikely (!Net_Common_Tools::dBusMessageValidate (iterator,
                                                        DBUS_TYPE_VARIANT)))
    goto error;
  dbus_message_iter_recurse (&iterator, &iterator_2);
  ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator_2) == DBUS_TYPE_STRING);
  dbus_message_iter_get_basic (&iterator_2, &device_identifier_p);
  ACE_ASSERT (device_identifier_p);
  result = device_identifier_p;
  dbus_message_unref (reply_p); reply_p = NULL;

  goto continue_;

error:
  if (message_p)
    dbus_message_unref (message_p);
  if (reply_p)
    dbus_message_unref (reply_p);

continue_:
  return result;
}

std::string
Net_Common_Tools::accessPointDBusPathToSSID (struct DBusConnection* connection_in,
                                             const std::string& accessPointObjectPath_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::accessPointDBusPathToSSID"));

  // initialize return value(s)
  std::string result;

  // sanity check(s)
  ACE_ASSERT (connection_in);
  ACE_ASSERT (!accessPointObjectPath_in.empty ());

  struct DBusMessage* reply_p = NULL;
  struct DBusMessage* message_p =
      dbus_message_new_method_call (ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_SERVICE),
                                    accessPointObjectPath_in.c_str (),
                                    ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_PROPERTIES_INTERFACE),
                                    ACE_TEXT_ALWAYS_CHAR ("Get"));
  if (unlikely (!message_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_new_method_call(Get): \"%m\", aborting\n")));
    return result;
  } // end IF
  struct DBusMessageIter iterator, iterator_2;
  char character_c = 0;
  const char* argument_string_p =
      ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_ACCESSPOINT_INTERFACE);
  dbus_message_iter_init_append (message_p, &iterator);
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_STRING,
                                                 &argument_string_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  argument_string_p = ACE_TEXT_ALWAYS_CHAR ("Ssid");
  dbus_message_iter_init_append (message_p, &iterator);
  if (unlikely (!dbus_message_iter_append_basic (&iterator,
                                                 DBUS_TYPE_STRING,
                                                 &argument_string_p)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  reply_p = Net_Common_Tools::dBusMessageExchange (connection_in,
                                                   message_p,
                                                   -1); // timeout (ms)
  ACE_ASSERT (!message_p);
  if (unlikely (!reply_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::dBusMessageExchange(-1): \"%m\", aborting\n")));
    goto error;
  } // end IF
  if (unlikely (!dbus_message_iter_init (reply_p, &iterator)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_init(), aborting\n")));
    goto error;
  } // end IF
  if (unlikely (!Net_Common_Tools::dBusMessageValidate (iterator,
                                                        DBUS_TYPE_VARIANT)))
    goto error;
  dbus_message_iter_recurse (&iterator, &iterator_2);
  ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator_2) == DBUS_TYPE_ARRAY);
  dbus_message_iter_recurse (&iterator_2, &iterator);
  do {
    ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator) == DBUS_TYPE_BYTE);
    dbus_message_iter_get_basic (&iterator, &character_c);
    ACE_ASSERT (character_c);
    result += character_c;
  } while (dbus_message_iter_next (&iterator));
  dbus_message_unref (reply_p); reply_p = NULL;

  goto continue_;

error:
  if (message_p)
    dbus_message_unref (message_p);
  if (reply_p)
    dbus_message_unref (reply_p);

continue_:
  return result;
}

std::string
Net_Common_Tools::connectionDBusPathToSSID (struct DBusConnection* connection_in,
                                            const std::string& connectionObjectPath_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::connectionDBusPathToSSID"));

  // initialize return value(s)
  std::string result;

  // sanity check(s)
  ACE_ASSERT (connection_in);
  ACE_ASSERT (!connectionObjectPath_in.empty ());

  struct DBusMessage* reply_p = NULL;
  struct DBusMessage* message_p =
  dbus_message_new_method_call (ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_SERVICE),
                                connectionObjectPath_in.c_str (),
                                ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_SETTINGSCONNECTION_INTERFACE),
                                ACE_TEXT_ALWAYS_CHAR ("GetSettings"));
  if (unlikely (!message_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_new_method_call(GetSettings): \"%m\", aborting\n")));
    return result;
  } // end IF
  struct DBusMessageIter iterator, iterator_2, iterator_3, iterator_4;
  struct DBusMessageIter iterator_5, iterator_6, iterator_7;
  const char* key_string_p, *key_string_2 = NULL;
  DBusBasicValue value_u;
  reply_p = Net_Common_Tools::dBusMessageExchange (connection_in,
                                                   message_p,
                                                   -1); // timeout (ms)
  ACE_ASSERT (!message_p);
  if (unlikely (!reply_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::dBusMessageExchange(-1): \"%m\", aborting\n")));
  goto error;
  } // end IF
  // *NOTE*: the schema is a{sa{sv}}
  if (unlikely (!dbus_message_iter_init (reply_p, &iterator)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_init(), aborting\n")));
    goto error;
  } // end IF
  if (unlikely (!Net_Common_Tools::dBusMessageValidate (iterator,
                                                        DBUS_TYPE_ARRAY)))
    goto error;
  dbus_message_iter_recurse (&iterator, &iterator_2);
  do {
    // connection settings --> wireless settings --> IPv4 configuration --> ...
    ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator_2) == DBUS_TYPE_DICT_ENTRY);
    dbus_message_iter_recurse (&iterator_2, &iterator_3);
    ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator_3) == DBUS_TYPE_STRING);
    key_string_p = NULL;
    dbus_message_iter_get_basic (&iterator_3, &key_string_p);
    ACE_ASSERT (key_string_p);
    if (ACE_OS::strcmp (key_string_p, ACE_TEXT_ALWAYS_CHAR ("802-11-wireless")))
      continue;
    dbus_message_iter_next (&iterator_3);
    ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator_3) == DBUS_TYPE_ARRAY);
    dbus_message_iter_recurse (&iterator_3, &iterator_4);
    do
    {
      ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator_4) == DBUS_TYPE_DICT_ENTRY);
      dbus_message_iter_recurse (&iterator_4, &iterator_5);
      ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator_5) == DBUS_TYPE_STRING);
      key_string_2 = NULL;
      dbus_message_iter_get_basic (&iterator_5, &key_string_2);
      ACE_ASSERT (key_string_2);
      dbus_message_iter_next (&iterator_5);
      ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator_5) == DBUS_TYPE_VARIANT);
      dbus_message_iter_recurse (&iterator_5, &iterator_6);
      switch (dbus_message_iter_get_arg_type (&iterator_6))
      {
        case DBUS_TYPE_BOOLEAN:
        case DBUS_TYPE_BYTE:
        case DBUS_TYPE_INT16:
        case DBUS_TYPE_UINT16:
        case DBUS_TYPE_INT32:
        case DBUS_TYPE_UINT32:
        case DBUS_TYPE_INT64:
        case DBUS_TYPE_UINT64:
        case DBUS_TYPE_DOUBLE:
        case DBUS_TYPE_STRING:
        case DBUS_TYPE_OBJECT_PATH:
        case DBUS_TYPE_SIGNATURE:
        case DBUS_TYPE_UNIX_FD:
        {
          dbus_message_iter_get_basic (&iterator_6, &value_u);
          break;
        }
        case DBUS_TYPE_ARRAY:
        {
          dbus_message_iter_recurse (&iterator_6, &iterator_7);
          if (!ACE_OS::strcmp (key_string_2, ACE_TEXT_ALWAYS_CHAR ("ssid")))
          { ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator_7) == DBUS_TYPE_BYTE);
            char character_c = 0;
            do
            {
              dbus_message_iter_get_basic (&iterator_7, &character_c);
              result += character_c;
            } while (dbus_message_iter_next (&iterator_7));

            goto done;
          } // end IF

          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown argument type (was: %d), continuing\n"),
                      dbus_message_iter_get_arg_type (&iterator_6)));
          break;
        }
      } // end SWITCH
    } while (dbus_message_iter_next (&iterator_4));
  } while (dbus_message_iter_next (&iterator_2));
done:
  dbus_message_unref (reply_p); reply_p = NULL;

  goto continue_;

error:
  if (message_p)
    dbus_message_unref (message_p);
  if (reply_p)
    dbus_message_unref (reply_p);

continue_:
  return result;
}

std::string
Net_Common_Tools::SSIDToAccessPointDBusPath (struct DBusConnection* connection_in,
                                             const std::string& deviceObjectPath_in,
                                             const std::string& SSID_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::SSIDToAccessPointDBusPath"));

  // initialize return value(s)
  std::string result;

  // sanity check(s)
  ACE_ASSERT (connection_in);
  ACE_ASSERT (!SSID_in.empty ());

  std::string device_object_path_string =
      (deviceObjectPath_in.empty () ? Net_Common_Tools::SSIDToDeviceDBusPath (connection_in,
                                                                              SSID_in)
                                    : deviceObjectPath_in);
  if (unlikely (device_object_path_string.empty ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::SSIDToDeviceDBusPath(0x%@,%s), aborting\n"),
                connection_in,
                ACE_TEXT (SSID_in.c_str ())));
    return result;
  } // end IF

  struct DBusMessage* reply_p = NULL;
  struct DBusMessage* message_p =
      dbus_message_new_method_call (ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_SERVICE),
                                    device_object_path_string.c_str (),
                                    ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_DEVICEWIRELESS_INTERFACE),
                                    ACE_TEXT_ALWAYS_CHAR ("GetAllAccessPoints"));
  if (unlikely (!message_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_new_method_call(GetAllAccessPoints): \"%m\", aborting\n")));
    return result;
  } // end IF
  struct DBusMessageIter iterator, iterator_2;
  char* object_path_p = NULL;
  std::string SSID_string;
  reply_p = Net_Common_Tools::dBusMessageExchange (connection_in,
                                                   message_p,
                                                   -1); // timeout (ms)
  ACE_ASSERT (!message_p);
  if (unlikely (!reply_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::dBusMessageExchange(-1): \"%m\", aborting\n")));
    goto error;
  } // end IF
  if (unlikely (!dbus_message_iter_init (reply_p, &iterator)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_init(), aborting\n")));
    goto error;
  } // end IF
  if (unlikely (!Net_Common_Tools::dBusMessageValidate (iterator,
                                                        DBUS_TYPE_ARRAY)))
    goto error;
  dbus_message_iter_recurse (&iterator, &iterator_2);
  do {
    if (unlikely (dbus_message_iter_get_arg_type (&iterator_2) == DBUS_TYPE_INVALID))
      break; // device exists, but no SSIDs found --> radio off ?
    ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator_2) == DBUS_TYPE_OBJECT_PATH);
    dbus_message_iter_get_basic (&iterator_2, &object_path_p);
    ACE_ASSERT (object_path_p);
    SSID_string = Net_Common_Tools::accessPointDBusPathToSSID (connection_in,
                                                               object_path_p);
    if (unlikely (SSID_string.empty ()))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_Common_Tools::accessPointDBusPathToSSID(\"%s\"), continuing\n"),
                  ACE_TEXT (object_path_p)));
      continue;
    } // end IF
    if (SSID_string == SSID_in)
    {
      result = object_path_p;
      break;
    } // end IF
  } while (dbus_message_iter_next (&iterator_2));
  dbus_message_unref (reply_p);
  reply_p = NULL;

  goto continue_;

error:
  if (message_p)
    dbus_message_unref (message_p);
  if (reply_p)
    dbus_message_unref (reply_p);

continue_:
  return result;
}

std::string
Net_Common_Tools::SSIDToDeviceDBusPath (struct DBusConnection* connection_in,
                                        const std::string& SSID_in)
{
  // initialize return value(s)
  std::string result;

  // sanity check(s)
  ACE_ASSERT (connection_in);
  ACE_ASSERT (!SSID_in.empty ());

  // step1: retrieve all wireless adapaters
  std::vector<std::string> wireless_device_identifiers_a;
#if defined (ACE_HAS_GETIFADDRS)
  struct ifaddrs* ifaddrs_p = NULL;
  int result_2 = ::getifaddrs (&ifaddrs_p);
  if (unlikely (result_2 == -1))
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ::getifaddrs(): \"%m\", aborting\n")));
    return result;
  } // end IF
  ACE_ASSERT (ifaddrs_p);

  for (struct ifaddrs* ifaddrs_2 = ifaddrs_p;
       ifaddrs_2;
       ifaddrs_2 = ifaddrs_2->ifa_next)
  {
//    if ((ifaddrs_2->ifa_flags & IFF_UP) == 0)
//      continue;
    if (!ifaddrs_2->ifa_addr)
      continue;
    if (ifaddrs_2->ifa_addr->sa_family != AF_INET)
      continue;
    if (!Net_Common_Tools::interfaceIsWLAN (ifaddrs_2->ifa_name))
      continue;

    wireless_device_identifiers_a.push_back (ifaddrs_2->ifa_name);
  } // end FOR

  // clean up
  ::freeifaddrs (ifaddrs_p);
#else
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (result);

  ACE_NOTREACHED (return result;)
#endif /* ACE_HAS_GETIFADDRS */

  // step2: retrieve the wireless adapter(s) that can see the SSID
  for (std::vector<std::string>::iterator iterator = wireless_device_identifiers_a.begin ();
       iterator != wireless_device_identifiers_a.end ();
       ++iterator)
    if (!Net_Common_Tools::hasSSID (*iterator,
                                    SSID_in))
      wireless_device_identifiers_a.erase (iterator);
  if (wireless_device_identifiers_a.empty ())
    return result;
  else if (wireless_device_identifiers_a.size () > 1)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("found several devices that can see SSID %s, choosing the first one\n"),
                ACE_TEXT (SSID_in.c_str ())));

  return Net_Common_Tools::deviceToDBusPath (connection_in,
                                             wireless_device_identifiers_a.front ());
}

std::string
Net_Common_Tools::SSIDToConnectionDBusPath (struct DBusConnection* connection_in,
                                            const std::string& deviceObjectPath_in,
                                            const std::string& SSID_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::SSIDToConnectionDBusPath"));

  // initialize return value(s)
  std::string result;

  // sanity check(s)
  ACE_ASSERT (connection_in);
  ACE_ASSERT (!SSID_in.empty ());

  std::string device_object_path_string =
      (deviceObjectPath_in.empty () ? Net_Common_Tools::SSIDToDeviceDBusPath (connection_in,
                                                                              SSID_in)
                                    : deviceObjectPath_in);
  if (unlikely (device_object_path_string.empty ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::SSIDToDeviceDBusPath(0x%@,%s), aborting\n"),
                connection_in,
                ACE_TEXT (SSID_in.c_str ())));
    return result;
  } // end IF

  struct DBusMessage* reply_p = NULL;
  // *NOTE*: a better alternative would be to retrieve the
  //        'AvailableConnections' property of the device; this seems to be
  //        broken at the moment *TODO*
  //        --> retrieve all connections
  struct DBusMessage* message_p =
      dbus_message_new_method_call (ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_SERVICE),
//                                    device_object_path_string.c_str (),
                                    ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_SETTINGS_OBJECT_PATH),
//                                    ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_PROPERTIES_INTERFACE),
                                    ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_SETTINGS_INTERFACE),
//                                    ACE_TEXT_ALWAYS_CHAR ("Get"));
                                    ACE_TEXT_ALWAYS_CHAR ("ListConnections"));
  if (unlikely (!message_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_new_method_call(Get): \"%m\", aborting\n")));
    return result;
  } // end IF
//  struct DBusMessageIter iterator;
  struct DBusMessageIter iterator_2, iterator_3;
  std::vector<std::string> connection_paths_a;
  std::vector<std::string>::const_iterator iterator_4;
  char* object_path_p = NULL;
  std::string SSID_string;
  //  const char* argument_string_p =
//      ACE_TEXT_ALWAYS_CHAR (NET_WLANMONITOR_DBUS_NETWORKMANAGER_DEVICE_INTERFACE);
//  dbus_message_iter_init_append (message_p, &iterator);
//  if (!dbus_message_iter_append_basic (&iterator,
//                                       DBUS_TYPE_STRING,
//                                       &argument_string_p))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
//    goto error;
//  } // end IF
//  argument_string_p = ACE_TEXT_ALWAYS_CHAR ("AvailableConnections");
//  dbus_message_iter_init_append (message_p, &iterator);
//  if (!dbus_message_iter_append_basic (&iterator,
//                                       DBUS_TYPE_STRING,
//                                       &argument_string_p))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to dbus_message_iter_append_basic(): \"%m\", aborting\n")));
//    goto error;
//  } // end IF
  reply_p = Net_Common_Tools::dBusMessageExchange (connection_in,
                                                   message_p,
                                                   -1); // timeout (ms)
  ACE_ASSERT (!message_p);
  if (unlikely (!reply_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::dBusMessageExchange(-1): \"%m\", aborting\n")));
    goto error;
  } // end IF
//  if (!dbus_message_iter_init (reply_p, &iterator))
  if (unlikely (!dbus_message_iter_init (reply_p, &iterator_2)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dbus_message_iter_init(), aborting\n")));
    goto error;
  } // end IF
//  ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator) == DBUS_TYPE_VARIANT);
//  dbus_message_iter_recurse (&iterator, &iterator_2);
  ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator_2) == DBUS_TYPE_ARRAY);
  dbus_message_iter_recurse (&iterator_2, &iterator_3);
  do {
    ACE_ASSERT (dbus_message_iter_get_arg_type (&iterator_3) == DBUS_TYPE_OBJECT_PATH);
    dbus_message_iter_get_basic (&iterator_3, &object_path_p);
    ACE_ASSERT (object_path_p);
    connection_paths_a.push_back (object_path_p);
  } while (dbus_message_iter_next (&iterator_3));
  dbus_message_unref (reply_p); reply_p = NULL;

  iterator_4 = connection_paths_a.begin ();
  for (;
       iterator_4 != connection_paths_a.end ();
       ++iterator_4)
  {
    SSID_string = Net_Common_Tools::connectionDBusPathToSSID (connection_in,
                                                              *iterator_4);
    if (SSID_string == SSID_in)
      break;
  } // end FOR
  if (unlikely (iterator_4 == connection_paths_a.end ()))
    goto error;

  result = *iterator_4;

  goto continue_;

error:
  if (message_p)
    dbus_message_unref (message_p);
  if (reply_p)
    dbus_message_unref (reply_p);

continue_:
  return result;
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
  if (unlikely (!std::regex_match (URL_in,
                                   match_results,
                                   regex,
                                   std::regex_constants::match_default)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid URL string (was: \"%s\"), aborting\n"),
                ACE_TEXT (URL_in.c_str ())));
    return result;
  } // end IF
  ACE_ASSERT (match_results.ready () && !match_results.empty ());
  if (unlikely (!match_results[2].matched))
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

//////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
ACE_INET_Addr
Net_Common_Tools::getGateway (const std::string& interfaceIdentifier_in,
                              struct DBusConnection* connection_in)
{
  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::getGateway"));

  ACE_INET_Addr result;

  // sanity check(s)
  ACE_ASSERT (!interfaceIdentifier_in.empty ());

  // *IMPORTANT NOTE*: the linux kernel does not maintain specific network
  //                   infrastructure information apart from its routing tables.
  //                   The routing tables are initialized and manipulated from
  //                   user space libraries and programs (i.e. glibc, dhclient,
  //                   etc.) using the Netlink protocol.
  //                   --> retrieve/parse the routing table entries to extract
  //                       the gateway address(es)
  // *TODO*: encapsulate the sockets/networking code with ACE

  int socket_handle = ACE_OS::socket (PF_NETLINK,     // protocol family
                                      SOCK_DGRAM,     // type
                                      NETLINK_ROUTE); // protocol
  if (unlikely (socket_handle < 0))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::socket(PF_NETLINK,NETLINK_ROUTE): \"%m\", aborting\n")));
    return result;
  } // end IF

  char buffer[BUFSIZ];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  struct nlmsghdr* nl_message_header_p =
      reinterpret_cast<struct nlmsghdr*> (buffer);
  struct rtmsg* rt_message_p =
      static_cast<struct rtmsg*> (NLMSG_DATA (nl_message_header_p));
  struct rtattr* rt_attribute_p = NULL;
  nl_message_header_p->nlmsg_len = NLMSG_LENGTH (sizeof (struct rtmsg));
  nl_message_header_p->nlmsg_type = RTM_GETROUTE;
  nl_message_header_p->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;
  int message_sequence_i = 0;
  pid_t pid_i = ACE_OS::getpid ();
  nl_message_header_p->nlmsg_seq = message_sequence_i++;
  nl_message_header_p->nlmsg_pid = pid_i;

  unsigned int received_bytes = 0;
  char* buffer_p = buffer;
  int length_i = 0;
  bool done, skip = false;
  ACE_INET_Addr current_interface_address;
  std::string current_interface_string;

  int flags = 0;
  // send request
  int result_2 = ACE_OS::send (socket_handle,
                               reinterpret_cast<char*> (nl_message_header_p),
                               nl_message_header_p->nlmsg_len,
                               flags);
  if (unlikely ((result_2 < 0) ||
                (static_cast<__u32> (result_2) != nl_message_header_p->nlmsg_len)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::send(%d,%u): \"%m\", aborting\n"),
                socket_handle,
                nl_message_header_p->nlmsg_len));
    goto clean;
  } // end IF

  ACE_OS::memset (buffer, 0, sizeof (buffer));
  // receive reply/ies
  do
  { ACE_ASSERT (received_bytes < BUFSIZ);
    result_2 = ACE_OS::recv (socket_handle,
                             buffer_p,
                             BUFSIZ - received_bytes);
    if (unlikely (result_2 < 0))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::recv(%d): \"%m\", aborting\n"),
                  socket_handle));
      goto clean;
    } // end IF
    nl_message_header_p = reinterpret_cast<struct nlmsghdr*> (buffer_p);

    if (unlikely (!NLMSG_OK (nl_message_header_p, result_2) ||
                  (nl_message_header_p->nlmsg_type == NLMSG_ERROR)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("error in Netlink message, aborting\n")));
      goto clean;
    } // end IF
    if (nl_message_header_p->nlmsg_type == NLMSG_DONE)
      break;

    buffer_p += result_2;
    received_bytes += result_2;

    if ((nl_message_header_p->nlmsg_flags & NLM_F_MULTI) == 0)
      break;
  } while ((nl_message_header_p->nlmsg_seq != static_cast<__u32> (message_sequence_i)) ||
           (nl_message_header_p->nlmsg_pid != static_cast<__u32> (pid_i)));

  // parse routing tables entries
  done = false;
  for (nl_message_header_p = reinterpret_cast<struct nlmsghdr*> (buffer);
       (NLMSG_OK (nl_message_header_p, received_bytes) && !done);
       nl_message_header_p = NLMSG_NEXT (nl_message_header_p, received_bytes))
  {
    current_interface_address.reset ();
    current_interface_string.resize (0);

    rt_message_p =
        static_cast<struct rtmsg*> (NLMSG_DATA (nl_message_header_p));
    ACE_ASSERT (rt_message_p);

    // inspect only AF_INET-specific rules from the main table
    if ((rt_message_p->rtm_family != AF_INET) ||
        (rt_message_p->rtm_table != RT_TABLE_MAIN))
      continue;

    // *NOTE*: the attributes don't seem to appear in any specfic order; retain
    //         address information until the device has been identified, and
    //         vice versa
    rt_attribute_p = static_cast<struct rtattr*> (RTM_RTA (rt_message_p));
    ACE_ASSERT (rt_attribute_p);
    length_i = RTM_PAYLOAD (nl_message_header_p);
    skip = false;
    for (;
         RTA_OK (rt_attribute_p, length_i) && !skip;
         rt_attribute_p = RTA_NEXT (rt_attribute_p, length_i))
    {
      switch (rt_attribute_p->rta_type)
      {
        case RTA_IIF:
        case RTA_OIF:
        {
          char buffer_2[IF_NAMESIZE];
          ACE_OS::memset (buffer_2, 0, sizeof (buffer_2));
          if (unlikely (!::if_indextoname (*static_cast<int*> (RTA_DATA (rt_attribute_p)),
                                           buffer_2)))
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to if_indextoname(): \"%m\", continuing\n")));
            break;
          } // end IF
          current_interface_string = buffer_2;
          if (!ACE_OS::strcmp (interfaceIdentifier_in.c_str (),
                               buffer_2))
          {
            if (!current_interface_address.is_any ())
            {
              result = current_interface_address;
              skip = true;
              done = true; // done
            } // end IF
          } // end IF
          else
            skip = true; // rule applies to a different interface --> skip ahead
          break;
        }
        case RTA_DST:
        {
          in_addr_t inet_address = 0;
          ACE_OS::memcpy (&inet_address,
                          RTA_DATA (rt_attribute_p),
                          sizeof (in_addr_t));
          ACE_INET_Addr inet_address_2;
          result_2 = inet_address_2.set (static_cast<u_short> (0),
                                         inet_address,
                                         0, // already in network byte order
                                         0);
          if (unlikely (result_2 == -1))
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_INET_Addr::set(): \"%m\", continuing\n")));
            break;
          } // end IF
          if (!inet_address_2.is_any ())
            skip = true; // apparently the gateway address is only set on rules
                         // with destination 0.0.0.0
          break;
        }
        case RTA_GATEWAY:
        {
          in_addr_t inet_address = 0;
          ACE_OS::memcpy (&inet_address,
                          RTA_DATA (rt_attribute_p),
                          sizeof (in_addr_t));
          result_2 =
              current_interface_address.set (static_cast<u_short> (0),
                                             inet_address,
                                             0, // already in network byte order
                                             0);
          if (unlikely (result_2 == -1))
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_INET_Addr::set(): \"%m\", continuing\n")));
            break;
          } // end IF

          if (!ACE_OS::strcmp (current_interface_string.c_str (),
                               interfaceIdentifier_in.c_str ()))
          {
            result = current_interface_address;
            skip = true; // skip
            done = true; // done
          } // end IF
          break;
        }
        default:
        {
//          ACE_DEBUG ((LM_DEBUG,
//                      ACE_TEXT ("found attribute (type was: %d)\n"),
//                      rt_attribute_p->rta_type));
          break;
        }
      } // end SWITCH
    } // end FOR
  } // end FOR

clean:
  if (socket_handle != ACE_INVALID_HANDLE)
  {
    result_2 = ACE_OS::close (socket_handle);
    if (unlikely (result_2 == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::close(%d): \"%m\", continuing\n"),
                  socket_handle));
  } // end IF

  // *IMPORTANT NOTE*: (as of kernel 3.16.0,x) dhclient apparently does not add
  //                   wireless gateway information to the routing table
  //                   reliably (i.e. Gateway is '*'). Specifically, when there
  //                   already is a gateway configured on a different interface
  //                   --> try DBus instead
  if (result.is_any ()                                           &&
      Net_Common_Tools::interfaceIsWLAN (interfaceIdentifier_in) &&
      connection_in)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("\"%s\": failed to retrieve gateway from kernel, trying dbus...\n"),
                ACE_TEXT (interfaceIdentifier_in.c_str ())));

    std::string result_string =
        Net_Common_Tools::deviceToDBusPath (connection_in,
                                            interfaceIdentifier_in);
    if (unlikely (result_string.empty ()))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_Common_Tools::deviceToDBusPath(\"%s\"), aborting\n"),
                  ACE_TEXT (interfaceIdentifier_in.c_str ())));
      return result;
    } // end IF
    result_string =
        Net_Common_Tools::deviceDBusPathToIp4ConfigDBusPath (connection_in,
                                                             result_string);
    if (unlikely (result_string.empty ()))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_Common_Tools::deviceDBusPathToIp4ConfigDBusPath(\"%s\",\"%s\"), aborting\n"),
                  ACE_TEXT (interfaceIdentifier_in.c_str ()),
                  ACE_TEXT (result_string.c_str ())));
      return result;
    } // end IF
    result_string =
        Net_Common_Tools::Ip4ConfigDBusPathToGateway (connection_in,
                                                      result_string);
    if (unlikely (result_string.empty ()))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_Common_Tools::Ip4ConfigDBusPathToGateway(\"%s\",\"%s\"), aborting\n"),
                  ACE_TEXT (interfaceIdentifier_in.c_str ()),
                  ACE_TEXT (result_string.c_str ())));
      return result;
    } // end IF
    result_2 = result.set (0,
                           result_string.c_str (),
                           1,
                           AF_INET);
    if (unlikely (result_2 == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::set(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (result_string.c_str ())));
      return result;
    } // end IF
  } // end IF

  return result;
}
#endif

//std::string
//Net_Common_Tools::generateUniqueName (const std::string& prefix_in)
//{
//  NETWORK_TRACE (ACE_TEXT ("Net_Common_Tools::generateUniqueName"));

//  std::string result;

//  // sanity check(s)
//  ACE_ASSERT (prefix_in.size () <= (BUFSIZ - 6 + 1));

//  // *NOTE*: see also: man 3 mkstemp
//  ACE_TCHAR buffer[BUFSIZ];
//  if (unlikely (!prefix_in.empty ()))
//    ACE_OS::strcpy (buffer, prefix_in.c_str ());
//  ACE_OS::strcpy (buffer + prefix_in.size (), ACE_TEXT ("XXXXXX"));
//  ACE_OS::mktemp (buffer);
//  if (unlikely (!ACE_OS::strlen (buffer)))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_OS::mktemp(): \"%m\", aborting\n")));
//    return std::string ();
//  } // end IF
//  result = buffer;

//  return result;
//}
