﻿add_library (${PROJECT_NAME}_Client STATIC "")
add_library (${PROJECT_NAME}_Server STATIC "")

if (SSL_SUPPORT)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC ${SSL_INCLUDE_DIRS})
 target_include_directories (${PROJECT_NAME}_Server PUBLIC ${SSL_INCLUDE_DIRS})
endif (SSL_SUPPORT)
if (UNIX)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC ${BIND9_CFLAGS})
 target_include_directories (${PROJECT_NAME}_Server PUBLIC ${BIND9_CFLAGS})

 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)
#                             $<BUILD_INTERFACE:/usr/local/src/ACE_wrappers>)
 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)
#                             $<BUILD_INTERFACE:/usr/local/src/ACE_wrappers>)

 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/cmake>)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src>)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/parser>)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/signal>)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/task>)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/timer>)

 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/cmake>)
 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src>)
 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log>)
 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/parser>)
 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/signal>)
 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/timer>)

 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/cmake>)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src>)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/lib>)

 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/cmake>)
 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src>)
 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/lib>)
elseif (WIN32)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC
#                             $<BUILD_INTERFACE:$ENV{ACE_ROOT}>)
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)
 target_include_directories (${PROJECT_NAME}_Server PUBLIC
#                             $<BUILD_INTERFACE:$ENV{ACE_ROOT}>)
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)

 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/cmake>)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src>)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/error>)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/parser>)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/signal>)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/task>)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/timer>)

 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/cmake>)
 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src>)
 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log>)
 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/parser>)
 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/timer>)

 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/cmake>)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src>)
 target_include_directories (${PROJECT_NAME}_Client PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/lib>)

 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/cmake>)
 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src>)
 target_include_directories (${PROJECT_NAME}_Server PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/lib>)
endif ()
target_include_directories (${PROJECT_NAME}_Client PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>)
target_include_directories (${PROJECT_NAME}_Client PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>)

target_include_directories (${PROJECT_NAME}_Server PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>)
target_include_directories (${PROJECT_NAME}_Server PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>)

############## next target ################
set (source_files_client
     net_client_asynchconnector.cpp
     net_client_asynchconnector.h
     net_client_asynchconnector.inl
     net_client_common.h
     net_client_connector.cpp
     net_client_connector.h
     net_client_connector.inl
     net_client_defines.h
#     net_client_exports.h
     net_client_ssl_connector.cpp
     net_client_ssl_connector.h
     net_client_ssl_connector.inl)

#set_target_properties (${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

foreach (source_file ${source_files_client})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files_client
       PRIVATE ${abs_source_file}
       )
#       $<INSTALL_INTERFACE:${INCLUDE_DIR}/${source_file}>)
endforeach ()
target_sources (${PROJECT_NAME}_Client
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files_client})

set_target_properties (${PROJECT_NAME}_Client PROPERTIES DEFINE_SYMBOL NET_CLIENT_BUILD_DLL)
set_target_properties (${PROJECT_NAME}_Client PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
set_target_properties (${PROJECT_NAME}_Client PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
#set_target_properties (${PROJECT_NAME}_Client PROPERTIES COTIRE_ENABLE_PRECOMPILED_HEADER FALSE)
#cotire (${PROJECT_NAME}_Client)

############## next target ################

set (source_files_server
     net_server_asynchlistener.cpp
     net_server_asynchlistener.h
     net_server_asynchlistener.inl
     net_server_common.h
     net_server_common_tools.cpp
     net_server_common_tools.h
     net_server_defines.h
#     net_server_exports.h
     net_server_listener.cpp
     net_server_listener.h
     net_server_listener.inl
     net_server_ssl_listener.cpp
     net_server_ssl_listener.h
     net_server_ssl_listener.inl)

#set_target_properties (${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

foreach (source_file ${source_files_server})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files_server
       PRIVATE ${abs_source_file}
       )
#       $<INSTALL_INTERFACE:${INCLUDE_DIR}/${source_file}>)
endforeach ()
target_sources (${PROJECT_NAME}_Server
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files_server})

if (WIN32)
 set_target_properties (${PROJECT_NAME}_Server PROPERTIES DEFINE_SYMBOL _CRT_SECURE_NO_WARNINGS)

 target_link_libraries (${PROJECT_NAME}_Server ${ACE_LIBRARY})
 target_link_libraries (${PROJECT_NAME}_Server Common)
endif ()

set_target_properties (${PROJECT_NAME}_Server PROPERTIES DEFINE_SYMBOL NET_SERVER_BUILD_DLL)
set_target_properties (${PROJECT_NAME}_Server PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
set_target_properties (${PROJECT_NAME}_Server PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
set_target_properties (${PROJECT_NAME}_Server PROPERTIES COTIRE_ENABLE_PRECOMPILED_HEADER FALSE)
cotire (${PROJECT_NAME}_Client ${PROJECT_NAME}_Server)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}_Client ${PROJECT_NAME}_Server
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         net_client_asynchconnector.h
         net_client_common.h
         net_client_connector.h
         net_client_defines.h
#         net_client_exports.h
         net_server_asynchlistener.h
         net_server_asynchlistener.inl
         net_server_common.h
         net_server_common_tools.h
         net_server_defines.h
#         net_server_exports.h
         net_server_listener.h
         net_server_listener.inl
         net_server_ssl_listener.h
         net_server_ssl_listener.inl
         DESTINATION include)
install (TARGETS ${PROJECT_NAME}_Client ${PROJECT_NAME}_Server
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
         LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
         RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
         COMPONENT dev)
