# cotire.cmake 1.8.0 generated file
# /mnt/win_d/projects/ACENetwork/clang/test_u/WLAN_monitor/wlan_monitor_CXX_cotire.cmake
set (COTIRE_ADDITIONAL_PREFIX_HEADER_IGNORE_EXTENSIONS "inc;inl;ipp")
set (COTIRE_ADDITIONAL_PREFIX_HEADER_IGNORE_PATH "")
set (COTIRE_CLEAN_ALL_TARGET_NAME "clean_cotire")
set (COTIRE_CLEAN_TARGET_SUFFIX "_clean_cotire")
set (COTIRE_CMAKE_MODULE_FILE "/media/erik/USB_BLACK/lib/cotire/CMake/cotire.cmake")
set (COTIRE_CMAKE_MODULE_VERSION "1.8.0")
set (COTIRE_DEBUG "OFF")
set (COTIRE_INTDIR "cotire")
set (COTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES "-j")
set (COTIRE_MINIMUM_NUMBER_OF_TARGET_SOURCES "2")
set (COTIRE_PCH_ALL_TARGET_NAME "all_pch")
set (COTIRE_PCH_TARGET_SUFFIX "_pch")
set (COTIRE_PREFIX_HEADER_FILENAME_SUFFIX "_prefix")
set (COTIRE_TARGETS_FOLDER "cotire")
set (COTIRE_TARGET_COMPILE_DEFINITIONS_DEBUG "_DEBUG;DEBUG_DEBUGGER;ACENETWORK_NTRACE=1;HAVE_CONFIG_H;SSL_SUPPORT;NETLINK_SUPPORT;ACE_HAS_DLL;GUI_SUPPORT;OPENGL_SUPPORT;GTK2_SUPPORT;GTK_SUPPORT;GTKGL_SUPPORT;wxUSE_LOG_DEBUG;CURSES_SUPPORT;GTK_USE;GTK2_USE;COMMON_HAS_DLL;COMMON_UI_HAS_DLL;COMMON_UI_GTK_HAS_DLL;COMMON_GL_HAS_DLL;LIBXML2_SUPPORT;LIBXML2_USE;DHCLIENT_SUPPORT;FRAMEWORK_DHCP_SUPPORT;DHCLIENT_USE;WEXT_SUPPORT;NL80211_SUPPORT;DBUS_SUPPORT;NL80211_USE;COMMON_XML_HAS_DLL;ACESTREAM_HAS_DLL;ACENETWORK_HAS_DLL;_GLIBCXX_INCLUDE_NEXT_C_HEADERS;_GLIBCXX_USE_CXX11_ABI=0")
set (COTIRE_TARGET_COMPILE_FLAGS_DEBUG "-g;-std=c++11")
set (COTIRE_TARGET_CONFIGURATION_TYPES "Debug")
set (COTIRE_TARGET_CXX_COMPILER_LAUNCHER "COTIRE_TARGET_CXX_COMPILER_LAUNCHER-NOTFOUND")
set (COTIRE_TARGET_IGNORE_PATH "/mnt/win_d/projects/ACENetwork")
set (COTIRE_TARGET_INCLUDE_DIRECTORIES_DEBUG "/media/erik/USB_BLACK/lib/ACE_TAO/ACE;/mnt/win_d/projects/ACENetwork/test_u/WLAN_monitor/../../../Common/src;/mnt/win_d/projects/ACENetwork/test_u/WLAN_monitor/../../../Common/src/signal;/mnt/win_d/projects/ACENetwork/test_u/WLAN_monitor/../../../Common/src/timer;/mnt/win_d/projects/ACENetwork/test_u/WLAN_monitor/../../../ACEStream/src;/mnt/win_d/projects/ACENetwork/test_u/WLAN_monitor/../../src;/mnt/win_d/projects/ACENetwork/test_u/WLAN_monitor/../../src/wlan;/mnt/win_d/projects/ACENetwork/test_u/WLAN_monitor/..;/mnt/win_d/projects/ACENetwork/clang/test_u/WLAN_monitor/../..;/mnt/win_d/projects/Common/src;/mnt/win_d/projects/Common/src/error;/mnt/win_d/projects/Common/src/signal;/mnt/win_d/projects/Common/src/timer;/mnt/win_d/projects/Common/cmake/src/..;/mnt/win_d/projects/Common/src/log/..;/mnt/win_d/projects/Common/cmake/src/log/../..;/mnt/win_d/projects/Common/cmake/src/signal/../..;/mnt/win_d/projects/Common/src/signal/..;/mnt/win_d/projects/Common/src/timer/..;/mnt/win_d/projects/Common/cmake/src/timer/../..;/usr/include/libxml2;/mnt/win_d/projects/Common/src/xml/..;/mnt/win_d/projects/Common/cmake/src/xml/../..;/usr/include/libnl3;$<BUILD_INTERFACE:/media/erik/USB_BLACK/lib/ACE_TAO/ACE>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/../../Common/cmake>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/../../Common/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/../../Common/src/timer>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/../../ACEStream/cmake>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/../../ACEStream/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/../../ACEStream/src/modules/lib>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/../../ACEStream/src/modules/stat>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/../3rd_party/bison>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/clang/src/..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/wlan>;$<BUILD_INTERFACE:/usr/include/x86_64-linux-gnu>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../Common/cmake>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../Common/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../Common/src/timer>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../ACEStream/cmake>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../ACEStream/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../ACEStream/src/modules/lib>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../ACEStream/src/modules/stat>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../3rd_party/bison>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/clang/src/protocol/dhcp/../../..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../client_server>;$<BUILD_INTERFACE:/usr/include/libnl3>;$<BUILD_INTERFACE:/usr/include/dbus-1.0;/usr/lib/x86_64-linux-gnu/dbus-1.0/include;/usr/include/libnm;/usr/include/libmount;/usr/include/blkid;/usr/include/glib-2.0;/usr/lib/x86_64-linux-gnu/glib-2.0/include>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/cmake>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src/dbus>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src/error>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src/math>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src/timer>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src/xml>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/wlan/../../../ACEStream/cmake>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/wlan/../../../ACEStream/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/wlan/../../../ACEStream/src/modules/lib>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/clang/src/wlan/../..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/wlan/..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/src/wlan/../protocol/dhcp>;/mnt/win_d/projects/ACENetwork/test_u/$<BUILD_INTERFACE:/usr/include/gtk-2.0;/usr/lib/x86_64-linux-gnu/gtk-2.0/include;/usr/include/pango-1.0;/usr/include/atk-1.0;/usr/include/cairo;/usr/include/pixman-1;/usr/include/gdk-pixbuf-2.0;/usr/include/harfbuzz;/usr/include/fribidi;/usr/lib/x86_64-linux-gnu/glib-2.0/include;/usr/include/uuid;/usr/include/freetype2;/usr/include/libpng16>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/../Common/cmake>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/../Common/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/../Common/src/timer>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/../Common/src/ui>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/../Common/src/ui/gtk>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/../ACEStream/cmake>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/../ACEStream/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/../ACEStream/src/modules/lib>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/clang/test_u/..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACENetwork/test_u/../src>;/usr/include/dbus-1.0;/usr/include/systemd;/mnt/win_d/projects/Common/src/dbus/..;/mnt/win_d/projects/Common/cmake/src/dbus/../..;/usr/include/x86_64-linux-gnu;/mnt/win_d/projects/ACEStream/src/../../Common/cmake;/mnt/win_d/projects/ACEStream/src/../../Common/src;/mnt/win_d/projects/ACEStream/src/../../Common/src/timer;/mnt/win_d/projects/ACEStream/src/../3rd_party/tree;/mnt/win_d/projects/ACEStream/cmake/src/..;/mnt/win_d/projects/ACEStream/src;/mnt/win_d/projects/ACEStream/src/modules/lib;/mnt/win_d/projects/ACEStream/src/modules/misc;/mnt/win_d/projects/ACEStream/src/modules/stat;/usr/include/gtk-2.0;/usr/include/libpng16;/mnt/win_d/projects/Common/src/ui/..;/mnt/win_d/projects/Common/src/ui/../error;/mnt/win_d/projects/Common/src/ui/../image;/mnt/win_d/projects/Common/src/ui/../log;/mnt/win_d/projects/Common/src/ui/../xml;/mnt/win_d/projects/Common/cmake/src/ui/../..;/usr/include/x86_64-linux-gnu//ImageMagick-6;/usr/include/ImageMagick-6;/mnt/win_d/projects/Common/src/gl/..;/mnt/win_d/projects/Common/src/gl/../image;/mnt/win_d/projects/Common/cmake/src/gl/../..;/media/erik/USB_BLACK/lib/gtkglarea;/mnt/win_d/projects/Common/cmake/src/ui/gtk/../../..;/mnt/win_d/projects/Common/src/ui/gtk/..;/mnt/win_d/projects/Common/src/ui/gtk/../..;/mnt/win_d/projects/Common/src/ui/gtk/../../image;/mnt/win_d/projects/Common/src/ui/gtk/../../log;/mnt/win_d/projects/Common/src/ui/gtk/../../timer")
set (COTIRE_TARGET_INCLUDE_PRIORITY_PATH "")
set (COTIRE_TARGET_LANGUAGE "CXX")
set (COTIRE_TARGET_MAXIMUM_NUMBER_OF_INCLUDES "-j")
set (COTIRE_TARGET_POST_UNDEFS "")
set (COTIRE_TARGET_PRE_UNDEFS "")
set (COTIRE_TARGET_SOURCES "stdafx.cpp;test_u_eventhandler.cpp;test_u_signalhandler.cpp;wlan_monitor.cpp;test_u_ui_callbacks.cpp")
set (COTIRE_UNITY_BUILD_ALL_TARGET_NAME "all_unity")
set (COTIRE_UNITY_BUILD_TARGET_SUFFIX "_unity")
set (COTIRE_UNITY_OUTPUT_DIRECTORY "unity")
set (COTIRE_UNITY_SOURCE_EXCLUDE_EXTENSIONS "m;mm")
set (COTIRE_UNITY_SOURCE_FILENAME_SUFFIX "_unity")
set (CMAKE_GENERATOR "Ninja")
set (CMAKE_BUILD_TYPE "Debug")
set (CMAKE_CXX_COMPILER_ID "Clang")
set (CMAKE_CXX_COMPILER_VERSION "9.0.0")
set (CMAKE_CXX_COMPILER "/usr/bin/clang++")
set (CMAKE_CXX_COMPILER_ARG1 "")
set (CMAKE_INCLUDE_FLAG_CXX "-I")
set (CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
set (CMAKE_CXX_SOURCE_FILE_EXTENSIONS "C;M;c++;cc;cpp;cxx;mm;CPP")
