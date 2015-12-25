///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __generated__
#define __generated__

#include "wx.h"
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class ConsoleDlgBase
///////////////////////////////////////////////////////////////////////////////
class ConsoleDlgBase : public wxDialog {
 private:
 protected:
  wxTextCtrl* dLog;
  wxTextCtrl* dInput;

  // Virtual event handlers, overide them in your derived class
  virtual void OnInputEnter(wxCommandEvent& event) { event.Skip(); }

 public:
  ConsoleDlgBase(wxWindow* parent, wxWindowID id = wxID_ANY,
                 const wxString& title = wxT("Console"),
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxSize(632, 434),
                 long style = wxCAPTION | wxCLOSE_BOX | wxDEFAULT_DIALOG_STYLE |
                              wxDIALOG_NO_PARENT | wxMAXIMIZE_BOX |
                              wxMINIMIZE_BOX | wxRESIZE_BORDER | wxSTAY_ON_TOP |
                              wxSYSTEM_MENU);
  ~ConsoleDlgBase();
};

#endif  //__generated__
