#include "mainframe.h"
#include "view.h"

#include <string>

#include <wx/stdpaths.h>

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
    : wxFrame(nullptr, wxID_ANY, title, pos, size) {
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

  mView = new View(this, &mData);

  Connect(wxEVT_ACTIVATE, wxActivateEventHandler(MainFrame::OnActivated));
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
  if (true) {
    wxMessageBox("Failed to successfully run command, see console for details",
                 "SiMo error", wxOK | wxICON_ERROR, this);
    return;
  }
  mView->Invalidate();
}

void MainFrame::OnActivated(wxActivateEvent& evt) {
  if (evt.GetActive() == false) return;
  evt.Skip();

  /*
  mScripts->reload();
  if (mScripts->hasErrors()) {
    mScripts->clearErrors();
    wxMessageBox("Failed to reload scripts, see console for details",
                 "SiMo error", wxOK | wxICON_ERROR, this);
  }
  */
}
