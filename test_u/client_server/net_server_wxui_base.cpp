// -*- C++ -*-
//
// generated by wxGlade 0.8.3 on Tue Mar 19 16:51:15 2019
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//

#include "net_server_wxui_base.h"

// begin wxGlade: ::extracode
// end wxGlade



dialog_main_base::dialog_main_base(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
  wxDialog(parent, id, title, pos, size, wxCAPTION|wxRESIZE_BORDER)
{
  // begin wxGlade: dialog_main_base::dialog_main_base
  togglebutton_listen = new wxToggleButton(this, wxID_OPEN, wxT("Listen"));
  button_close = new wxButton(this, wxID_CLOSE, wxEmptyString);
  button_report = new wxButton(this, wxID_NEW, wxEmptyString);
  spinbutton_connections = new wxSpinButton(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_HORIZONTAL);
  spinbutton_messages = new wxSpinButton(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_HORIZONTAL);
  spinbutton_sessionmessages = new wxSpinButton(this, wxID_ANY);
  spinbutton_ping_timer = new wxSpinButton(this, wxID_ANY);
  const wxString radiobox_transport_layer_choices[] = {
    wxT("TCP"),
    wxT("UDP"),
  };
  radiobox_transport_layer = new wxRadioBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 2, radiobox_transport_layer_choices, 2, wxRA_SPECIFY_ROWS);
  button_about = new wxButton(this, wxID_HELP, wxEmptyString);
  button_quit = new wxButton(this, wxID_EXIT, wxEmptyString);
  gauge_progress = new wxGauge(this, wxID_ANY, 10, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_SMOOTH);

  set_properties();
  do_layout();
  // end wxGlade
}


void dialog_main_base::set_properties()
{
  // begin wxGlade: dialog_main_base::set_properties
  SetTitle(wxT("server"));
  togglebutton_listen->SetToolTip(wxT("toggle listening"));
  togglebutton_listen->SetFocus();
  button_close->SetToolTip(wxT("close all connections"));
  button_report->SetToolTip(wxT("report status"));
  spinbutton_connections->SetToolTip(wxT("connections"));
  spinbutton_connections->Enable(0);
  spinbutton_messages->SetToolTip(wxT("messages"));
  spinbutton_messages->Enable(0);
  spinbutton_sessionmessages->SetToolTip(wxT("session messages"));
  spinbutton_sessionmessages->Enable(0);
  spinbutton_ping_timer->SetToolTip(wxT("ping timer (seconds)"));
  radiobox_transport_layer->SetToolTip(wxT("transport layer"));
  radiobox_transport_layer->SetSelection(0);
  button_about->SetToolTip(wxT("about"));
  button_quit->SetToolTip(wxT("quit"));
  // end wxGlade
}


void dialog_main_base::do_layout()
{
  // begin wxGlade: dialog_main_base::do_layout
  wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* sizer_3 = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* sizer_4 = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizer_7 = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* sizer_5 = new wxBoxSizer(wxHORIZONTAL);
  wxFlexGridSizer* grid_sizer_2 = new wxFlexGridSizer(2, 3, 0, 0);
  wxBoxSizer* sizer_6 = new wxBoxSizer(wxVERTICAL);
  sizer_6->Add(togglebutton_listen, 0, wxEXPAND, 0);
  sizer_6->Add(button_close, 0, 0, 0);
  sizer_6->Add(button_report, 0, 0, 0);
  sizer_5->Add(sizer_6, 0, 0, 0);
  wxStaticText* label_1 = new wxStaticText(this, wxID_ANY, wxT("connections"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
  grid_sizer_2->Add(label_1, 0, wxALIGN_CENTER|wxEXPAND|wxLEFT|wxRIGHT, 3);
  grid_sizer_2->Add(spinbutton_connections, 1, wxEXPAND, 0);
  grid_sizer_2->Add(0, 0, 0, 0, 0);
  wxStaticText* label_2 = new wxStaticText(this, wxID_ANY, wxT("messages"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
  grid_sizer_2->Add(label_2, 0, wxALIGN_CENTER|wxEXPAND|wxLEFT|wxRIGHT, 3);
  grid_sizer_2->Add(spinbutton_messages, 1, wxEXPAND, 0);
  grid_sizer_2->Add(spinbutton_sessionmessages, 1, wxEXPAND, 0);
  grid_sizer_2->AddGrowableCol(1);
  grid_sizer_2->AddGrowableCol(2);
  sizer_5->Add(grid_sizer_2, 1, wxEXPAND, 0);
  sizer_4->Add(sizer_5, 1, wxEXPAND, 0);
  sizer_7->Add(spinbutton_ping_timer, 1, 0, 0);
  sizer_7->Add(radiobox_transport_layer, 1, 0, 0);
  sizer_4->Add(sizer_7, 0, wxEXPAND, 0);
  sizer_1->Add(sizer_4, 0, wxEXPAND, 0);
  sizer_3->Add(button_about, 0, 0, 0);
  sizer_3->Add(button_quit, 0, 0, 0);
  sizer_1->Add(sizer_3, 0, wxALIGN_RIGHT, 0);
  sizer_2->Add(gauge_progress, 0, wxEXPAND, 0);
  sizer_2->Add(0, 0, 0, 0, 0);
  sizer_1->Add(sizer_2, 0, wxEXPAND, 0);
  SetSizer(sizer_1);
  sizer_1->Fit(this);
  Layout();
  // end wxGlade
}


//BEGIN_EVENT_TABLE(dialog_main_base, wxDialog)
//  // begin wxGlade: dialog_main_base::event_table
//  EVT_TOGGLEBUTTON(wxID_OPEN, dialog_main_base::togglebutton_listen_cb)
//  EVT_BUTTON(wxID_ANY, dialog_main_base::button_close_cb)
//  EVT_BUTTON(wxID_ANY, dialog_main_base::button_report_cb)
//  EVT_RADIOBOX(wxID_ANY, dialog_main_base::radiobox_transportlayer_changed_cb)
//  EVT_BUTTON(wxID_ABOUT, dialog_main_base::button_about_cb)
//  EVT_BUTTON(wxID_ANY, dialog_main_base::button_quit_cb)
//  // end wxGlade
//END_EVENT_TABLE();


void dialog_main_base::togglebutton_listen_cb(wxCommandEvent &event)  // wxGlade: dialog_main_base.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (dialog_main_base::togglebutton_listen_cb) not implemented yet"));
}

void dialog_main_base::button_close_cb(wxCommandEvent &event)  // wxGlade: dialog_main_base.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (dialog_main_base::button_close_cb) not implemented yet"));
}

void dialog_main_base::button_report_cb(wxCommandEvent &event)  // wxGlade: dialog_main_base.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (dialog_main_base::button_report_cb) not implemented yet"));
}

void dialog_main_base::radiobox_transportlayer_changed_cb(wxCommandEvent &event)  // wxGlade: dialog_main_base.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (dialog_main_base::radiobox_transportlayer_changed_cb) not implemented yet"));
}

void dialog_main_base::button_about_cb(wxCommandEvent &event)  // wxGlade: dialog_main_base.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (dialog_main_base::button_about_cb) not implemented yet"));
}

void dialog_main_base::button_quit_cb(wxCommandEvent &event)  // wxGlade: dialog_main_base.<event_handler>
{
  event.Skip();
  // notify the user that he hasn't implemented the event handler yet
  wxLogDebug(wxT("Event handler (dialog_main_base::button_quit_cb) not implemented yet"));
}


// wxGlade: add dialog_main_base event handlers


//class server: public wxApp {
//public:
//  bool OnInit();
//};

//IMPLEMENT_APP(server)

//bool server::OnInit()
//{
//  wxInitAllImageHandlers();
//  dialog_main_base* dialog_main = new dialog_main_base(NULL, wxID_ANY, wxEmptyString);
//  SetTopWindow(dialog_main);
//  dialog_main->Show();
//  return true;
//}
