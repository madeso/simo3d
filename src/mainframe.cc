#include "mainframe.h"
#include "view.h"

#include <string>

#include <wx/stdpaths.h>
#include <wx/splitter.h>

#include <google/protobuf/message.h>
#include <wx/filename.h>
#include <external/pbjson/src/pbjson.hpp>

#include "idgenerator.h"
#include "gui.h"

MainFrame* MainFrame::sInstance = nullptr;
MainFrame* MainFrame::Get() {
  assert(sInstance);
  return sInstance;
}

Data& MainFrame::getData() { return mData; }

MainFrame::~MainFrame() {
  sInstance = nullptr;
  google::protobuf::ShutdownProtobufLibrary();
}

MainFrame::MainFrame(const wxString& title, const wxPoint& pos,
                     const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size),
      script_(),
      library_(&script_) {
  CreateStatusBar();
  SetStatusText(_("Welcome to SiMo!"));

  assert(sInstance == nullptr);
  sInstance = this;

  const wxString file =
      wxStandardPaths::Get().GetResourcesDir() + "/gui/default.info";

  if (false == loadGui(file.c_str().AsChar())) {
    wxMessageBox(
        "Failed to load gui from " + std::string(file.c_str().AsChar()),
        "SiMo error", wxOK | wxICON_ERROR, this);
    Close();
    return;
  }

  wxSplitterWindow* split =
      new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                           wxSP_LIVE_UPDATE | wxSP_3DBORDER);
  split->SetSashGravity(1.0);

  mView = new View(split, &mData);

  log_ = new wxTextCtrl(split, wxID_ANY, "", wxDefaultPosition, wxSize(0, 60),
                        wxTE_READONLY | wxHSCROLL | wxTE_MULTILINE);

  split->SplitHorizontally(mView, log_, 100);

  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(split, wxSizerFlags(1).Expand());
  this->SetSizer(sizer);

  Connect(wxEVT_ACTIVATE, wxActivateEventHandler(MainFrame::OnActivated));
}

void MainFrame::AddLog(const std::string& str) {
  log_->AppendText(str.c_str());
  log_->AppendText("\n");
}

bool MainFrame::loadGui(const std::string& file) { return LoadGui(file, this); }

void MainFrame::addResponse(int id, const std::string& cmd) {
  mCommands.insert(std::make_pair(id, cmd));
}

std::string MainFrame::getResponse(int id) const {
  IdCmdMap::const_iterator res = mCommands.find(id);
  if (res == mCommands.end())
    return "";
  else
    return res->second;
}

void MainFrame::RunCommand(wxCommandEvent& ev) {
  const std::string res = getResponse(ev.GetId());
  if (false == script_.RunCommand(res)) {
    wxMessageBox("Failed to successfully run command, see console for details",
                 "SiMo error", wxOK | wxICON_ERROR, this);
    return;
  }
  mView->Invalidate();
}

void MainFrame::OnActivated(wxActivateEvent& evt) {
  if (evt.GetActive() == false) return;
  evt.Skip();

  if (false == library_.reload()) {
    wxMessageBox("Failed to reload scripts, see console for details",
                 "SiMo error", wxOK | wxICON_ERROR, this);
  }
}
