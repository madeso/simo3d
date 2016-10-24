#include "gui.h"

#include "wx.h"
#include "mainframe.h"

#include <wx/stdpaths.h>

#include <google/protobuf/message.h>
#include <wx/filename.h>
#include <external/pbjson/src/pbjson.hpp>
#include "gui.pb.h"

#include "idgenerator.h"

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

wxMenu* loadMenu(MainFrame* mf, const gui::Menu& menuEl, IdGenerator* idg,
                 std::string* title) {
  wxMenu* menu = new wxMenu;

  *title = menuEl.title();

  for (const gui::MenuItem& itemEl : menuEl.items()) {
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
        mf->AddLog("Missing title in submenu");
        display = "EMPTY";
      }
      menu->Append(0, display, sub);
    } else if (itemEl.has_seperator()) {
      menu->Append(wxID_SEPARATOR, "-", "", wxITEM_SEPARATOR);
    } else {
      mf->AddLog("Unrecognized type");
    }
  }

  return menu;
}

bool LoadGui(const std::string& file, MainFrame* frame) {
  IdGenerator idg;

  gui::Gui gui;
  wxString r = LoadProtoJson(&gui, file.c_str());
  if (r.IsEmpty() == false) {
    frame->AddLog(r.c_str().AsChar());
    return false;
  }

  wxMenuBar* menuBar = nullptr;

  for (const gui::Menu& menu : gui.menu()) {
    if (menuBar == nullptr) menuBar = new wxMenuBar;
    std::string name = "No Title";
    wxMenu* m = loadMenu(frame, menu, &idg, &name);
    menuBar->Append(m, name);
  }

  if (menuBar != nullptr) {
    frame->SetMenuBar(menuBar);
  }

  return true;
}
