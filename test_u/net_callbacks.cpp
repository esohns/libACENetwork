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

#include "net_callbacks.h"

#include "glade/glade.h"

#include "common_timer_manager.h"

#include "common_ui_common.h"
#include "common_ui_defines.h"
#include "common_ui_gtk_manager.h"
#include "common_ui_tools.h"

#include "net_common.h"
#include "net_connection_common.h"
#include "net_connection_manager_common.h"
#include "net_defines.h"

#include "net_client_timeouthandler.h"

#include "net_server_common.h"

#include "net_macros.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gboolean
idle_initialize_client_UI_cb (gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::idle_initialize_client_UI_cb"));

  Net_GTK_CBData* data_p = static_cast<Net_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->timeoutHandler);

  //Common_UI_GladeXMLsIterator_t iterator =
  //  data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  //// sanity check(s)
  //ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());
  Common_UI_GTKBuildersIterator_t iterator =
    data_p->GTKState.builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.builders.end ());

  // step1: initialize dialog window(s)
  GtkWidget* dialog_p =
  //  GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
  //                                    ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_DIALOG_MAIN_NAME)));
    GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_DIALOG_MAIN_NAME)));
  ACE_ASSERT (dialog_p);
  //  GtkWidget* image_icon_p = gtk_image_new_from_file (path.c_str ());
  //  ACE_ASSERT (image_icon_p);
  //  gtk_window_set_icon (GTK_WINDOW (dialog_p),
  //                       gtk_image_get_pixbuf (GTK_IMAGE (image_icon_p)));
  //GdkWindow* dialog_window_p = gtk_widget_get_window (dialog_p);
  //gtk_window_set_title (,
  //                      caption.c_str ());

//  GtkWidget* about_dialog_p =
//    //GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
//    //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_DIALOG_ABOUT_NAME)));
//    GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
//                                        ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_DIALOG_ABOUT_NAME)));
//  ACE_ASSERT (about_dialog_p);

  // step2: initialize info view
  GtkSpinButton* spinbutton_p =
    //GTK_SPIN_BUTTON (glade_xml_get_widget ((*iterator).second.second,
    //                                       ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME)));
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME)));
  ACE_ASSERT (spinbutton_p);
  gtk_spin_button_set_range (spinbutton_p,
                             0.0,
                             std::numeric_limits<double>::max ());
  spinbutton_p =
    //GTK_SPIN_BUTTON (glade_xml_get_widget ((*iterator).second.second,
    //                                       ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME)));
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMMESSAGES_NAME)));
  ACE_ASSERT (spinbutton_p);
  gtk_spin_button_set_range (spinbutton_p,
                             0.0,
                             std::numeric_limits<double>::max ());

  // step3: initialize options
  Net_Client_TimeoutHandler::ActionMode_t action_mode =
    data_p->timeoutHandler->mode ();
  std::string radio_button_name;
  switch (action_mode)
  {
    case Net_Client_TimeoutHandler::ACTION_NORMAL:
      radio_button_name = ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_RADIOBUTTON_NORMAL_NAME);
      break;
    case Net_Client_TimeoutHandler::ACTION_ALTERNATING:
      radio_button_name = ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_RADIOBUTTON_ALTERNATING_NAME);
      break;
    case Net_Client_TimeoutHandler::ACTION_STRESS:
      radio_button_name = ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_RADIOBUTTON_STRESS_NAME);
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("unknown/invalid mode (was: %d), aborting\n"),
                  action_mode));
      return FALSE; // G_SOURCE_REMOVE
    }
  } // end SWITCH
  GtkToggleButton* togglebutton_p =
    //GTK_TOGGLE_BUTTON (glade_xml_get_widget ((*iterator).second.second,
    //                                         radio_button_name.c_str ()));
    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               radio_button_name.c_str ()));
  ACE_ASSERT (togglebutton_p);
  gtk_toggle_button_set_active (togglebutton_p, TRUE);

  // step4: initialize text view, setup auto-scrolling
  GtkTextView* view_p =
    //GTK_TEXT_VIEW (glade_xml_get_widget ((*iterator).second.second,
    //                                     ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_TEXTVIEW_NAME)));
    GTK_TEXT_VIEW (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_TEXTVIEW_NAME)));
  ACE_ASSERT (view_p);
//  GtkTextBuffer* buffer_p =
////    gtk_text_buffer_new (NULL); // text tag table --> create new
//      gtk_text_view_get_buffer (view_p);
//  ACE_ASSERT (buffer_p);
////  gtk_text_view_set_buffer (view_p, buffer_p);

  PangoFontDescription* font_description_p =
    pango_font_description_from_string (ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_PANGO_LOG_FONT_DESCRIPTION));
  if (!font_description_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to pango_font_description_from_string(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (NET_UI_GTK_PANGO_LOG_FONT_DESCRIPTION)));
    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  // apply font
  GtkRcStyle* rc_style_p = gtk_rc_style_new ();
  if (!rc_style_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_rc_style_new(): \"%m\", aborting\n")));
    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  rc_style_p->font_desc = font_description_p;
  GdkColor base_colour, text_colour;
  gdk_color_parse (ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_PANGO_LOG_COLOR_BASE),
                   &base_colour);
  rc_style_p->base[GTK_STATE_NORMAL] = base_colour;
  gdk_color_parse (ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_PANGO_LOG_COLOR_TEXT),
                   &text_colour);
  rc_style_p->text[GTK_STATE_NORMAL] = text_colour;
  rc_style_p->color_flags[GTK_STATE_NORMAL] =
    static_cast<GtkRcFlags> (GTK_RC_BASE |
                             GTK_RC_TEXT);
  gtk_widget_modify_style (GTK_WIDGET (view_p),
                           rc_style_p);
  gtk_rc_style_unref (rc_style_p);

  //  GtkTextIter iterator;
  //  gtk_text_buffer_get_end_iter (buffer_p,
  //                                &iterator);
  //  gtk_text_buffer_create_mark (buffer_p,
  //                               ACE_TEXT_ALWAYS_CHAR (NET_UI_SCROLLMARK_NAME),
  //                               &iterator,
  //                               TRUE);
  //  g_object_unref (buffer_p);

  // step5: initialize updates
  // schedule asynchronous updates of the log view
  guint event_source_id = g_timeout_add_seconds (1,
                                                 idle_update_log_display_cb,
                                                 userData_in);
  if (event_source_id > 0)
    data_p->GTKState.eventSourceIds.push_back (event_source_id);
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_timeout_add_seconds(): \"%m\", aborting\n")));
    return FALSE; // G_SOURCE_REMOVE
  } // end ELSE
  // schedule asynchronous updates of the info view
  event_source_id = g_timeout_add (NET_UI_GTKEVENT_RESOLUTION,
                                   idle_update_info_display_cb,
                                   userData_in);
  if (event_source_id > 0)
    data_p->GTKState.eventSourceIds.push_back (event_source_id);
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_timeout_add(): \"%m\", aborting\n")));
    return FALSE; // G_SOURCE_REMOVE
  } // end ELSE

  // step6: disable some functions ?
  GtkButton* button_p =
    //GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
    //                                  ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_CLOSE_NAME)));
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_CLOSE_NAME)));
  ACE_ASSERT (button_p);
  gtk_widget_set_sensitive (GTK_WIDGET (button_p), FALSE);
  button_p =
      //GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
      //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLOSEALL_NAME)));
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLOSEALL_NAME)));
  ACE_ASSERT (button_p);
  gtk_widget_set_sensitive (GTK_WIDGET (button_p), FALSE);
  button_p =
      //GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
      //                                  ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_PING_NAME)));
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_PING_NAME)));
  ACE_ASSERT (button_p);
  gtk_widget_set_sensitive (GTK_WIDGET (button_p), FALSE);

  // step7: (auto-)connect signals/slots
  // *NOTE*: glade_xml_signal_autoconnect does not work reliably
  //glade_xml_signal_autoconnect(userData_out.xml);

  // step6a: connect default signals
  gulong result =
      g_signal_connect (dialog_p,
                        ACE_TEXT_ALWAYS_CHAR ("destroy"),
                        G_CALLBACK (gtk_widget_destroyed),
                        NULL);
  ACE_ASSERT (result);

  // step6b: connect custom signals
  //gtk_builder_connect_signals ((*iterator).second.second,
  //                             userData_in);
  GObject* object_p =
      gtk_builder_get_object ((*iterator).second.second,
                              ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_CONNECT_NAME));
  ACE_ASSERT (object_p);
  result = g_signal_connect (object_p,
                             ACE_TEXT_ALWAYS_CHAR ("clicked"),
                             G_CALLBACK (button_connect_clicked_cb),
                             userData_in);
  ACE_ASSERT (result);
  object_p =
      gtk_builder_get_object ((*iterator).second.second,
                              ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_CLOSE_NAME));
  ACE_ASSERT (object_p);
  result =
      g_signal_connect (object_p,
                        ACE_TEXT_ALWAYS_CHAR ("clicked"),
                        G_CALLBACK (button_close_clicked_cb),
                        userData_in);
  ACE_ASSERT (result);
  object_p =
      gtk_builder_get_object ((*iterator).second.second,
                              ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLOSEALL_NAME));
  ACE_ASSERT (object_p);
  result =
      g_signal_connect (object_p,
                        ACE_TEXT_ALWAYS_CHAR ("clicked"),
                        G_CALLBACK (button_close_all_clicked_cb),
                        userData_in);
  ACE_ASSERT (result);

  object_p =
      gtk_builder_get_object ((*iterator).second.second,
                              ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_TEST_NAME));
  ACE_ASSERT (object_p);
  result =
      g_signal_connect (object_p,
                        ACE_TEXT_ALWAYS_CHAR ("clicked"),
                        G_CALLBACK (togglebutton_test_toggled_cb),
                        userData_in);
  ACE_ASSERT (result);
  //-------------------------------------
  object_p =
      gtk_builder_get_object ((*iterator).second.second,
                              ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_RADIOBUTTON_NORMAL_NAME));
  ACE_ASSERT (object_p);
  result =
      g_signal_connect (object_p,
                        ACE_TEXT_ALWAYS_CHAR ("toggled"),
                        G_CALLBACK (radiobutton_mode_toggled_cb),
                        userData_in);
  ACE_ASSERT (result);
  object_p =
      gtk_builder_get_object ((*iterator).second.second,
                              ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_RADIOBUTTON_ALTERNATING_NAME));
  ACE_ASSERT (object_p);
  result =
      g_signal_connect (object_p,
                        ACE_TEXT_ALWAYS_CHAR ("toggled"),
                        G_CALLBACK (radiobutton_mode_toggled_cb),
                        userData_in);
  ACE_ASSERT (result);
  object_p =
      gtk_builder_get_object ((*iterator).second.second,
                              ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_RADIOBUTTON_STRESS_NAME));
  ACE_ASSERT (object_p);
  result =
      g_signal_connect (object_p,
                        ACE_TEXT_ALWAYS_CHAR ("toggled"),
                        G_CALLBACK (radiobutton_mode_toggled_cb),
                        userData_in);
  ACE_ASSERT (result);
  //-------------------------------------
  object_p =
      gtk_builder_get_object ((*iterator).second.second,
                              ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_PING_NAME));
  ACE_ASSERT (object_p);
  result =
      g_signal_connect (object_p,
                        ACE_TEXT_ALWAYS_CHAR ("clicked"),
                        G_CALLBACK (button_ping_clicked_cb),
                        userData_in);
  ACE_ASSERT (result);

  object_p =
    gtk_builder_get_object ((*iterator).second.second,
                            ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLEAR_NAME));
  ACE_ASSERT (object_p);
  result =
    g_signal_connect (object_p,
                      ACE_TEXT_ALWAYS_CHAR ("clicked"),
                      G_CALLBACK (button_clear_clicked_cb),
                      userData_in);
  ACE_ASSERT (result);
  object_p =
      gtk_builder_get_object ((*iterator).second.second,
                              ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_ABOUT_NAME));
  ACE_ASSERT (object_p);
  result =
      g_signal_connect (object_p,
                        ACE_TEXT_ALWAYS_CHAR ("clicked"),
                        G_CALLBACK (button_about_clicked_cb),
                        userData_in);
  ACE_ASSERT (result);
  object_p =
      gtk_builder_get_object ((*iterator).second.second,
                              ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_QUIT_NAME));
  ACE_ASSERT (object_p);
  result =
      g_signal_connect (object_p,
                        ACE_TEXT_ALWAYS_CHAR ("clicked"),
                        G_CALLBACK (button_quit_clicked_cb),
                        userData_in);
  ACE_ASSERT (result);
  ACE_UNUSED_ARG (result);

  //   // step8: use correct screen
  //   if (parentWidget_in)
  //     gtk_window_set_screen (GTK_WINDOW (dialog_p),
  //                            gtk_widget_get_screen (const_cast<GtkWidget*> (//parentWidget_in)));

  // step9: draw main dialog
  gtk_widget_show_all (dialog_p);

  return FALSE; // G_SOURCE_REMOVE
}

G_MODULE_EXPORT gboolean
idle_initialize_server_UI_cb (gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::idle_initialize_server_UI_cb"));

  Net_GTK_CBData* data_p = static_cast<Net_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  //Common_UI_GladeXMLsIterator_t iterator =
  //  data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  Common_UI_GTKBuildersIterator_t iterator =
    data_p->GTKState.builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  //ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());
  ACE_ASSERT (iterator != data_p->GTKState.builders.end ());

  // step1: initialize dialog window(s)
  GtkWidget* dialog_p =
    //GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
    //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_DIALOG_MAIN_NAME)));
    GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_DIALOG_MAIN_NAME)));
  ACE_ASSERT (dialog_p);
  //  GtkWidget* image_icon = gtk_image_new_from_file (path.c_str());
  //  ACE_ASSERT (image_icon);
  //  gtk_window_set_icon (GTK_WINDOW (dialog_p),
  //                       gtk_image_get_pixbuf (GTK_IMAGE (image_icon)));
  //GdkWindow* dialog_window_p = gtk_widget_get_window (dialog_p);
  //gtk_window_set_title(,
  //                     caption.c_str ());

//  GtkWidget* about_dialog_p =
//    //GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
//    //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_DIALOG_ABOUT_NAME)));
//    GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
//                                        ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_DIALOG_ABOUT_NAME)));
//  ACE_ASSERT (about_dialog_p);

  // step2: initialize info view
  GtkSpinButton* spinbutton_p =
    //GTK_SPIN_BUTTON (glade_xml_get_widget ((*iterator).second.second,
    //                                       ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME)));
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME)));
  ACE_ASSERT (spinbutton_p);
  gtk_spin_button_set_range (spinbutton_p,
                             0.0,
                             std::numeric_limits<double>::max ());

  // step3: initialize text view, setup auto-scrolling
  GtkTextView* view_p =
    //GTK_TEXT_VIEW (glade_xml_get_widget ((*iterator).second.second,
    //                                     ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_TEXTVIEW_NAME)));
    GTK_TEXT_VIEW (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_TEXTVIEW_NAME)));
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
  //                               ACE_TEXT_ALWAYS_CHAR (NET_UI_SCROLLMARK_NAME),
  //                               &iterator,
  //                               TRUE);
//  g_object_unref (buffer_p);

  PangoFontDescription* font_description_p =
    pango_font_description_from_string (ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_PANGO_LOG_FONT_DESCRIPTION));
  if (!font_description_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to pango_font_description_from_string(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (NET_UI_GTK_PANGO_LOG_FONT_DESCRIPTION)));
    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  // apply font
  GtkRcStyle* rc_style_p = gtk_rc_style_new ();
  if (!rc_style_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_rc_style_new(): \"%m\", aborting\n")));
    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  rc_style_p->font_desc = font_description_p;
  GdkColor base_colour, text_colour;
  gdk_color_parse (ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_PANGO_LOG_COLOR_BASE),
                   &base_colour);
  rc_style_p->base[GTK_STATE_NORMAL] = base_colour;
  gdk_color_parse (ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_PANGO_LOG_COLOR_TEXT),
                   &text_colour);
  rc_style_p->text[GTK_STATE_NORMAL] = text_colour;
  rc_style_p->color_flags[GTK_STATE_NORMAL] =
      static_cast<GtkRcFlags>(GTK_RC_BASE |
                              GTK_RC_TEXT);
  gtk_widget_modify_style (GTK_WIDGET (view_p),
                           rc_style_p);
  gtk_rc_style_unref (rc_style_p);

  // step4: initialize updates
  // schedule asynchronous updates of the log view
  guint event_source_id = g_timeout_add_seconds (1,
                                                 idle_update_log_display_cb,
                                                 userData_in);
  if (event_source_id > 0)
    data_p->GTKState.eventSourceIds.push_back (event_source_id);
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_timeout_add_seconds(): \"%m\", aborting\n")));
    return FALSE; // G_SOURCE_REMOVE
  } // end ELSE
  // schedule asynchronous updates of the info view
  event_source_id = g_timeout_add (NET_UI_GTKEVENT_RESOLUTION,
                                   idle_update_info_display_cb,
                                   userData_in);
  if (event_source_id > 0)
    data_p->GTKState.eventSourceIds.push_back (event_source_id);
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_timeout_add(): \"%m\", aborting\n")));
    return FALSE; // G_SOURCE_REMOVE
  } // end ELSE

  // step5: disable some functions ?
  GtkButton* button_p =
      //GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
      //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLOSEALL_NAME)));
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLOSEALL_NAME)));
  ACE_ASSERT (button_p);
  gtk_widget_set_sensitive (GTK_WIDGET (button_p), FALSE);
  button_p =
      //GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
      //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_REPORT_NAME)));
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_REPORT_NAME)));
  ACE_ASSERT (button_p);
  gtk_widget_set_sensitive (GTK_WIDGET (button_p),
                            data_p->allowUserRuntimeStatistic);

  // step6: (auto-)connect signals/slots
  // *NOTE*: glade_xml_signal_connect_data does not work reliably
  //glade_xml_signal_autoconnect(userData_out.xml);
  //gtk_builder_connect_signals ((*iterator).second.second,
  //                             userData_in);

  // step6a: connect default signals
  gulong result =
      g_signal_connect (dialog_p,
                        ACE_TEXT_ALWAYS_CHAR ("destroy"),
                        G_CALLBACK (gtk_widget_destroyed),
                        &dialog_p);
  ACE_ASSERT (result);

  // step6b: connect custom signals
  // *NOTE*: glade_xml_signal_connect_data does not work reliably on Windows
  //glade_xml_signal_connect_data(userData_out.xml,
  //                              ACE_TEXT_ALWAYS_CHAR("togglebutton_listen_toggled_cb"),
  //                              G_CALLBACK(togglebutton_listen_toggled_cb),
  //                              &userData_out);
  GObject* object_p =
    //GTK_TOGGLE_BUTTON (glade_xml_get_widget ((*iterator).second.second,
    //                                         ACE_TEXT_ALWAYS_CHAR (NET_SERVER_UI_GTK_BUTTON_LISTEN_NAME)));
    gtk_builder_get_object ((*iterator).second.second,
                            ACE_TEXT_ALWAYS_CHAR (NET_SERVER_UI_GTK_BUTTON_LISTEN_NAME));
  ACE_ASSERT (object_p);
  result =
      g_signal_connect (object_p,
                        ACE_TEXT_ALWAYS_CHAR ("toggled"),
                        G_CALLBACK (togglebutton_listen_toggled_cb),
                        userData_in);
  ACE_ASSERT (result);
  object_p =
    //GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
    //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLOSEALL_NAME)));
      gtk_builder_get_object ((*iterator).second.second,
                              ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLOSEALL_NAME));
  ACE_ASSERT (object_p);
  result =
      g_signal_connect (object_p,
                        ACE_TEXT_ALWAYS_CHAR ("clicked"),
                        G_CALLBACK (button_close_all_clicked_cb),
                        userData_in);
  ACE_ASSERT (result);

  object_p =
    //GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
    //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_REPORT_NAME)));
    gtk_builder_get_object ((*iterator).second.second,
                            ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_REPORT_NAME));
  ACE_ASSERT (object_p);
  result =
      g_signal_connect (object_p,
                        ACE_TEXT_ALWAYS_CHAR ("clicked"),
                        G_CALLBACK (button_report_clicked_cb),
                        userData_in);
  ACE_ASSERT (result);

  object_p =
    gtk_builder_get_object ((*iterator).second.second,
                            ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLEAR_NAME));
  ACE_ASSERT (object_p);
  result =
    g_signal_connect (object_p,
                      ACE_TEXT_ALWAYS_CHAR ("clicked"),
                      G_CALLBACK (button_clear_clicked_cb),
                      userData_in);
  ACE_ASSERT (result);
  object_p =
    //GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
    //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_ABOUT_NAME)));
      gtk_builder_get_object ((*iterator).second.second,
                              ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_ABOUT_NAME));
  ACE_ASSERT (object_p);
  result = g_signal_connect (object_p,
                             ACE_TEXT_ALWAYS_CHAR ("clicked"),
                             G_CALLBACK (button_about_clicked_cb),
                             userData_in);
  ACE_ASSERT (result);
  object_p =
    //GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
    //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_QUIT_NAME)));
      gtk_builder_get_object ((*iterator).second.second,
                              ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_QUIT_NAME));
  ACE_ASSERT (object_p);
  result =
      g_signal_connect (object_p,
                        ACE_TEXT_ALWAYS_CHAR ("clicked"),
                        G_CALLBACK (button_quit_clicked_cb),
                        userData_in);
  ACE_ASSERT (result);
  ACE_UNUSED_ARG (result);

  //   // step7: use correct screen
  //   if (parentWidget_in)
  //     gtk_window_set_screen (GTK_WINDOW (dialog_p),
  //                            gtk_widget_get_screen (const_cast<GtkWidget*> (//parentWidget_in)));

  // step8: draw main dialog
  gtk_widget_show_all (dialog_p);

  return FALSE; // G_SOURCE_REMOVE
}

G_MODULE_EXPORT gboolean
idle_finalize_UI_cb (gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::idle_finalize_UI_cb"));

  // leave GTK
  gtk_main_quit ();

  return FALSE; // G_SOURCE_REMOVE
}

G_MODULE_EXPORT gboolean
idle_update_log_display_cb (gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::idle_update_log_display_cb"));

  Net_GTK_CBData* data_p = static_cast<Net_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  //Common_UI_GladeXMLsIterator_t iterator =
  //  data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  Common_UI_GTKBuildersIterator_t iterator =
    data_p->GTKState.builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  //ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());
  ACE_ASSERT (iterator != data_p->GTKState.builders.end ());

  GtkTextView* view_p =
      //GTK_TEXT_VIEW (glade_xml_get_widget ((*iterator).second.second,
      //                                     ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_TEXTVIEW_NAME)));
      GTK_TEXT_VIEW (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_TEXTVIEW_NAME)));
  ACE_ASSERT (view_p);
  GtkTextBuffer* buffer_p = gtk_text_view_get_buffer (view_p);
  ACE_ASSERT (buffer_p);

  GtkTextIter text_iterator;
  gtk_text_buffer_get_end_iter (buffer_p,
                                &text_iterator);

  gchar* converted_text = NULL;
  { // synch access
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (data_p->stackLock);

    // sanity check
    if (data_p->logStack.empty ())
      return TRUE; // G_SOURCE_CONTINUE

    // step1: convert text
    for (Common_MessageStackConstIterator_t iterator_2 = data_p->logStack.begin ();
         iterator_2 != data_p->logStack.end ();
         iterator_2++)
    {
      converted_text = Common_UI_Tools::Locale2UTF8 (*iterator_2);
      if (!converted_text)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to convert message text (was: \"%s\"), aborting\n"),
                    ACE_TEXT ((*iterator_2).c_str ())));
        return FALSE; // G_SOURCE_REMOVE
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
//                                ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SCROLLMARK_NAME));
////  gtk_text_buffer_move_mark (buffer_p,
////                             text_mark_p,
////                             &text_iterator);

//  // scroll the mark onscreen
//  gtk_text_view_scroll_mark_onscreen (view_p,
//                                      text_mark_p);
  GtkAdjustment* adjustment_p =
      GTK_ADJUSTMENT (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_ADJUSTMENT_NAME)));
  ACE_ASSERT (adjustment_p);
  gtk_adjustment_set_value (adjustment_p,
                            gtk_adjustment_get_upper (adjustment_p));

  return TRUE; // G_SOURCE_CONTINUE
}

G_MODULE_EXPORT gboolean
idle_update_info_display_cb (gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::idle_update_info_display_cb"));

  Net_GTK_CBData* data_p = static_cast<Net_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  //Common_UI_GladeXMLsIterator_t iterator =
  //  data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  Common_UI_GTKBuildersIterator_t iterator =
    data_p->GTKState.builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  //ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());
  ACE_ASSERT (iterator != data_p->GTKState.builders.end ());

  bool update_buttons = false;
  GtkSpinButton* spinbutton_p = NULL;
  { // synch access
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (data_p->stackLock);

    if (data_p->eventStack.empty ())
      return TRUE; // G_SOURCE_CONTINUE

    for (Net_GTK_EventsIterator_t iterator_2 = data_p->eventStack.begin ();
         iterator_2 != data_p->eventStack.end ();
         iterator_2++)
    {
      switch (*iterator_2)
      {
        case NET_GTKEVENT_CONNECT:
        {
          spinbutton_p =
            //GTK_SPIN_BUTTON (glade_xml_get_widget ((*iterator).second.second,
            //                                       ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME)));
            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME)));
          ACE_ASSERT (spinbutton_p);
          gtk_spin_button_spin (spinbutton_p,
                                GTK_SPIN_STEP_FORWARD,
                                1.0);

          update_buttons = true;

          break;
        }
        case NET_GTKEVENT_DATA:
        {
          if (!data_p->listenerHandle) // <-- client
          {
            spinbutton_p =
              //GTK_SPIN_BUTTON (glade_xml_get_widget ((*iterator).second.second,
              //                                       ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMMESSAGES_NAME)));
              GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                       ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMMESSAGES_NAME)));
            ACE_ASSERT (spinbutton_p);
            gtk_spin_button_spin (spinbutton_p,
                                  GTK_SPIN_STEP_FORWARD,
                                  1.0);
          } // end IF

          break;
        }
        case NET_GTKEVENT_DISCONNECT:
        {
          spinbutton_p =
            //GTK_SPIN_BUTTON (glade_xml_get_widget ((*iterator).second.second,
            //                                       ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME)));
            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME)));
          ACE_ASSERT (spinbutton_p);
          gtk_spin_button_spin (spinbutton_p,
                                GTK_SPIN_STEP_BACKWARD,
                                1.0);

          update_buttons = true;

          break;
        }
        case NET_GTKEVENT_STATISTICS:
        {
          // *TODO*
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
    } // end FOR

    data_p->eventStack.clear ();
  } // end lock scope

  if (update_buttons)
  {
    bool active_connections =
        (gtk_spin_button_get_value_as_int (spinbutton_p) > 0);
    GtkWidget* widget_p = NULL;
    if (!data_p->listenerHandle) // <-- client
    {
      widget_p =
        //GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
        //                                  ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_CLOSE_NAME)));
        GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_CLOSE_NAME)));
      if (!widget_p)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to gtk_builder_get_object(\"%s\"): \"%m\", continuing\n"),
                    ACE_TEXT (NET_CLIENT_UI_GTK_BUTTON_CLOSE_NAME)));
      else
        gtk_widget_set_sensitive (widget_p, active_connections);
    } // end IF
    widget_p =
      //GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
      //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLOSEALL_NAME)));
      GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLOSEALL_NAME)));
    if (!widget_p)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gtk_builder_get_object(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (NET_UI_GTK_BUTTON_CLOSEALL_NAME)));
    else
      gtk_widget_set_sensitive (widget_p, active_connections);
    if (!data_p->listenerHandle) // <-- client
    {
      widget_p =
        //GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
        //                                  ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_PING_NAME)));
        GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_PING_NAME)));
      if (!widget_p)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to gtk_builder_get_object(\"%s\"): \"%m\", continuing\n"),
                    ACE_TEXT (NET_CLIENT_UI_GTK_BUTTON_PING_NAME)));
      else
        gtk_widget_set_sensitive (widget_p, active_connections);
    } // end IF

    if (!active_connections &&
        !data_p->listenerHandle) // <-- client
    {
      spinbutton_p =
        //GTK_SPIN_BUTTON (glade_xml_get_widget ((*iterator).second.second,
        //                                       ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMMESSAGES_NAME)));
        GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMMESSAGES_NAME)));
      ACE_ASSERT (spinbutton_p);
      gtk_spin_button_set_value (spinbutton_p, 0.0);
    } // end IF
  } // end IF

  return TRUE; // G_SOURCE_CONTINUE
}

// -----------------------------------------------------------------------------

G_MODULE_EXPORT gint
button_connect_clicked_cb (GtkWidget* widget_in,
                           gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::button_connect_clicked_cb"));

  int result = -1;

  ACE_UNUSED_ARG (widget_in);
  ACE_UNUSED_ARG (userData_in);
  //Net_GTK_CBData* data_p = static_cast<Net_GTK_CBData*> (userData_in);

  //// sanity check(s)
  //ACE_ASSERT (data_p);

  //Common_UI_GladeXMLsIterator_t iterator =
  //  data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  //// sanity check(s)
  //ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

// *PORTABILITY*: on Windows SIGUSRx are not defined
// --> use SIGBREAK (21) and SIGTERM (15) instead...
  int signal = 0;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  signal = SIGUSR1;
#else
  signal = SIGBREAK;
#endif
  result = ACE_OS::raise (signal);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
                signal));

  return FALSE;
} // button_connect_clicked_cb

G_MODULE_EXPORT gint
button_close_clicked_cb (GtkWidget* widget_in,
                         gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::button_close_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  ACE_UNUSED_ARG (userData_in);
  //Net_GTK_CBData* data_p = static_cast<Net_GTK_CBData*> (userData_in);

  //// sanity check(s)
  //ACE_ASSERT (data_p);

  //Common_UI_GladeXMLsIterator_t iterator =
  //  data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  //// sanity check(s)
  //ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

// *PORTABILITY*: on Windows SIGUSRx are not defined
// --> use SIGBREAK (21) and SIGTERM (15) instead...
  int signal = 0;
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  signal = SIGUSR2;
#else
  signal = SIGTERM;
#endif
  if (ACE_OS::raise (signal) == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
                signal));

  return FALSE;
} // button_close_clicked_cb

G_MODULE_EXPORT gint
button_close_all_clicked_cb (GtkWidget* widget_in,
                             gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::button_close_all_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  ACE_UNUSED_ARG (userData_in);
  //Net_GTK_CBData* data_p = static_cast<Net_GTK_CBData*> (userData_in);

  //// sanity check(s)
  //ACE_ASSERT (data_p);

  //Common_UI_GladeXMLsIterator_t iterator =
  //  data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  //// sanity check(s)
  //ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

  NET_CONNECTIONMANAGER_SINGLETON::instance ()->abort ();

  return FALSE;
} // button_close_all_clicked_cb

G_MODULE_EXPORT gint
button_ping_clicked_cb (GtkWidget* widget_in,
                        gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::button_ping_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  ACE_UNUSED_ARG (userData_in);

  // sanity check
  unsigned int num_connections =
    NET_CONNECTIONMANAGER_SINGLETON::instance ()->numConnections ();
  if (num_connections == 0)
    return FALSE;

  // grab a (random) connection handler
  int index = 0;
  // *PORTABILITY*: outside glibc, this is not very portable...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  index = ((::random () % num_connections) + 1);
#else
  // *NOTE*: use ACE_OS::rand_r() for improved thread safety !
  //results_out.push_back((ACE_OS::rand() % range_in) + 1);
  unsigned int usecs = static_cast<unsigned int> (COMMON_TIME_NOW.usec ());
  index = ((ACE_OS::rand_r (&usecs) % num_connections) + 1);
#endif

  Net_InetConnectionManager_t::CONNECTION_T* connection_base_p =
      NET_CONNECTIONMANAGER_SINGLETON::instance ()->operator[] (index - 1);
  if (!connection_base_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve connection handler (%d), aborting\n"),
                index - 1));
    return FALSE;
  } // end IF
  Net_ITransportLayer_t* connection_p =
    dynamic_cast<Net_ITransportLayer_t*> (connection_base_p);
  if (!connection_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dynamic_cast<const Net_ITransportLayer_t*> (%@), aborting\n"),
                connection_base_p));
    return FALSE;
  } // end IF

  try
  {
    connection_p->ping ();
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Net_ITransportLayer_t::ping(), aborting\n")));

    // clean up
    connection_base_p->decrease ();
    connection_base_p->close ();

    return FALSE;
  }
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("session %u: sent ping...\n"),
  //            connection_base_p->id ()));

  // clean up
  connection_base_p->decrease ();

  return FALSE;
} // button_ping_clicked_cb

G_MODULE_EXPORT gint
togglebutton_test_toggled_cb (GtkWidget* widget_in,
                              gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::togglebutton_test_toggled_cb"));

  int result = -1;

  ACE_UNUSED_ARG (widget_in);
  Net_GTK_CBData* data_p = static_cast<Net_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->signalHandlerConfiguration);
  ACE_ASSERT (data_p->timeoutHandler);

  //Common_UI_GladeXMLsIterator_t iterator =
  //  data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  Common_UI_GTKBuildersIterator_t iterator =
    data_p->GTKState.builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  //ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());
  ACE_ASSERT (iterator != data_p->GTKState.builders.end ());

  // schedule action interval timer ?
  if (data_p->signalHandlerConfiguration->actionTimerId == -1)
  {
    ACE_Event_Handler* handler_p = data_p->timeoutHandler;
    ACE_Time_Value interval = ACE_Time_Value::max_time;
    switch (data_p->timeoutHandler->mode ())
    {
      case Net_Client_TimeoutHandler::ActionMode_t::ACTION_ALTERNATING:
      case Net_Client_TimeoutHandler::ActionMode_t::ACTION_NORMAL:
      {
        interval.set ((NET_CLIENT_DEF_SERVER_TEST_INTERVAL / 1000),
                      ((NET_CLIENT_DEF_SERVER_TEST_INTERVAL % 1000) * 1000));
        break;
      }
      case Net_Client_TimeoutHandler::ActionMode_t::ACTION_STRESS:
      {
        interval.set ((NET_CLIENT_DEF_SERVER_STRESS_INTERVAL / 1000),
                      ((NET_CLIENT_DEF_SERVER_STRESS_INTERVAL % 1000) * 1000));
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown action mode (was: %d), aborting\n"),
                    data_p->timeoutHandler->mode ()));
        return FALSE;
      }
    } // end SWITCH
    data_p->signalHandlerConfiguration->actionTimerId =
      COMMON_TIMERMANAGER_SINGLETON::instance ()->schedule_timer (handler_p,                  // event handler
                                                                  NULL,                       // ACT
                                                                  COMMON_TIME_NOW + interval, // first wakeup time
                                                                  interval);                  // interval
    if (data_p->signalHandlerConfiguration->actionTimerId == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to schedule action timer: \"%m\", aborting\n")));
      return FALSE;
    } // end IF
  } // end IF
  else
  {
    const void* act_p = NULL;
    result =
      COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel_timer (data_p->signalHandlerConfiguration->actionTimerId,
                                                                &act_p);
    if (result <= 0)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to cancel action timer (ID: %d): \"%m\", continuing\n"),
                  data_p->signalHandlerConfiguration->actionTimerId));

    // clean up
    data_p->signalHandlerConfiguration->actionTimerId = -1;
  } // end ELSE

  // toggle button image/label
  bool is_active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget_in));
  GtkImage* image_p =
    //GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
    //                                  ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_GTK_IMAGE_START_NAME)));
    GTK_IMAGE (gtk_builder_get_object ((*iterator).second.second,
                                       (is_active ? ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_IMAGE_STOP_NAME)
                                                  : ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_IMAGE_START_NAME))));
  ACE_ASSERT (image_p);
  gtk_button_set_image (GTK_BUTTON (widget_in), GTK_WIDGET (image_p));
  gtk_button_set_label (GTK_BUTTON (widget_in),
                        (is_active ? ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_TEST_LABEL_STOP)
                                   : ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_TEST_LABEL_START)));

  return FALSE;
}

G_MODULE_EXPORT gint
radiobutton_mode_toggled_cb (GtkWidget* widget_in,
                             gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::radiobutton_mode_toggled_cb"));

  int result = -1;

  Net_GTK_CBData* data_p = static_cast<Net_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->timeoutHandler);

//  //Common_UI_GladeXMLsIterator_t iterator =
//  //  data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
//  Common_UI_GTKBuildersIterator_t iterator =
//    data_p->GTKState.builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
//  // sanity check(s)
//  //ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());
//  ACE_ASSERT (iterator != data_p->GTKState.builders.end ());

  // step0: activated ?
  GtkToggleButton* toggle_button_p = GTK_TOGGLE_BUTTON (widget_in);
  ACE_ASSERT (toggle_button_p);
  if (!gtk_toggle_button_get_active (toggle_button_p))
    return FALSE;

  Net_Client_TimeoutHandler::ActionMode_t mode =
    Net_Client_TimeoutHandler::ACTION_INVALID;
  GtkButton* button_p = GTK_BUTTON (widget_in);
  ACE_ASSERT (button_p);
  const gchar* label_text_p = gtk_button_get_label (button_p);
  result = ACE_OS::strcmp (label_text_p,
                           ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_RADIOBUTTON_NORMAL_LABEL));
  if (result == 0)
    mode = Net_Client_TimeoutHandler::ACTION_NORMAL;
  else
  {
    result = ACE_OS::strcmp (label_text_p,
                             ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_RADIOBUTTON_ALTERNATING_LABEL));
    if (result == 0)
      mode = Net_Client_TimeoutHandler::ACTION_ALTERNATING;
    else
    {
      result = ACE_OS::strcmp (label_text_p,
                               ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_RADIOBUTTON_STRESS_LABEL));
      if (result == 0)
        mode = Net_Client_TimeoutHandler::ACTION_STRESS;
      else
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown mode (was: \"%s\"), aborting\n"),
                    ACE_TEXT (label_text_p)));
        return TRUE; // propagate
      } // end ELSE
    } // end ELSE
  } // end ELSE

  try
  {
    data_p->timeoutHandler->mode (mode);
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Net_Client_TimeoutHandler::mode(), aborting\n")));
    return TRUE; // propagate
  }

  return FALSE;
}

// -----------------------------------------------------------------------------

G_MODULE_EXPORT gint
togglebutton_listen_toggled_cb (GtkWidget* widget_in,
                                gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::togglebutton_listen_toggled_cb"));

  Net_GTK_CBData* data_p = static_cast<Net_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->listenerHandle);

  ////Common_UI_GladeXMLsIterator_t iterator =
  ////  data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  //Common_UI_GTKBuildersIterator_t iterator =
  //  data_p->GTKState.builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  //// sanity check(s)
  ////ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());
  //ACE_ASSERT (iterator != data_p->GTKState.builders.end ());

  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget_in)))
  {
    try
    {
      data_p->listenerHandle->start ();
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in RPG_Net_Server_IListener::start(): \"%m\", continuing\n")));
    } // end catch
  } // end IF
  else
  {
    try
    {
      data_p->listenerHandle->stop ();
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in RPG_Net_Server_IListener::stop(): \"%m\", continuing\n")));
    } // end catch
  } // end IF

  return FALSE;
} // togglebutton_listen_toggled_cb

G_MODULE_EXPORT gint
button_report_clicked_cb (GtkWidget* widget_in,
                          gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::button_report_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
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

  return FALSE;
}

// -----------------------------------------------------------------------------

G_MODULE_EXPORT gint
button_clear_clicked_cb (GtkWidget* widget_in,
                         gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::button_clear_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  Net_GTK_CBData* data_p = static_cast<Net_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  //Common_UI_GladeXMLsIterator_t iterator =
  //  data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  Common_UI_GTKBuildersIterator_t iterator =
    data_p->GTKState.builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  //ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());
  ACE_ASSERT (iterator != data_p->GTKState.builders.end ());

  GtkTextView* view_p =
    //GTK_TEXT_VIEW (glade_xml_get_widget ((*iterator).second.second,
    //                                     ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_TEXTVIEW_NAME)));
    GTK_TEXT_VIEW (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_TEXTVIEW_NAME)));
  ACE_ASSERT (view_p);
  GtkTextBuffer* buffer_p =
//    gtk_text_buffer_new (NULL); // text tag table --> create new
    gtk_text_view_get_buffer (view_p);
  ACE_ASSERT (buffer_p);
  gtk_text_buffer_set_text (buffer_p,
                            ACE_TEXT_ALWAYS_CHAR (""), 0);

  return FALSE;
}

G_MODULE_EXPORT gint
button_about_clicked_cb (GtkWidget* widget_in,
                         gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::button_about_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  Net_GTK_CBData* data_p = static_cast<Net_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  //Common_UI_GladeXMLsIterator_t iterator =
  //  data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  Common_UI_GTKBuildersIterator_t iterator =
    data_p->GTKState.builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  //ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());
  ACE_ASSERT (iterator != data_p->GTKState.builders.end ());

  // retrieve about dialog handle
  GtkDialog* about_dialog =
    //GTK_DIALOG (glade_xml_get_widget ((*iterator).second.second,
    //                                  ACE_TEXT_ALWAYS_CHAR(NET_UI_GTK_DIALOG_ABOUT_NAME)));
    GTK_DIALOG (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_DIALOG_ABOUT_NAME)));
  if (!about_dialog)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (NET_UI_GTK_DIALOG_ABOUT_NAME)));
    return TRUE; // propagate
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

  return FALSE;
} // button_about_clicked_cb

G_MODULE_EXPORT gint
button_quit_clicked_cb (GtkWidget* widget_in,
                        gpointer userData_in)
{
  NETWORK_TRACE (ACE_TEXT ("::button_quit_clicked_cb"));

  int result = -1;

  ACE_UNUSED_ARG (widget_in);
  ACE_UNUSED_ARG (userData_in);
  //Net_GTK_CBData* data_p = static_cast<Net_GTK_CBData*> (userData_in);
  //// sanity check(s)
  //ACE_ASSERT (data_p);

  //// step1: remove event sources
  //{
  //  ACE_Guard<ACE_Thread_Mutex> aGuard (data_p->GTKState.lock);

  //  for (Common_UI_GTKEventSourceIdsIterator_t iterator = data_p->GTKState.eventSourceIds.begin ();
  //       iterator != data_p->GTKState.eventSourceIds.end ();
  //       iterator++)
  //    if (!g_source_remove (*iterator))
  //      ACE_DEBUG ((LM_ERROR,
  //                  ACE_TEXT ("failed to g_source_remove(%u), continuing\n"),
  //                  *iterator));
  //  data_p->GTKState.eventSourceIds.clear ();
  //} // end lock scope

  // step2: initiate shutdown sequence
  result = ACE_OS::raise (SIGINT);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
                SIGINT));

  // step3: stop GTK event processing
  COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->close (1);

  return FALSE;
} // button_quit_clicked_cb

#ifdef __cplusplus
}
#endif /* __cplusplus */
