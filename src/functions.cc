#include "wx.h"
#include "mainframe.h"
#include "data.h"
#include "script.h"

#include <wx/textdlg.h>

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

class PrintObject : public sel::BaseFun {
  int Apply(lua_State* L) override {
    int nargs = lua_gettop(L);

    std::ostringstream ss;

    for (int i = 1; i <= nargs; i++) {
      const std::string str = lua_tostring(L, i);
      ss << str;
    }

    MainFrame::Get()->AddLog(ss.str());

    return 0;
  }
};

int add_log(lua_State* L) {
  int nargs = lua_gettop(L);

  std::ostringstream ss;

  for (int i = 1; i <= nargs; i++) {
    const char* c = lua_tostring(L, i);
    const std::string str = c ? c : "<nil>";
    ss << str;
  }

  MainFrame::Get()->AddLog(ss.str());

  return 0;
}

int luaopen_foo(lua_State* L) {
  static const luaL_Reg foo[] = {{"log", add_log}, {NULL, NULL}};

  luaL_newlib(L, foo);
  return 1;
}

void import(const std::string& file) {
  MainFrame::Get()->getData().import(file);
}

void runcmd(const std::string& cmd) {
  if (cmd.empty()) return;
  MainFrame::Get()->script().RunCommand(cmd);
}

std::string getstring(const std::string& title) {
  wxTextEntryDialog dlg(MainFrame::Get(), title, wxGetTextFromUserPromptStr);
  if (dlg.ShowModal() == wxID_OK) {
    return dlg.GetValue().c_str().AsChar();
  } else {
    return "";
  }
}

void LoadFunctions(Script* script) {
  auto& s = script->state();

  s["msg"] = &msg;
  s["yesno"] = &yesno;
  s["openfile"] = &openfile;
  s["closemain"] = &closemain;
  s["reloadscripts"] = &reloadscripts;
  s["run"] = &runcmd;
  s["input"] = &getstring;

  s["fileimport"] = &import;

  s.OpenLib("simo", &luaopen_foo);

  s["file"].SetObj(MainFrame::Get()->getData(), "import", &Data::runimport);

  // s["print"] = PrintObject();
  // s["error"] = PrintObject();
}

void AddLog(const std::string& str) { MainFrame::Get()->AddLog(str); }
