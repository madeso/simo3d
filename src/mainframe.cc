#include "mainframe.h"
#include "view.h"

#include <string>

#include <wx/stdpaths.h>

#include <google/protobuf/message.h>
#include <wx/filename.h>
#include <external/pbjson/src/pbjson.hpp>
#include "gui.pb.h"

#include "idgenerator.h"

enum { ID_Quit = 1, ID_About };

namespace {
namespace simodetail {
void msg(const std::string& msg, const std::string& title) {
  wxMessageBox(msg, title, wxOK | wxICON_INFORMATION, MainFrame::Get());
}

bool yesno(const std::string& msg, const std::string& title) {
  return wxMessageBox(msg, title, wxICON_QUESTION | wxYES_NO,
                      MainFrame::Get()) == wxYES;
}

std::string openfile(const std::string& title, const std::string& pattern) {
  wxFileDialog openFileDialog(MainFrame::Get(), title, "", "", pattern,
                              wxFD_OPEN | wxFD_FILE_MUST_EXIST);

  if (openFileDialog.ShowModal() == wxID_CANCEL)
    return "";
  else
    return openFileDialog.GetPath().c_str().AsChar();
}

void closemain() { MainFrame::Get()->Close(true); }

void reloadscripts() {
  // Get()->reload();
}

Data& currentfile() { return MainFrame::Get()->getData(); }
}
}

MainFrame* MainFrame::Get() {
  assert(sInstance);
  return sInstance;
}

Data& MainFrame::getData() { return mData; }
MainFrame* MainFrame::sInstance = 0;

MainFrame::~MainFrame() { sInstance = 0; }

MainFrame::MainFrame(const wxString& title, const wxPoint& pos,
                     const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size) {
  CreateStatusBar();
  SetStatusText(_("Welcome to SiMo!"));

  assert(sInstance == 0);
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

  mView = new View(this, &mData, wxID_ANY, wxDefaultPosition, wxDefaultSize);

  Connect(wxEVT_ACTIVATE, wxActivateEventHandler(MainFrame::OnActivated));
}

wxString LoadProtoJson(google::protobuf::Message* message,
                       const wxString& path) {
  std::string err;
  int load_result = pbjson::json2pb_file(path.c_str().AsChar(), message, err);
  if (load_result < 0) {
    return err.c_str();
  }

  return "";
}

wxString SaveProtoJson(const google::protobuf::Message& t,
                       const wxString& path) {
  bool write_result = pbjson::pb2json_file(&t, path.c_str().AsChar(), true);
  if (write_result == false) {
    return "Unable to write to file";
  }

  return "";
}

wxMenu* loadMenu(MainFrame* mf, const simo::Menu& menuEl, IdGenerator* idg,
                 std::string* title) {
  wxMenu* menu = new wxMenu;

  *title = menuEl.title();

  for (const simo::MenuItem& itemEl : menuEl.items()) {
    if (itemEl.has_button()) {
      auto& button = itemEl.button();
      std::string display = button.display();

      std::string shortcut = button.shortcut();
      if (shortcut.empty() == false) {
        display += "\t" + shortcut;
      }

      std::string cmd = button.cmd();

      int id = idg->generate();

      mf->Connect(id, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction)&MainFrame::RunCommand);
      mf->addResponse(id, cmd);
      menu->Append(id, display);
    } else if (itemEl.has_menu()) {
      std::string display;
      wxMenu* sub = loadMenu(mf, itemEl.menu(), idg, &display);
      if (display.empty()) {
        // mf->addLog("missing title in submenu");
        display = "EMPTY";
      }
      menu->Append(0, display, sub);
    } else if (itemEl.has_seperator()) {
      menu->Append(wxID_SEPARATOR, "-", "", wxITEM_SEPARATOR);
    } else {
      // mf->addLog("Unrecognized type");
    }
  }

  return menu;
}

bool MainFrame::loadGui(const std::string& file) {
  IdGenerator idg;

  simo::Gui gui;
  wxString r = LoadProtoJson(&gui, file.c_str());
  if (r.IsEmpty() == false) {
    // addLog(r);
    return false;
  }

  wxMenuBar* menuBar = nullptr;

  for (const simo::Menu& menu : gui.menu()) {
    if (menuBar == nullptr) menuBar = new wxMenuBar;
    std::string name = "No Title";
    wxMenu* m = loadMenu(this, menu, &idg, &name);
    menuBar->Append(m, name);
  }

  if (menuBar != nullptr) {
    SetMenuBar(menuBar);
  }

  return true;
}

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
  // check if it lost focus
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
  // addLog("app got focus");
}