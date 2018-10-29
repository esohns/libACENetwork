﻿# *NOTE*: the first entry is the default option
if (UNIX)
 set (DHCP_CLIENT_IMPLEMENTATION "dhclient" CACHE STRING "use ISC dhclient (default)")
 set (DHCP_CLIENT_IMPLEMENTATION "dhcpd" CACHE STRING "use dhcpd")
 set (DHCP_CLIENT_IMPLEMENTATION "framework" CACHE STRING "use framework (libACENetwork)")

 set_property (CACHE MONITOR_IMPLEMENTATION PROPERTY STRINGS "dhclient" "dhcpd" "framework")
elseif (WIN32)
 set (DHCP_CLIENT_IMPLEMENTATION "wlanapi" CACHE STRING "use wlanapi (default)")

 set_property (CACHE MONITOR_IMPLEMENTATION PROPERTY STRINGS "wlanapi")
endif ()

if (NOT (DEFINED DHCP_CLIENT_IMPLEMENTATION_LAST))
 set (DHCP_CLIENT_IMPLEMENTATION_LAST "NotAnImplementation" CACHE STRING "last DHCP client implementation used")
 mark_as_advanced (FORCE DHCP_CLIENT_IMPLEMENTATION_LAST)
endif ()
if (NOT (${DHCP_CLIENT_IMPLEMENTATION} MATCHES ${DHCP_CLIENT_IMPLEMENTATION_LAST}))
 if (UNIX)
  unset (DHCLIENT_USE CACHE)
  unset (DHCPD_USE CACHE)
  unset (FRAMEWORK_USE CACHE)
 elseif (WIN32)
  unset (WLANAPI_USE CACHE)
 endif ()
  set (DHCP_CLIENT_IMPLEMENTATION_LAST ${DHCP_CLIENT_IMPLEMENTATION} CACHE STRING "Updating DHCP Client Implementation Option" FORCE)
endif ()

if (UNIX)
 if (${DHCP_CLIENT_IMPLEMENTATION} MATCHES "dhclient")
  if (NOT DHCLIENT_SUPPORT)
   message (FATAL_ERROR "ISC dhclient not supported")
  endif ()
  set (DHCLIENT_USE ON CACHE STRING "use ISC dhclient")
  mark_as_advanced (FORCE DHCLIENT_USE)
  add_definitions (-DDHCLIENT_USE)
 elseif (${DHCP_CLIENT_IMPLEMENTATION} MATCHES "dhcpd")
  if (NOT DHCPD_SUPPORT)
   message (FATAL_ERROR "dhcpd not supported")
  endif ()
  set (DHCPD_USE ON CACHE STRING "use dhcpd")
  mark_as_advanced (FORCE DHCPD_USE)
  add_definitions (-DDHCPD_USE)
 elseif (${DHCP_CLIENT_IMPLEMENTATION} MATCHES "framework")
  if (NOT FRAMEWORK_SUPPORT)
   message (FATAL_ERROR "framework (libACENetwork) not supported")
  endif ()
  set (FRAMEWORK_USE ON CACHE STRING "use framework (libACENetwork)")
  mark_as_advanced (FORCE FRAMEWORK_USE)
  add_definitions (-DFRAMEWORK_USE)
 endif ()
elseif (WIN32)
 if (${DHCP_CLIENT_IMPLEMENTATION} MATCHES "wlanapi")
  if (NOT WLANAPI_SUPPORT)
   message (FATAL_ERROR "wlanapi not supported")
  endif ()
  set (WLANAPI_USE ON CACHE STRING "use wlanapi")
  mark_as_advanced (FORCE WLANAPI_USE)
  add_definitions (-DWLANAPI_USE)
 endif ()
endif ()
