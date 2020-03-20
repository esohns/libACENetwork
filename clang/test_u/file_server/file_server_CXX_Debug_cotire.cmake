# cotire.cmake 1.8.0 generated file
# /mnt/win_d/projects/ACENetwork/clang/test_u/file_server/file_server_CXX_cotire.cmake
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
set (COTIRE_TARGET_COMPILE_DEFINITIONS_DEBUG "_DEBUG;DEBUG_DEBUGGER;ACENETWORK_NTRACE=1;HAVE_CONFIG_H;SSL_SUPPORT;NETLINK_SUPPORT;ACE_HAS_DLL;GUI_SUPPORT;OPENGL_SUPPORT;GTK2_SUPPORT;GTK_SUPPORT;GTKGL_SUPPORT;wxUSE_LOG_DEBUG;CURSES_SUPPORT;GTK_USE;GTK2_USE;COMMON_HAS_DLL;COMMON_UI_HAS_DLL;COMMON_UI_GTK_HAS_DLL;COMMON_GL_HAS_DLL;ACESTREAM_HAS_DLL;ACENETWORK_HAS_DLL;_GLIBCXX_INCLUDE_NEXT_C_HEADERS;_GLIBCXX_USE_CXX11_ABI=0")
set (COTIRE_TARGET_COMPILE_FLAGS_DEBUG "-g;-std=c++11")
set (COTIRE_TARGET_CONFIGURATION_TYPES "Debug")
set (COTIRE_TARGET_CXX_COMPILER_LAUNCHER "COTIRE_TARGET_CXX_COMPILER_LAUNCHER-NOTFOUND")
set (COTIRE_TARGET_IGNORE_PATH "/mnt/win_d/projects/ACENetwork")
set (COTIRE_TARGET_INCLUDE_DIRECTORIES_DEBUG "/usr/include/x86_64-linux-gnu;/usr/local/src/ACE_wrappers;/mnt/win_d/projects/ACENetwork/test_u/file_server/../../../Common/src;/mnt/win_d/projects/ACENetwork/test_u/file_server/../../../Common/src/log;/mnt/win_d/projects/ACENetwork/test_u/file_server/../../../Common/src/signal;/mnt/win_d/projects/ACENetwork/test_u/file_server/../../../Common/src/timer;/mnt/win_d/projects/ACENetwork/test_u/file_server/../../../ACEStream/src;/mnt/win_d/projects/ACENetwork/test_u/file_server/../../../ACEStream/src/modules/dec;/mnt/win_d/projects/ACENetwork/test_u/file_server/../../../ACEStream/src/modules/dev;/mnt/win_d/projects/ACENetwork/test_u/file_server/../../../ACEStream/src/modules/file;/mnt/win_d/projects/ACENetwork/test_u/file_server/../../../ACEStream/src/modules/lib;/mnt/win_d/projects/ACENetwork/test_u/file_server/../../../ACEStream/src/modules/misc;/mnt/win_d/projects/ACENetwork/test_u/file_server/../../../ACEStream/src/modules/net;/mnt/win_d/projects/ACENetwork/test_u/file_server/../../../ACEStream/src/modules/stat;/mnt/win_d/projects/ACENetwork/test_u/file_server/../../src;/mnt/win_d/projects/ACENetwork/test_u/file_server/../../src/client_server;/mnt/win_d/projects/ACENetwork/test_u/file_server/..;/mnt/win_d/projects/ACENetwork/clang/test_u/file_server/../..;/media/erik/USB_BLACK/lib/ACE_TAO/ACE;/mnt/win_d/projects/Common/src;/mnt/win_d/projects/Common/src/error;/mnt/win_d/projects/Common/src/signal;/mnt/win_d/projects/Common/src/timer;/mnt/win_d/projects/Common/cmake/src/..;/mnt/win_d/projects/Common/src/log/..;/mnt/win_d/projects/Common/cmake/src/log/../..;/mnt/win_d/projects/Common/cmake/src/signal/../..;/mnt/win_d/projects/Common/src/signal/..;/mnt/win_d/projects/Common/src/timer/..;/mnt/win_d/projects/Common/cmake/src/timer/../..;/mnt/win_d/projects/ACEStream/src/../../Common/cmake;/mnt/win_d/projects/ACEStream/src/../../Common/src;/mnt/win_d/projects/ACEStream/src/../../Common/src/timer;/mnt/win_d/projects/ACEStream/src/../3rd_party/tree;/mnt/win_d/projects/ACEStream/cmake/src/..;/mnt/win_d/projects/ACEStream/src;/mnt/win_d/projects/ACEStream/src/modules/lib;/mnt/win_d/projects/ACEStream/src/modules/misc;/mnt/win_d/projects/ACEStream/src/modules/stat;/mnt/win_d/projects/ACEStream/src/modules/dec/../../../../Common/cmake;/mnt/win_d/projects/ACEStream/src/modules/dec/../../../../Common/src;/mnt/win_d/projects/ACEStream/src/modules/dec/../../../../Common/src/error;/mnt/win_d/projects/ACEStream/src/modules/dec/../../../../Common/src/image;/mnt/win_d/projects/ACEStream/src/modules/dec/../../../../Common/src/log;/mnt/win_d/projects/ACEStream/src/modules/dec/../../../../Common/src/timer;/mnt/win_d/projects/ACEStream/src/modules/dec/../../../../Common/src/ui;/mnt/win_d/projects/ACEStream/src/modules/dec/../../../3rd_party/bison;/mnt/win_d/projects/ACEStream/src/modules/dec/../..;/mnt/win_d/projects/ACEStream/src/modules/dec/../dev;/mnt/win_d/projects/ACEStream/src/modules/dec/../lib;/mnt/win_d/projects/ACEStream/src/modules/dec/../vis;/mnt/win_d/projects/ACEStream/src/modules/file/../../../../Common/cmake;/mnt/win_d/projects/ACEStream/src/modules/file/../../../../Common/src;/mnt/win_d/projects/ACEStream/src/modules/file/../../../../Common/src/error;/mnt/win_d/projects/ACEStream/src/modules/file/../../../../Common/src/timer;/mnt/win_d/projects/ACEStream/src/modules/file/../..;/mnt/win_d/projects/ACEStream/src/modules/file/../lib;/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src;/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/error;/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/image;/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/log;/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/timer;/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/ui;/mnt/win_d/projects/ACEStream/src/modules/misc/../../../3rd_party/bison;/mnt/win_d/projects/ACEStream/cmake/src/modules/misc/../../..;/mnt/win_d/projects/ACEStream/src/modules/misc/../..;/mnt/win_d/projects/ACEStream/src/modules/misc/../dec;/mnt/win_d/projects/ACEStream/src/modules/misc/../dev;/mnt/win_d/projects/ACEStream/src/modules/misc/../file;/mnt/win_d/projects/ACEStream/src/modules/misc/../lib;/mnt/win_d/projects/ACEStream/src/modules/net/../../../../Common/src;/mnt/win_d/projects/ACEStream/src/modules/net/../../../../Common/src/timer;/mnt/win_d/projects/ACEStream/src/modules/net/../../../../ACENetwork/src;/mnt/win_d/projects/ACEStream/src/modules/net/../../../../ACENetwork/src/client_server;/mnt/win_d/projects/ACEStream/cmake/src/modules/net/../../..;/mnt/win_d/projects/ACEStream/src/modules/net/../..;/mnt/win_d/projects/ACEStream/src/modules/net/../lib;/mnt/win_d/projects/ACEStream/src/modules/net/../stat;/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src;/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src/error;/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src/log;/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src/math;/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src/timer;/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src/ui;/mnt/win_d/projects/ACEStream/cmake/src/modules/stat/../../..;/mnt/win_d/projects/ACEStream/src/modules/stat/../..;/mnt/win_d/projects/ACEStream/src/modules/stat/../dev;/mnt/win_d/projects/ACEStream/src/modules/stat/../lib;/usr/include/libnl3;/media/erik/USB_BLACK/lib/ACE_TAO/ACE;/mnt/win_d/projects/ACENetwork/src/../../Common/cmake;/mnt/win_d/projects/ACENetwork/src/../../Common/src;/mnt/win_d/projects/ACENetwork/src/../../Common/src/timer;/mnt/win_d/projects/ACENetwork/src/../../ACEStream/cmake;/mnt/win_d/projects/ACENetwork/src/../../ACEStream/src;/mnt/win_d/projects/ACENetwork/src/../../ACEStream/src/modules/lib;/mnt/win_d/projects/ACENetwork/src/../../ACEStream/src/modules/stat;/mnt/win_d/projects/ACENetwork/src/../3rd_party/bison;/mnt/win_d/projects/ACENetwork/clang/src/..;/mnt/win_d/projects/ACENetwork/src/wlan;/mnt/win_d/projects/ACENetwork/src/client_server/../../../Common/cmake;/mnt/win_d/projects/ACENetwork/src/client_server/../../../Common/src;/mnt/win_d/projects/ACENetwork/src/client_server/../../../Common/src/log;/mnt/win_d/projects/ACENetwork/src/client_server/../../../Common/src/timer;/mnt/win_d/projects/ACENetwork/src/client_server/../../../ACEStream/cmake;/mnt/win_d/projects/ACENetwork/src/client_server/../../../ACEStream/src;/mnt/win_d/projects/ACENetwork/src/client_server/../../../ACEStream/src/modules/lib;/mnt/win_d/projects/ACENetwork/clang/src/client_server/../..;/mnt/win_d/projects/ACENetwork/src/client_server/../;/usr/include/x86_64-linux-gnu;/mnt/win_d/projects/ACENetwork/test_u//usr/include/gtk-2.0;/usr/lib/x86_64-linux-gnu/gtk-2.0/include;/usr/include/pango-1.0;/usr/include/atk-1.0;/usr/include/cairo;/usr/include/pixman-1;/usr/include/gdk-pixbuf-2.0;/usr/include/libmount;/usr/include/blkid;/usr/include/harfbuzz;/usr/include/fribidi;/usr/include/glib-2.0;/usr/lib/x86_64-linux-gnu/glib-2.0/include;/usr/include/uuid;/usr/include/freetype2;/usr/include/libpng16;/mnt/win_d/projects/ACENetwork/../Common/cmake;/mnt/win_d/projects/ACENetwork/../Common/src;/mnt/win_d/projects/ACENetwork/../Common/src/timer;/mnt/win_d/projects/ACENetwork/../Common/src/ui;/mnt/win_d/projects/ACENetwork/../Common/src/ui/gtk;/mnt/win_d/projects/ACENetwork/../ACEStream/cmake;/mnt/win_d/projects/ACENetwork/../ACEStream/src;/mnt/win_d/projects/ACENetwork/../ACEStream/src/modules/lib;/mnt/win_d/projects/ACENetwork/clang/test_u/..;/mnt/win_d/projects/ACENetwork/test_u/../src;/usr/include/libnl3;/usr/include/dbus-1.0;/usr/lib/x86_64-linux-gnu/dbus-1.0/include;/usr/include/libnm;/usr/lib/x86_64-linux-gnu/glib-2.0/include;/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/cmake;/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src;/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src/dbus;/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src/error;/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src/math;/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src/timer;/mnt/win_d/projects/ACENetwork/src/wlan/../../../Common/src/xml;/mnt/win_d/projects/ACENetwork/src/wlan/../../../ACEStream/cmake;/mnt/win_d/projects/ACENetwork/src/wlan/../../../ACEStream/src;/mnt/win_d/projects/ACENetwork/src/wlan/../../../ACEStream/src/modules/lib;/mnt/win_d/projects/ACENetwork/clang/src/wlan/../..;/mnt/win_d/projects/ACENetwork/src/wlan/..;/mnt/win_d/projects/ACENetwork/src/wlan/../protocol/dhcp;/usr/include/gtk-2.0;/usr/include/libpng16;/mnt/win_d/projects/Common/src/ui/..;/mnt/win_d/projects/Common/src/ui/../error;/mnt/win_d/projects/Common/src/ui/../image;/mnt/win_d/projects/Common/src/ui/../log;/mnt/win_d/projects/Common/src/ui/../xml;/mnt/win_d/projects/Common/cmake/src/ui/../..;/usr/include/x86_64-linux-gnu//ImageMagick-6;/usr/include/ImageMagick-6;/mnt/win_d/projects/Common/src/gl/..;/mnt/win_d/projects/Common/src/gl/../image;/mnt/win_d/projects/Common/cmake/src/gl/../..;/media/erik/USB_BLACK/lib/gtkglarea;/mnt/win_d/projects/Common/cmake/src/ui/gtk/../../..;/mnt/win_d/projects/Common/src/ui/gtk/..;/mnt/win_d/projects/Common/src/ui/gtk/../..;/mnt/win_d/projects/Common/src/ui/gtk/../../image;/mnt/win_d/projects/Common/src/ui/gtk/../../log;/mnt/win_d/projects/Common/src/ui/gtk/../../timer;/usr/include/dbus-1.0;/usr/include/systemd;/mnt/win_d/projects/Common/src/dbus/..;/mnt/win_d/projects/Common/cmake/src/dbus/../..;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../Common/cmake;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../Common/src;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../Common/src/timer;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../ACEStream/cmake;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../ACEStream/src;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../ACEStream/src/modules/lib;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../../ACEStream/src/modules/stat;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../../3rd_party/bison;/mnt/win_d/projects/ACENetwork/clang/src/protocol/dhcp/../../..;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../..;/mnt/win_d/projects/ACENetwork/src/protocol/dhcp/../../client_server")
set (COTIRE_TARGET_INCLUDE_PRIORITY_PATH "")
set (COTIRE_TARGET_LANGUAGE "CXX")
set (COTIRE_TARGET_MAXIMUM_NUMBER_OF_INCLUDES "-j")
set (COTIRE_TARGET_POST_UNDEFS "")
set (COTIRE_TARGET_PRE_UNDEFS "")
set (COTIRE_TARGET_SOURCES "file_server.cpp;file_server_signalhandler.cpp;test_u_message.cpp;test_u_eventhandler.cpp;test_u_module_eventhandler.cpp;test_u_module_headerparser.cpp;test_u_module_protocolhandler.cpp;test_u_sessionmessage.cpp;test_u_stream.cpp;test_u_tcpconnection.cpp;stdafx.cpp;test_u_callbacks.cpp")
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
