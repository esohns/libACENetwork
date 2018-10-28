﻿/***************************************************************************
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
#include <regex>
#include <string>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#elif defined (ACE_LINUX)
#include <sys/capability.h>
#include <linux/capability.h>
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Get_Opt.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "ace/Log_Msg.h"
#include "ace/Profile_Timer.h"
#include "ace/Sig_Handler.h"
#include "ace/Signal.h"
#include "ace/Synch.h"
#include "ace/Version.h"

#if defined (HAVE_CONFIG_H)
#include "libCommon_config.h"
#endif // HAVE_CONFIG_H

#include "common.h"
#include "common_tools.h"

#include "common_log_tools.h"
#include "common_logger.h"

#include "common_timer_tools.h"

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_defines.h"
#include "common_ui_gtk_manager_common.h"
#endif // GTK_USE
#endif // GUI_SUPPORT

#if defined (HAVE_CONFIG_H)
#include "libACEStream_config.h"
#endif // HAVE_CONFIG_H

#include "stream_allocatorheap.h"

#include "stream_misc_defines.h"

#include "stream_file_sink.h"

#if defined (HAVE_CONFIG_H)
#include "libACENetwork_config.h"
#endif // HAVE_CONFIG_H

#include "net_common_tools.h"
#include "net_defines.h"
#include "net_macros.h"

#include "net_client_defines.h"

#include "http_defines.h"

#include "test_i_callbacks.h"
#include "test_i_common.h"
#include "test_i_connection_common.h"
#include "test_i_connection_manager_common.h"
#include "test_i_connection_stream.h"
#include "test_i_defines.h"
#include "test_i_eventhandler.h"
#include "test_i_message.h"
#include "test_i_module_eventhandler.h"
#include "test_i_session_message.h"
#include "test_i_signalhandler.h"

#include "test_i_url_stream_load_common.h"
#include "test_i_url_stream_load_defines.h"

const char stream_name_string_[] = ACE_TEXT_ALWAYS_CHAR ("URLStreamLoadStream");

void
do_print_usage (const std::string& programName_in)
{
  NETWORK_TRACE (ACE_TEXT ("::do_print_usage"));

  // enable verbatim boolean output
  std::cout.setf (std::ios::boolalpha);

  std::string path =
    Common_File_Tools::getWorkingDirectory ();

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-d        : debug parser [")
            << COMMON_PARSER_DEFAULT_YACC_TRACE
            << ACE_TEXT_ALWAYS_CHAR ("])")
            << std::endl;
  std::string configuration_path = path;
  configuration_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  configuration_path +=
      ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
  std::string gtk_rc_file = configuration_path;
  gtk_rc_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  gtk_rc_file += ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_DEFAULT_RC_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-e [PATH] : Gtk .rc file [\"")
            << gtk_rc_file
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  std::string output_file = path;
  output_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  output_file +=
    ACE_TEXT_ALWAYS_CHAR (TEST_I_URLSTREAMLOAD_DEFAULT_OUTPUT_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-f[[PATH]]: (output) file name [")
            << output_file
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::string UI_file = configuration_path;
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UI_file += ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_DEFAULT_GLADE_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-g[[PATH]]: UI definition file [\"")
            << UI_file
            << ACE_TEXT_ALWAYS_CHAR ("\"] {\"\": no GUI}")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-l        : log to a file [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-p        : use thread pool [")
            << COMMON_EVENT_REACTOR_DEFAULT_USE_THREADPOOL
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-r        : use reactor [")
            << (COMMON_EVENT_DEFAULT_DISPATCH == COMMON_EVENT_DISPATCH_REACTOR)
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-s [VALUE]: statistic reporting interval (second(s)) [")
            << STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL
            << ACE_TEXT_ALWAYS_CHAR ("] [0: off]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t        : trace information [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-u [URL]  : stream URL")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-v        : print version information and exit [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_process_arguments (int argc_in,
                      ACE_TCHAR** argv_in, // cannot be const...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                      bool& showConsole_out,
#endif
                      bool& debugParser_out,
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
                      std::string& GtkRcFileName_out,
#endif // GTK_USE
#endif // GUI_SUPPORT
                      std::string& fileName_out,
                      std::string& UIDefinitonFileName_out,
                      std::string& hostName_out,
                      bool& logToFile_out,
                      bool& useThreadPool_out,
                      unsigned short& port_out,
                      bool& useReactor_out,
                      ACE_Time_Value& statisticReportingInterval_out,
                      bool& traceInformation_out,
                      std::string& URL_out,
                      bool& printVersionAndExit_out,
                      ACE_INET_Addr& remoteHost_out)
{
  NETWORK_TRACE (ACE_TEXT ("::do_process_arguments"));

  std::string working_directory =
    Common_File_Tools::getWorkingDirectory ();
  std::string temp_directory = Common_File_Tools::getTempDirectory ();
  std::string configuration_directory = working_directory;
  configuration_directory += ACE_DIRECTORY_SEPARATOR_STR_A;
  configuration_directory +=
    ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);

  // initialize results
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  showConsole_out = false;
#endif
  debugParser_out = COMMON_PARSER_DEFAULT_YACC_TRACE;
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  GtkRcFileName_out = configuration_directory;
  GtkRcFileName_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  GtkRcFileName_out += ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_DEFAULT_RC_FILE);
#endif // GTK_USE
#endif // GUI_SUPPORT
  fileName_out = temp_directory;
  fileName_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  fileName_out +=
    ACE_TEXT_ALWAYS_CHAR (TEST_I_URLSTREAMLOAD_DEFAULT_OUTPUT_FILE);
  UIDefinitonFileName_out = configuration_directory;
  UIDefinitonFileName_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UIDefinitonFileName_out +=
    ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_DEFAULT_GLADE_FILE);
  hostName_out.clear ();
  logToFile_out = false;
  useThreadPool_out = COMMON_EVENT_REACTOR_DEFAULT_USE_THREADPOOL;
  port_out = 0;
  useReactor_out =
      (COMMON_EVENT_DEFAULT_DISPATCH == COMMON_EVENT_DISPATCH_REACTOR);
  statisticReportingInterval_out =
    (STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL ? ACE_Time_Value (STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL, 0)
                                                 : ACE_Time_Value::zero);
  traceInformation_out = false;
  URL_out.clear ();
  printVersionAndExit_out = false;
  int result =
    remoteHost_out.set (static_cast<u_short> (HTTP_DEFAULT_SERVER_PORT),
                        static_cast<ACE_UINT32> (INADDR_LOOPBACK),
                        1,
                        0);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::set (): \"%m\", aborting\n")));
    return false;
  } // end IF

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                               ACE_TEXT ("cde:f:g:lprs:tu:v"),
#else
                               ACE_TEXT ("de:f:g:lprs:tu:v"),
#endif
                               1,                         // skip command name
                               1,                         // report parsing errors
                               ACE_Get_Opt::PERMUTE_ARGS, // ordering
                               0);                        // for now, don't use long options

  int option = 0;
  std::stringstream converter;
  while ((option = argument_parser ()) != EOF)
  {
    switch (option)
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      case 'c':
      {
        showConsole_out = true;
        break;
      }
#endif
      case 'd':
      {
        debugParser_out = true;
        break;
      }
      case 'e':
      {
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
        GtkRcFileName_out = ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
#endif // GTK_USE
#endif // GUI_SUPPORT
        break;
      }
      case 'f':
      {
        fileName_out = ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
        break;
      }
      case 'g':
      {
        UIDefinitonFileName_out =
          ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
        break;
      }
      case 'l':
      {
        logToFile_out = true;
        break;
      }
      case 'p':
      {
        useThreadPool_out = true;
        break;
      }
      case 'r':
      {
        useReactor_out = true;
        break;
      }
      case 's':
      {
        unsigned int value = 0;
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
        converter >> value;
        statisticReportingInterval_out =
          (value ? ACE_Time_Value (value, 0) : ACE_Time_Value::zero);
        break;
      }
      case 't':
      {
        traceInformation_out = true;
        break;
      }
      case 'u':
      {
        URL_out = ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());

        // step1: parse URL
        std::string URI_s;
        bool use_ssl = false;
        if (!HTTP_Tools::parseURL (URL_out,
                                   hostName_out,
                                   URI_s,
                                   use_ssl))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to HTTP_Tools::parseURL(\"%s\"), aborting\n"),
                      ACE_TEXT (URL_out.c_str ())));
          return false;
        } // end IF

        std::string hostname_string = hostName_out;
        size_t position =
          hostname_string.find_last_of (':', std::string::npos);
        if (position == std::string::npos)
        {
          port_out = (use_ssl ? HTTPS_DEFAULT_SERVER_PORT
                              : HTTP_DEFAULT_SERVER_PORT);
          hostname_string += ':';
          std::ostringstream converter;
          converter << port_out;
          hostname_string += converter.str ();
        } // end IF
        else
        {
          std::istringstream converter (hostname_string.substr (position + 1,
                                                                std::string::npos));
          converter >> port_out;
        } // end ELSE
        result = remoteHost_out.set (hostname_string.c_str (),
                                     AF_INET);
        if (result == -1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_INET_Addr::set(\"%s\"): \"%m\", aborting\n"),
                      ACE_TEXT (hostname_string.c_str ())));
          return false;
        } // end IF

        // step2: validate address/verify host name exists
        //        --> resolve
        ACE_TCHAR buffer[HOST_NAME_MAX];
        ACE_OS::memset (buffer, 0, sizeof (buffer));
        result = remoteHost_out.get_host_name (buffer,
                                               sizeof (buffer));
        if (result == -1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_INET_Addr::get_host_name(): \"%m\", aborting\n")));
          return false;
        } // end IF
        std::string hostname = ACE_TEXT_ALWAYS_CHAR (buffer);
        std::string dotted_decimal_string;
        if (!Net_Common_Tools::getAddress (hostname,
                                           dotted_decimal_string))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Net_Common_Tools::getAddress(\"%s\"), aborting\n"),
                      ACE_TEXT (hostname.c_str ())));
          return false;
        } // end IF

        break;
      }
      case 'v':
      {
        printVersionAndExit_out = true;
        break;
      }
      // error handling
      case ':':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("option \"%c\" requires an argument, aborting\n"),
                    argument_parser.opt_opt ()));
        return false;
      }
      case '?':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%s\", aborting\n"),
                    ACE_TEXT (argument_parser.last_option ())));
        return false;
      }
      case 0:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("found long option \"%s\", aborting\n"),
                    ACE_TEXT (argument_parser.long_option ())));
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
do_initialize_signals (bool allowUserRuntimeConnect_in,
                       ACE_Sig_Set& signals_out,
                       ACE_Sig_Set& ignoredSignals_out)
{
  NETWORK_TRACE (ACE_TEXT ("::do_initialize_signals"));

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
                ACE_TEXT ("failed to ACE_Sig_Set::fill_set(): \"%m\", returning\n")));
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

#if defined (LIBACENETWORK_ENABLE_VALGRIND_SUPPORT)
  // *NOTE*: valgrind uses SIGRT32 (--> SIGRTMAX ?) and apparently will not work
  // if the application installs its own handler (see documentation)
  if (RUNNING_ON_VALGRIND)
    signals_out.sig_del (SIGRTMAX);        // 64
#endif // LIBACENETWORK_ENABLE_VALGRIND_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64
}

void
do_work (bool debugParser_in,
         const std::string& fileName_in,
         const std::string& UIDefinitionFileName_in,
         bool useThreadPool_in,
         bool useReactor_in,
         const ACE_Time_Value& statisticReportingInterval_in,
         const std::string& URL_in,
         const ACE_INET_Addr& remoteHost_in,
         struct Test_I_URLStreamLoad_Configuration& configuration_in,
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
         struct Test_I_URLStreamLoad_GTK_CBData& CBData_in,
#endif // GTK_USE
#endif // GUI_SUPPORT
         const ACE_Sig_Set& signalSet_in,
         const ACE_Sig_Set& ignoredSignalSet_in,
         Common_SignalActions_t& previousSignalActions_inout,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
         bool showConsole_in,
#endif // ACE_WIN32 || ACE_WIN64
         Test_I_SignalHandler& signalHandler_in)
{
  NETWORK_TRACE (ACE_TEXT ("::do_work"));

  // step0a: initialize configuration and stream
#if defined (GUI_SUPPORT) && defined (GTK_USE)
  Test_I_EventHandler message_handler (&CBData_in);
#else
  Test_I_EventHandler message_handler;
#endif // GUI_SUPPORT && GTK_USE
  Test_I_Module_EventHandler_Module event_handler_module (NULL,
                                                          ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_MESSAGEHANDLER_DEFAULT_NAME_STRING));

  Stream_AllocatorHeap_T<ACE_MT_SYNCH,
                         struct Common_FlexParserAllocatorConfiguration> heap_allocator;
  if (!heap_allocator.initialize (configuration_in.streamConfiguration.allocatorConfiguration_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize heap allocator, returning\n")));
    return;
  } // end IF
  Test_I_MessageAllocator_t message_allocator (NET_STREAM_MAX_MESSAGES, // maximum #buffers
                                               &heap_allocator,         // heap allocator handle
                                               true);                   // block ?

  Test_I_ConnectionManager_t* connection_manager_p =
    TEST_I_CONNECTIONMANAGER_SINGLETON::instance ();
  ACE_ASSERT (connection_manager_p);
  connection_manager_p->initialize (std::numeric_limits<unsigned int>::max ());

  // *********************** socket configuration data ************************
  Test_I_URLStreamLoad_ConnectionConfiguration_t connection_configuration;
  connection_configuration.socketHandlerConfiguration.socketConfiguration_2.address =
    remoteHost_in;
  connection_configuration.socketHandlerConfiguration.socketConfiguration_2.useLoopBackDevice =
    connection_configuration.socketHandlerConfiguration.socketConfiguration_2.address.is_loopback ();
  connection_configuration.socketHandlerConfiguration.statisticReportingInterval =
    statisticReportingInterval_in;
  //connection_configuration.socketHandlerConfiguration.userData =
  //  &CBData_in.configuration->userData;
  connection_configuration.messageAllocator = &message_allocator;
  //connection_configuration.PDUSize = bufferSize_in;
  //connection_configuration.userData = &CBData_in.configuration->userData;
  connection_configuration.initialize (configuration_in.streamConfiguration.allocatorConfiguration_,
                                       configuration_in.streamConfiguration);

  configuration_in.connectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (""),
                                                                    connection_configuration));
  Test_I_URLStreamLoad_ConnectionConfigurationIterator_t iterator =
    configuration_in.connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != configuration_in.connectionConfigurations.end ());
  (*iterator).second.socketHandlerConfiguration.connectionConfiguration =
    &((*iterator).second);

  // ********************** stream configuration data **************************
  // ********************** parser configuration data **************************
  configuration_in.parserConfiguration.debugParser = debugParser_in;
  if (debugParser_in)
    configuration_in.parserConfiguration.debugScanner = true;
  // ********************** module configuration data **************************
  struct Stream_ModuleConfiguration module_configuration;
  struct Test_I_URLStreamLoad_ModuleHandlerConfiguration modulehandler_configuration;
  modulehandler_configuration.allocatorConfiguration =
    &configuration_in.streamConfiguration.allocatorConfiguration_;
  modulehandler_configuration.connectionConfigurations =
    &configuration_in.connectionConfigurations;
  //modulehandler_configuration.connectionManager = connection_manager_p;
  modulehandler_configuration.parserConfiguration =
    &configuration_in.parserConfiguration;
  modulehandler_configuration.statisticReportingInterval =
    statisticReportingInterval_in;
//  modulehandler_configuration.streamConfiguration =
//    &configuration_in.streamConfiguration;
  modulehandler_configuration.subscriber = &message_handler;
  modulehandler_configuration.targetFileName = fileName_in;
  modulehandler_configuration.URL = URL_in;
  // ******************** (sub-)stream configuration data *********************
  //if (bufferSize_in)
  //  CBData_in.configuration->allocatorConfiguration.defaultBufferSize =
  //    bufferSize_in;
  configuration_in.streamConfiguration.configuration_.messageAllocator =
    &message_allocator;
  configuration_in.streamConfiguration.configuration_.module =
    &event_handler_module;
  configuration_in.streamConfiguration.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (""),
                                                               std::make_pair (module_configuration,
                                                                               modulehandler_configuration)));
  configuration_in.streamConfiguration.configuration_.printFinalReport =
    true;
  //configuration_in.streamConfiguration.configuration_.userData =
  //  &configuration_in.userData;

  //module_handler_p->initialize (configuration.moduleHandlerConfiguration);

  // step0c: initialize connection manager
  connection_manager_p->initialize (std::numeric_limits<unsigned int>::max ());
  connection_manager_p->set ((*iterator).second,
                             &configuration_in.userData);

  Common_Timer_Manager_t* timer_manager_p =
    COMMON_TIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (timer_manager_p);
  struct Common_TimerConfiguration timer_configuration;
  int group_id = -1;
  Common_UI_GTK_Manager_t* gtk_manager_p = NULL;
  if (useReactor_in)
    configuration_in.dispatchConfiguration.numberOfReactorThreads =
      1;
  else
    configuration_in.dispatchConfiguration.numberOfProactorThreads =
      1;
  struct Common_EventDispatchState event_dispatch_state_s;
  event_dispatch_state_s.configuration =
    &configuration_in.dispatchConfiguration;

  // step0b: initialize event dispatch
  if (!Common_Tools::initializeEventDispatch (configuration_in.dispatchConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeEventDispatch(), returning\n")));
    goto clean;
  } // end IF

  //// step0d: initialize regular (global) statistic reporting
  //Stream_StatisticHandler_Reactor_t statistic_handler (ACTION_REPORT,
  //                                                     connection_manager_p,
  //                                                     false);
  ////Stream_StatisticHandler_Proactor_t statistic_handler_proactor (ACTION_REPORT,
  ////                                                               connection_manager_p,
  ////                                                               false);
  //long timer_id = -1;
  //if (statisticReportingInterval_in)
  //{
  //  ACE_Event_Handler* handler_p = &statistic_handler;
  //  ACE_Time_Value interval (statisticReportingInterval_in, 0);
  //  timer_id =
  //    timer_manager_p->schedule_timer (handler_p,                  // event handler
  //                                     NULL,                       // ACT
  //                                     COMMON_TIME_NOW + interval, // first wakeup time
  //                                     interval);                  // interval
  //  if (timer_id == -1)
  //  {
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to schedule timer: \"%m\", returning\n")));

  //    // clean up
  //    timer_manager_p->stop ();
  //    delete stream_p;

  //    return;
  //  } // end IF
  //} // end IF

  // step0c: initialize signal handling
  //CBData_in.configuration->signalHandlerConfiguration.hasUI =
  //  !interfaceDefinitionFile_in.empty ();
  configuration_in.signalHandlerConfiguration.dispatchState =
    &event_dispatch_state_s;
  //configuration.signalHandlerConfiguration.statisticReportingHandler =
  //  connection_manager_p;
  //configuration.signalHandlerConfiguration.statisticReportingTimerID = timer_id;
  if (!signalHandler_in.initialize (configuration_in.signalHandlerConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize signal handler, returning\n")));
    goto clean;
  } // end IF
  if (!Common_Signal_Tools::initialize ((useReactor_in ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                       : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                        signalSet_in,
                                        ignoredSignalSet_in,
                                        &signalHandler_in,
                                        previousSignalActions_inout))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Signal_Tools::initialize(), returning\n")));
    goto clean;
  } // end IF

  // intialize timers
  timer_manager_p->initialize (timer_configuration);
  timer_manager_p->start ();

  // step1a: start GTK event loop ?
#if defined (GUI_SUPPORT)
  if (!UIDefinitionFileName_in.empty ())
  {
#if defined (GTK_USE)
    gtk_manager_p = COMMON_UI_GTK_MANAGER_SINGLETON::instance ();
    ACE_ASSERT (gtk_manager_p);
    gtk_manager_p->start ();
    ACE_Time_Value timeout (0,
                            COMMON_UI_GTK_TIMEOUT_DEFAULT_MANAGER_INITIALIZATION * 1000);
    int result = ACE_OS::sleep (timeout);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::sleep(%#T): \"%m\", continuing\n"),
                  &timeout));
    if (!gtk_manager_p->isRunning ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to start GTK event dispatch, returning\n")));
      goto clean;
    } // end IF
#endif // GTK_USE

#if defined (ACE_WIN32) || defined (ACE_WIN64)
    HWND window_p = GetConsoleWindow ();
    if (!window_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::GetConsoleWindow(), returning\n")));
      goto clean;
    } // end IF
    BOOL was_visible_b = false;
    if (!showConsole_in)
      was_visible_b = ShowWindow (window_p, SW_HIDE);
    ACE_UNUSED_ARG (was_visible_b);
#endif
  } // end IF
#endif // GUI_SUPPORT

  // *WARNING*: from this point on, clean up any remote connections !

  // step1: handle events (signals, incoming connections/data, timers, ...)
  // reactor/proactor event loop:
  // - dispatch connection attempts to acceptor
  // - dispatch socket events
  // timer events:
  // - perform statistics collecting/reporting

  // step1a: initialize worker(s)
  if (!Common_Tools::startEventDispatch (event_dispatch_state_s))
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

    goto clean;
  } // end IF

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  gtk_manager_p->wait ();
#endif // GTK_USE
#endif // GUI_SUPPORT

  // step3: clean up
  connection_manager_p->stop ();
  //Common_Tools::finalizeEventDispatch (useReactor_in,
  //                                     !useReactor_in,
  //                                     group_id);
  connection_manager_p->wait ();

  timer_manager_p->stop ();

  //		{ // synch access
  //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

  //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
  //					 iterator != CBData_in.event_source_ids.end();
  //					 iterator++)
  //				g_source_remove(*iterator);
  //		} // end lock scope
  //timer_manager_p->stop ();

  //  { // synch access
  //    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

  //		for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
  //				 iterator != CBData_in.event_source_ids.end();
  //				 iterator++)
  //			g_source_remove(*iterator);
  //	} // end lock scope

  //if (!interfaceDefinitionFile_in.empty ())
  //{
  //  int result = event_handler_module.close (ACE_Module_Base::M_DELETE_NONE);
  //  if (result == -1)
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("%s: failed to ACE_Module::close (): \"%m\", continuing\n"),
  //                event_handler_module.name ()));
  //} // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));

  return;

clean:
  Common_Tools::finalizeEventDispatch (useReactor_in,
                                       !useReactor_in,
                                       group_id);
  timer_manager_p->stop ();
#if defined (GUI_SUPPORT)
  if (!UIDefinitionFileName_in.empty ())
#if defined (GTK_USE)
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();
#else
    ;
#endif // GTK_USE
#endif // GUI_SUPPORT
}

void
do_print_version (const std::string& programName_in)
{
  NETWORK_TRACE (ACE_TEXT ("::do_print_version"));

  std::ostringstream converter;

  // compiler version string
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
#if defined (HAVE_CONFIG_H)
            << ACE_TEXT (ACENETWORK_PACKAGE_NAME)
            << ACE_TEXT (": ")
            << ACE_TEXT (ACENETWORK_PACKAGE_VERSION)
            << std::endl
#endif // HAVE_CONFIG_H
            ;

  converter.str ("");
  // ACE version string
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
  ACE_Profile_Timer process_profile;
  std::string path;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  bool show_console;
#endif // ACE_WIN32 || ACE_WIN64
  bool debug_parser;
  std::string configuration_path;
  std::string output_file;
  std::string hostname;
  std::string ui_definition_file;
  bool log_to_file;
  bool use_thread_pool;
  unsigned short port;
  bool use_reactor;
  ACE_Time_Value statistic_reporting_interval;
  bool trace_information;
  std::string url;
  bool print_version_and_exit;
  ACE_INET_Addr address;
  struct Test_I_URLStreamLoad_Configuration configuration;
  Common_MessageStack_t* logstack_p = NULL;
  ACE_SYNCH_MUTEX* lock_p = NULL;
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  std::string gtk_rc_file;
  struct Test_I_URLStreamLoad_GTK_CBData gtk_cb_data;
  Common_UI_GTK_Manager_t* gtk_manager_p =
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ();
  ACE_ASSERT (gtk_manager_p);
  Common_UI_GTK_State_t& state_r =
    const_cast<Common_UI_GTK_State_t&> (gtk_manager_p->getR_2 ());
  logstack_p = &state_r.logStack;
  lock_p = &state_r.logStackLock;
#endif // GTK_USE
#endif // GUI_SUPPORT
  Common_Logger_t logger (logstack_p,
                          lock_p);
  std::string log_file_name;
  ACE_Sig_Set signal_set (0);
  ACE_Sig_Set ignored_signal_set (0);
  Common_SignalActions_t previous_signal_actions;
  sigset_t previous_signal_mask;
  ACE_SYNCH_RECURSIVE_MUTEX* lock_2 = NULL;
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  lock_2 = &state_r.subscribersLock;
#endif // GTK_USE
#endif // GUI_SUPPORT
  Test_I_SignalHandler signal_handler (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                       lock_2);
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  Test_I_URLStreamLoad_GtkBuilderDefinition_t ui_definition (argc_in,
                                                             argv_in,
                                                             &gtk_cb_data);
#endif // GTK_USE
#endif // GUI_SUPPORT
  ACE_High_Res_Timer timer;
  std::string working_time_string;
  ACE_Time_Value working_time;
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  ACE_Profile_Timer::Rusage elapsed_rusage;
  std::string user_time_string, system_time_string;
  ACE_Time_Value user_time, system_time;

  // step0: initialize
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::init ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64
  process_profile.start ();

  Common_Tools::initialize ();

  // step1a set defaults
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  show_console = false;
#endif // ACE_WIN32 || ACE_WIN64
  debug_parser = COMMON_PARSER_DEFAULT_YACC_TRACE;
  path =
    Common_File_Tools::getWorkingDirectory ();
#if defined (GUI_SUPPORT)
  configuration_path = path;
  configuration_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  configuration_path +=
      ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
#if defined (GTK_USE)
  gtk_rc_file = configuration_path;
  gtk_rc_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  gtk_rc_file += ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_DEFAULT_RC_FILE);
#endif // GTK_USE
#endif // GUI_SUPPORT
  output_file = path;
  output_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  output_file +=
    ACE_TEXT_ALWAYS_CHAR (TEST_I_URLSTREAMLOAD_DEFAULT_OUTPUT_FILE);
  ui_definition_file = configuration_path;
  ui_definition_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  ui_definition_file +=
    ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_DEFAULT_GLADE_FILE);
  log_to_file = false;
  use_thread_pool = COMMON_EVENT_REACTOR_DEFAULT_USE_THREADPOOL;
  port = HTTP_DEFAULT_SERVER_PORT;
  use_reactor =
      (COMMON_EVENT_DEFAULT_DISPATCH == COMMON_EVENT_DISPATCH_REACTOR);
  statistic_reporting_interval =
    (STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL ? ACE_Time_Value (STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL, 0)
                                                 : ACE_Time_Value::zero);
  trace_information = false;
  print_version_and_exit = false;
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (elapsed_rusage));

  // step1b: parse/process/validate configuration
  if (!do_process_arguments (argc_in,
                             argv_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                             show_console,
#endif // ACE_WIN32 || ACE_WIN64
                             debug_parser,
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
                             gtk_rc_file,
#endif // GTK_USE
#endif // GUI_SUPPORT
                             output_file,
                             ui_definition_file,
                             hostname,
                             log_to_file,
                             use_thread_pool,
                             port,
                             use_reactor,
                             statistic_reporting_interval,
                             trace_information,
                             url,
                             print_version_and_exit,
                             address))
  {
    do_print_usage (std::string (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0],
                                                                      ACE_DIRECTORY_SEPARATOR_CHAR))));
    goto error;
  } // end IF

  // step1c: validate arguments
  // *IMPORTANT NOTE*: iff the number of message buffers is limited, the
  //                   reactor/proactor thread could (dead)lock on the
  //                   allocator lock, as it cannot dispatch events that would
  //                   free slots
  if (TEST_I_MAX_MESSAGES)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("limiting the number of message buffers could (!) lead to deadlocks --> make sure you know what you are doing...\n")));
  //if (use_reactor                      &&
  //    (number_of_dispatch_threads > 1) &&
  //    !use_thread_pool)
  //{ // *NOTE*: see also: man (2) select
  //  // *TODO*: verify this for MS Windows based systems
  //  ACE_DEBUG ((LM_WARNING,
  //              ACE_TEXT ("the select()-based reactor is not reentrant, using the thread-pool reactor instead...\n")));
  //  use_thread_pool = true;
  //} // end IF
  if ((!ui_definition_file.empty () &&
       !Common_File_Tools::isReadable (ui_definition_file))
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
      || (!gtk_rc_file.empty () &&
          !Common_File_Tools::isReadable (gtk_rc_file)))//                       ||
#else
     )
#endif // GTK_USE
#endif // GUI_SUPPORT
      //(use_reactor && (number_of_dispatch_threads > 1) && !use_thread_pool))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid arguments, aborting\n")));
    do_print_usage (std::string (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0],
                                                                      ACE_DIRECTORY_SEPARATOR_CHAR))));
    goto error;
  } // end IF

  // step1d: initialize logging and/or tracing
  if (log_to_file)
  {
    log_file_name =
      Common_Log_Tools::getLogFilename (std::string (ACE_TEXT_ALWAYS_CHAR (ACENETWORK_PACKAGE_NAME)),
                                        std::string (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0],
                                                                                          ACE_DIRECTORY_SEPARATOR_CHAR))));
    if (log_file_name.empty ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_File_Tools::getLogFilename(), aborting\n")));
      goto error;
    } // end IF
  } // end IF
  if (!Common_Log_Tools::initializeLogging (std::string (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0],
                                                                                              ACE_DIRECTORY_SEPARATOR_CHAR))), // program name
                                            log_file_name,                                                                     // log file name
                                            false,                                                                             // log to syslog ?
                                            false,                                                                             // trace messages ?
                                            trace_information,                                                                 // debug messages ?
                                            (ui_definition_file.empty () ? NULL
                                                                         : &logger)))                                          // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initializeLogging(), aborting\n")));
    goto error;
  } // end IF

  // step1e: pre-initialize signal handling
  do_initialize_signals (true, // allow SIGUSR1/SIGBREAK
                         signal_set,
                         ignored_signal_set);
  result = ACE_OS::sigemptyset (&previous_signal_mask);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  if (!Common_Signal_Tools::preInitialize (signal_set,
                                           use_reactor,
                                           previous_signal_actions,
                                           previous_signal_mask))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Signal_Tools::preInitialize(), aborting\n")));
    goto error;
  } // end IF

  // step1f: handle specific program modes
  if (print_version_and_exit)
  {
    do_print_version (std::string (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0],
                                                                        ACE_DIRECTORY_SEPARATOR_CHAR))));

    Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                   signal_set,
                                   previous_signal_actions,
                                   previous_signal_mask);
    Common_Log_Tools::finalizeLogging ();
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
    return EXIT_SUCCESS;
  } // end IF

  // step1g: set process resource limits
  // *NOTE*: settings will be inherited by any child processes
  if (!Common_Tools::setResourceLimits (false, // file descriptors
                                        true,  // stack traces
                                        true)) // pending signals
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::setResourceLimits(), aborting\n")));
    goto error;
  } // end IF

  // step1h: initialize GLIB / G(D|T)K[+] / GNOME ?
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  if (!gtk_rc_file.empty ())
    state_r.RCFiles.push_back (gtk_rc_file);
#endif // GTK_USE
#endif // GUI_SUPPORT
  //Common_UI_GladeDefinition ui_definition (argc_in,
  //                                         argv_in);
  if (!ui_definition_file.empty ())
  {
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
    state_r.argc = argc_in;
    state_r.argv = argv_in;
    state_r.builders[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)] =
      std::make_pair (ui_definition_file, static_cast<GtkBuilder*> (NULL));
    state_r.eventHooks.finiHook = idle_finalize_UI_cb;
    state_r.eventHooks.initHook = idle_initialize_UI_cb;
    gtk_cb_data.configuration = &configuration;
    gtk_cb_data.progressData.state = &state_r;
    //gtk_cb_data.userData = &configuration.userData;
    if (!gtk_manager_p->initialize (argc_in,
                                    argv_in,
                                    &ui_definition))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_UI_GTK_Manager::initialize(), aborting\n")));
      goto error;
    } // end IF
#endif // GTK_USE
#endif // GUI_SUPPORT
  } // end IF

  timer.start ();
  // step2: do actual work
  do_work (debug_parser,
           output_file,
           ui_definition_file,
           use_thread_pool,
           use_reactor,
           statistic_reporting_interval,
           url,
           address,
           configuration,
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
           gtk_cb_data,
#endif // GTK_USE
#endif // GUI_SUPPORT
           signal_set,
           ignored_signal_set,
           previous_signal_actions,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
           show_console,
#endif // ACE_WIN32 || ACE_WIN64
           signal_handler);
  timer.stop ();

  // debug info
  timer.elapsed_time (working_time);
  working_time_string = Common_Timer_Tools::periodToString (working_time);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));
  process_profile.stop ();

  // only process profile left to do
  elapsed_time.real_time = 0.0;
  elapsed_time.user_time = 0.0;
  elapsed_time.system_time = 0.0;
  result = process_profile.elapsed_time (elapsed_time);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Profile_Timer::elapsed_time: \"%m\", aborting\n")));
    goto error;
  } // end IF
  process_profile.elapsed_rusage (elapsed_rusage);
  user_time.set (elapsed_rusage.ru_utime);
  system_time.set (elapsed_rusage.ru_stime);
  user_time_string = Common_Timer_Tools::periodToString (user_time);
  system_time_string = Common_Timer_Tools::periodToString (system_time);

  // debug info
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              ACE_TEXT (user_time_string.c_str ()),
              ACE_TEXT (system_time_string.c_str ())));
#else
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
#endif // ACE_WIN32 || ACE_WIN64

  Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                              : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                 signal_set,
                                 previous_signal_actions,
                                 previous_signal_mask);
  Common_Log_Tools::finalizeLogging ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

  return EXIT_SUCCESS;

error:
  Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                              : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                 signal_set,
                                 previous_signal_actions,
                                 previous_signal_mask);
  Common_Log_Tools::finalizeLogging ();
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
  return EXIT_FAILURE;
} // end main
