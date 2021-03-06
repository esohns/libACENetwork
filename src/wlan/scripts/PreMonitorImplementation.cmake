include (CMakeDependentOption)
include (FeatureSummary)

if (UNIX)
 option (NETLINK_SUPPORT "enable netlink support" ON)

# check_library_exists (iw iw_sockets_open /usr/lib/x86_64-linux-gnu HAVE_LIBIW)
 set (IW_LIB_FILE libiw.so)
 find_library (IW_LIBRARY ${IW_LIB_FILE}
               PATHS /usr
               PATH_SUFFIXES lib64 lib
               DOC "searching for ${IW_LIB_FILE}")
 if (IW_LIBRARY)
  message (STATUS "Found iw library \"${IW_LIBRARY}\"")
  set (WEXT_SUPPORT_ENABLE ON)
  add_definitions (-DWEXT_SUPPORT)
  add_feature_info (Wext WEXT_SUPPORT "support wireless extensions")
 else ()
  message (WARNING "could not find ${IW_LIB_FILE}, continuing")
  set (WEXT_SUPPORT_ENABLE OFF)
 endif (IW_LIBRARY)
 option (WEXT_SUPPORT "enable Wext support" ${WEXT_SUPPORT_ENABLE})

 pkg_check_modules (PKG_LIBNL3 libnl-3.0 libnl-genl-3.0)
 if (PKG_LIBNL3_FOUND)
#  message (STATUS "Found libnl-3.0")
  set (NL80211_SUPPORT_ENABLE ON)
  add_definitions (-DNL80211_SUPPORT)
  add_feature_info (nl80211 NL80211_SUPPORT "support nl80211")
 else ()
  message (WARNING "could not find libnl-3.0, continuing")
  pkg_check_modules (PKG_LIBNL2 libnl-2.0 libnl-genl-2.0)
  if (PKG_LIBNL2_FOUND)
#  message (STATUS "Found libnl-2.0")
   set (NL80211_SUPPORT_ENABLE ON)
   add_definitions (-DNL80211_SUPPORT)
   add_feature_info (nl80211 NL80211_SUPPORT "support nl80211")
  else ()
   message (WARNING "could not find libnl-2.0, continuing")
   set (NL80211_SUPPORT_ENABLE OFF)
  endif (PKG_LIBNL2_FOUND)
 endif (PKG_LIBNL3_FOUND)
# option (NL80211_SUPPORT "enable nl80211 support" ${NL80211_SUPPORT_ENABLE})
 CMAKE_DEPENDENT_OPTION (NL80211_SUPPORT "enable nl80211 support" ${NL80211_SUPPORT_ENABLE}
                         "NETLINK_SUPPORT" OFF)

 pkg_check_modules (PKG_DBUS dbus-1 libnm)
 if (PKG_DBUS_FOUND)
#  message (STATUS "Found dbus-1")
#  message (STATUS "Found libnm")
  set (DBUS_SUPPORT_ENABLE ON)
  add_definitions (-DDBUS_SUPPORT)
  add_feature_info (DBus/NetworkManager DBUS_SUPPORT "support DBus/NetworkManager")
 else ()
  message (WARNING "could not find dbus-1, continuing")
  message (WARNING "could not find libnm, continuing")
  set (DBUS_SUPPORT_ENABLE OFF)
 endif (PKG_DBUS_FOUND)
 option (DBUS_SUPPORT "enable DBus/NetworkManager support" ${DBUS_SUPPORT_ENABLE})
# *TODO*: currently (!) implies DHCLIENT_SUPPORT off
# CMAKE_DEPENDENT_OPTION (DBUS_SUPPORT "compile DBus/NetworkManager support" ${DBUS_SUPPORT_ENABLE}
#                         "DHCLIENT_SUPPORT" OFF)
elseif (WIN32)
 option (WLANAPI_SUPPORT "compile wlanapi support" ON)
 add_definitions (-DWLANAPI_SUPPORT)
 add_feature_info (wlanapi WLANAPI_SUPPORT "support wlanapi")
endif ()

