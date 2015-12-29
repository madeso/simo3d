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
  auto& s = script->chai();

  s.add(chaiscript::fun(&msg), "msg");
  s.add(chaiscript::fun(&yesno), "yesno");
  s.add(chaiscript::fun(&openfile), "openfile");
  s.add(chaiscript::fun(&closemain), "closemain");
  s.add(chaiscript::fun(&reloadscripts), "reloadscripts");
  s.add(chaiscript::fun(&runcmd), "run");
  s.add(chaiscript::fun(&getstring), "input");
  s.add(chaiscript::fun(&import), "fileimport");

  // s["file"].SetObj(MainFrame::Get()->getData(), "import", &Data::runimport);

  // s["print"] = PrintObject();
  // s["error"] = PrintObject();
}

void AddLog(const std::string& str) { MainFrame::Get()->AddLog(str); }
void AddLogWithoutEndline(const std::string& str) {
  MainFrame::Get()->AddLogWithoutEndline(str);
}
