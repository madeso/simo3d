#include "wx.h"
#include "mainframe.h"
#include "data.h"
#include "script.h"
#include "view.h"

#include <wx/textdlg.h>

#include "sol.hpp"

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

void Invalidate() {
  MainFrame::Get()->view().Invalidate();
}
void DrawNormals(const rgba& c) {
  MainFrame::Get()->view().DrawNormals(c);
}
void DrawEdges(const rgba& c, float width) {
  MainFrame::Get()->view().DrawEdges(c, width);
}
void DrawPoints(const rgba& c, float size) {
  MainFrame::Get()->view().DrawPoints(c, size);
}
void DrawFacesShaded() {
  MainFrame::Get()->view().DrawFacesShaded(material());
}
void DrawFacesPlain(const rgba& c) {
  MainFrame::Get()->view().DrawFacesPlain(c);
}

Data& currentfile() { return MainFrame::Get()->getData(); }

void AddLog(const std::string& str) { MainFrame::Get()->AddLog(str); }
void AddLogWithoutEndline(const std::string& str) {
  MainFrame::Get()->AddLogWithoutEndline(str);
}

void LoadFunctions(Script* script) {
  sol::state& state = script->state();
  state.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::string);

  state["msg"] = &msg;
  state["yesno"] = &yesno;
  state["openfile"] = &openfile;
  state["closemain"] = &closemain;
  state["reloadscripts"] = &reloadscripts;
  state["run"] = &runcmd;
  state["input"] = &getstring;
  state["fileimport"] = &import;
  state["file"] = &currentfile;
  state["logline"] = &AddLog;
  state["log"] = &AddLogWithoutEndline;

  sol::table render_table = state.create_named_table("render");
  render_table["invalidate"] = &Invalidate;
  render_table["normals"] = &DrawNormals;
  render_table["edges"] = &DrawEdges;
  render_table["points"] = &DrawPoints;
  render_table["faces_plain"] = &DrawFacesPlain;
  render_table["faces_shaded"] = &DrawFacesShaded;

  state.new_usertype<Data>("Data",
                           "new", sol::no_constructor,
                           "meshes", &Data::meshes);

  state.new_usertype<Mesh>("Mesh",
                             "new", sol::no_constructor,
                             "name", &Mesh::name,
                             "vertices", &Mesh::vertices);
  state.new_usertype<vec3>("vec3",
                             "new", sol::no_constructor,
                             "x", &vec3::x,
                             "y", &vec3::y,
                             "z", &vec3::z);
  state.new_usertype<rgba>("Rgba",
                             sol::constructors<sol::types<>, sol::types<float, float, float>, sol::types<float, float, float, float>>(),
                             "White", sol::factories(&White),
                             "Black", sol::factories(&Black),
                             "r", &rgba::r,
                             "g", &rgba::g,
                             "b", &rgba::b,
                             "a", &rgba::a);
}
