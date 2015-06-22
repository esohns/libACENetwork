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

#include "IRC_client_gui_messagehandler.h"

#include "common_file_tools.h"

#include "common_ui_tools.h"

#include "net_macros.h"

#include "IRC_client_gui_callbacks.h"
#include "IRC_client_gui_connection.h"
#include "IRC_client_gui_defines.h"
#include "IRC_client_iIRCControl.h"
#include "IRC_client_tools.h"

IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler (Common_UI_GTKState* GTKState_in,
                                                              GtkTextView* view_in)
 : CBData_ ()
 , eventSourceIDs_ ()
 , isFirstMemberListMsg_ (true)
 , messageQueue_ ()
 , messageQueueLock_ ()
 , parent_ (NULL)
 , view_ (view_in)
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler"));

  // sanity check(s)
  ACE_ASSERT (GTKState_in);
  ACE_ASSERT (view_in);

  // initialize cb data
  CBData_.id.clear ();
  CBData_.GTKState = GTKState_in;
  CBData_.controller = NULL;

  // setup auto-scrolling
  GtkTextIter iterator;
  gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (view_),
                                &iterator);
  gtk_text_buffer_create_mark (gtk_text_view_get_buffer (view_),
                               ACE_TEXT_ALWAYS_CHAR ("scroll"),
                               &iterator,
                               TRUE);
}

IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler (Common_UI_GTKState* GTKState_in,
                                                              IRC_Client_GUI_Connection* connection_in,
                                                              IRC_Client_IIRCControl* controller_in,
                                                              const std::string& id_in,
                                                              const std::string& UIFileDirectory_in,
                                                              GtkNotebook* parent_in)
 : CBData_ ()
 , eventSourceIDs_ ()
 , isFirstMemberListMsg_ (true)
 , messageQueue_ ()
 , messageQueueLock_ ()
 , parent_ (parent_in)
 , view_ (NULL)
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler"));

  // sanity check(s)
  ACE_ASSERT (GTKState_in);
  ACE_ASSERT (connection_in);
  ACE_ASSERT (controller_in);
  ACE_ASSERT (!id_in.empty ());
  if (!Common_File_Tools::isDirectory (UIFileDirectory_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument (was: \"%s\"): not a directory, returning\n"),
                ACE_TEXT (UIFileDirectory_in.c_str ())));
    return;
  } // end IF
  ACE_ASSERT (parent_in);

  // initialize cb data
  CBData_.GTKState = GTKState_in;
  CBData_.connection = connection_in;
  CBData_.id = id_in;
  CBData_.controller = controller_in;
  CBData_.channelModes = 0;

  // create new GtkBuilder
  GtkBuilder* builder_p = gtk_builder_new ();
  if (!builder_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", returning\n")));
    return;
  } // end IF
  std::string ui_definition_filename = UIFileDirectory_in;
  ui_definition_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  ui_definition_filename += ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_UI_CHANNEL_FILE);
  if (!Common_File_Tools::isReadable (ui_definition_filename))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid UI file (was: \"%s\"): not readable, returning\n"),
                ACE_TEXT (ui_definition_filename.c_str ())));

    // clean up
    g_object_unref (G_OBJECT (builder_p));

    return;
  } // end IF

  // load widget tree
  GError* error_p = NULL;
  gtk_builder_add_from_file (builder_p,
                             ui_definition_filename.c_str (),
                             &error_p);
  if (error_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_builder_add_from_file(\"%s\"): \"%s\", returning\n"),
                ACE_TEXT (ui_definition_filename.c_str ()),
                ACE_TEXT (error_p->message)));

    // clean up
    g_error_free (error_p);
    g_object_unref (G_OBJECT (builder_p));

    return;
  } // end IF

  // setup auto-scrolling in textview
  view_ =
      GTK_TEXT_VIEW (gtk_builder_get_object (builder_p,
                                             ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TEXTVIEW_CHANNEL)));
  ACE_ASSERT (view_);
  GtkTextIter text_iter;
  gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (view_),
                                &text_iter);
  gtk_text_buffer_create_mark (gtk_text_view_get_buffer (view_),
                               ACE_TEXT_ALWAYS_CHAR ("scroll"),
                               &text_iter,
                               TRUE);

  // enable multi-selection in treeview
  GtkTreeView* tree_view_p =
    GTK_TREE_VIEW (gtk_builder_get_object (builder_p,
                                           ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TREEVIEW_CHANNEL)));
  ACE_ASSERT (tree_view_p);
  GtkTreeSelection* tree_selection_p =
    gtk_tree_view_get_selection (tree_view_p);
  ACE_ASSERT (tree_selection_p);
  gtk_tree_selection_set_mode (tree_selection_p,
                               GTK_SELECTION_MULTIPLE);

  // add the invite_channel_members_menu to the "Invite" menu item
  GtkMenu* menu_p =
    GTK_MENU (gtk_builder_get_object (builder_p,
                                      ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_MENU_CHANNEL_INVITE)));
  ACE_ASSERT (menu_p);
  GtkMenuItem* menu_item_p =
    GTK_MENU_ITEM (gtk_builder_get_object (builder_p,
                                           ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_MENUITEM_INVITE)));
  ACE_ASSERT (menu_item_p);
  gtk_menu_item_set_submenu (menu_item_p, GTK_WIDGET (menu_p));

  // connect signal(s)
  GtkButton* button_p =
    GTK_BUTTON (gtk_builder_get_object (builder_p,
                                        ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_BUTTON_PART)));
  ACE_ASSERT (button_p);
  gulong result = g_signal_connect (button_p,
                                    ACE_TEXT_ALWAYS_CHAR ("clicked"),
                                    G_CALLBACK (part_clicked_cb),
                                    &CBData_);
  ACE_ASSERT (result);

  // togglebuttons
  GtkToggleButton* toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TOGGLEBUTTON_CHANNELMODE_ANONYMOUS)));
  ACE_ASSERT (toggle_button_p);
  result = g_signal_connect (toggle_button_p,
                             ACE_TEXT_ALWAYS_CHAR ("toggled"),
                             G_CALLBACK (channel_mode_toggled_cb),
                             &CBData_);
  ACE_ASSERT (result);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TOGGLEBUTTON_CHANNELMODE_BAN)));
  ACE_ASSERT (toggle_button_p);
  result = g_signal_connect (toggle_button_p,
                             ACE_TEXT_ALWAYS_CHAR ("toggled"),
                             G_CALLBACK (channel_mode_toggled_cb),
                             &CBData_);
  ACE_ASSERT (result);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TOGGLEBUTTON_CHANNELMODE_INVITEONLY)));
  ACE_ASSERT (toggle_button_p);
  result = g_signal_connect (toggle_button_p,
                             ACE_TEXT_ALWAYS_CHAR ("toggled"),
                             G_CALLBACK (channel_mode_toggled_cb),
                             &CBData_);
  ACE_ASSERT (result);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TOGGLEBUTTON_CHANNELMODE_KEY)));
  ACE_ASSERT (toggle_button_p);
  result = g_signal_connect (toggle_button_p,
                             ACE_TEXT_ALWAYS_CHAR ("toggled"),
                             G_CALLBACK (channel_mode_toggled_cb),
                             &CBData_);
  ACE_ASSERT (result);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                              ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TOGGLEBUTTON_CHANNELMODE_USERLIMIT)));
  ACE_ASSERT (toggle_button_p);
  result = g_signal_connect (toggle_button_p,
                             ACE_TEXT_ALWAYS_CHAR ("toggled"),
                             G_CALLBACK (channel_mode_toggled_cb),
                             &CBData_);
  ACE_ASSERT (result);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TOGGLEBUTTON_CHANNELMODE_MODERATED)));
  ACE_ASSERT (toggle_button_p);
  result = g_signal_connect (toggle_button_p,
                             ACE_TEXT_ALWAYS_CHAR ("toggled"),
                             G_CALLBACK (channel_mode_toggled_cb),
                             &CBData_);
  ACE_ASSERT (result);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TOGGLEBUTTON_CHANNELMODE_BLOCKFOREIGN)));
  ACE_ASSERT (toggle_button_p);
  result = g_signal_connect (toggle_button_p,
                             ACE_TEXT_ALWAYS_CHAR ("toggled"),
                             G_CALLBACK (channel_mode_toggled_cb),
                             &CBData_);
  ACE_ASSERT (result);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TOGGLEBUTTON_CHANNELMODE_OPERATOR)));
  ACE_ASSERT (toggle_button_p);
  result = g_signal_connect (toggle_button_p,
                             ACE_TEXT_ALWAYS_CHAR ("toggled"),
                             G_CALLBACK (channel_mode_toggled_cb),
                             &CBData_);
  ACE_ASSERT (result);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TOGGLEBUTTON_CHANNELMODE_PRIVATE)));
  ACE_ASSERT (toggle_button_p);
  result = g_signal_connect (toggle_button_p,
                             ACE_TEXT_ALWAYS_CHAR ("toggled"),
                             G_CALLBACK (channel_mode_toggled_cb),
                             &CBData_);
  ACE_ASSERT (result);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TOGGLEBUTTON_CHANNELMODE_QUIET)));
  ACE_ASSERT (toggle_button_p);
  result = g_signal_connect (toggle_button_p,
                             ACE_TEXT_ALWAYS_CHAR ("toggled"),
                             G_CALLBACK (channel_mode_toggled_cb),
                             &CBData_);
  ACE_ASSERT (result);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TOGGLEBUTTON_CHANNELMODE_REOP)));
  ACE_ASSERT (toggle_button_p);
  result = g_signal_connect (toggle_button_p,
                             ACE_TEXT_ALWAYS_CHAR ("toggled"),
                             G_CALLBACK (channel_mode_toggled_cb),
                             &CBData_);
  ACE_ASSERT (result);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TOGGLEBUTTON_CHANNELMODE_SECRET)));
  ACE_ASSERT (toggle_button_p);
  result = g_signal_connect (toggle_button_p,
                             ACE_TEXT_ALWAYS_CHAR ("toggled"),
                             G_CALLBACK (channel_mode_toggled_cb),
                             &CBData_);
  ACE_ASSERT (result);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TOGGLEBUTTON_CHANNELMODE_RESTRICTOPIC)));
  ACE_ASSERT (toggle_button_p);
  result = g_signal_connect (toggle_button_p,
                             ACE_TEXT_ALWAYS_CHAR ("toggled"),
                             G_CALLBACK (channel_mode_toggled_cb),
                             &CBData_);
  ACE_ASSERT (result);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TOGGLEBUTTON_CHANNELMODE_VOICE)));
  ACE_ASSERT (toggle_button_p);
  result = g_signal_connect (toggle_button_p,
                             ACE_TEXT_ALWAYS_CHAR ("toggled"),
                             G_CALLBACK (channel_mode_toggled_cb),
                             &CBData_);
  ACE_ASSERT (result);

  // topic label
  GtkEventBox* event_box_p =
    GTK_EVENT_BOX (gtk_builder_get_object (builder_p,
                                           ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_EVENTBOX_TOPIC)));
  ACE_ASSERT (event_box_p);
  result = g_signal_connect (event_box_p,
                             ACE_TEXT_ALWAYS_CHAR ("button-press-event"),
                             G_CALLBACK (topic_clicked_cb),
                             &CBData_);
  ACE_ASSERT (result);

  // context menu in treeview
  result = g_signal_connect (tree_view_p,
                             ACE_TEXT_ALWAYS_CHAR ("button-press-event"),
                             G_CALLBACK (members_clicked_cb),
                             &CBData_);
  ACE_ASSERT (result);
  // actions in treeview
  GtkAction* action_p =
    GTK_ACTION (gtk_builder_get_object (builder_p,
                                        ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_ACTION_MESSAGE)));
  ACE_ASSERT (action_p);
  result = g_signal_connect (action_p,
                             ACE_TEXT_ALWAYS_CHAR ("activate"),
                             G_CALLBACK (action_msg_cb),
                             &CBData_);
  ACE_ASSERT (result);
  action_p =
    GTK_ACTION (gtk_builder_get_object (builder_p,
                                        ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_ACTION_INVITE)));
  ACE_ASSERT (action_p);
  result = g_signal_connect (action_p,
                             ACE_TEXT_ALWAYS_CHAR ("activate"),
                             G_CALLBACK (action_invite_cb),
                             &CBData_);
  ACE_ASSERT (result);
  action_p =
    GTK_ACTION (gtk_builder_get_object (builder_p,
                                        ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_ACTION_INFO)));
  ACE_ASSERT (action_p);
  result = g_signal_connect (action_p,
                             ACE_TEXT_ALWAYS_CHAR ("activate"),
                             G_CALLBACK (action_info_cb),
                             &CBData_);
  ACE_ASSERT (result);
  action_p =
    GTK_ACTION (gtk_builder_get_object (builder_p,
                                        ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_ACTION_KICK)));
  ACE_ASSERT (action_p);
  result = g_signal_connect (action_p,
                             ACE_TEXT_ALWAYS_CHAR ("activate"),
                             G_CALLBACK (action_kick_cb),
                             &CBData_);
  ACE_ASSERT (result);
  action_p =
    GTK_ACTION (gtk_builder_get_object (builder_p,
                                        ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_ACTION_BAN)));
  ACE_ASSERT (action_p);
  result = g_signal_connect (action_p,
                             ACE_TEXT_ALWAYS_CHAR ("activate"),
                             G_CALLBACK (action_ban_cb),
                             &CBData_);
  ACE_ASSERT (result);

  // add new channel page to notebook (== server log)
  // retrieve (dummy) parent window
  GtkWindow* window_p =
    GTK_WINDOW (gtk_builder_get_object (builder_p,
                                        ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_WINDOW_TAB_CHANNEL)));
  ACE_ASSERT (window_p);
  // retrieve channel tab label
  GtkHBox* hbox_p =
    GTK_HBOX (gtk_builder_get_object (builder_p,
                                      ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_HBOX_CHANNEL_TAB)));
  ACE_ASSERT (hbox_p);
  g_object_ref (hbox_p);
  gtk_container_remove (GTK_CONTAINER (window_p),
                        GTK_WIDGET (hbox_p));
  // set tab label
  GtkLabel* label_p =
    GTK_LABEL (gtk_builder_get_object (builder_p,
                                       ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_LABEL_CHANNEL_TAB)));
  ACE_ASSERT (label_p);
  std::string page_tab_label_string;
  if (!IRC_Client_Tools::isValidIRCChannelName (CBData_.id))
  {
    // --> private conversation window, modify label accordingly
    page_tab_label_string = ACE_TEXT_ALWAYS_CHAR ("<b>");
    page_tab_label_string += CBData_.id;
    page_tab_label_string += ACE_TEXT_ALWAYS_CHAR ("</b>");

    // hide channel mode tab frame
    GtkFrame* frame_p =
      GTK_FRAME (gtk_builder_get_object (builder_p,
                                         ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_FRAME_CHANNELMODE)));
    ACE_ASSERT (frame_p);
    gtk_widget_hide (GTK_WIDGET (frame_p));
    // hide channel tab treeview
    tree_view_p =
      GTK_TREE_VIEW (gtk_builder_get_object (builder_p,
                                             ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TREEVIEW_CHANNEL)));
    ACE_ASSERT (tree_view_p);
    gtk_widget_hide (GTK_WIDGET (tree_view_p));

    // erase "topic" label
    GtkLabel* label_2 =
      GTK_LABEL (gtk_builder_get_object (builder_p,
                                         ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_LABEL_TOPIC)));
    ACE_ASSERT (label_2);
    gtk_label_set_text (label_2, NULL);
  } // end IF
  else
    page_tab_label_string = CBData_.id;
  gtk_label_set_text (label_p,
                      page_tab_label_string.c_str ());

  // retrieve (dummy) parent window
  window_p =
    GTK_WINDOW (gtk_builder_get_object (builder_p,
                                        ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_WINDOW_CHANNEL)));
  ACE_ASSERT (window_p);
  // retrieve channel tab
  GtkVBox* vbox_p =
    GTK_VBOX (gtk_builder_get_object (builder_p,
                                      ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_VBOX_CHANNEL)));
  ACE_ASSERT (vbox_p);
  g_object_ref (vbox_p);
  gtk_container_remove (GTK_CONTAINER (window_p),
                        GTK_WIDGET (vbox_p));
  gint page_num = gtk_notebook_append_page (parent_,
                                            GTK_WIDGET (vbox_p),
                                            GTK_WIDGET (hbox_p));
  if (page_num == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_notebook_append_page(%@), returning\n"),
                parent_));

    // clean up
    g_object_unref (hbox_p);
    g_object_unref (vbox_p);
    g_object_unref (G_OBJECT (builder_p));

    return;
  } // end IF
  g_object_unref (hbox_p);

  // allow reordering
  gtk_notebook_set_tab_reorderable (parent_,
                                    GTK_WIDGET (vbox_p),
                                    TRUE);
  g_object_unref (vbox_p);

  // activate new page (iff it's a channel tab !)
  if (IRC_Client_Tools::isValidIRCChannelName (CBData_.id))
    gtk_notebook_set_current_page (parent_,
                                   page_num);

  CBData_.builderLabel = connection_in->getLabel ();
  CBData_.builderLabel += ACE_TEXT_ALWAYS_CHAR ("::");
  CBData_.builderLabel += page_tab_label_string;
  // synch access
  {
    ACE_Guard<ACE_SYNCH_MUTEX> aGuard (CBData_.GTKState->lock);

    CBData_.GTKState->builders[CBData_.builderLabel] =
        std::make_pair (ui_definition_filename, builder_p);
  } // end lock scope
}

IRC_Client_GUI_MessageHandler::~IRC_Client_GUI_MessageHandler ()
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::~IRC_Client_GUI_MessageHandler"));

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  // remove (queued) gtk events
  bool result = false;
  unsigned int removed_events = 0;
  do
  {
    result = g_idle_remove_by_data (&CBData_);
    if (result)
      removed_events++;
  } while (result);
  if (removed_events)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s: removed %u queued events...\n"),
                ACE_TEXT (CBData_.id.c_str ()),
                removed_events));
  {
    ACE_Guard<ACE_SYNCH_MUTEX> aGuard (CBData_.GTKState->lock);

    Common_UI_GTKEventSourceIdsIterator_t iterator;
    for (Common_UI_GTKEventSourceIdsIterator_t iterator_2 = eventSourceIDs_.begin ();
         iterator_2 != eventSourceIDs_.end ();
         ++iterator_2)
    {
      iterator = CBData_.GTKState->eventSourceIds.begin ();
      do
      {
        if (iterator == CBData_.GTKState->eventSourceIds.end ())
          break; // not found --> done

        if (*iterator_2 == *iterator)
        {
          CBData_.GTKState->eventSourceIds.erase (iterator);
          break; // found --> removed
        } // end IF

        ++iterator;
      } while (true);
    } // end FOR
  } // end lock scope

  // *NOTE*: the server log handler MUST NOT do this...
  if (parent_)
  {
    // remove server page from parent notebook
    Common_UI_GTKBuildersIterator_t iterator =
      CBData_.GTKState->builders.find (CBData_.builderLabel);
    // sanity check(s)
    if (iterator == CBData_.GTKState->builders.end ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("handler (was: \"%s\") builder not found, returning\n"),
                  ACE_TEXT (CBData_.builderLabel.c_str ())));
      return;
    } // end IF

    GtkVBox* vbox_p =
      GTK_VBOX (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_VBOX_CHANNEL)));
    ACE_ASSERT (vbox_p);
    guint page_num = gtk_notebook_page_num (parent_,
                                            GTK_WIDGET (vbox_p));

    // flip away from "this" page ?
    if (gtk_notebook_get_current_page (parent_) == static_cast<gint> (page_num))
      gtk_notebook_prev_page (parent_);

    // remove channel page from channel tabs notebook
    gtk_notebook_remove_page (parent_,
                              page_num);

    g_object_unref (G_OBJECT ((*iterator).second.second));
    CBData_.GTKState->builders.erase (iterator);
  } // end IF
}

bool
IRC_Client_GUI_MessageHandler::isServerLog () const
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::isServerLog"));

  return (parent_ == NULL);
}

void
IRC_Client_GUI_MessageHandler::queueForDisplay (const std::string& text_in)
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::queueForDisplay"));

  // synch access
  ACE_Guard<ACE_SYNCH_MUTEX> aGuard (messageQueueLock_);

  messageQueue_.push_front (text_in);
}

void
IRC_Client_GUI_MessageHandler::update ()
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::update"));

  // always insert new text at the END of the buffer...
  ACE_ASSERT (view_);

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("printing: \"%s\"\n"),
//              ACE_TEXT(displayQueue_.front().c_str())));

  GtkTextIter text_iterator;
  gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (view_),
                                &text_iterator);

  //gchar* string_p = NULL;
  std::string message_text;
  {  // synch access
    ACE_Guard<ACE_SYNCH_MUTEX> aGuard (messageQueueLock_);

    // sanity check
    if (messageQueue_.empty ())
      return; // nothing to do...

    message_text = messageQueue_.back () + '\n';
    //// step1: convert text (GTK uses UTF-8 to represent strings)
    //string_p =
    //  Common_UI_Tools::Locale2UTF8 (messageQueue_.back () + '\n');
    //if (!string_p)
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to Common_UI_Tools::Locale2UTF8(\"%s\"), returning\n"),
    //              ACE_TEXT (messageQueue_.back ().c_str ())));

    //  // clean up
    //  messageQueue_.pop_back ();

    //  return;
    //} // end IF
    messageQueue_.pop_back ();
  } // end lock scope
  // step2: display text
  //gtk_text_buffer_insert (gtk_text_view_get_buffer (view_), &text_iterator,
  //                        string_p, -1);
  gtk_text_buffer_insert (gtk_text_view_get_buffer (view_), &text_iterator,
                          message_text.c_str (), -1);

  //// clean up
  //g_free (string_p);

//   // get the new "end"...
//   gtk_text_buffer_get_end_iter(myTargetBuffer,
//                                &iter);
  // move the iterator to the beginning of line, so we don't scroll
  // in horizontal direction
  gtk_text_iter_set_line_offset (&text_iterator, 0);

  // ...and place the mark at iter. The mark will stay there after we
  // insert some text at the end because it has right gravity
  GtkTextMark* mark_p =
    gtk_text_buffer_get_mark (gtk_text_view_get_buffer (view_),
                              ACE_TEXT_ALWAYS_CHAR ("scroll"));
  ACE_ASSERT (mark_p);
  gtk_text_buffer_move_mark (gtk_text_view_get_buffer (view_),
                             mark_p,
                             &text_iterator);

  // scroll the mark onscreen
  gtk_text_view_scroll_mark_onscreen (view_,
                                      mark_p);

  // redraw view area...
//   // sanity check(s)
//   ACE_ASSERT(myBuilder);
// //   GtkScrolledWindow* scrolledwindow = NULL;
//   GtkWindow* dialog = NULL;
//   dialog = GTK_WINDOW(gtk_builder_get_object(myBuilder,
//                                        ACE_TEXT_ALWAYS_CHAR("dialog")));
//   ACE_ASSERT(dialog);
//   GdkRegion* region = NULL;
//   region = gdk_drawable_get_clip_region(GTK_WIDGET(dialog)->window);
//   ACE_ASSERT(region);
//   gdk_window_invalidate_region(GTK_WIDGET(dialog)->window,
//                                region,
//                                TRUE);
  gdk_window_invalidate_rect (GTK_WIDGET (view_)->window,
                              NULL,
                              TRUE);
//   gdk_region_destroy(region);
//   gtk_widget_queue_draw(GTK_WIDGET(view_));
  gdk_window_process_updates (GTK_WIDGET (view_)->window, TRUE);
//   gdk_window_process_all_updates();
}

GtkWidget*
IRC_Client_GUI_MessageHandler::getTopLevelPageChild ()
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::getTopLevelPageChild"));

  GtkWidget* widget_p = NULL;

  // *WARNING*: the server log handler doesn't have a builder...
  if (!parent_)
  {
    // sanity check(s)
    ACE_ASSERT (view_);

    widget_p =  gtk_widget_get_ancestor (GTK_WIDGET (view_),
                                         GTK_TYPE_WIDGET);
  } // end IF
  else
  {
    // sanity check(s)
    ACE_ASSERT (CBData_.GTKState);

    ACE_Guard<ACE_SYNCH_MUTEX> aGuard (CBData_.GTKState->lock);

    Common_UI_GTKBuildersIterator_t iterator =
      CBData_.GTKState->builders.find (CBData_.builderLabel);
    // sanity check(s)
    ACE_ASSERT (iterator != CBData_.GTKState->builders.end ());

    widget_p =
        GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_VBOX_CHANNEL)));
  } // end ELSE
  ACE_ASSERT (widget_p);

  return widget_p;
}

// const std::string
// IRC_Client_GUI_MessageHandler::getChannel() const
// {
//   NETWORK_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::getChannel"));
//
//   // sanity check: 'this' might be a private message handler !...
//   ACE_ASSERT(RPG_Net_Protocol_Tools::isValidIRCChannelName(CBData_.id));
//
//   return CBData_.id;
// }

void
IRC_Client_GUI_MessageHandler::setTopic (const std::string& topic_in)
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::setTopic"));

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  //ACE_Guard<ACE_SYNCH_MUTEX> aGuard (CBData_.GTKState->lock);

  Common_UI_GTKBuildersIterator_t iterator =
    CBData_.GTKState->builders.find (CBData_.builderLabel);
  // sanity check(s)
  ACE_ASSERT (iterator != CBData_.GTKState->builders.end ());

  gdk_threads_enter ();

  // retrieve label handle
  GtkLabel* label_p =
      GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_LABEL_TOPIC)));
  ACE_ASSERT (label_p);
  gtk_label_set_text (label_p,
                      topic_in.c_str ());

  gdk_threads_leave ();
}

void
IRC_Client_GUI_MessageHandler::setModes (const std::string& modes_in,
                                         const std::string& parameter_in)
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::setModes"));

  ACE_UNUSED_ARG (parameter_in);

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  CBData_.acknowledgements +=
    IRC_Client_Tools::merge (modes_in,
                             CBData_.channelModes);

  guint event_source_id = g_idle_add (idle_update_channel_modes_cb,
                                      &CBData_);
  if (event_source_id == 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_idle_add(idle_update_channel_modes_cb): \"%m\", returning\n")));
    return;
  } // end IF

  ACE_Guard<ACE_SYNCH_MUTEX> aGuard (CBData_.GTKState->lock);

  CBData_.GTKState->eventSourceIds.push_back (event_source_id);
}

void
IRC_Client_GUI_MessageHandler::clearMembers ()
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::clearMembers"));

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  ACE_Guard<ACE_SYNCH_MUTEX> aGuard (CBData_.GTKState->lock);

  Common_UI_GTKBuildersIterator_t iterator =
    CBData_.GTKState->builders.find (CBData_.builderLabel);
  // sanity check(s)
  ACE_ASSERT (iterator != CBData_.GTKState->builders.end ());

  gdk_threads_enter ();

  // retrieve channel liststore handle
  GtkListStore* liststore_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_LISTSTORE_CHANNEL)));
  ACE_ASSERT (liststore_p);

  // clear liststore
  gtk_list_store_clear (liststore_p);

  gdk_threads_leave ();
}

void
IRC_Client_GUI_MessageHandler::updateNick (const std::string& oldNick_in)
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::updateNick"));

  // sanity check(s)
  ACE_ASSERT (CBData_.connection);

  std::string new_nick = CBData_.connection->getNickname ();
  if (CBData_.channelModes.test (CHANNELMODE_OPERATOR))
    new_nick.insert (new_nick.begin (), '@');

  remove (oldNick_in);
  add (new_nick);
}

void
IRC_Client_GUI_MessageHandler::add (const std::string& nick_in)
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::add"));

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  ACE_Guard<ACE_SYNCH_MUTEX> aGuard (CBData_.GTKState->lock);

  Common_UI_GTKBuildersIterator_t iterator =
    CBData_.GTKState->builders.find (CBData_.builderLabel);
  // sanity check(s)
  ACE_ASSERT (iterator != CBData_.GTKState->builders.end ());

  gdk_threads_enter ();

  // retrieve channel liststore handle
  GtkListStore* list_store_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_LISTSTORE_CHANNEL)));
  ACE_ASSERT (list_store_p);

  // step1: convert text
  GtkTreeIter iter;
  gchar* converted_nick_string = Common_UI_Tools::Locale2UTF8 (nick_in);
  if (!converted_nick_string)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to convert nickname: \"%s\", returning\n")));

    // clean up
    gdk_threads_leave ();

    return;
  } // end IF

  // step2: append new (text) entry
  gtk_list_store_append (list_store_p, &iter);
  gtk_list_store_set (list_store_p, &iter,
                      0, converted_nick_string, // column 0
                      -1);

  // clean up
  g_free (converted_nick_string);
  gdk_threads_leave ();
}

void
IRC_Client_GUI_MessageHandler::remove (const std::string& nick_in)
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::remove"));

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  ACE_Guard<ACE_SYNCH_MUTEX> aGuard (CBData_.GTKState->lock);

  Common_UI_GTKBuildersIterator_t iterator =
    CBData_.GTKState->builders.find (CBData_.builderLabel);
  // sanity check(s)
  ACE_ASSERT (iterator != CBData_.GTKState->builders.end ());

  gdk_threads_enter ();

  // retrieve channel liststore handle
  GtkListStore* liststore_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_LISTSTORE_CHANNEL)));
  ACE_ASSERT (liststore_p);

  // step1: convert text
  gchar* converted_nick_string = Common_UI_Tools::Locale2UTF8 (nick_in);
  if (!converted_nick_string)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to convert nickname: \"%s\", returning\n")));

    // clean up
    gdk_threads_leave ();

    return;
  } // end IF

  // step2: find matching entry
  GtkTreeIter current_iter;
//   GValue current_value;
  gchar* current_value_string = NULL;
  if (!gtk_tree_model_get_iter_first (GTK_TREE_MODEL (liststore_p),
                                      &current_iter))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_tree_model_get_iter_first(%@), returning\n"),
                liststore_p));

    // clean up
    g_free (converted_nick_string);
    gdk_threads_leave ();

    return;
  } // end IF
  bool found_row = false;
  do
  {
    current_value_string = NULL;

    // retrieve value
//     gtk_tree_model_get_value(GTK_TREE_MODEL(list_store_p),
//                              current_iter,
//                              0, &current_value);
    gtk_tree_model_get (GTK_TREE_MODEL (liststore_p),
                        &current_iter,
                        0, &current_value_string,
                        -1);
    if (g_str_equal (current_value_string,
                     converted_nick_string) ||
        (g_str_has_suffix (current_value_string,
                           converted_nick_string) &&
         ((current_value_string[0] == '@'))))
      found_row = true;

    // clean up
    g_free (current_value_string);

    if (found_row)
      break; // found value
  } while (gtk_tree_model_iter_next (GTK_TREE_MODEL (liststore_p),
                                     &current_iter));

  // clean up
  g_free (converted_nick_string);

  if (found_row)
    gtk_list_store_remove (liststore_p,
                           &current_iter);
  else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to remove nick (was: \"%s\"), aborting\n"),
                ACE_TEXT (nick_in.c_str ())));

  gdk_threads_leave ();
}

void
IRC_Client_GUI_MessageHandler::members (const string_list_t& list_in)
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::members"));

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  //ACE_Guard<ACE_SYNCH_MUTEX> aGuard (CBData_.GTKState->lock);

  Common_UI_GTKBuildersIterator_t iterator =
    CBData_.GTKState->builders.find (CBData_.builderLabel);
  // sanity check(s)
  ACE_ASSERT (iterator != CBData_.GTKState->builders.end ());

  if (isFirstMemberListMsg_)
  {
    clearMembers ();

    isFirstMemberListMsg_ = false;
  } // end IF

  gdk_threads_enter ();

  // retrieve channel liststore handle
  GtkListStore* liststore_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_LISTSTORE_CHANNEL)));
  ACE_ASSERT (liststore_p);

  GtkTreeIter iter;
  gchar* converted_nick_string = NULL;
  for (string_list_const_iterator_t iterator = list_in.begin ();
       iterator != list_in.end ();
       iterator++)
  {
    // step1: convert text
    converted_nick_string = Common_UI_Tools::Locale2UTF8 (*iterator);
    if (!converted_nick_string)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to convert nickname: \"%s\", returning\n")));

      // clean up
      gdk_threads_leave ();

      return;
    } // end IF

    // step2: append new (text) entry
    gtk_list_store_append (liststore_p, &iter);
    gtk_list_store_set (liststore_p, &iter,
                        0, converted_nick_string, // column 0
                        -1);

    // clean up
    g_free (converted_nick_string);
  } // end FOR

  gdk_threads_leave ();
}

void
IRC_Client_GUI_MessageHandler::endMembers ()
{
  NETWORK_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::endMembers"));

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  ACE_Guard<ACE_SYNCH_MUTEX> aGuard (CBData_.GTKState->lock);

  Common_UI_GTKBuildersIterator_t iterator =
    CBData_.GTKState->builders.find (CBData_.builderLabel);
  // sanity check(s)
  ACE_ASSERT (iterator != CBData_.GTKState->builders.end ());

  isFirstMemberListMsg_ = true;

  gdk_threads_enter ();

  // retrieve treeview handle
  GtkTreeView* treeview_p =
      GTK_TREE_VIEW (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_TREEVIEW_CHANNEL)));
  ACE_ASSERT (treeview_p);
  gtk_widget_set_sensitive (GTK_WIDGET (treeview_p), TRUE);

  gdk_threads_leave ();
}
