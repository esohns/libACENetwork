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

#ifndef NET_COMMON_H
#define NET_COMMON_H

#include <set>

#if defined (ACE_HAS_NETLINK)
#include "ace/Netlink_Addr.h"
#endif
#include "ace/Synch_Traits.h"
#include "ace/Time_Value.h"

#include "stream_common.h"

// forward declarations
template <typename ConfigurationType>
class Net_ITransportLayer_T;
struct Net_SocketConfiguration;

enum Net_LinkLayerType
{
  NET_LINKLAYER_802_3  = 0x01, // i.e. CSMA/CD, aka 'Ethernet'
  NET_LINKLAYER_802_11 = 0x02, // i.e. Wireless LAN aka 'WLAN' (EU; US: 'WiFi')
  NET_LINKLAYER_ATM    = 0x04,
  NET_LINKLAYER_FDDI   = 0x08,
  NET_LINKLAYER_PPP    = 0x10,
  ////////////////////////////////////////
  NET_LINKLAYER_MAX,
  NET_LINKLAYER_INVALID = -1,
};

enum Net_NetworkLayerType
{
  NET_NETWORKLAYER_IP_UNICAST   = 0x0001, // i.e. "routable" IP
  NET_NETWORKLAYER_IP_BROADCAST = 0x0002,
  NET_NETWORKLAYER_IP_MULTICAST = 0x0004,
  ////////////////////////////////////////
  NET_NETWORKLAYER_MAX,
  NET_NETWORKLAYER_INVALID = -1,
};

enum Net_TransportLayerType
{
  NET_TRANSPORTLAYER_INVALID = -1,
  NET_TRANSPORTLAYER_IP_CAST = 0, // 'pseudo' (LAN-only, no flow control)
  NET_TRANSPORTLAYER_NETLINK, // 'pseudo' ((Linux-)host only, no flow control) kernel<->user space protocol
  NET_TRANSPORTLAYER_TCP,
  NET_TRANSPORTLAYER_UDP,
  ////////////////////////////////////////
  NET_TRANSPORTLAYER_MAX
};

enum Net_ClientServerRole
{
  NET_ROLE_INVALID = -1,
  NET_ROLE_CLIENT = 0,
  NET_ROLE_SERVER,
  ////////////////////////////////////////
  NET_ROLE_MAX
};

//enum Net_Stream_ControlMessageType : int
//{
//  NET_STREAM_CONTROL_MESSAGE_MASK = STREAM_CONTROL_MESSAGE_USER_MASK,
//  ////////////////////////////////////////
//  NET_STREAM_CONTROL_MESSAGE_MAX,
//  NET_STREAM_CONTROL_MESSAGE_INVALID
//};

//enum Net_Stream_SessionMessageType : int
//{
//  NET_STREAM_SESSION_MESSAGE_MASK = STREAM_SESSION_MESSAGE_USER_MASK,
//  // *** notification ***
//  NET_STREAM_SESSION_MESSAGE_CLOSE,
//  ////////////////////////////////////////
//  NET_STREAM_SESSION_MESSAGE_MAX,
//  NET_STREAM_SESSION_MESSAGE_INVALID
//};

//enum Net_Stream_ControlType : int
//{
//  NET_STREAM_CONTROL_MASK = STREAM_CONTROL_USER_MASK,
//  ////////////////////////////////////////
//  NET_STREAM_CONTROL_MAX,
//  NET_STREAM_CONTROL_INVALID
//};

enum Net_Connection_AbortStrategy
{
  NET_CONNECTION_ABORT_STRATEGY_RECENT_LEAST = 0,
  NET_CONNECTION_ABORT_STRATEGY_RECENT_MOST,
  ////////////////////////////////////////
  NET_CONNECTION_ABORT_STRATEGY_MAX,
  NET_CONNECTION_ABORT_STRATEGY_INVALID
};

// *NOTE*: this extends ACE_Svc_Handler_Close (see Svc_Handler.h)
enum Net_Connection_CloseReason
{
  NET_CONNECTION_CLOSE_REASON_INVALID = -1,
  ////////////////////////////////////////
  NET_CONNECTION_CLOSE_REASON_INITIALIZATION = 0x02,
  NET_CONNECTION_CLOSE_REASON_USER_ABORT,
  ////////////////////////////////////////
  NET_CONNECTION_CLOSE_REASON_MAX
};

enum Net_Connection_Status
{
  NET_CONNECTION_STATUS_INVALID = -1,
  ////////////////////////////////////////
  NET_CONNECTION_STATUS_OK = 0,
  ////////////////////////////////////////
  NET_CONNECTION_STATUS_INITIALIZATION_FAILED,
  ////////////////////////////////////////
  NET_CONNECTION_STATUS_CLOSED,      // (local) close ()
  NET_CONNECTION_STATUS_PEER_CLOSED, // connection closed by the peer
  ////////////////////////////////////////
  NET_CONNECTION_STATUS_MAX
};

typedef Stream_Statistic Net_RuntimeStatistic_t;
typedef Net_ITransportLayer_T<struct Net_SocketConfiguration> Net_ITransportLayer_t;

struct Net_UserData
{
  inline Net_UserData ()
   : userData (NULL)
  {};

  void* userData;
};

typedef unsigned int Net_ConnectionId_t;

struct Net_ConnectionState
{
  inline Net_ConnectionState ()
   : status (NET_CONNECTION_STATUS_INVALID)
   , currentStatistic ()
   , lastCollectionTimestamp (ACE_Time_Value::zero)
   , lock ()
   , userData (NULL)
  {};

  enum Net_Connection_Status status;

  Net_RuntimeStatistic_t     currentStatistic;
  ACE_Time_Value             lastCollectionTimestamp;
  ACE_SYNCH_MUTEX            lock;

  struct Net_UserData*       userData;
};

typedef std::set<Net_ConnectionId_t> Net_Connections_t;
typedef Net_Connections_t::iterator Net_ConnectionsIterator_t;

#if defined (ACE_HAS_NETLINK)
class Net_Netlink_Addr
 : public ACE_Netlink_Addr
{
 public:
  Net_Netlink_Addr& operator= (const ACE_Addr&);

  virtual int addr_to_string (ACE_TCHAR[],
                              size_t,
                              int = 1) const;
  inline bool is_any (void) const { ACE_ASSERT (false); ACE_NOTSUP_RETURN (false); ACE_NOTREACHED (return false;) };

 private:
  typedef ACE_Netlink_Addr inherited;
};
#endif /* ACE_HAS_NETLINK */

#endif
