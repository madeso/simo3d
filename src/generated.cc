///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "generated.h"

///////////////////////////////////////////////////////////////////////////

ConsoleDlgBase::ConsoleDlgBase(wxWindow* parent, wxWindowID id,
                               const wxString& title, const wxPoint& pos,
                               const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style) {
  this->SetSizeHints(wxDefaultSize, wxDefaultSize);

  wxFlexGridSizer* fgSizer1;
  fgSizer1 = new wxFlexGridSizer(2, 1, 0, 0);
  fgSizer1->AddGrowableCol(0);
  fgSizer1->AddGrowableRow(0);
  fgSizer1->SetFlexibleDirection(wxBOTH);
  fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);

  dLog = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                        wxDefaultSize,
                        wxTE_RICH | wxHSCROLL | wxTE_DONTWRAP | wxTE_MULTILINE |
                            wxTE_READONLY | wxVSCROLL);
  fgSizer1->Add(dLog, 0, wxALL | wxEXPAND, 5);

  dInput = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                          wxDefaultSize, wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB);
  fgSizer1->Add(dInput, 0, wxALL | wxEXPAND, 5);

  this->SetSizer(fgSizer1);
  this->Layout();

  // Connect Events
  dInput->Connect(wxEVT_COMMAND_TEXT_ENTER,
                  wxCommandEventHandler(ConsoleDlgBase::OnInputEnter), NULL,
                  this);
}

ConsoleDlgBase::~ConsoleDlgBase() {
  // Disconnect Events
  dInput->Disconnect(wxEVT_COMMAND_TEXT_ENTER,
                     wxCommandEventHandler(ConsoleDlgBase::OnInputEnter), NULL,
                     this);
}
