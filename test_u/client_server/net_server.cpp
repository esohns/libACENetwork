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

#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

//#include "ace/streams.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif
#include "ace/Log_Msg.h"
#include "ace/Synch.h"
#include "ace/Proactor.h"
#include "ace/Profile_Timer.h"
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#include "ace/POSIX_Proactor.h"
#endif
#include "ace/Reactor.h"
#include "ace/Sig_Handler.h"
#include "ace/Signal.h"
#include "ace/Version.h"

#include "common_file_tools.h"
#include "common_logger.h"
#include "common_tools.h"

#include "common_ui_defines.h"
//#include "common_ui_glade_definition.h"
#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_manager_common.h"

#include "stream_allocatorheap.h"

#include "net_common_tools.h"
#include "net_macros.h"

#ifdef HAVE_CONFIG_H
#include "libACENetwork_config.h"
#endif

#include "test_u_callbacks.h"
#include "test_u_common.h"
#include "test_u_connection_manager_common.h"
#include "test_u_defines.h"
#include "test_u_stream.h"
#include "test_u_eventhandler.h"
#include "test_u_module_eventhandler.h"

#include "net_client_defines.h"

#include "net_server_common.h"
#include "net_server_common_tools.h"
#include "net_server_defines.h"
#include "net_server_listener_common.h"
#include "net_server_signalhandler.h"

// globals
unsigned int random_seed;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
struct random_data random_data;
char random_state_buffer[BUFSIZ];
#endif

const char stream_name_string_[] = ACE_TEXT_ALWAYS_CHAR ("NetServerStream");

void
do_printUsage (const std::string& programName_in)
{
  NETWORK_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf (std::ios::boolalpha);

  std::string configuration_path =
    Common_File_Tools::getWorkingDirectory ();

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:") << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-c [VALUE]   : maximum number of (concurrent) connections [")
            << NET_SERVER_MAXIMUM_NUMBER_OF_OPEN_CONNECTIONS
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_DIRECTORY);
  std::string UI_file = path;
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UI_file += ACE_TEXT_ALWAYS_CHAR (NET_SERVER_UI_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-g[[STRING]] : UI definition file [\"")
            << path
            << ACE_TEXT_ALWAYS_CHAR ("\"] {\"\": no GUI}")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-h           : use thread-pool [")
            << NET_EVENT_USE_THREAD_POOL
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-i [VALUE]   : client ping interval (ms) [")
            << NET_SERVER_DEFAULT_CLIENT_PING_INTERVAL
            << ACE_TEXT_ALWAYS_CHAR ("] {0: off})")
            << std::endl;
//  std::cout << ACE_TEXT_ALWAYS_CHAR("-k [VALUE]  : client keep-alive timeout ([")
//            << NET_SERVER_DEF_CLIENT_KEEPALIVE
//            << ACE_TEXT_ALWAYS_CHAR("] second(s) {0 --> no timeout})")
//            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-l           : log to a file [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m           : receive uni/multi/broadcast UDP [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#else
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-n [STRING]  : network interface identifier [\"")
            << ACE_TEXT_ALWAYS_CHAR (NET_INTERFACE_DEFAULT_ETHERNET)
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
#endif
  // *TODO*: this doesn't really make sense (see '-n' option)
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-o           : use loopback [")
            << NET_INTERFACE_DEFAULT_USE_LOOPBACK
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-p [VALUE]   : listening port [")
            << NET_SERVER_DEFAULT_LISTENING_PORT
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-r           : use reactor [")
            << NET_EVENT_USE_REACTOR
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-s [VALUE]   : statistic reporting interval (second(s)) [")
            << NET_SERVER_DEFAULT_STATISTIC_REPORTING_INTERVAL
            << ACE_TEXT_ALWAYS_CHAR ("] {0: off})")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t           : trace information")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-u           : use UDP [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-v           : print version information and exit")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-x [VALUE]   : #dispatch threads [")
            << NET_SERVER_DEFAULT_NUMBER_OF_DISPATCHING_THREADS
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_processArguments (const int& argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     unsigned int& maximumNumberOfConnections_out,
                     std::string& UIFile_out,
                     bool& useThreadPool_out,
                     ACE_Time_Value& pingInterval_out,
                     //unsigned int& keepAliveTimeout_out,
                     bool& logToFile_out,
                     std::string& networkInterface_out,
                     bool& useLoopBack_out,
                     unsigned short& listeningPortNumber_out,
                     bool& useReactor_out,
                     unsigned int& statisticReportingInterval_out,
                     bool& traceInformation_out,
                     bool& useUDP_out,
                     bool& printVersionAndExit_out,
                     unsigned int& numberOfDispatchThreads_out)
{
  NETWORK_TRACE (ACE_TEXT ("::do_processArguments"));

  std::string configuration_path =
    Common_File_Tools::getWorkingDirectory ();

  // initialize results
  maximumNumberOfConnections_out =
    NET_SERVER_MAXIMUM_NUMBER_OF_OPEN_CONNECTIONS;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_DIRECTORY);
  UIFile_out = path;
  UIFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UIFile_out += ACE_TEXT_ALWAYS_CHAR (NET_SERVER_UI_FILE);
  useThreadPool_out = NET_EVENT_USE_THREAD_POOL;
  pingInterval_out.set (NET_SERVER_DEFAULT_CLIENT_PING_INTERVAL / 1000,
                        (NET_SERVER_DEFAULT_CLIENT_PING_INTERVAL % 1000) * 1000);
//  keepAliveTimeout_out = NET_SERVER_DEF_CLIENT_KEEPALIVE;
  logToFile_out = false;
  useUDP_out = false;
  networkInterface_out =
    ACE_TEXT_ALWAYS_CHAR (NET_INTERFACE_DEFAULT_ETHERNET);
  useLoopBack_out = NET_INTERFACE_DEFAULT_USE_LOOPBACK;
  listeningPortNumber_out = NET_SERVER_DEFAULT_LISTENING_PORT;
  useReactor_out = NET_EVENT_USE_REACTOR;
  statisticReportingInterval_out =
      NET_SERVER_DEFAULT_STATISTIC_REPORTING_INTERVAL;
  traceInformation_out = false;
  useUDP_out = false;
  printVersionAndExit_out = false;
  numberOfDispatchThreads_out =
    NET_SERVER_DEFAULT_NUMBER_OF_DISPATCHING_THREADS;

  ACE_Get_Opt argument_parser (argc_in,                                // argc
                               argv_in,                                // argv
                               ACE_TEXT ("c:g::hi:k:lmn:op:rs:tuvx:"), // optstring
                               1,                                      // skip_args
                               1,                                      // report_errors
                               ACE_Get_Opt::PERMUTE_ARGS,              // ordering
                               0);                                     // long_only
  int option = 0;
  std::stringstream converter;
  while ((option = argument_parser ()) != EOF)
  {
    switch (option)
    {
      case 'c':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argument_parser.opt_arg ();
        converter >> maximumNumberOfConnections_out;
        break;
      }
      case 'g':
      {
        ACE_TCHAR* opt_arg = argument_parser.opt_arg ();
        if (opt_arg)
          UIFile_out = ACE_TEXT_ALWAYS_CHAR (opt_arg);
        else
          UIFile_out.clear ();
        break;
      }
      case 'h':
      {
        useThreadPool_out = true;
        break;
      }
      case 'i':
      {
        unsigned int ping_interval = 0;
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argument_parser.opt_arg ();
        converter >> ping_interval;
        pingInterval_out.set (ping_interval / 1000,
                              (ping_interval % 1000) * 1000);
        break;
      }
//      case 'k':
//      {
//        converter.clear();
//        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
//        converter << argument_parser.opt_arg();
//        converter >> keepAliveTimeout_out;
//        break;
//      }
      case 'l':
      {
        logToFile_out = true;
        break;
      }
      case 'm':
      {
        useUDP_out = true;
        break;
      }
      case 'n':
      {
        networkInterface_out =
          ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
        break;
      }
      case 'o':
      {
        useLoopBack_out = true;
        break;
      }
      case 'p':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argument_parser.opt_arg ();
        converter >> listeningPortNumber_out;
        break;
      }
      case 'r':
      {
        useReactor_out = true;
        break;
      }
      case 's':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argument_parser.opt_arg ();
        converter >> statisticReportingInterval_out;
        break;
      }
      case 't':
      {
        traceInformation_out = true;
        break;
      }
      case 'u':
      {
        useUDP_out = true;
        break;
      }
      case 'v':
      {
        printVersionAndExit_out = true;
        break;
      }
      case 'x':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argument_parser.opt_arg ();
        converter >> numberOfDispatchThreads_out;
        break;
      }
      // error handling
      case '?':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%s\", aborting\n"),
                    ACE_TEXT (argument_parser.last_option ())));
        return false;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%c\", continuing\n"),
                    option));
        break;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

void
do_initializeSignals (bool useReactor_in,
                      bool allowUserRuntimeStats_in,
                      ACE_Sig_Set& signals_out,
                      ACE_Sig_Set& ignoredSignals_out)
{
  NETWORK_TRACE (ACE_TEXT ("::do_initializeSignals"));

  int result = -1;

  // initialize return value(s)
  result = signals_out.empty_set ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", returning\n")));
    return;
  } // end IF
  result = ignoredSignals_out.empty_set ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", returning\n")));
    return;
  } // end IF

  // *PORTABILITY*: on Windows most signals are not defined,
  // and ACE_Sig_Set::fill_set() doesn't really work as specified
  // --> add valid signals (see <signal.h>)...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  signals_out.sig_add (SIGINT);            // 2       /* interrupt */
  signals_out.sig_add (SIGILL);            // 4       /* illegal instruction - invalid function image */
  signals_out.sig_add (SIGFPE);            // 8       /* floating point exception */
  //  signals_out.sig_add(SIGSEGV);          // 11      /* segment violation */
  signals_out.sig_add (SIGTERM);           // 15      /* Software termination signal from kill */
  if (allowUserRuntimeStats_in)
  {
    signals_out.sig_add (SIGBREAK);        // 21      /* Ctrl-Break sequence */
    ignoredSignals_out.sig_add (SIGBREAK); // 21      /* Ctrl-Break sequence */
  } // end IF
  signals_out.sig_add (SIGABRT);           // 22      /* abnormal termination triggered by abort call */
  signals_out.sig_add (SIGABRT_COMPAT);    // 6       /* SIGABRT compatible with other platforms, same as SIGABRT */
#else
  result = signals_out.fill_set ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Set::fill_set(): \"%m\", returning\n")));
    return;
  } // end IF
  // *NOTE*: cannot handle some signals --> registration fails for these...
  signals_out.sig_del (SIGKILL);           // 9       /* Kill signal */
  // ---------------------------------------------------------------------------
  if (!allowUserRuntimeStats_in)
  {
    signals_out.sig_del (SIGUSR1);         // 10      /* User-defined signal 1 */
    ignoredSignals_out.sig_add (SIGUSR1);  // 10      /* User-defined signal 1 */
  } // end IF
  // *NOTE* core dump on SIGSEGV
  signals_out.sig_del (SIGSEGV);           // 11      /* Segmentation fault: Invalid memory reference */
  // *NOTE* don't care about SIGPIPE
  signals_out.sig_del (SIGPIPE);           // 12      /* Broken pipe: write to pipe with no readers */
  signals_out.sig_del (SIGSTOP);           // 19      /* Stop process */

  // *IMPORTANT NOTE*: "...NPTL makes internal use of the first two real-time
  //                   signals (see also signal(7)); these signals cannot be
  //                   used in applications. ..." (see 'man 7 pthreads')
  // --> on POSIX platforms, make sure that ACE_SIGRTMIN == 34
//  for (int i = ACE_SIGRTMIN;
//       i <= ACE_SIGRTMAX;
//       i++)
//    signals_out.sig_del (i);

  if (!useReactor_in)
  {
    ACE_Proactor* proactor_p = ACE_Proactor::instance ();
    ACE_ASSERT (proactor_p);
    ACE_POSIX_Proactor* proactor_impl_p =
        dynamic_cast<ACE_POSIX_Proactor*> (proactor_p->implementation ());
    ACE_ASSERT (proactor_impl_p);
    if (proactor_impl_p->get_impl_type () == ACE_POSIX_Proactor::PROACTOR_SIG)
      signals_out.sig_del (COMMON_EVENT_PROACTOR_SIG_RT_SIGNAL);
  } // end IF
#endif
}

void
do_work (unsigned int maximumNumberOfConnections_in,
         const std::string& UIDefinitionFile_in,
         bool useThreadPool_in,
         const ACE_Time_Value& pingInterval_in,
         //unsigned int keepAliveTimeout_in,
         const std::string& networkInterface_in,
         bool useLoopBack_in,
         unsigned short listeningPortNumber_in,
         bool useReactor_in,
         unsigned int statisticReportingInterval_in,
         unsigned int numberOfDispatchThreads_in,
         bool useUDP_in,
         struct Test_U_Server_GTK_CBData& CBData_in,
         const ACE_Sig_Set& signalSet_in,
         const ACE_Sig_Set& ignoredSignalSet_in,
         Common_SignalActions_t& previousSignalActions_inout,
         Test_U_Server_SignalHandler& signalHandler_in)
{
  NETWORK_TRACE (ACE_TEXT ("::do_work"));

  int result = -1;
  Test_U_InetConnectionManager_t* connection_manager_p =
    TEST_U_CONNECTIONMANAGER_SINGLETON::instance ();
  ACE_ASSERT (connection_manager_p);

  // step0a: initialize configuration
  struct Test_U_Server_Configuration configuration;
  CBData_in.configuration = &configuration;

  Test_U_EventHandler ui_event_handler (&CBData_in);
  Test_U_Module_EventHandler_Module event_handler (NULL,
                                                   ACE_TEXT_ALWAYS_CHAR ("EventHandler"));
  Test_U_Module_EventHandler* event_handler_p =
    dynamic_cast<Test_U_Module_EventHandler*> (event_handler.writer ());
  if (!event_handler_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Test_U_Module_EventHandler> failed, returning\n")));
    return;
  } // end IF

  Stream_AllocatorHeap_T<ACE_MT_SYNCH,
                         struct Stream_AllocatorConfiguration> heap_allocator;
  if (!heap_allocator.initialize (configuration.allocatorConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize allocator, returning\n")));
    return;
  } // end IF
  Test_U_StreamMessageAllocator_t message_allocator (NET_STREAM_MAX_MESSAGES, // maximum #buffers
                                                     &heap_allocator,         // heap allocator handle
                                                     true);                   // block ?

  // ******************** protocol configuration data **************************
  configuration.protocolConfiguration.pingInterval = pingInterval_in;
  configuration.protocolConfiguration.printPongMessages =
    UIDefinitionFile_in.empty ();
  // ********************** stream configuration data **************************
  configuration.streamConfiguration.configuration_.cloneModule =
    !(UIDefinitionFile_in.empty ());
  configuration.streamConfiguration.configuration_.messageAllocator =
    &message_allocator;
  configuration.streamConfiguration.configuration_.module =
    (!UIDefinitionFile_in.empty () ? &event_handler
                                   : NULL);

  struct Stream_ModuleConfiguration module_configuration;
  struct Test_U_ModuleHandlerConfiguration modulehandler_configuration;
  modulehandler_configuration.printFinalReport = true;
  modulehandler_configuration.protocolConfiguration =
    &configuration.protocolConfiguration;
  modulehandler_configuration.statisticReportingInterval =
    statisticReportingInterval_in;
  modulehandler_configuration.streamConfiguration =
    &configuration.streamConfiguration;
  modulehandler_configuration.subscriber = &ui_event_handler;
  modulehandler_configuration.subscribers = &CBData_in.subscribers;
  modulehandler_configuration.subscribersLock = &CBData_in.subscribersLock;

  configuration.streamConfiguration.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (""),
                                                            std::make_pair (module_configuration,
                                                                            modulehandler_configuration)));
  // *TODO*: is this correct ?
  configuration.streamConfiguration.configuration_.serializeOutput =
    useThreadPool_in;
  configuration.streamConfiguration.configuration_.userData =
    &configuration.userData;

  // ********************** connection configuration data **********************
  struct Test_U_ConnectionConfiguration connection_configuration;
  connection_configuration.socketHandlerConfiguration.userData =
    &configuration.userData;
  connection_configuration.messageAllocator = &message_allocator;
  connection_configuration.streamConfiguration =
    &configuration.streamConfiguration;
  connection_configuration.userData = &configuration.userData;
  configuration.connectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (""),
                                                                 connection_configuration));
  Test_U_ConnectionConfigurationIterator_t iterator =
    configuration.connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != configuration.connectionConfigurations.end ());
  (*iterator).second.socketHandlerConfiguration.connectionConfiguration =
    &((*iterator).second);
  if (useUDP_in)
    (*iterator).second.socketHandlerConfiguration.socketConfiguration =
      &((*iterator).second.socketHandlerConfiguration.socketConfiguration_3);

  //  config.delete_module = false;
  // *WARNING*: set at runtime, by the appropriate connection handler
  //  config.sessionID = 0; // (== socket handle !)
  //  config.statisticsReportingInterval = 0; // == off
  //	config.printFinalReport = false;
  // ************ runtime data ************
  //	config.currentStatistics = {};
  //	config.lastCollectionTimestamp = ACE_Time_Value::zero;

  // step0b: initialize event dispatch
  struct Common_DispatchThreadData thread_data;
  thread_data.numberOfDispatchThreads = numberOfDispatchThreads_in;
  thread_data.useReactor = useReactor_in;
  if (!Common_Tools::initializeEventDispatch (thread_data.useReactor,
                                              useThreadPool_in,
                                              thread_data.numberOfDispatchThreads,
                                              thread_data.proactorType,
                                              thread_data.reactorType,
                                              configuration.streamConfiguration.configuration_.serializeOutput))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeEventDispatch(), returning\n")));
    return;
  } // end IF

  // step1: initialize regular (global) statistics reporting
  Common_Timer_Manager_t* timer_manager_p =
      COMMON_TIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (timer_manager_p);
  struct Common_TimerConfiguration timer_configuration;
  timer_manager_p->initialize (timer_configuration);
  timer_manager_p->start ();
  Net_StatisticHandler_t statistic_handler (STATISTIC_ACTION_REPORT,
                                            connection_manager_p,
                                            false);
  long timer_id = -1;
  if (statisticReportingInterval_in)
  {
    ACE_Time_Value interval (statisticReportingInterval_in,
                             0);
    timer_id =
      timer_manager_p->schedule_timer (&statistic_handler,         // event handler handle
                                       NULL,                       // asynchronous completion token
                                       COMMON_TIME_NOW + interval, // first wakeup time
                                       interval);                  // interval
    if (timer_id == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to schedule timer: \"%m\", returning\n")));

      // clean up
      timer_manager_p->stop ();

      return;
    } // end IF
  } // end IF

  // step2: signal handling
  struct Test_U_Server_SignalHandlerConfiguration signal_handler_configuration;
  if (!useUDP_in)
  {
    if (useReactor_in)
      CBData_in.configuration->listener =
        TEST_U_SERVER_LISTENER_SINGLETON::instance ();
    else
      CBData_in.configuration->listener =
        TEST_U_SERVER_ASYNCHLISTENER_SINGLETON::instance ();
    signal_handler_configuration.listener =
      CBData_in.configuration->listener;
  } // end IF
  signal_handler_configuration.statisticReportingHandler =
    connection_manager_p;
  signal_handler_configuration.statisticReportingTimerId = timer_id;
  signal_handler_configuration.useReactor = useReactor_in;
  if (!signalHandler_in.initialize (signal_handler_configuration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize signal handler, returning\n")));

    // clean up
    timer_manager_p->stop ();

    return;
  } // end IF
  if (!Common_Tools::initializeSignals ((useReactor_in ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                       : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                        signalSet_in,
                                        ignoredSignalSet_in,
                                        &signalHandler_in,
                                        previousSignalActions_inout))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeSignals(), returning\n")));

    // clean up
    timer_manager_p->stop ();

    return;
  } // end IF

  // step3: initialize connection manager
  struct Test_U_UserData user_data;
  connection_manager_p->initialize (maximumNumberOfConnections_in);
  connection_manager_p->set ((*iterator).second,
                             &user_data);

  // step4: handle events (signals, incoming connections/data, timers, ...)
  // reactor/proactor event loop:
  // - dispatch connection attempts to acceptor
  // - dispatch socket events
  // timer events:
  // - perform statistics collecting/reporting
  // [GTK events:]
  // - dispatch UI events (if any)

  // step4a: start GTK event loop ?
  Test_U_Server_GTK_Manager_t* gtk_manager_p = NULL;
  if (!UIDefinitionFile_in.empty ())
  {
    CBData_in.finalizationHook = idle_finalize_UI_cb;
    CBData_in.initializationHook = idle_initialize_server_UI_cb;
    //CBData_in.gladeXML[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN)] =
    //  std::make_pair (UIDefinitionFile_in, static_cast<GladeXML*> (NULL));
    CBData_in.builders[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN)] =
      std::make_pair (UIDefinitionFile_in, static_cast<GtkBuilder*> (NULL));
    CBData_in.userData = &CBData_in;

    gtk_manager_p = SERVER_UI_GTK_MANAGER_SINGLETON::instance ();
    ACE_ASSERT (gtk_manager_p);
    gtk_manager_p->start ();
    ACE_Time_Value timeout (0,
                            COMMON_UI_GTK_TIMEOUT_DEFAULT_MANAGER_INITIALIZATION * 1000);
    result = ACE_OS::sleep (timeout);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::sleep(%#T): \"%m\", continuing\n"),
                  &timeout));
    if (!gtk_manager_p->isRunning ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to start GTK event dispatch, returning\n")));

      // clean up
      timer_manager_p->stop ();

      return;
    } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
    HWND window_p = GetConsoleWindow ();
    if (!window_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::GetConsoleWindow(), returning\n")));

      // clean up
      timer_manager_p->stop ();
      gtk_manager_p->stop (true);

      return;
    } // end IF
    BOOL was_visible_b = ShowWindow (window_p, SW_HIDE);
#endif
  } // end IF

  // step4b: initialize worker(s)
  int group_id = -1;
  if (!Common_Tools::startEventDispatch (thread_data,
                                         group_id))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start event dispatch, returning\n")));

    // clean up
    //		{ // synch access
    //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

    //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
    //					 iterator != CBData_in.event_source_ids.end();
    //					 iterator++)
    //				g_source_remove(*iterator);
    //		} // end lock scope
    if (!UIDefinitionFile_in.empty ())
      gtk_manager_p->stop ();
    timer_manager_p->stop ();

    return;
  } // end IF

  // step4c: start listening
  configuration.listenerConfiguration.connectionConfiguration =
    &((*iterator).second);
  //if (networkInterface_in.empty ()); // *TODO*
  if (useLoopBack_in)
  {
    if (useUDP_in)
    {
      result =
        (*iterator).second.socketHandlerConfiguration.socketConfiguration_3.listenAddress.set (listeningPortNumber_in,
                                                                                               INADDR_LOOPBACK,
                                                                                               1,
                                                                                               0);
      result =
        (*iterator).second.socketHandlerConfiguration.socketConfiguration_3.peerAddress.set (listeningPortNumber_in,
                                                                                             INADDR_LOOPBACK,
                                                                                             1,
                                                                                             0);
    } // end IF
    else
      result =
        (*iterator).second.socketHandlerConfiguration.socketConfiguration_2.address.set (listeningPortNumber_in,
                                                                                         INADDR_LOOPBACK,
                                                                                         1,
                                                                                         0);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::set(): \"%m\", returning\n")));

      // clean up
      //		{ // synch access
      //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

      //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
      //					 iterator != CBData_in.event_source_ids.end();
      //					 iterator++)
      //				g_source_remove(*iterator);
      //		} // end lock scope
      if (!UIDefinitionFile_in.empty ())
        gtk_manager_p->stop ();
      timer_manager_p->stop ();

      return;
    } // end IF
  } // end IF
  else
  {
    if (useUDP_in)
    {
      (*iterator).second.socketHandlerConfiguration.socketConfiguration_3.listenAddress.set_port_number (listeningPortNumber_in,
                                                                                                         1);
      (*iterator).second.socketHandlerConfiguration.socketConfiguration_3.peerAddress.set_port_number (listeningPortNumber_in,
                                                                                                       1);
    } // end IF
    else
      (*iterator).second.socketHandlerConfiguration.socketConfiguration_2.address.set_port_number (listeningPortNumber_in,
                                                                                                   1);
  } // end ELSE
  configuration.listenerConfiguration.connectionManager = connection_manager_p;
  //configuration.listenerConfiguration.useLoopBackDevice = useLoopBack_in;

  Test_U_Server_AsynchUDPConnector_t udp_asynch_connector;
  Test_U_Server_UDPConnector_t udp_connector;
  if (!useUDP_in)
  {
    ACE_ASSERT (CBData_in.configuration->listener);
    if (!CBData_in.configuration->listener->initialize (configuration.listenerConfiguration))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize listener, returning\n")));

      // clean up
      Common_Tools::finalizeEventDispatch (useReactor_in,
                                           !useReactor_in,
                                           group_id);
      //		{ // synch access
      //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

      //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
      //					 iterator != CBData_in.event_source_ids.end();
      //					 iterator++)
      //				g_source_remove(*iterator);
      //		} // end lock scope
      if (!UIDefinitionFile_in.empty ())
        gtk_manager_p->stop ();
      timer_manager_p->stop ();

      return;
    } // end IF
    CBData_in.configuration->listener->start ();
    if (!CBData_in.configuration->listener->isRunning ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to start listener (port: %u), returning\n"),
                  listeningPortNumber_in));

      // clean up
      Common_Tools::finalizeEventDispatch (useReactor_in,
                                           !useReactor_in,
                                           group_id);
      //		{ // synch access
      //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

      //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
      //					 iterator != CBData_in.event_source_ids.end();
      //					 iterator++)
      //				g_source_remove(*iterator);
      //		} // end lock scope
      if (!UIDefinitionFile_in.empty ())
        gtk_manager_p->stop ();
      timer_manager_p->stop ();

      return;
    } // end IF
  } // end IF
  else
  {
    if (useReactor_in)
      CBData_in.configuration->connector = &udp_connector;
    else
      CBData_in.configuration->connector = &udp_asynch_connector;
    if (!CBData_in.configuration->connector->initialize ((*iterator).second))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize connector, returning\n")));

      // clean up
      Common_Tools::finalizeEventDispatch (useReactor_in,
                                           !useReactor_in,
                                           group_id);
      //		{ // synch access
      //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

      //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
      //					 iterator != CBData_in.event_source_ids.end();
      //					 iterator++)
      //				g_source_remove(*iterator);
      //		} // end lock scope
      if (!UIDefinitionFile_in.empty ())
        gtk_manager_p->stop ();
      timer_manager_p->stop ();

      return;
    } // end IF

    ACE_HANDLE handle_h =
      CBData_in.configuration->connector->connect ((*iterator).second.socketHandlerConfiguration.socketConfiguration_3.listenAddress);
    typename Test_U_InetConnectionManager_t::ICONNECTION_T* iconnection_p =
      NULL;
    if (CBData_in.configuration->connector->useReactor ())
    {
      if (handle_h != ACE_INVALID_HANDLE)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        iconnection_p =
          connection_manager_p->get (reinterpret_cast<Net_ConnectionId_t> (handle_h));
#else
        iconnection_p =
          connection_manager_p->get (static_cast<Net_ConnectionId_t> (handle_h));
#endif
    } // end IF
    else
    {
      // step1: wait for the connection to register with the manager
      // *TODO*: avoid these tight loops
      ACE_Time_Value timeout (NET_CLIENT_DEFAULT_ASYNCH_CONNECT_TIMEOUT, 0);
      ACE_Time_Value deadline = COMMON_TIME_NOW + timeout;
      // *TODO*: this may not be accurate/applicable for/to all protocols
      do
      {
        // *TODO*: avoid these tight loops
        iconnection_p =
          connection_manager_p->get ((*iterator).second.socketHandlerConfiguration.socketConfiguration_3.listenAddress,
                                     false);
        if (iconnection_p)
          break;
      } while (COMMON_TIME_NOW < deadline);
    } // end ELSE
    if (!iconnection_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to connect to %s, returning\n"),
                  ACE_TEXT (Net_Common_Tools::IPAddressToString ((*iterator).second.socketHandlerConfiguration.socketConfiguration_3.listenAddress).c_str ())));

      // clean up
      Common_Tools::finalizeEventDispatch (useReactor_in,
                                           !useReactor_in,
                                           group_id);
      //		{ // synch access
      //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

      //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
      //					 iterator != CBData_in.event_source_ids.end();
      //					 iterator++)
      //				g_source_remove(*iterator);
      //		} // end lock scope
      if (!UIDefinitionFile_in.empty ())
        gtk_manager_p->stop ();
      timer_manager_p->stop ();

      return;
    } // end IF
    iconnection_p->decrease ();
  } // end ELSE

  // *NOTE*: from this point on, clean up any remote connections !

  Common_Tools::dispatchEvents (useReactor_in,
                                group_id);

  // clean up
  // *NOTE*: listener has stopped, interval timer has been cancelled,
  // and connections have been aborted...
  //		{ // synch access
  //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

  //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
  //					 iterator != CBData_in.event_source_ids.end();
  //					 iterator++)
  //				g_source_remove(*iterator);
  //		} // end lock scope
  if (!UIDefinitionFile_in.empty ())
    gtk_manager_p->stop ();
  timer_manager_p->stop ();

  //// wait for connection processing to complete
  //NET_CONNECTIONMANAGER_SINGLETON::instance ()->abort ();
  //NET_CONNECTIONMANAGER_SINGLETON::instance ()->wait ();

  result = event_handler.close (ACE_Module_Base::M_DELETE_NONE);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to ACE_Module::close (): \"%m\", continuing\n"),
                event_handler.name ()));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
}

void
do_printVersion (const std::string& programName_in)
{
  NETWORK_TRACE (ACE_TEXT ("::do_printVersion"));

  std::ostringstream converter;

  // compiler version string...
  converter << ACE::compiler_major_version ();
  converter << ACE_TEXT (".");
  converter << ACE::compiler_minor_version ();
  converter << ACE_TEXT (".");
  converter << ACE::compiler_beta_version ();

  std::cout << programName_in
            << ACE_TEXT (" compiled on ")
            << ACE::compiler_name ()
            << ACE_TEXT (" ")
            << converter.str ()
            << std::endl << std::endl;

  std::cout << ACE_TEXT ("libraries: ")
            << std::endl
#ifdef HAVE_CONFIG_H
            << ACE_TEXT (LIBACENETWORK_PACKAGE)
            << ACE_TEXT (": ")
            << ACE_TEXT (LIBACENETWORK_PACKAGE_VERSION)
            << std::endl
#endif
  ;

  converter.str ("");
  // ACE version string...
  converter << ACE::major_version ();
  converter << ACE_TEXT (".");
  converter << ACE::minor_version ();
  converter << ACE_TEXT (".");
  converter << ACE::beta_version ();

  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta
  // version number... Need this, as the library soname is compared to this
  // string
  std::cout << ACE_TEXT ("ACE: ")
//             << ACE_VERSION
            << converter.str ()
            << std::endl;
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR** argv_in)
{
  NETWORK_TRACE (ACE_TEXT ("::main"));

  int result = -1;

  // step0: initialize
// *PORTABILITY*: on Windows, initialize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::init ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

  // *PROCESS PROFILE*
  ACE_Profile_Timer process_profile;
  // start profile timer...
  process_profile.start ();

  // initialize randomness
  // *TODO*: use STL functionality here
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("initializing random seed (RAND_MAX = %d)...\n"),
              RAND_MAX));
  ACE_Time_Value now = COMMON_TIME_NOW;
  random_seed = static_cast<unsigned int> (now.sec ());
  // *PORTABILITY*: outside glibc, this is not very portable...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  ACE_OS::memset (random_state_buffer, 0, sizeof (random_state_buffer));
  result = ::initstate_r (random_seed,
                          random_state_buffer, sizeof (random_state_buffer),
                          &random_data);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initstate_r(): \"%s\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
  result = ::srandom_r (random_seed, &random_data);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize random seed: \"%s\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#else
  ACE_OS::srand (static_cast<u_int> (random_seed));
#endif
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("initializing random seed...DONE\n")));

  std::string configuration_path =
    Common_File_Tools::getWorkingDirectory ();

  // step1a set defaults
  unsigned int maximum_number_of_connections =
    NET_SERVER_MAXIMUM_NUMBER_OF_OPEN_CONNECTIONS;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_DIRECTORY);
  std::string UI_file = path;
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UI_file += ACE_TEXT_ALWAYS_CHAR (NET_SERVER_UI_FILE);
  bool use_thread_pool = NET_EVENT_USE_THREAD_POOL;
  ACE_Time_Value ping_interval (NET_SERVER_DEFAULT_CLIENT_PING_INTERVAL / 1000,
                                (NET_SERVER_DEFAULT_CLIENT_PING_INTERVAL % 1000) * 1000);
  //  unsigned int keep_alive_timeout = NET_SERVER_DEFAULT_TCP_KEEPALIVE;
  bool log_to_file = false;
  std::string network_interface =
    ACE_TEXT_ALWAYS_CHAR (NET_INTERFACE_DEFAULT_ETHERNET);
  bool use_loopback = NET_INTERFACE_DEFAULT_USE_LOOPBACK;
  unsigned short listening_port_number = NET_SERVER_DEFAULT_LISTENING_PORT;
  bool use_reactor = NET_EVENT_USE_REACTOR;
  unsigned int statistic_reporting_interval =
    NET_SERVER_DEFAULT_STATISTIC_REPORTING_INTERVAL;
  bool trace_information = false;
  bool use_UDP = false;
  bool print_version_and_exit = false;
  unsigned int number_of_dispatch_threads =
    NET_SERVER_DEFAULT_NUMBER_OF_DISPATCHING_THREADS;

  // step1b: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            maximum_number_of_connections,
                            UI_file,
                            use_thread_pool,
                            ping_interval,
                            //keep_alive_timeout,
                            log_to_file,
                            network_interface,
                            use_loopback,
                            listening_port_number,
                            use_reactor,
                            statistic_reporting_interval,
                            trace_information,
                            use_UDP,
                            print_version_and_exit,
                            number_of_dispatch_threads))
  {
    do_printUsage (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, finalize ACE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1c: validate arguments
  // *NOTE*: probably requires CAP_NET_BIND_SERVICE
  if (listening_port_number <= 1023)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("using (privileged) port #: %d\n"),
                listening_port_number));
  if (use_reactor && (number_of_dispatch_threads > 1))
    use_thread_pool = true;

  // *IMPORTANT NOTE*: iff the number of message buffers is limited, the
  //                   reactor/proactor thread could (dead)lock on the
  //                   allocator lock, as it cannot dispatch events that would
  //                   free slots
  if (NET_STREAM_MAX_MESSAGES)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("limiting the number of message buffers could lead to deadlocks...\n")));
  if ((!UI_file.empty () && !Common_File_Tools::isReadable (UI_file))      ||
      (use_thread_pool && !use_reactor)                                    ||
      (use_reactor && (number_of_dispatch_threads > 1) && !use_thread_pool))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid arguments, aborting\n")));

    do_printUsage (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  if (number_of_dispatch_threads == 0)
    number_of_dispatch_threads = 1;

  struct Test_U_Server_GTK_CBData gtk_cb_user_data;
  gtk_cb_user_data.allowUserRuntimeStatistic =
    (statistic_reporting_interval == 0); // handle SIGUSR1/SIGBREAK
                                         // iff regular reporting
                                         // is off
  gtk_cb_user_data.progressData.GTKState = &gtk_cb_user_data;

  // step1d: initialize logging and/or tracing
  Common_Logger_t logger (&gtk_cb_user_data.logStack,
                          &gtk_cb_user_data.lock);
  std::string log_file_name;
  if (log_to_file)
    log_file_name =
        Net_Server_Common_Tools::getNextLogFileName (ACE_TEXT_ALWAYS_CHAR (LIBACENETWORK_PACKAGE_NAME),
                                                     ACE_TEXT_ALWAYS_CHAR (NET_SERVER_LOG_FILENAME_PREFIX));
  if (!Common_Tools::initializeLogging (ACE::basename (argv_in[0]),    // program name
                                        log_file_name,                 // log file name
                                        true,                          // log to syslog ?
                                        false,                         // trace messages ?
                                        trace_information,             // debug messages ?
                                        (UI_file.empty () ? NULL
                                                          : &logger))) // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeLogging(), aborting\n")));

    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1e: (pre-)initialize signal handling
  ACE_Sig_Set signal_set (0);
  ACE_Sig_Set ignored_signal_set (0);
  do_initializeSignals (use_reactor,
                        gtk_cb_user_data.allowUserRuntimeStatistic, // handle SIGUSR1/SIGBREAK
                                                                    // iff regular reporting
                                                                    // is off
                        signal_set,
                        ignored_signal_set);
  Common_SignalActions_t previous_signal_actions;
  sigset_t previous_signal_mask;
  result = ACE_OS::sigemptyset (&previous_signal_mask);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", aborting\n")));

    Common_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  if (!Common_Tools::preInitializeSignals (signal_set,
                                           use_reactor,
                                           previous_signal_actions,
                                           previous_signal_mask))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::preInitializeSignals(), aborting\n")));

    Common_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  Test_U_Server_SignalHandler signal_handler ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                           : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                              &gtk_cb_user_data.lock);

  // step1f: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion (ACE::basename (argv_in[0]));

    Common_Tools::finalizeSignals ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                   signal_set,
                                   previous_signal_actions,
                                   previous_signal_mask);
    Common_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_SUCCESS;
  } // end IF

  // step1g: set process resource limits
  // *NOTE*: settings will be inherited by any child processes
  // *TODO*: the reasoning here is incomplete
  bool use_fd_based_reactor = use_reactor;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  use_fd_based_reactor =
    (use_reactor && !(COMMON_EVENT_REACTOR_TYPE == COMMON_REACTOR_WFMO));
#endif
  bool stack_traces = true;
  bool use_signal_based_proactor = !use_reactor;
  if (!Common_Tools::setResourceLimits (use_fd_based_reactor,       // file descriptors
                                        stack_traces,               // stack traces
                                        use_signal_based_proactor)) // pending signals
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::setResourceLimits(), aborting\n")));

    Common_Tools::finalizeSignals ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                   signal_set,
                                   previous_signal_actions,
                                   previous_signal_mask);
    Common_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1h: init GLIB / G(D|T)K[+] / GNOME ?
  //Common_UI_GladeDefinition ui_definition (argc_in,
  //                                         argv_in);
  Test_U_Server_GtkBuilderDefinition_t ui_definition (argc_in,
                                                      argv_in);
  if (!UI_file.empty ())
    SERVER_UI_GTK_MANAGER_SINGLETON::instance ()->initialize (argc_in,
                                                              argv_in,
                                                              &gtk_cb_user_data,
                                                              &ui_definition);

  ACE_High_Res_Timer timer;
  timer.start ();
  // step2: do actual work
  do_work (maximum_number_of_connections,
           UI_file,
           use_thread_pool,
           ping_interval,
           //keep_alive_timeout,
           network_interface,
           use_loopback,
           listening_port_number,
           use_reactor,
           statistic_reporting_interval,
           number_of_dispatch_threads,
           use_UDP,
           gtk_cb_user_data,
           signal_set,
           ignored_signal_set,
           previous_signal_actions,
           signal_handler);
  timer.stop ();

  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time (working_time);
  Common_Tools::periodToString (working_time,
                                working_time_string);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"\n"),
              ACE_TEXT (working_time_string.c_str ())));

  // stop profile timer...
  process_profile.stop ();

  // only process profile left to do...
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  elapsed_time.real_time = 0.0;
  elapsed_time.user_time = 0.0;
  elapsed_time.system_time = 0.0;
  if (process_profile.elapsed_time (elapsed_time) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Profile_Timer::elapsed_time: \"%m\", aborting\n")));

    Common_Tools::finalizeSignals ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                   signal_set,
                                   previous_signal_actions,
                                   previous_signal_mask);
    Common_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (ACE_Profile_Timer::Rusage));
  process_profile.elapsed_rusage (elapsed_rusage);
  ACE_Time_Value user_time (elapsed_rusage.ru_utime);
  ACE_Time_Value system_time (elapsed_rusage.ru_stime);
  std::string user_time_string;
  std::string system_time_string;
  Common_Tools::periodToString (user_time,
                               user_time_string);
  Common_Tools::periodToString (system_time,
                               system_time_string);

  // debug info
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT(" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\nmaximum resident set size = %d\nintegral shared memory size = %d\nintegral unshared data size = %d\nintegral unshared stack size = %d\npage reclaims = %d\npage faults = %d\nswaps = %d\nblock input operations = %d\nblock output operations = %d\nmessages sent = %d\nmessages received = %d\nsignals received = %d\nvoluntary context switches = %d\ninvoluntary context switches = %d\n"),
             elapsed_time.real_time,
             elapsed_time.user_time,
             elapsed_time.system_time,
             user_time_string.c_str(),
             system_time_string.c_str(),
             elapsed_rusage.ru_maxrss,
             elapsed_rusage.ru_ixrss,
             elapsed_rusage.ru_idrss,
             elapsed_rusage.ru_isrss,
             elapsed_rusage.ru_minflt,
             elapsed_rusage.ru_majflt,
             elapsed_rusage.ru_nswap,
             elapsed_rusage.ru_inblock,
             elapsed_rusage.ru_oublock,
             elapsed_rusage.ru_msgsnd,
             elapsed_rusage.ru_msgrcv,
             elapsed_rusage.ru_nsignals,
             elapsed_rusage.ru_nvcsw,
             elapsed_rusage.ru_nivcsw));
#else
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              ACE_TEXT (user_time_string.c_str ()),
              ACE_TEXT (system_time_string.c_str ())));
#endif

  Common_Tools::finalizeSignals ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                              : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                 signal_set,
                                 previous_signal_actions,
                                 previous_signal_mask);
  Common_Tools::finalizeLogging ();

  // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_SUCCESS;
} // end main
