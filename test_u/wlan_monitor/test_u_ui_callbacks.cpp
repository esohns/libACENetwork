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

#include "test_u_ui_callbacks.h"

#include "ace/Log_Msg.h"
#include "ace/Synch_Traits.h"

#include "ace/Synch.h"
#include "common_timer_manager.h"

#include "common_ui_common.h"
#include "common_ui_defines.h"
#include "common_ui_gtk_manager_common.h"
#include "common_ui_tools.h"

#include "net_defines.h"
#include "net_macros.h"

#include "net_wlan_configuration.h"

#include "wlan_monitor_common.h"
#include "wlan_monitor_defines.h"

bool
load_wlan_interfaces (GtkListStore* listStore_in)
{
  NETWORK_TRACE (ACE_TEXT ("::load_wlan_interfaces"));

  // initialize result
  gtk_list_store_clear (listStore_in);

  bool result = false;
  GtkTreeIter iterator;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  HANDLE handle_client = NULL;
  // *TODO*: support WinXP
  DWORD maximum_client_version =
    WLAN_API_MAKE_VERSION (2, 0); // *NOTE*: 1 for <= WinXP_SP2
  DWORD current_version = 0;
  DWORD result_2 = 0;
  PWLAN_INTERFACE_INFO_LIST interface_list_p = NULL;
  PWLAN_INTERFACE_INFO interface_info_p = NULL;

  result_2 = WlanOpenHandle (maximum_client_version,
                             NULL,
                             &current_version,
                             &handle_client);
  if (result_2 != ERROR_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::WlanOpenHandle(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
    return false;
  } // end IF
  result_2 = WlanEnumInterfaces (handle_client,
                                 NULL,
                                 &interface_list_p);
  if (result_2 != ERROR_SUCCESS)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::WlanEnumInterfaces(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
    goto error;
  } // end IF
  ACE_ASSERT (interface_list_p);

  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("found %u wireless adapter(s)\n"),
  //            interface_list_p->dwNumberOfItems));
  for (DWORD i = 0;
       i < interface_list_p->dwNumberOfItems;
       ++i)
  {
    interface_info_p = &interface_list_p->InterfaceInfo[i];
    gtk_list_store_append (listStore_in, &iterator);
    gtk_list_store_set (listStore_in, &iterator,
                        0, ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (interface_info_p->strInterfaceDescription)),
                        1, ACE_TEXT_ALWAYS_CHAR (Common_Tools::GUIDToString (interface_info_p->InterfaceGuid).c_str ()),
                        -1);
  } // end FOR

  result = true;

error:
  if (interface_list_p)
    WlanFreeMemory (interface_list_p);
  result_2 = WlanCloseHandle (handle_client,
                              NULL);
  if (result_2 != ERROR_SUCCESS)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::WlanCloseHandle(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Tools::errorToString (result_2).c_str ())));
#else
#if defined (ACE_HAS_GETIFADDRS)
  struct ifaddrs* ifaddrs_p = NULL;
  int result_2 = ::getifaddrs (&ifaddrs_p);
  if (result_2 == -1)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("failed to ::getifaddrs(): \"%m\", aborting\n")));
    return false;
  } // end IF
  ACE_ASSERT (ifaddrs_p);

  for (struct ifaddrs* ifaddrs_2 = ifaddrs_p;
       ifaddrs_2;
       ifaddrs_2 = ifaddrs_2->ifa_next)
  {
    if (!ifaddrs_2->ifa_addr                                        ||
        !(ifaddrs_2->ifa_addr->sa_family == AF_INET)                ||
        !Net_WLAN_Tools::interfaceIsWLAN (ifaddrs_2->ifa_name))
      continue;

    gtk_list_store_append (listStore_in, &iterator);
    gtk_list_store_set (listStore_in, &iterator,
                        0, ACE_TEXT_ALWAYS_CHAR (ifaddrs_2->ifa_name),
                        -1);
  } // end FOR

  // clean up
  ::freeifaddrs (ifaddrs_p);

  result = true;
#else
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);
  ACE_NOTREACHED (return false;)
#endif /* ACE_HAS_GETIFADDRS */
#endif

  return result;
}

void
#if defined (ACE_WIN32) || defined (ACE_WIN64)
load_ssids (REFGUID interfaceIdentifier_in,
#else
load_ssids (const std::string& interfaceIdentifier_in,
#endif
            Net_WLAN_SSIDs_t& SSIDs_out,
            GtkListStore* listStore_in)
{
  NETWORK_TRACE (ACE_TEXT ("::load_ssids"));

  // initialize result(s)
  SSIDs_out.clear ();
  gtk_list_store_clear (listStore_in);

  // sanity check(s)
  ACE_ASSERT (listStore_in);

  GtkTreeIter iterator;
  Net_WLAN_IInetMonitor_t* iinetwlanmonitor_p =
      NET_WLAN_INETMONITOR_SINGLETON::instance ();
  ACE_ASSERT (iinetwlanmonitor_p);
  SSIDs_out =
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      Net_WLAN_Tools::getSSIDs (iinetwlanmonitor_p->get (),
                                interfaceIdentifier_in);
#else
      Net_WLAN_Tools::getSSIDs (interfaceIdentifier_in,
                                iinetwlanmonitor_p->get ());
#endif
  for (Net_WLAN_SSIDsIterator_t iterator_2 = SSIDs_out.begin ();
       iterator_2 != SSIDs_out.end ();
       ++iterator_2)
  {
    gtk_list_store_append (listStore_in, &iterator);
    gtk_list_store_set (listStore_in, &iterator,
                        0, ACE_TEXT_ALWAYS_CHAR ((*iterator_2).c_str ()),
                        -1);
  } // end FOR
}

gboolean
idle_finalize_ui_cb (gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::idle_finalize_ui_cb"));

  // leave GTK
  gtk_main_quit ();

  return G_SOURCE_REMOVE;
}

gboolean
idle_update_log_display_cb (gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::idle_update_log_display_cb"));

  struct WLANMonitor_GTK_CBData* data_p =
    static_cast<struct WLANMonitor_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  GtkTextView* view_p =
    GTK_TEXT_VIEW (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_TEXTVIEW_LOG_NAME)));
  ACE_ASSERT (view_p);
  GtkTextBuffer* buffer_p = gtk_text_view_get_buffer (view_p);
  ACE_ASSERT (buffer_p);

  GtkTextIter text_iterator;
  gtk_text_buffer_get_end_iter (buffer_p,
                                &text_iterator);

  gchar* converted_text = NULL;
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->lock, G_SOURCE_REMOVE);
    // sanity check
    if (data_p->logStack.empty ())
      return G_SOURCE_CONTINUE;

    // step1: convert text
    for (Common_MessageStackConstIterator_t iterator_2 = data_p->logStack.begin ();
         iterator_2 != data_p->logStack.end ();
         ++iterator_2)
    {
      converted_text = Common_UI_Tools::LocaleToUTF8 (*iterator_2);
      if (!converted_text)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_UI_Tools::LocaleToUTF8(\"%s\"), aborting\n"),
                    ACE_TEXT ((*iterator_2).c_str ())));
        return G_SOURCE_REMOVE;
      } // end IF

        // step2: display text
      gtk_text_buffer_insert (buffer_p,
                              &text_iterator,
                              converted_text,
                              -1);

      // clean up
      g_free (converted_text);
    } // end FOR

    data_p->logStack.clear ();
  } // end lock scope

    // step3: scroll the view accordingly
    //  // move the iterator to the beginning of line, so it doesn't scroll
    //  // in horizontal direction
    //  gtk_text_iter_set_line_offset (&text_iterator, 0);

    //  // ...and place the mark at iter. The mark will stay there after insertion
    //  // because it has "right" gravity
    //  GtkTextMark* text_mark_p =
    //      gtk_text_buffer_get_mark (buffer_p,
    //                                ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_SCROLLMARK_NAME));
    ////  gtk_text_buffer_move_mark (buffer_p,
    ////                             text_mark_p,
    ////                             &text_iterator);

    //  // scroll the mark onscreen
    //  gtk_text_view_scroll_mark_onscreen (view_p,
    //                                      text_mark_p);
  GtkAdjustment* adjustment_p =
    GTK_ADJUSTMENT (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_ADJUSTMENT_SCROLLEDWINDOW_V_NAME)));
  ACE_ASSERT (adjustment_p);
  gtk_adjustment_set_value (adjustment_p,
                            gtk_adjustment_get_upper (adjustment_p));

  return G_SOURCE_CONTINUE;
}

gboolean
idle_session_start_cb (gpointer userData_in)
{
  STREAM_TRACE (ACE_TEXT ("::idle_session_start_cb"));

  struct WLANMonitor_GTK_CBData* data_p =
    static_cast<struct WLANMonitor_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  GtkSpinButton* spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_SPINBUTTON_NOTIFICATIONS_NAME)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_value (spin_button_p, 0.0);

  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->lock, G_SOURCE_REMOVE);
    ACE_OS::memset (&data_p->progressData.statistic, 0, sizeof (Stream_Statistic));
  } // end lock scope

  return G_SOURCE_REMOVE;
}

gboolean
idle_update_info_display_cb (gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::idle_update_info_display_cb"));

  struct WLANMonitor_GTK_CBData* data_p =
    static_cast<struct WLANMonitor_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  GtkSpinButton* spin_button_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_SPINBUTTON_NOTIFICATIONS_NAME)));
  ACE_ASSERT (spin_button_p);
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->lock, G_SOURCE_REMOVE);
    if (data_p->eventStack.empty ())
      return G_SOURCE_CONTINUE;

    for (WLANMMMonitor_EventsIterator_t iterator_2 = data_p->eventStack.begin ();
         iterator_2 != data_p->eventStack.end ();
         iterator_2++)
    {
      switch (*iterator_2)
      {
        case WLAN_MONITOR_EVENT_ASSOCIATE_AP:
        {
          break;
        }
        case WLAN_MONITOR_EVENT_CONNECT_ESSID:
        {
          break;
        }
        case WLAN_MONITOR_EVENT_SCAN_COMPLETE:
        {
          break;
        }
        case WLAN_MONITOR_EVENT_INTERFACE_HOTPLUG:
        {
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown event type (was: %d), continuing\n"),
                      *iterator_2));
          break;
        }
      } // end SWITCH

      gtk_spin_button_spin (spin_button_p,
                            GTK_SPIN_STEP_FORWARD,
                            1.0);
    } // end FOR
    data_p->eventStack.clear ();
  } // end lock scope

  return G_SOURCE_CONTINUE;
}

gboolean
idle_update_ssids_cb (gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::idle_update_ssids_cb"));

  struct WLANMonitor_GTK_CBData* data_p =
    static_cast<struct WLANMonitor_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  GtkListStore* list_store_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_LISTSTORE_SSID_NAME)));
  ACE_ASSERT (list_store_p);

  GtkTreeIter iterator_2;
  Net_WLAN_IInetMonitor_t* iinetwlanmonitor_p =
      NET_WLAN_INETMONITOR_SINGLETON::instance ();
  ACE_ASSERT (iinetwlanmonitor_p);
  Net_WLAN_SSIDs_t ssids = iinetwlanmonitor_p->SSIDs ();
  for (Net_WLAN_SSIDsIterator_t iterator_3 = ssids.begin ();
       iterator_3 != ssids.end ();
       ++iterator_3)
  {
    gtk_list_store_append (list_store_p, &iterator_2);
    gtk_list_store_set (list_store_p, &iterator_2,
                        0, ACE_TEXT_ALWAYS_CHAR ((*iterator_3).c_str ()),
                        -1);
  } // end FOR

  return G_SOURCE_CONTINUE;
}

//////////////////////////////////////////

gboolean
idle_initialize_ui_cb (gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::idle_initialize_ui_cb"));

  struct WLANMonitor_GTK_CBData* data_p =
    static_cast<struct WLANMonitor_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->configuration);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  // step1: initialize dialog window(s)
  GtkWidget* dialog_p =
    GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_DIALOG_MAIN_NAME)));
  ACE_ASSERT (dialog_p);
  //  GtkWidget* image_icon = gtk_image_new_from_file (path.c_str());
  //  ACE_ASSERT (image_icon);
  //  gtk_window_set_icon (GTK_WINDOW (dialog_p),
  //                       gtk_image_get_pixbuf (GTK_IMAGE (image_icon)));
  //GdkWindow* dialog_window_p = gtk_widget_get_window (dialog_p);
  //gtk_window_set_title(,
  //                     caption.c_str ());

  //  GtkWidget* about_dialog_p =
  //    GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
  //                                        ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_DIALOG_ABOUT_NAME)));
  //  ACE_ASSERT (about_dialog_p);

  // step2: initialize info view
  GtkSpinButton* spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_SPINBUTTON_NOTIFICATIONS_NAME)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             std::numeric_limits<double>::max ());

  // configuration -----------------------
  GtkListStore* list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_LISTSTORE_INTERFACE_NAME)));
  ACE_ASSERT (list_store_p);
  gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (list_store_p),
                                        0, GTK_SORT_ASCENDING);
  if (!load_wlan_interfaces (list_store_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::load_wlan_interfaces(), aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF
  GtkComboBox* combo_box_p =
    GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_COMBOBOX_INTERFACE_NAME)));
  ACE_ASSERT (combo_box_p);
  //gtk_combo_box_set_model (combo_box_p,
  //                         GTK_TREE_MODEL (list_store_p));
  GtkCellRenderer* cell_renderer_p = gtk_cell_renderer_text_new ();
  if (!cell_renderer_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to gtk_cell_renderer_text_new(), aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo_box_p), cell_renderer_p,
                              true);
  // *NOTE*: cell_renderer_p does not need to be g_object_unref()ed because it
  //         is GInitiallyUnowned and the floating reference has been
  //         passed to combo_box_p by the gtk_cell_layout_pack_start() call
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo_box_p), cell_renderer_p,
                                  //"cell-background", 0,
                                  //"text", 1,
                                  "text", 0,
                                  NULL);

  list_store_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_LISTSTORE_SSID_NAME)));
  ACE_ASSERT (list_store_p);
  gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (list_store_p),
                                        0, GTK_SORT_ASCENDING);
  combo_box_p =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_COMBOBOX_SSID_NAME)));
  ACE_ASSERT (combo_box_p);
  //gtk_combo_box_set_model (combo_box_p,
    //                         GTK_TREE_MODEL (list_store_p));
  cell_renderer_p = gtk_cell_renderer_text_new ();
  if (!cell_renderer_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to gtk_cell_renderer_text_new(), aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo_box_p), cell_renderer_p,
                              true);
  // *NOTE*: cell_renderer_p does not need to be g_object_unref()ed because it
  //         is GInitiallyUnowned and the floating reference has been
  //         passed to combo_box_p by the gtk_cell_layout_pack_start() call
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo_box_p), cell_renderer_p,
                                  //"cell-background", 0,
                                  //"text", 1,
                                  "text", 0,
                                  NULL);

  // step3: initialize text view, setup auto-scrolling
  GtkTextView* view_p =
    //GTK_TEXT_VIEW (glade_xml_get_widget ((*iterator).second.second,
    //                                     ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_TEXTVIEW_NAME)));
    GTK_TEXT_VIEW (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_TEXTVIEW_LOG_NAME)));
  ACE_ASSERT (view_p);
  //  GtkTextBuffer* buffer_p =
  ////      gtk_text_buffer_new (NULL); // text tag table --> create new
  ////      gtk_text_view_set_buffer (view_p, buffer_p);
  //      gtk_text_view_get_buffer (view_p);
  //  ACE_ASSERT (buffer_p);

  //  GtkTextIter iterator;
  //  gtk_text_buffer_get_end_iter (buffer_p,
  //                                &iterator);
  //  gtk_text_buffer_create_mark (buffer_p,
  //                               ACE_TEXT_ALWAYS_CHAR (TEST_U_SCROLLMARK_NAME),
  //                               &iterator,
  //                               TRUE);
  //  g_object_unref (buffer_p);

  PangoFontDescription* font_description_p =
    pango_font_description_from_string (ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_PANGO_LOG_FONT_DESCRIPTION));
  if (!font_description_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to pango_font_description_from_string(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (WLAN_MONITOR_GTK_PANGO_LOG_FONT_DESCRIPTION)));
    return G_SOURCE_REMOVE;
  } // end IF
  // apply font
  GtkRcStyle* rc_style_p = gtk_rc_style_new ();
  if (!rc_style_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_rc_style_new(): \"%m\", aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF
  rc_style_p->font_desc = font_description_p;
  GdkColor base_colour, text_colour;
  gdk_color_parse (ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_PANGO_LOG_COLOR_BASE),
                   &base_colour);
  rc_style_p->base[GTK_STATE_NORMAL] = base_colour;
  gdk_color_parse (ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_PANGO_LOG_COLOR_TEXT),
                   &text_colour);
  rc_style_p->text[GTK_STATE_NORMAL] = text_colour;
  rc_style_p->color_flags[GTK_STATE_NORMAL] =
    static_cast<GtkRcFlags>(GTK_RC_BASE |
                            GTK_RC_TEXT);
  gtk_widget_modify_style (GTK_WIDGET (view_p),
                           rc_style_p);
  //gtk_rc_style_unref (rc_style_p);
  g_object_unref (rc_style_p);

  GtkProgressBar* progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_PROGRESSBAR_NAME)));
  ACE_ASSERT (progress_bar_p);
  gint width, height;
  gtk_widget_get_size_request (GTK_WIDGET (progress_bar_p),
                               &width, &height);
  gtk_progress_bar_set_pulse_step (progress_bar_p,
                                   1.0 / static_cast<double> (width));

  // step5: disable some functions ?
  GtkButton* button_p =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_BUTTON_REPORT_NAME)));
  ACE_ASSERT (button_p);
  gtk_widget_set_sensitive (GTK_WIDGET (button_p),
                            data_p->allowUserRuntimeStatistic);

  // step6: (auto-)connect signals/slots
  gtk_builder_connect_signals ((*iterator).second.second,
                               userData_in);

  // step6a: connect default signals
  gulong result =
    g_signal_connect (dialog_p,
                      ACE_TEXT_ALWAYS_CHAR ("destroy"),
                      G_CALLBACK (gtk_widget_destroyed),
                      &dialog_p);
  ACE_ASSERT (result);

  //   // step7: use correct screen
  //   if (parentWidget_in)
  //     gtk_window_set_screen (GTK_WINDOW (dialog_p),
  //                            gtk_widget_get_screen (const_cast<GtkWidget*> (//parentWidget_in)));

  // step8: draw main dialog
  gtk_widget_show_all (dialog_p);

  // step9: initialize updates
  guint event_source_id = 0;
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->lock, G_SOURCE_REMOVE);
    // schedule asynchronous updates of the log view
    event_source_id = g_timeout_add_seconds (1,
                                             idle_update_log_display_cb,
                                             data_p);
    if (event_source_id > 0)
      data_p->eventSourceIds.insert (event_source_id);
    else
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_timeout_add_seconds(): \"%m\", aborting\n")));
      return G_SOURCE_REMOVE;
    } // end ELSE
    // schedule asynchronous updates of the info view
    event_source_id = g_timeout_add (COMMON_UI_GTK_WIDGET_UPDATE_INTERVAL,
                                     idle_update_info_display_cb,
                                     data_p);
    if (event_source_id > 0)
      data_p->eventSourceIds.insert (event_source_id);
    else
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_timeout_add(): \"%m\", aborting\n")));
      return G_SOURCE_REMOVE;
    } // end ELSE
  } // end lock scope

  // step10: activate some widgets
  GtkToggleButton* toggle_button_p =
      GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                  ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_TOGGLEBUTTON_MONITOR_NAME)));
  ACE_ASSERT (toggle_button_p);
  gtk_toggle_button_set_active (toggle_button_p,
                                true);

  combo_box_p =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_COMBOBOX_INTERFACE_NAME)));
    ACE_ASSERT (combo_box_p);
  list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_LISTSTORE_INTERFACE_NAME)));
  ACE_ASSERT (list_store_p);
  gint n_rows = gtk_tree_model_iter_n_children (GTK_TREE_MODEL (list_store_p),
                                                NULL);
  gtk_widget_set_sensitive (GTK_WIDGET (combo_box_p), (n_rows > 0));
  if (n_rows > 0)
    gtk_combo_box_set_active (combo_box_p, 0);


  GtkCheckButton* check_button_p =
      GTK_CHECK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_CHECKBUTTON_AUTOASSOCIATE_NAME)));
  ACE_ASSERT (check_button_p);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_button_p),
                                data_p->configuration->WLANMonitorConfiguration.autoAssociate);

  return G_SOURCE_REMOVE;
}

gboolean
idle_update_progress_cb (gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::idle_update_progress_cb"));

  struct WLANMonitor_GTK_CBData* data_p =
    static_cast<struct WLANMonitor_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->configuration);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  GtkProgressBar* progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_PROGRESSBAR_NAME)));
  ACE_ASSERT (progress_bar_p);
  gtk_progress_bar_pulse (progress_bar_p);

  return G_SOURCE_CONTINUE;
}

//------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
void
togglebutton_monitor_toggled_cb (GtkToggleButton* toggleButton_in,
                                 gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::togglebutton_monitor_toggled_cb"));

  struct WLANMonitor_GTK_CBData* data_p =
    static_cast<struct WLANMonitor_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->configuration);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  bool is_active = gtk_toggle_button_get_active (toggleButton_in);
  Net_WLAN_IInetMonitor_t* iinetwlanmonitor_p =
      NET_WLAN_INETMONITOR_SINGLETON::instance ();
  ACE_ASSERT (iinetwlanmonitor_p);
//  GtkFrame* frame_p =
//      GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
//                                         ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_FRAME_CONFIGURATION_NAME)));
//  ACE_ASSERT (frame_p);
  GtkProgressBar* progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_PROGRESSBAR_NAME)));
  ACE_ASSERT (progress_bar_p);

  if (is_active)
  {
    // update configuration
    gtk_button_set_label (GTK_BUTTON (toggleButton_in),
                          GTK_STOCK_MEDIA_STOP);

    GtkSpinButton* spin_button_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_SPINBUTTON_NOTIFICATIONS_NAME)));
    ACE_ASSERT (spin_button_p);
    gtk_spin_button_set_value (spin_button_p, 0.0);

    GtkListStore* list_store_p = NULL;
#if GTK_CHECK_VERSION (3,0,0)
    GValue value = G_VALUE_INIT;
#else
    GValue value;
    g_value_init (&value, G_TYPE_STRING);
#endif
    GtkTreeIter iterator_2;
    GtkComboBox* combo_box_p =
        GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_COMBOBOX_INTERFACE_NAME)));
    ACE_ASSERT (combo_box_p);
    if (gtk_combo_box_get_active_iter (combo_box_p,
                                       &iterator_2))
    {
      list_store_p =
          GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                                  ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_LISTSTORE_INTERFACE_NAME)));
      ACE_ASSERT (list_store_p);
      gtk_tree_model_get_value (GTK_TREE_MODEL (list_store_p),
                                &iterator_2,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                1, &value);
      ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_STRING);
      data_p->configuration->WLANMonitorConfiguration.interfaceIIdentifier =
        Common_Tools::StringToGUID (g_value_get_string (&value));
#else
                                0, &value);
      ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_STRING);
      data_p->configuration->WLANMonitorConfiguration.interfaceIdentifier =
          ACE_TEXT_ALWAYS_CHAR (g_value_get_string (&value));
#endif
      g_value_unset (&value);
    } // end IF

    combo_box_p =
        GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_COMBOBOX_SSID_NAME)));
    ACE_ASSERT (combo_box_p);
    if (gtk_combo_box_get_active_iter (combo_box_p,
                                       &iterator_2))
    {
      list_store_p =
          GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                                  ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_LISTSTORE_SSID_NAME)));
      ACE_ASSERT (list_store_p);
#if GTK_CHECK_VERSION (3,0,0)
      value = G_VALUE_INIT;
#else
      g_value_init (&value, G_TYPE_STRING);
#endif
      gtk_tree_model_get_value (GTK_TREE_MODEL (list_store_p),
                                &iterator_2,
                                0, &value);
      ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_STRING);
      data_p->configuration->WLANMonitorConfiguration.SSID =
          ACE_TEXT_ALWAYS_CHAR (g_value_get_string (&value));
      g_value_unset (&value);
    } // end IF

    if (!iinetwlanmonitor_p->initialize (data_p->configuration->WLANMonitorConfiguration))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_IInitialize_T::initialize(), aborting\n")));
      goto error;
    } // end IF

    try {
      iinetwlanmonitor_p->start ();
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Common_ITask_T::start(): \"%m\", aborting\n")));
      goto error;
    } // end catch

    // start progress reporting
    ACE_ASSERT (!data_p->eventSourceId);
    { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, data_p->lock);
      data_p->eventSourceId =
          //g_idle_add_full (G_PRIORITY_DEFAULT_IDLE, // _LOW doesn't work (on Win32)
          //                 idle_update_progress_cb,
          //                 &data_p->progressData,
          //                 NULL);
          g_timeout_add_full (G_PRIORITY_DEFAULT_IDLE,                   // _LOW doesn't work (on Win32)
                              COMMON_UI_GTK_PROGRESSBAR_UPDATE_INTERVAL, // ms (?)
                              idle_update_progress_cb,
                              userData_in,
                              NULL);
      if (data_p->eventSourceId > 0)
        data_p->eventSourceIds.insert (data_p->eventSourceId);
      else
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to g_timeout_add_full(idle_update_progress_cb): \"%m\", continuing\n")));
    } // end lock scope
  } // end IF
  else
  {
    gtk_button_set_label (GTK_BUTTON (toggleButton_in),
                          GTK_STOCK_MEDIA_PLAY);

    try {
      iinetwlanmonitor_p->stop (false,
                                true);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Common_ITask_T::stop(): \"%m\", aborting\n")));
      goto error;
    } // end catch

    // stop progress reporting
    ACE_ASSERT (data_p->eventSourceId);
    { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, data_p->lock);
      if (!g_source_remove (data_p->eventSourceId))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to g_source_remove(%u), continuing\n"),
                    data_p->eventSourceId));
      data_p->eventSourceIds.erase (data_p->eventSourceId);
      data_p->eventSourceId = 0;
    } // end lock scope
  } // end ELSE
  //  gtk_widget_set_sensitive (GTK_WIDGET (frame_p),
//                            !is_active);
  gtk_widget_set_sensitive (GTK_WIDGET (progress_bar_p),
                            is_active);

  return;

error:
//  gtk_widget_set_sensitive (GTK_WIDGET (frame_p),
//                            true);
  gtk_widget_set_sensitive (GTK_WIDGET (progress_bar_p),
                            false);
} // togglebutton_monitor_toggled_cb

void
togglebutton_connect_toggled_cb (GtkToggleButton* toggleButton_in,
                                 gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::togglebutton_connect_toggled_cb"));

  struct WLANMonitor_GTK_CBData* data_p =
    static_cast<struct WLANMonitor_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->configuration);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  bool is_active = gtk_toggle_button_get_active (toggleButton_in);
  Net_WLAN_IInetMonitor_t* iinetwlanmonitor_p =
      NET_WLAN_INETMONITOR_SINGLETON::instance ();
  ACE_ASSERT (iinetwlanmonitor_p);
  GtkFrame* frame_p =
      GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_FRAME_CONFIGURATION_NAME)));
  ACE_ASSERT (frame_p);

  if (is_active)
  {
    // update configuration
    gtk_button_set_label (GTK_BUTTON (toggleButton_in),
                          GTK_STOCK_MEDIA_STOP);

    GtkSpinButton* spin_button_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_SPINBUTTON_NOTIFICATIONS_NAME)));
    ACE_ASSERT (spin_button_p);
    gtk_spin_button_set_value (spin_button_p, 0.0);

    GtkListStore* list_store_p = NULL;
#if GTK_CHECK_VERSION (3,0,0)
    GValue value = G_VALUE_INIT;
#else
    GValue value;
    g_value_init (&value, G_TYPE_STRING);
#endif
    GtkTreeIter iterator_2;
    GtkComboBox* combo_box_p =
        GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_COMBOBOX_INTERFACE_NAME)));
    ACE_ASSERT (combo_box_p);
    if (gtk_combo_box_get_active_iter (combo_box_p,
                                       &iterator_2))
    {
      list_store_p =
          GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                                  ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_LISTSTORE_INTERFACE_NAME)));
      ACE_ASSERT (list_store_p);
      gtk_tree_model_get_value (GTK_TREE_MODEL (list_store_p),
                                &iterator_2,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                1, &value);
      ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_STRING);
      data_p->configuration->WLANMonitorConfiguration.interfaceIIdentifier =
        Common_Tools::StringToGUID (g_value_get_string (&value));
#else
                                0, &value);
      ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_STRING);
      data_p->configuration->WLANMonitorConfiguration.interfaceIdentifier =
          ACE_TEXT_ALWAYS_CHAR (g_value_get_string (&value));
#endif
      g_value_unset (&value);
    } // end IF

    combo_box_p =
        GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_COMBOBOX_SSID_NAME)));
    ACE_ASSERT (combo_box_p);
    if (gtk_combo_box_get_active_iter (combo_box_p,
                                       &iterator_2))
    {
      list_store_p =
          GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                                  ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_LISTSTORE_SSID_NAME)));
      ACE_ASSERT (list_store_p);
#if GTK_CHECK_VERSION (3,0,0)
      value = G_VALUE_INIT;
#else
      g_value_init (&value, G_TYPE_STRING);
#endif
      gtk_tree_model_get_value (GTK_TREE_MODEL (list_store_p),
                                &iterator_2,
                                0, &value);
      ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_STRING);
      data_p->configuration->WLANMonitorConfiguration.SSID =
          ACE_TEXT_ALWAYS_CHAR (g_value_get_string (&value));
      g_value_unset (&value);
    } // end IF

    if (!iinetwlanmonitor_p->initialize (data_p->configuration->WLANMonitorConfiguration))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_IInitialize_T::initialize(), aborting\n")));
      goto error;
    } // end IF

    try {
      iinetwlanmonitor_p->start ();
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Common_ITask_T::start(): \"%m\", aborting\n")));
      goto error;
    } // end catch
  } // end IF
  else
  {
    gtk_button_set_label (GTK_BUTTON (toggleButton_in),
                          GTK_STOCK_MEDIA_PLAY);

    try {
      iinetwlanmonitor_p->stop (false,
                                true);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Common_ITask_T::stop(): \"%m\", aborting\n")));
      goto error;
    } // end catch
  } // end IF
  gtk_widget_set_sensitive (GTK_WIDGET (frame_p),
                            !is_active);

  return;

error:
  gtk_widget_set_sensitive (GTK_WIDGET (frame_p), true);
} // togglebutton_monitor_toggled_cb

void
button_report_clicked_cb (GtkButton* button_in,
                          gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::button_report_clicked_cb"));

  ACE_UNUSED_ARG (button_in);
  ACE_UNUSED_ARG (userData_in);

// *PORTABILITY*: on Windows SIGUSRx are not defined
// --> use SIGBREAK (21) instead...
  int signal = 0;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  signal = SIGUSR1;
#else
  signal = SIGBREAK;
#endif
  if (ACE_OS::raise (signal) == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
                signal));
}

void
combobox_interface_changed_cb (GtkComboBox* comboBox_in,
                               gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::combobox_interface_changed_cb"));

  struct WLANMonitor_GTK_CBData* data_p =
    static_cast<struct WLANMonitor_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->configuration);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  GtkTreeIter iterator_2;
  if (!gtk_combo_box_get_active_iter (comboBox_in,
                                      &iterator_2))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_combo_box_get_active_iter(), returning\n")));
    return;
  } // end IF
  GtkListStore* list_store_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_LISTSTORE_INTERFACE_NAME)));
  ACE_ASSERT (list_store_p);
  GValue value;
#if GTK_CHECK_VERSION (3,0,0)
  value = G_VALUE_INIT;
#else
  g_value_init (&value, G_TYPE_STRING);
#endif
  gtk_tree_model_get_value (GTK_TREE_MODEL (list_store_p),
                            &iterator_2,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                            1, &value);
#else
                            0, &value);
#endif
  ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_STRING);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  cb_data_p->configuration->WLANMonitorConfiguration.interfaceIdentifier =
    Common_Tools::StringToGUID (ACE_TEXT_ALWAYS_CHAR (g_value_get_string (&value)));
#else
  data_p->configuration->WLANMonitorConfiguration.interfaceIdentifier =
    ACE_TEXT_ALWAYS_CHAR (g_value_get_string (&value));
#endif
  g_value_unset (&value);

  Net_WLAN_SSIDs_t ssids;
  GtkTreeIter tree_iterator;
  unsigned int index_i;
  bool select_ssid = false;
  list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_LISTSTORE_SSID_NAME)));
  ACE_ASSERT (list_store_p);
  load_ssids (data_p->configuration->WLANMonitorConfiguration.interfaceIdentifier,
              ssids,
              list_store_p);
  if (!data_p->configuration->WLANMonitorConfiguration.SSID.empty ())
  {
    for (Net_WLAN_SSIDsIterator_t iterator_2 = ssids.begin ();
         iterator_2 != ssids.end ();
         ++iterator_2)
      if (!ACE_OS::strcmp (data_p->configuration->WLANMonitorConfiguration.SSID.c_str (),
                           (*iterator_2).c_str ()))
      {
        for (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store_p),
                                            &tree_iterator);
             gtk_tree_model_iter_next (GTK_TREE_MODEL (list_store_p),
                                       &tree_iterator);
             ++index_i)
        {
#if GTK_CHECK_VERSION (3,0,0)
          value = G_VALUE_INIT;
#else
          g_value_init (&value, G_TYPE_STRING);
#endif
          gtk_tree_model_get_value (GTK_TREE_MODEL (list_store_p),
                                    &tree_iterator,
                                    0, &value);
          ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_STRING);
          if (!ACE_OS::strcmp (ACE_TEXT (g_value_get_string (&value)),
                               data_p->configuration->WLANMonitorConfiguration.SSID.c_str ()))
          {
            g_value_unset (&value);
            break;
          } // end IF
          g_value_unset (&value);
        } // end FOR

        select_ssid = true;

        break;
      } // end IF
  } // end IF

  Net_WLAN_IInetMonitor_t* iinetwlanmonitor_p =
      NET_WLAN_INETMONITOR_SINGLETON::instance ();
  ACE_ASSERT (iinetwlanmonitor_p);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (unlikely (ACE_OS::strcmp (data_p->configuration->WLANMonitorConfiguration.SSID.c_str (),
                                Net_WLAN_Tools::associatedSSID (iinetwlanmonitor_p->get (),
                                                                data_p->configuration->WLANMonitorConfiguration.interfaceIdentifier).c_str ()) &&
                data_p->configuration->WLANMonitorConfiguration.autoAssociate))
#else
  if (unlikely (ACE_OS::strcmp (data_p->configuration->WLANMonitorConfiguration.SSID.c_str (),
                                Net_WLAN_Tools::associatedSSID (data_p->configuration->WLANMonitorConfiguration.interfaceIdentifier,
                                                                ACE_INVALID_HANDLE).c_str ()) &&
                data_p->configuration->WLANMonitorConfiguration.autoAssociate))
#endif
  {
    GtkSpinner* spinner_p =
      GTK_SPINNER (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_SPINNER_NAME)));
    ACE_ASSERT (spinner_p);
    gtk_widget_set_visible (GTK_WIDGET (spinner_p),
                            true);
    gtk_widget_set_sensitive (GTK_WIDGET (spinner_p),
                              true);
    gtk_spinner_start (spinner_p);
  } // end IF

  GtkToggleButton* toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_TOGGLEBUTTON_MONITOR_NAME)));
  ACE_ASSERT (toggle_button_p);
  gtk_widget_set_sensitive (GTK_WIDGET (toggle_button_p),
                            true);

  if (select_ssid)
  {
    GtkComboBox* combo_box_p =
        GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_COMBOBOX_SSID_NAME)));
    ACE_ASSERT (combo_box_p);
    gtk_combo_box_set_active (combo_box_p,
                              index_i);
  } //  end IF
}

void
combobox_ssid_changed_cb (GtkComboBox* comboBox_in,
                          gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::combobox_ssid_changed_cb"));

  struct WLANMonitor_GTK_CBData* data_p =
    static_cast<struct WLANMonitor_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->configuration);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  GtkTreeIter iterator_2;
  if (!gtk_combo_box_get_active_iter (comboBox_in,
                                      &iterator_2))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_combo_box_get_active_iter(), returning\n")));
    return;
  } // end IF
  Net_WLAN_IInetMonitor_t* iinetwlanmonitor_p =
      NET_WLAN_INETMONITOR_SINGLETON::instance ();
  ACE_ASSERT (iinetwlanmonitor_p);
  if (!iinetwlanmonitor_p->isRunning ())
    return; // nothing to do

  GtkListStore* list_store_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_LISTSTORE_SSID_NAME)));
  ACE_ASSERT (list_store_p);
  GValue value;
#if GTK_CHECK_VERSION (3,0,0)
  value = G_VALUE_INIT;
#else
  g_value_init (&value, G_TYPE_STRING);
#endif
  gtk_tree_model_get_value (GTK_TREE_MODEL (list_store_p),
                            &iterator_2,
                            0, &value);
  ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_STRING);
  data_p->configuration->WLANMonitorConfiguration.SSID =
    ACE_TEXT_ALWAYS_CHAR (g_value_get_string (&value));
  g_value_unset (&value);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  struct ether_addr ap_mac_address;
  ACE_OS::memset (&ap_mac_address, 0, sizeof (struct ether_addr));
#endif
  if (ACE_OS::strcmp (iinetwlanmonitor_p->SSID ().c_str (),
                      data_p->configuration->WLANMonitorConfiguration.SSID.c_str ()) &&
      data_p->configuration->WLANMonitorConfiguration.autoAssociate)
  {
    GtkSpinner* spinner_p =
      GTK_SPINNER (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_SPINNER_NAME)));
    ACE_ASSERT (spinner_p);
    gtk_widget_set_visible (GTK_WIDGET (spinner_p),
                            true);
    gtk_widget_set_sensitive (GTK_WIDGET (spinner_p),
                              true);
    gtk_spinner_start (spinner_p);

    if (!iinetwlanmonitor_p->associate (data_p->configuration->WLANMonitorConfiguration.interfaceIdentifier,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
                                        ap_mac_address,
#endif
                                        data_p->configuration->WLANMonitorConfiguration.SSID))
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_IWLANMonitor_T::associate(\"%s\",%s), returning\n"),
                  ACE_TEXT (Net_Common_Tools::interfaceToString (data_p->configuration->WLANMonitorConfiguration.interfaceIdentifier).c_str ()),
                  ACE_TEXT (data_p->configuration->WLANMonitorConfiguration.SSID.c_str ())));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_IWLANMonitor_T::associate(\"%s\",%s,%s), returning\n"),
                  ACE_TEXT (data_p->configuration->WLANMonitorConfiguration.interfaceIdentifier.c_str ()),
                  ACE_TEXT (Net_Common_Tools::LinkLayerAddressToString (reinterpret_cast<unsigned char*> (&ap_mac_address)).c_str ()),
                  ACE_TEXT (data_p->configuration->WLANMonitorConfiguration.SSID.c_str ())));
#endif
      return;
    } // end IF
  } // end IF
}

void
togglebutton_autoassociate_toggled_cb (GtkToggleButton* toggleButton_in,
                                       gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::togglebutton_autoassociate_toggled_cb"));

  struct WLANMonitor_GTK_CBData* data_p =
    static_cast<struct WLANMonitor_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->configuration);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  // update configuration
  data_p->configuration->WLANMonitorConfiguration.autoAssociate =
    gtk_toggle_button_get_active (toggleButton_in);

  Net_WLAN_IInetMonitor_t* iinetwlanmonitor_p =
      NET_WLAN_INETMONITOR_SINGLETON::instance ();
  ACE_ASSERT (iinetwlanmonitor_p);
  if (!iinetwlanmonitor_p->isRunning ())
    return; // nothing to do

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  struct ether_addr ap_mac_address;
  ACE_OS::memset (&ap_mac_address, 0, sizeof (struct ether_addr));
#endif
  if (ACE_OS::strcmp (iinetwlanmonitor_p->SSID ().c_str (),
                      data_p->configuration->WLANMonitorConfiguration.SSID.c_str ()) &&
      data_p->configuration->WLANMonitorConfiguration.autoAssociate)
  {
    GtkSpinner* spinner_p =
      GTK_SPINNER (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_SPINNER_NAME)));
    ACE_ASSERT (spinner_p);
    gtk_widget_set_visible (GTK_WIDGET (spinner_p),
                            true);
    gtk_widget_set_sensitive (GTK_WIDGET (spinner_p),
                              true);
    gtk_spinner_start (spinner_p);

    if (!iinetwlanmonitor_p->associate (data_p->configuration->WLANMonitorConfiguration.interfaceIdentifier,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
                                        ap_mac_address,
#endif
                                        data_p->configuration->WLANMonitorConfiguration.SSID))
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_IWLANMonitor_T::associate(\"%s\",%s), returning\n"),
                  ACE_TEXT (Net_Common_Tools::interfaceToString (data_p->configuration->WLANMonitorConfiguration.interfaceIdentifier).c_str ()),
                  ACE_TEXT (data_p->configuration->WLANMonitorConfiguration.SSID.c_str ())));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_IWLANMonitor_T::associate(\"%s\",%s,%s), returning\n"),
                  ACE_TEXT (data_p->configuration->WLANMonitorConfiguration.interfaceIdentifier.c_str ()),
                  ACE_TEXT (Net_Common_Tools::LinkLayerAddressToString (reinterpret_cast<unsigned char*> (&ap_mac_address)).c_str ()),
                  ACE_TEXT (data_p->configuration->WLANMonitorConfiguration.SSID.c_str ())));
#endif
      return;
    } // end IF
  } // end IF
}

void
button_clear_clicked_cb (GtkButton* button_in,
                         gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::button_clear_clicked_cb"));

  ACE_UNUSED_ARG (button_in);

  WLANMonitor_GTK_CBData* data_p =
    static_cast<WLANMonitor_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  GtkTextView* view_p =
    GTK_TEXT_VIEW (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_TEXTVIEW_LOG_NAME)));
  ACE_ASSERT (view_p);
  GtkTextBuffer* buffer_p =
//    gtk_text_buffer_new (NULL); // text tag table --> create new
    gtk_text_view_get_buffer (view_p);
  ACE_ASSERT (buffer_p);
  gtk_text_buffer_set_text (buffer_p,
                            ACE_TEXT_ALWAYS_CHAR (""), 0);
}

void
button_about_clicked_cb (GtkButton* button_in,
                         gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::button_about_clicked_cb"));

  ACE_UNUSED_ARG (button_in);

  WLANMonitor_GTK_CBData* data_p =
    static_cast<WLANMonitor_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  // retrieve about dialog handle
  GtkDialog* about_dialog =
    GTK_DIALOG (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (WLAN_MONITOR_GTK_ABOUTDIALOG_NAME)));
  if (!about_dialog)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_builder_get_object(\"%s\"): \"%m\", returning\n"),
                ACE_TEXT (WLAN_MONITOR_GTK_ABOUTDIALOG_NAME)));
    return;
  } // end IF

  // run dialog
  gint result = gtk_dialog_run (about_dialog);
  switch (result)
  {
    case GTK_RESPONSE_ACCEPT:
      break;
    default:
      break;
  } // end SWITCH
  gtk_widget_hide (GTK_WIDGET (about_dialog));
} // button_about_clicked_cb

void
button_quit_clicked_cb (GtkButton* button_in,
                        gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::button_quit_clicked_cb"));

  ACE_UNUSED_ARG (button_in);
  ACE_UNUSED_ARG (userData_in);

  int result = -1;

  //WLANMonitor_GTK_CBData* data_p = static_cast<WLANMonitor_GTK_CBData*> (userData_in);
  //// sanity check(s)
  //ACE_ASSERT (data_p);

  //// step1: remove event sources
  //{
  //  ACE_Guard<ACE_Thread_Mutex> aGuard (data_p->lock);

  //  for (Common_UI_GTKEventSourceIdsIterator_t iterator = data_p->eventSourceIds.begin ();
  //       iterator != data_p->eventSourceIds.end ();
  //       iterator++)
  //    if (!g_source_remove (*iterator))
  //      ACE_DEBUG ((LM_ERROR,
  //                  ACE_TEXT ("failed to g_source_remove(%u), continuing\n"),
  //                  *iterator));
  //  data_p->eventSourceIds.clear ();
  //} // end lock scope

  // step2: initiate shutdown sequence
  result = ACE_OS::raise (SIGINT);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
                SIGINT));

  // step3: stop GTK event processing
  // *NOTE*: triggering UI shutdown here is more consistent, compared to doing
  //         it from the signal handler
  WLANMONITOR_UI_GTK_MANAGER_SINGLETON::instance()->stop (false,  // wait ?
                                                          false); // N/A
} // button_quit_clicked_cb
#ifdef __cplusplus
}
#endif /* __cplusplus */
