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

#include <iostream>
#include <limits>
#include <list>
#include <sstream>
#include <string>

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
#include "common_timer_manager.h"
#include "common_tools.h"

#include "common_ui_defines.h"
//#include "common_ui_glade_definition.h"
#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_manager_common.h"

#include "stream_allocatorheap.h"

#include "net_iconnector.h"
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
#include "test_u_message.h"
#include "test_u_module_eventhandler.h"
#include "test_u_sessionmessage.h"

#include "net_client_common.h"
#include "net_client_connector_common.h"
#include "net_client_defines.h"
#include "net_client_signalhandler.h"
#include "net_client_timeouthandler.h"

#include "net_server_defines.h"

// globals
unsigned int random_seed;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
struct random_data random_data;
char random_state_buffer[BUFSIZ];
#endif

const char stream_name_string_[] = ACE_TEXT_ALWAYS_CHAR ("NetClientStream");

void
do_printUsage (const std::string& programName_in)
{
  NETWORK_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf (std::ios::boolalpha);

  std::string configuration_path =
    Common_File_Tools::getWorkingDirectory ();
#if defined (DEBUG_DEBUGGER)
  //configuration_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  //configuration_path += ACE_TEXT_ALWAYS_CHAR ("test_u");
  //configuration_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  //configuration_path += ACE_TEXT_ALWAYS_CHAR ("client_server");
#endif // #ifdef DEBUG_DEBUGGER

  std::cout << ACE_TEXT("usage: ")
            << programName_in
            << ACE_TEXT(" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT("-a           : alternating mode [")
            << false
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-c [VALUE]   : maximum #connections [")
            << NET_CLIENT_DEF_MAX_NUM_OPEN_CONNECTIONS
            << ACE_TEXT("] {0 --> unlimited}")
            << std::endl;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_DIRECTORY);
  std::string UI_file = path;
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UI_file += ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_FILE);
  std::cout << ACE_TEXT ("-g[[STRING]] : UI file [\"")
            << UI_file
            << ACE_TEXT ("\"] {\"\" --> no GUI}")
            << std::endl;
  std::cout << ACE_TEXT ("-h           : use thread-pool [\"")
            << NET_EVENT_USE_THREAD_POOL
            << ACE_TEXT ("\"]")
            << std::endl;
  std::cout << ACE_TEXT ("-i [VALUE]   : connection interval (s) [")
            << NET_CLIENT_DEF_SERVER_CONNECT_INTERVAL
            << ACE_TEXT ("] {0 --> OFF}")
            << std::endl;
  std::cout << ACE_TEXT ("-l           : log to a file [")
            << false
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-n [STRING]  : server hostname [\"")
            << NET_CLIENT_DEF_SERVER_HOSTNAME
            << ACE_TEXT ("\"]")
            << std::endl;
  std::cout << ACE_TEXT ("-p [VALUE]   : server port [")
            << NET_SERVER_DEFAULT_LISTENING_PORT
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-r           : use reactor [")
            << NET_EVENT_USE_REACTOR
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-s           : ping interval (ms) [")
            << NET_CLIENT_DEF_SERVER_PING_INTERVAL
            << ACE_TEXT ("] {0: OFF}")
            << std::endl;
  std::cout << ACE_TEXT ("-t           : trace information [")
            << false
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-u           : use UDP [")
            << false
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-v           : print version information and exit [")
            << false
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-x [VALUE]   : #dispatch threads [")
            << NET_CLIENT_DEFAULT_NUMBER_OF_DISPATCH_THREADS
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-y           : run stress-test [")
            << false
            << ACE_TEXT ("]")
            << std::endl;
}

bool
do_processArguments (int argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     bool& alternatingMode_out,
                     unsigned int& maximumNumberOfConnections_out,
                     std::string& UIFile_out,
                     bool& useThreadPool_out,
                     unsigned int& connectionInterval_out,
                     bool& logToFile_out,
                     std::string& serverHostname_out,
                     unsigned short& serverPortNumber_out,
                     bool& useReactor_out,
                     ACE_Time_Value& pingInterval_out,
                     bool& traceInformation_out,
                     bool& useUDP_out,
                     bool& printVersionAndExit_out,
                     unsigned int& numDispatchThreads_out,
                     bool& runStressTest_out)
{
  NETWORK_TRACE (ACE_TEXT ("::do_processArguments"));

  std::string configuration_path =
    Common_File_Tools::getWorkingDirectory ();
#if defined (DEBUG_DEBUGGER)
  //configuration_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  //configuration_path += ACE_TEXT_ALWAYS_CHAR ("test_u");
  //configuration_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  //configuration_path += ACE_TEXT_ALWAYS_CHAR ("client_server");
#endif // #ifdef DEBUG_DEBUGGER

  // initialize results
  alternatingMode_out = false;
  maximumNumberOfConnections_out = NET_CLIENT_DEF_MAX_NUM_OPEN_CONNECTIONS;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_DIRECTORY);
  UIFile_out = path;
  UIFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UIFile_out += ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_FILE);
  useThreadPool_out = NET_EVENT_USE_THREAD_POOL;
  connectionInterval_out = NET_CLIENT_DEF_SERVER_CONNECT_INTERVAL;
  logToFile_out = false;
  serverHostname_out = NET_CLIENT_DEF_SERVER_HOSTNAME;
  serverPortNumber_out = NET_SERVER_DEFAULT_LISTENING_PORT;
  useReactor_out = NET_EVENT_USE_REACTOR;
  pingInterval_out.set (NET_CLIENT_DEF_SERVER_PING_INTERVAL / 1000,
                        (NET_CLIENT_DEF_SERVER_PING_INTERVAL % 1000) * 1000);
  traceInformation_out = false;
  useUDP_out = false;
  printVersionAndExit_out = false;
  numDispatchThreads_out = NET_CLIENT_DEFAULT_NUMBER_OF_DISPATCH_THREADS;
  runStressTest_out = false;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
                              ACE_TEXT ("ac:g::hi:ln:p:rs:tuvx:y"),
                              1,                         // skip command name
                              1,                         // report parsing errors
                              ACE_Get_Opt::PERMUTE_ARGS, // ordering
                              0);                        // for now, don't use long options

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser ()) != EOF)
  {
    switch (option)
    {
      case 'a':
      {
        alternatingMode_out = true;
        break;
      }
      case 'c':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argumentParser.opt_arg ();
        converter >> maximumNumberOfConnections_out;
        break;
      }
      case 'g':
      {
        ACE_TCHAR* opt_arg = argumentParser.opt_arg ();
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
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argumentParser.opt_arg ();
        converter >> connectionInterval_out;
        break;
      }
      case 'l':
      {
        logToFile_out = true;
        break;
      }
      case 'n':
      {
        serverHostname_out = argumentParser.opt_arg ();
        break;
      }
      case 'p':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argumentParser.opt_arg ();
        converter >> serverPortNumber_out;
        break;
      }
      case 'r':
      {
        useReactor_out = true;
        break;
      }
      case 's':
      {
        unsigned int ping_interval = 0;
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argumentParser.opt_arg ();
        converter >> ping_interval;
        pingInterval_out.set (ping_interval / 1000,
                              (ping_interval % 1000) * 1000);
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
        converter << argumentParser.opt_arg ();
        converter >> numDispatchThreads_out;
        break;
      }
      case 'y':
      {
        runStressTest_out = true;
        break;
      }
      // error handling
      case ':':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("option \"%c\" requires an argument, aborting\n"),
                    argumentParser.opt_opt ()));
        return false;
      }
      case '?':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%s\", aborting\n"),
                    ACE_TEXT (argumentParser.last_option ())));
        return false;
      }
      case 0:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("found long option \"%s\", aborting\n"),
                    ACE_TEXT (argumentParser.long_option ())));
        return false;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("parse error, aborting\n")));
        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

void
do_initializeSignals (bool allowUserRuntimeConnect_in,
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

  // *PORTABILITY*: on Windows(TM) platforms most signals are not defined, and
  //                ACE_Sig_Set::fill_set() doesn't really work as specified
  // --> add valid signals (see <signal.h>)...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  signals_out.sig_add (SIGINT);            // 2       /* interrupt */
  signals_out.sig_add (SIGILL);            // 4       /* illegal instruction - invalid function image */
  signals_out.sig_add (SIGFPE);            // 8       /* floating point exception */
//  signals_out.sig_add (SIGSEGV);           // 11      /* segment violation */
  signals_out.sig_add (SIGTERM);           // 15      /* Software termination signal from kill */
  if (allowUserRuntimeConnect_in)
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
                ACE_TEXT("failed to ACE_Sig_Set::fill_set(): \"%m\", returning\n")));
    return;
  } // end IF
  // *NOTE*: cannot handle some signals --> registration fails for these...
  signals_out.sig_del (SIGKILL);           // 9       /* Kill signal */
  signals_out.sig_del (SIGSTOP);           // 19      /* Stop process */
  // ---------------------------------------------------------------------------
  if (!allowUserRuntimeConnect_in)
  {
    signals_out.sig_del (SIGUSR1);         // 10      /* User-defined signal 1 */
    ignoredSignals_out.sig_add (SIGUSR1);  // 10      /* User-defined signal 1 */
  } // end IF
  // *NOTE* core dump on SIGSEGV
  signals_out.sig_del (SIGSEGV);           // 11      /* Segmentation fault: Invalid memory reference */
  // *NOTE* don't care about SIGPIPE
  signals_out.sig_del (SIGPIPE);           // 12      /* Broken pipe: write to pipe with no readers */

#ifdef RPG_ENABLE_VALGRIND_SUPPORT
  // *NOTE*: valgrind uses SIGRT32 (--> SIGRTMAX ?) and apparently will not work
  // if the application installs its own handler (see documentation)
  if (RUNNING_ON_VALGRIND)
    signals_out.sig_del (SIGRTMAX);        // 64
#endif
#endif
}

void
do_work (enum Test_U_Client_TimeoutHandler::ActionModeType actionMode_in,
         unsigned int maxNumConnections_in,
         const std::string& UIDefinitionFile_in,
         bool useThreadPool_in,
         unsigned int connectionInterval_in,
         const std::string& serverHostname_in,
         unsigned short serverPortNumber_in,
         bool useReactor_in,
         const ACE_Time_Value& pingInterval_in,
         const ACE_Time_Value& statisticReportingInterval_in,
         unsigned int numberOfDispatchThreads_in,
         bool useUDP_in,
         struct Test_U_Client_GTK_CBData& CBData_in,
         const ACE_Sig_Set& signalSet_in,
         const ACE_Sig_Set& ignoredSignalSet_in,
         Common_SignalActions_t& previousSignalActions_inout,
         Test_U_Client_SignalHandler& signalHandler_in)
{
  NETWORK_TRACE (ACE_TEXT ("::do_work"));

  int result = -1;

  // step0a: initialize random number generator
  Common_Tools::initialize ();

  // step0a: initialize configuration
  struct Test_U_Client_Configuration configuration;
  CBData_in.configuration = &configuration;

  Test_U_EventHandler ui_event_handler (&CBData_in);
  Test_U_Module_EventHandler_Module event_handler (NULL,
                                                   ACE_TEXT_ALWAYS_CHAR ("EventHandler"));
  Test_U_Module_EventHandler* event_handler_p =
    dynamic_cast<Test_U_Module_EventHandler*> (event_handler.writer ());
  if (!event_handler_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Net_Module_EventHandler> failed, returning\n")));
    return;
  } // end IF

  Stream_AllocatorHeap_T<struct Stream_AllocatorConfiguration> heap_allocator;
  if (!heap_allocator.initialize (configuration.streamConfiguration.allocatorConfiguration_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize heap allocator, returning\n")));
    return;
  } // end IF
  Test_U_StreamMessageAllocator_t message_allocator (NET_STREAM_MAX_MESSAGES, // maximum #buffers
                                                     &heap_allocator,         // heap allocator handle
                                                     true);                   // block ?
  // ********************* protocol configuration data *************************
  configuration.protocolConfiguration.pingInterval =
    ((actionMode_in == Test_U_Client_TimeoutHandler::ACTION_STRESS) ? ACE_Time_Value::zero
                                                                    : pingInterval_in);
  configuration.protocolConfiguration.printPongMessages =
    UIDefinitionFile_in.empty ();
  // ********************** stream configuration data **************************
  struct Test_U_ModuleHandlerConfiguration modulehandler_configuration;
  modulehandler_configuration.protocolConfiguration =
    &configuration.protocolConfiguration;
  modulehandler_configuration.streamConfiguration =
    &configuration.streamConfiguration;
  modulehandler_configuration.printFinalReport = true;
  modulehandler_configuration.subscriber = &ui_event_handler;
  modulehandler_configuration.subscribersLock = &CBData_in.subscribersLock;
  modulehandler_configuration.subscribers = &CBData_in.subscribers;

  configuration.streamConfiguration.configuration_.cloneModule =
    !(UIDefinitionFile_in.empty ());
  configuration.streamConfiguration.configuration_.messageAllocator =
    &message_allocator;
  configuration.streamConfiguration.configuration_.module =
    (!UIDefinitionFile_in.empty () ? &event_handler
                                   : NULL);
  configuration.streamConfiguration.configuration_.printFinalReport = true;
  // *TODO*: is this correct ?
  configuration.streamConfiguration.configuration_.serializeOutput =
    useThreadPool_in;
  configuration.streamConfiguration.configuration_.userData =
    &configuration.userData;
  configuration.streamConfiguration.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (""),
                                                            modulehandler_configuration));

  // ********************** connection configuration data **********************
  struct Test_U_ConnectionConfiguration connection_configuration;
  connection_configuration.socketHandlerConfiguration.statisticReportingInterval =
    statisticReportingInterval_in;
  connection_configuration.streamConfiguration =
    &configuration.streamConfiguration;
  connection_configuration.messageAllocator = &message_allocator;
  connection_configuration.userData =
    &configuration.userData;

  configuration.connectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (""),
                                                                 connection_configuration));
  Test_U_ConnectionConfigurationIterator_t iterator =
    configuration.connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != configuration.connectionConfigurations.end ());
  (*iterator).second.socketHandlerConfiguration.connectionConfiguration =
    &((*iterator).second);

  //  config.useThreadPerConnection = false;
  //  config.serializeOutput = false;

  //  config.notificationStrategy = NULL;
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
                ACE_TEXT ("failed to initialize event dispatch, returing\n")));
    return;
  } // end IF

  // step0c: initialize connector
  Test_U_InetConnectionManager_t* connection_manager_p =
    TEST_U_CONNECTIONMANAGER_SINGLETON::instance ();
  ACE_ASSERT (connection_manager_p);
  Test_U_IInetConnectionManager_t* iconnection_manager_p =
    connection_manager_p;
  Test_U_Client_TCP_AsynchConnector_t asynch_connector (iconnection_manager_p,
                                                        statisticReportingInterval_in);
  Test_U_Client_TCP_Connector_t connector (iconnection_manager_p,
                                           statisticReportingInterval_in);
  Test_U_IConnector_t* connector_p = NULL;
  if (useReactor_in)
    connector_p = &connector;
  else
    connector_p = &asynch_connector;
  if (!connector_p->initialize ((*iterator).second))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize connector: \"%m\", returning\n")));
    return;
  } // end IF

  // step0d: initialize connection manager
  connection_manager_p->initialize (std::numeric_limits<unsigned int>::max ());
  connection_manager_p->set ((*iterator).second,
                             &configuration.userData);

  // step0e: initialize action timer
  configuration.signalHandlerConfiguration.connector = connector_p;
  result =
    configuration.signalHandlerConfiguration.peerAddress.set (serverPortNumber_in,
                                                              serverHostname_in.c_str (),
                                                              1,
                                                              ACE_ADDRESS_FAMILY_INET);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::set(): \"%m\", returning\n")));
    return;
  } // end IF

  Test_U_Client_TimeoutHandler timeout_handler (actionMode_in,
                                                maxNumConnections_in,
                                                configuration.signalHandlerConfiguration.peerAddress,
                                                connector_p);
  configuration.timeoutHandler = &timeout_handler;
  Common_Timer_Manager_t* timer_manager_p =
      COMMON_TIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (timer_manager_p);
  struct Common_TimerConfiguration timer_configuration;
  timer_manager_p->initialize (timer_configuration);
  timer_manager_p->start ();
  if (UIDefinitionFile_in.empty () && (connectionInterval_in > 0))
  {
    // schedule action interval timer
    ACE_Time_Value interval (((actionMode_in == Test_U_Client_TimeoutHandler::ACTION_STRESS) ? (NET_CLIENT_DEF_SERVER_STRESS_INTERVAL / 1000)
                                                                                             : connectionInterval_in),
                             ((actionMode_in == Test_U_Client_TimeoutHandler::ACTION_STRESS) ? ((NET_CLIENT_DEF_SERVER_STRESS_INTERVAL % 1000) * 1000)
                                                                                             : 0));
    configuration.signalHandlerConfiguration.actionTimerId =
        timer_manager_p->schedule_timer (&timeout_handler,           // event handler handle
                                         NULL,                       // asynchronous completion token
                                         COMMON_TIME_NOW + interval, // first wakeup time
                                         interval);                  // interval
    if (configuration.signalHandlerConfiguration.actionTimerId == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to schedule action timer: \"%m\", returning\n")));

      // clean up
      timer_manager_p->stop ();

      return;
    } // end IF
  } // end IF

  // step0e: initialize signal handling
  configuration.signalHandlerConfiguration.messageAllocator =
    &message_allocator;
  configuration.signalHandlerConfiguration.connectionConfiguration =
    &((*iterator).second);
  configuration.signalHandlerConfiguration.useReactor = useReactor_in;
  if (!signalHandler_in.initialize (configuration.signalHandlerConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize signal handler, returning\n")));

    // clean up
    timer_manager_p->stop ();

    return;
  } // end IF
  if (!Common_Tools::initializeSignals (signalSet_in,
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

  // event loop(s):
  // - catch SIGINT/SIGQUIT/SIGTERM/... signals (connect / perform orderly shutdown)
  // [- signal timer expiration to perform server queries] (see above)

  // step1a: start GTK event loop ?
  if (!UIDefinitionFile_in.empty ())
  {
    CBData_in.finalizationHook = idle_finalize_UI_cb;
    CBData_in.initializationHook = idle_initialize_client_UI_cb;
    //CBData_in.gladeXML[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN)] =
    //  std::make_pair (UIDefinitionFile_in, static_cast<GladeXML*> (NULL));
    CBData_in.builders[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN)] =
      std::make_pair (UIDefinitionFile_in, static_cast<GtkBuilder*> (NULL));
    CBData_in.userData = &CBData_in;

    CLIENT_UI_GTK_MANAGER_SINGLETON::instance ()->start ();
    result = ACE_OS::sleep (ACE_Time_Value (1, 0));
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::sleep(): \"%m\", continuing\n")));
    if (!CLIENT_UI_GTK_MANAGER_SINGLETON::instance ()->isRunning ())
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
      CLIENT_UI_GTK_MANAGER_SINGLETON::instance ()->stop (true);

      return;
    } // end IF
    BOOL was_visible_b = ShowWindow (window_p, SW_HIDE);
#endif
  } // end IF

  // step1b: initialize worker(s)
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
      CLIENT_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();
    timer_manager_p->stop ();

    return;
  } // end IF

  // step1c: connect immediately ?
  if (UIDefinitionFile_in.empty () && (connectionInterval_in == 0))
  {
    bool result_2 =
      connector_p->connect (configuration.signalHandlerConfiguration.peerAddress);
    if (!useReactor_in)
    {
      ACE_Time_Value one_second (1, 0);
      result = ACE_OS::sleep (one_second);
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::sleep(%#T): \"%m\", continuing\n"),
                    &one_second));
      if (TEST_U_CONNECTIONMANAGER_SINGLETON::instance ()->count () != 1)
        result_2 = false;
    } // end IF

    if (!result_2)
    {
      char buffer[BUFSIZ];
      ACE_OS::memset (buffer, 0, sizeof (buffer));
      result =
        configuration.signalHandlerConfiguration.peerAddress.addr_to_string (buffer,
                                                                             sizeof (buffer));
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string: \"%m\", continuing\n")));
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to connect to \"%s\", returning\n"),
                  ACE_TEXT (buffer)));

      // clean up
      if (numberOfDispatchThreads_in >= 1)
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
      timer_manager_p->stop ();

      return;
    } // end IF
  } // end IF

  // *NOTE*: from this point on, clean up any remote connections !

  // step2: dispatch events
  Common_Tools::dispatchEvents (useReactor_in,
                                group_id);

  // step3: clean up
  if (!UIDefinitionFile_in.empty ())
    CLIENT_UI_GTK_MANAGER_SINGLETON::instance ()->wait ();
  //		{ // synch access
  //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

  //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
  //					 iterator != CBData_in.event_source_ids.end();
  //					 iterator++)
  //				g_source_remove(*iterator);
  //		} // end lock scope
  timer_manager_p->stop ();

//  connection_manager_p->abort ();
  TEST_U_CONNECTIONMANAGER_SINGLETON::instance ()->abort ();
  // *IMPORTANT NOTE*: as long as connections are inactive (i.e. events are
  // dispatched by reactor thread(s), there is no real reason to wait here)
//  connection_manager_p->wait ();
  TEST_U_CONNECTIONMANAGER_SINGLETON::instance ()->wait ();

//  { // synch access
//    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

//		for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
//				 iterator != CBData_in.event_source_ids.end();
//				 iterator++)
//			g_source_remove(*iterator);
//	} // end lock scope

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
           ACE_TCHAR* argv_in[])
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
  random_seed = static_cast <unsigned int> (now.sec ());
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
#if defined (DEBUG_DEBUGGER)
  //configuration_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  //configuration_path += ACE_TEXT_ALWAYS_CHAR ("test_u");
  //configuration_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  //configuration_path += ACE_TEXT_ALWAYS_CHAR ("client_server");
#endif // #ifdef DEBUG_DEBUGGER

  // step1a set defaults
  enum Test_U_Client_TimeoutHandler::ActionModeType action_mode =
    Test_U_Client_TimeoutHandler::ACTION_NORMAL;
  bool alternating_mode = false;
  unsigned int maximum_number_of_connections =
   NET_CLIENT_DEF_MAX_NUM_OPEN_CONNECTIONS;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_DIRECTORY);
  std::string UI_file = path;
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UI_file += ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_FILE);
  bool use_thread_pool = NET_EVENT_USE_THREAD_POOL;
  unsigned int connection_interval =
   NET_CLIENT_DEF_SERVER_CONNECT_INTERVAL;
  bool log_to_file = false;
  std::string server_hostname =
    NET_CLIENT_DEF_SERVER_HOSTNAME;
  unsigned short server_port_number =
   NET_SERVER_DEFAULT_LISTENING_PORT;
  bool use_reactor = NET_EVENT_USE_REACTOR;
  ACE_Time_Value ping_interval (NET_CLIENT_DEF_SERVER_PING_INTERVAL / 1000,
                                (NET_CLIENT_DEF_SERVER_PING_INTERVAL % 1000) * 1000);
  ACE_Time_Value statistic_reporting_interval (NET_STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL,
                                               0);
  bool trace_information = false;
  bool use_UDP = false;
  bool print_version_and_exit = false;
  unsigned int number_of_dispatch_threads =
   NET_CLIENT_DEFAULT_NUMBER_OF_DISPATCH_THREADS;
  bool run_stress_test = false;

  // step1b: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            alternating_mode,
                            maximum_number_of_connections,
                            UI_file,
                            use_thread_pool,
                            connection_interval,
                            log_to_file,
                            server_hostname,
                            server_port_number,
                            use_reactor,
                            ping_interval,
                            trace_information,
                            use_UDP,
                            print_version_and_exit,
                            number_of_dispatch_threads,
                            run_stress_test))
  {
    // make 'em learn
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
  // *IMPORTANT NOTE*: iff the number of message buffers is limited, the
  //                   reactor/proactor thread could (dead)lock on the
  //                   allocator lock, as it cannot dispatch events that would
  //                   free slots
  if (NET_STREAM_MAX_MESSAGES)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("limiting the number of message buffers could lead to deadlocks...\n")));
  if ((!UI_file.empty () && !Common_File_Tools::isReadable (UI_file))       ||
      (use_reactor && (number_of_dispatch_threads > 1) && !use_thread_pool) ||
      (run_stress_test && ((ping_interval != ACE_Time_Value::zero) ||
                           (connection_interval != 0)))                     ||
      (alternating_mode && run_stress_test))
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
  if (!UI_file.empty () &&
      !Common_File_Tools::isReadable (UI_file))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid UI definition file (was: %s), aborting\n"),
                ACE_TEXT (UI_file.c_str ())));

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
  if (alternating_mode)
    action_mode = Test_U_Client_TimeoutHandler::ACTION_ALTERNATING;
  if (run_stress_test)
    action_mode = Test_U_Client_TimeoutHandler::ACTION_STRESS;

  struct Test_U_Client_GTK_CBData gtk_cb_user_data;
  gtk_cb_user_data.progressData.GTKState = &gtk_cb_user_data;
  // step1d: initialize logging and/or tracing
  Common_Logger_t logger (&gtk_cb_user_data.logStack,
                          &gtk_cb_user_data.lock);
  std::string log_file_name;
  if (log_to_file)
    log_file_name =
        Common_File_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (LIBACENETWORK_PACKAGE_NAME),
                                           ACE::basename (argv_in[0]));
  if (!Common_Tools::initializeLogging (ACE::basename (argv_in[0]),    // program name
                                        log_file_name,                 // log file name
                                        false,                         // log to syslog ?
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

  // step1e: pre-initialize signal handling
  ACE_Sig_Set signal_set (0);
  ACE_Sig_Set ignored_signal_set (0);
  do_initializeSignals ((connection_interval == 0), // allow SIGUSR1/SIGBREAK iff
                                                    // regular connections are off
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
  Test_U_Client_SignalHandler signal_handler;

  // step1f: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion (ACE::basename (argv_in[0]));

    Common_Tools::finalizeSignals (signal_set,
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

    Common_Tools::finalizeSignals (signal_set,
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
  Test_U_Client_GtkBuilderDefinition_t ui_definition (argc_in,
                                                      argv_in);
  if (!UI_file.empty ())
    CLIENT_UI_GTK_MANAGER_SINGLETON::instance ()->initialize (argc_in,
                                                              argv_in,
                                                              &gtk_cb_user_data,
                                                              &ui_definition);

  ACE_High_Res_Timer timer;
  timer.start ();
  // step2: do actual work
  do_work (action_mode,
           maximum_number_of_connections,
           UI_file,
           use_thread_pool,
           connection_interval,
           server_hostname,
           server_port_number,
           use_reactor,
           ping_interval,
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
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
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

    Common_Tools::finalizeSignals (signal_set,
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
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (elapsed_rusage));
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
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\nmaximum resident set size = %d\nintegral shared memory size = %d\nintegral unshared data size = %d\nintegral unshared stack size = %d\npage reclaims = %d\npage faults = %d\nswaps = %d\nblock input operations = %d\nblock output operations = %d\nmessages sent = %d\nmessages received = %d\nsignals received = %d\nvoluntary context switches = %d\ninvoluntary context switches = %d\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              ACE_TEXT (user_time_string.c_str ()),
              ACE_TEXT (system_time_string.c_str ()),
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

  Common_Tools::finalizeSignals (signal_set,
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
} // end main
