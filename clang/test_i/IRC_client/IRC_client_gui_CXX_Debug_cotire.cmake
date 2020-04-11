# cotire.cmake 1.8.0 generated file
# /mnt/win_d/projects/ACENetwork/clang/test_i/IRC_client/IRC_client_gui_CXX_cotire.cmake
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
set (COTIRE_TARGET_COMPILE_DEFINITIONS_DEBUG "_DEBUG;DEBUG_DEBUGGER;ACENETWORK_NTRACE=1;_GLIBCXX_USE_CXX11_ABI=0;HAVE_CONFIG_H;SSL_SUPPORT;NETLINK_SUPPORT;ACE_HAS_DLL;GUI_SUPPORT;OPENGL_SUPPORT;GTK3_SUPPORT;GTK2_SUPPORT;GTK_SUPPORT;GTKGLAREA_SUPPORT;GTKGL_SUPPORT;wxUSE_LOG_DEBUG;CURSES_SUPPORT;GTK_USE;GTK3_USE;_GLIBCXX_INCLUDE_NEXT_C_HEADERS")
set (COTIRE_TARGET_COMPILE_FLAGS_DEBUG "-g;-fPIC;-std=c++11")
set (COTIRE_TARGET_CONFIGURATION_TYPES "Debug")
set (COTIRE_TARGET_CXX_COMPILER_LAUNCHER "COTIRE_TARGET_CXX_COMPILER_LAUNCHER-NOTFOUND")
set (COTIRE_TARGET_IGNORE_PATH "/mnt/win_d/projects/ACENetwork")
set (COTIRE_TARGET_INCLUDE_DIRECTORIES_DEBUG "/media/erik/USB_BLACK/lib/ACE_TAO/ACE;/mnt/win_d/projects/ACENetwork/test_i/IRC_client/../../../Common/src;/mnt/win_d/projects/ACENetwork/test_i/IRC_client/../../../Common/src/log;/mnt/win_d/projects/ACENetwork/test_i/IRC_client/../../../Common/src/signal;/mnt/win_d/projects/ACENetwork/test_i/IRC_client/../../../Common/src/timer;/mnt/win_d/projects/ACENetwork/test_i/IRC_client/../../../ACEStream/src;/mnt/win_d/projects/ACENetwork/test_i/IRC_client/../../../ACEStream/src/modules/lib;/mnt/win_d/projects/ACENetwork/test_i/IRC_client/../../../ACEStream/src/modules/misc;/mnt/win_d/projects/ACENetwork/test_i/IRC_client/../../../ACEStream/src/modules/net;/mnt/win_d/projects/ACENetwork/test_i/IRC_client/../../../ACEStream/src/modules/stat;/mnt/win_d/projects/ACENetwork/test_i/IRC_client/../../3rd_party/bison;/mnt/win_d/projects/ACENetwork/test_i/IRC_client/../../src;/mnt/win_d/projects/ACENetwork/test_i/IRC_client/../../src/client_server;/mnt/win_d/projects/ACENetwork/test_i/IRC_client/../../src/protocol/irc;/mnt/win_d/projects/ACENetwork/test_i/IRC_client/..;/mnt/win_d/projects/ACENetwork/clang/test_i/IRC_client/../..;/mnt/win_d/projects/Common/src;/mnt/win_d/projects/Common/src/error;/mnt/win_d/projects/Common/src/signal;/mnt/win_d/projects/Common/src/timer;/mnt/win_d/projects/Common/clang/src/..;/mnt/win_d/projects/Common/src/log/..;/mnt/win_d/projects/Common/clang/src/log/../..;/mnt/win_d/projects/Common/clang/src/signal/../..;/mnt/win_d/projects/Common/src/signal/..;/mnt/win_d/projects/Common/src/timer/..;/mnt/win_d/projects/Common/clang/src/timer/../..;/usr/include/x86_64-linux-gnu;/mnt/win_d/projects/ACEStream/src/../../Common/cmake;/mnt/win_d/projects/ACEStream/src/../../Common/src;/mnt/win_d/projects/ACEStream/src/../../Common/src/parser;/mnt/win_d/projects/ACEStream/src/../../Common/src/timer;/mnt/win_d/projects/ACEStream/src/../3rd_party/tree;/mnt/win_d/projects/ACEStream/clang/src/..;/mnt/win_d/projects/ACEStream/src;/mnt/win_d/projects/ACEStream/src/modules/lib;/mnt/win_d/projects/ACEStream/src/modules/misc;/mnt/win_d/projects/ACEStream/src/modules/stat;/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src;/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/error;/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/image;/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/log;/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/parser;/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/timer;/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/ui;/mnt/win_d/projects/ACEStream/src/modules/misc/../../../3rd_party/bison;/mnt/win_d/projects/ACEStream/clang/src/modules/misc/../../..;/mnt/win_d/projects/ACEStream/src/modules/misc/../..;/mnt/win_d/projects/ACEStream/src/modules/misc/../dec;/mnt/win_d/projects/ACEStream/src/modules/misc/../dev;/mnt/win_d/projects/ACEStream/src/modules/misc/../file;/mnt/win_d/projects/ACEStream/src/modules/misc/../lib;/mnt/win_d/projects/ACEStream/src/modules/net/../../../../Common/src;/mnt/win_d/projects/ACEStream/src/modules/net/../../../../Common/src/timer;/mnt/win_d/projects/ACEStream/src/modules/net/../../../../ACENetwork/src;/mnt/win_d/projects/ACEStream/src/modules/net/../../../../ACENetwork/src/client_server;/mnt/win_d/projects/ACEStream/clang/src/modules/net/../../..;/mnt/win_d/projects/ACEStream/src/modules/net/../..;/mnt/win_d/projects/ACEStream/src/modules/net/../lib;/mnt/win_d/projects/ACEStream/src/modules/net/../stat;/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src;/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src/error;/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src/log;/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src/math;/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src/timer;/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src/ui;/mnt/win_d/projects/ACEStream/clang/src/modules/stat/../../..;/mnt/win_d/projects/ACEStream/src/modules/stat/../..;/mnt/win_d/projects/ACEStream/src/modules/stat/../dev;/mnt/win_d/projects/ACEStream/src/modules/stat/../lib;/usr/include/libnl3;/media/erik/USB_BLACK/lib/ACE_TAO/ACE;/mnt/win_d/projects/ACENetwork/src/../../Common/cmake;/mnt/win_d/projects/ACENetwork/src/../../Common/src;/mnt/win_d/projects/ACENetwork/src/../../Common/src/parser;/mnt/win_d/projects/ACENetwork/src/../../Common/src/timer;/mnt/win_d/projects/ACENetwork/src/../../ACEStream/cmake;/mnt/win_d/projects/ACENetwork/src/../../ACEStream/src;/mnt/win_d/projects/ACENetwork/src/../../ACEStream/src/modules/lib;/mnt/win_d/projects/ACENetwork/src/../../ACEStream/src/modules/stat;/mnt/win_d/projects/ACENetwork/src/../3rd_party/bison;/mnt/win_d/projects/ACENetwork/clang/src/..;/mnt/win_d/projects/ACENetwork/src/wlan;/usr/include/x86_64-linux-gnu;/mnt/win_d/projects/ACENetwork/src/protocol/irc/../../../../Common/cmake;/mnt/win_d/projects/ACENetwork/src/protocol/irc/../../../../Common/src;/mnt/win_d/projects/ACENetwork/src/protocol/irc/../../../../Common/src/log;/mnt/win_d/projects/ACENetwork/src/protocol/irc/../../../../Common/src/timer;/mnt/win_d/projects/ACENetwork/src/protocol/irc/../../../../ACEStream/cmake;/mnt/win_d/projects/ACENetwork/src/protocol/irc/../../../../ACEStream/src;/mnt/win_d/projects/ACENetwork/src/protocol/irc/../../../../ACEStream/src/modules/lib;/mnt/win_d/projects/ACENetwork/src/protocol/irc/../../../../ACEStream/src/modules/net;/mnt/win_d/projects/ACENetwork/src/protocol/irc/../../../../ACEStream/src/modules/stat;/mnt/win_d/projects/ACENetwork/clang/src/protocol/irc/../../..;/mnt/win_d/projects/ACENetwork/src/protocol/irc/../..;/mnt/win_d/projects/ACENetwork/src/protocol/irc/../../client_server;/usr/include/gtk-3.0;/usr/include/pango-1.0;/usr/include/glib-2.0;/usr/lib64/glib-2.0/include;/usr/include/fribidi;/usr/include/cairo;/usr/include/pixman-1;/usr/include/freetype2;/usr/include/libpng16;/usr/include/uuid;/usr/include/harfbuzz;/usr/include/gdk-pixbuf-2.0;/usr/include/gio-unix-2.0/;/usr/include/libdrm;/usr/include/atk-1.0;/usr/include/at-spi2-atk/2.0;/usr/include/at-spi-2.0;/usr/include/dbus-1.0;/usr/lib64/dbus-1.0/include;/mnt/win_d/projects/ACENetwork/test_i/../../gtkglarea;/mnt/win_d/projects/ACENetwork/test_i/../../Common/cmake;/mnt/win_d/projects/ACENetwork/test_i/../../Common/src;/mnt/win_d/projects/ACENetwork/test_i/../../Common/src/log;/mnt/win_d/projects/ACENetwork/test_i/../../Common/src/parser;/mnt/win_d/projects/ACENetwork/test_i/../../Common/src/signal;/mnt/win_d/projects/ACENetwork/test_i/../../Common/src/timer;/mnt/win_d/projects/ACENetwork/test_i/../../Common/src/ui;/mnt/win_d/projects/ACENetwork/test_i/../../Common/src/ui/gtk;/mnt/win_d/projects/ACENetwork/test_i/../../ACEStream/cmake;/mnt/win_d/projects/ACENetwork/test_i/../../ACEStream/src;/mnt/win_d/projects/ACENetwork/test_i/../../ACEStream/src/modules/lib;/mnt/win_d/projects/ACENetwork/clang/test_i/..;/mnt/win_d/projects/ACENetwork/test_i/../src;/usr/include/libnl3;/usr/include/dbus-1.0;/usr/lib/x86_64-linux-gnu/dbus-1.0/include;/usr/include/libnm;/usr/include/libmount;/usr/include/blkid;/usr/lib/x86_64-linux-gnu/glib-2.0/include;/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/cmake;/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src;/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src/dbus;/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src/error;/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src/math;/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src/timer;/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src/xml;/mnt/win_d/projects/ACENetwork/src/wlan/../../../ACEStream/cmake;/mnt/win_d/projects/ACENetwork/src/wlan/../../../ACEStream/src;/mnt/win_d/projects/ACENetwork/src/wlan/../../../ACEStream/src/modules/lib;/mnt/win_d/projects/ACENetwork/clang/src/wlan/../..;/mnt/win_d/projects/ACENetwork/src/wlan/..;/mnt/win_d/projects/ACENetwork/src/wlan/../protocol/dhcp;/usr/include/x86_64-linux-gnu//ImageMagick-6;/usr/include/ImageMagick-6;/mnt/win_d/projects/Common/src/gl/..;/mnt/win_d/projects/Common/src/gl/../image;/mnt/win_d/projects/Common/clang/src/gl/../..;/usr/include/gtk-2.0;/usr/lib/x86_64-linux-gnu/gtk-2.0/include;/usr/lib/x86_64-linux-gnu/glib-2.0/include;/media/erik/USB_BLACK/lib/gtkglarea;/mnt/win_d/projects/Common/clang/src/ui/gtk/../../..;/mnt/win_d/projects/Common/src/ui/gtk/..;/mnt/win_d/projects/Common/src/ui/gtk/../..;/mnt/win_d/projects/Common/src/ui/gtk/../../image;/mnt/win_d/projects/Common/src/ui/gtk/../../log;/mnt/win_d/projects/Common/src/ui/gtk/../../timer;/mnt/win_d/projects/Common/src/ui/..;/mnt/win_d/projects/Common/src/ui/../error;/mnt/win_d/projects/Common/src/ui/../image;/mnt/win_d/projects/Common/src/ui/../log;/mnt/win_d/projects/Common/src/ui/../xml;/mnt/win_d/projects/Common/clang/src/ui/../..;/usr/include/systemd;/mnt/win_d/projects/Common/src/dbus/..;/mnt/win_d/projects/Common/clang/src/dbus/../..;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../Common/cmake;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../Common/src;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../Common/src/timer;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../ACEStream/cmake;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../ACEStream/src;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../ACEStream/src/modules/lib;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../ACEStream/src/modules/stat;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../3rd_party/bison;/mnt/win_d/projects/ACENetwork/clang/src/protocol/dhcp/../../..;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../..;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../client_server")
set (COTIRE_TARGET_INCLUDE_PRIORITY_PATH "")
set (COTIRE_TARGET_LANGUAGE "CXX")
set (COTIRE_TARGET_MAXIMUM_NUMBER_OF_INCLUDES "-j")
set (COTIRE_TARGET_POST_UNDEFS "")
set (COTIRE_TARGET_PRE_UNDEFS "")
set (COTIRE_TARGET_SOURCES "IRC_client_gui.cpp;IRC_client_gui_connection.cpp;IRC_client_gui_messagehandler.cpp;IRC_client_gui_tools.cpp;IRC_client_gui_callbacks.cpp;stdafx.cpp;IRC_client_module_IRChandler.cpp;IRC_client_sessionmessage.cpp;IRC_client_signalhandler.cpp;IRC_client_stream.cpp;IRC_client_tools.cpp")
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
