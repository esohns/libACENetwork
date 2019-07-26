// -*- C++ -*-
//
// generated by wxGlade 0.8.3 on Tue Mar 19 17:49:08 2019
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//

#ifndef NET_SERVER_WXUI_BASE_H
#define NET_SERVER_WXUI_BASE_H

#include <wx/wx.h>
#include <wx/image.h>

// begin wxGlade: ::dependencies
#include <wx/spinbutt.h>
#include <wx/tglbtn.h>
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade


class dialog_main_base: public wxDialog {
public:
  // begin wxGlade: dialog_main_base::ids
  // end wxGlade

  dialog_main_base(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);

private:
  // begin wxGlade: dialog_main_base::methods
  void set_properties();
  void do_layout();
  // end wxGlade

protected:
  // begin wxGlade: dialog_main_base::attributes
  wxToggleButton* togglebutton_listen;
  wxButton* button_close;
  wxButton* button_report;
  wxSpinButton* spinbutton_connections;
  wxSpinButton* spinbutton_messages;
  wxSpinButton* spinbutton_sessionmessages;
  wxSpinButton* spinbutton_ping_timer;
  wxRadioBox* radiobox_transport_layer;
  wxButton* button_about;
  wxButton* button_quit;
  wxGauge* gauge_progress;
  // end wxGlade

//  DECLARE_EVENT_TABLE();

public:
  void togglebutton_listen_cb(wxCommandEvent &event); // wxGlade: <event_handler>
  void button_close_cb(wxCommandEvent &event); // wxGlade: <event_handler>
  void button_report_cb(wxCommandEvent &event); // wxGlade: <event_handler>
  void radiobox_transportlayer_changed_cb(wxCommandEvent &event); // wxGlade: <event_handler>
  void button_about_cb(wxCommandEvent &event); // wxGlade: <event_handler>
  void button_quit_cb(wxCommandEvent &event); // wxGlade: <event_handler>
}; // wxGlade: end class


#endif // NET_SERVER_WXUI_BASE_H
