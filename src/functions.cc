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
void DrawNormals(const simo::rgba& c) {
  MainFrame::Get()->view().DrawNormals(c);
}
void DrawEdges(const simo::rgba& c, float width) {
  MainFrame::Get()->view().DrawEdges(c, width);
}
void DrawPoints(const simo::rgba& c, float size) {
  MainFrame::Get()->view().DrawPoints(c, size);
}
void DrawFacesShaded() {
  MainFrame::Get()->view().DrawFacesShaded(DefaultMaterial());
}
void DrawFacesPlain(const simo::rgba& c) {
  MainFrame::Get()->view().DrawFacesPlain(c);
}

Data& currentfile() { return MainFrame::Get()->getData(); }

void AddLog(const std::string& str) { MainFrame::Get()->AddLog(str); }
void AddLogWithoutEndline(const std::string& str) {
  MainFrame::Get()->AddLogWithoutEndline(str);
}

std::string vec3_ToString() {
  // print(file().meshes.vertices.at(0))
  return "dog dog";
}

typedef Array<simo::vec3, Mesh> MeshArray;

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
                           "meshes", sol::property(&Data::meshes));

  /*
  state.new_usertype<MeshArray>("Vec3Array",
                           "new", sol::no_constructor,
                           "at", &MeshArray::at,
                           sol::meta_function::index, sol::property(&MeshArray::at, &MeshArray::set),
                           sol::meta_function::length, sol::property(&MeshArray::size),
                           "csize", sol::property(&MeshArray::size)
  );*/

  state.new_usertype<Mesh>("Mesh",
                             "new", sol::no_constructor,
                             "vertices", sol::property(&Mesh::vertices),
                              sol::meta_function::to_string, &Mesh::toString);
  state.new_usertype<simo::vec3>("vec3",
                             "new", sol::no_constructor,
                             "x", sol::property(&simo::vec3::x, &simo::vec3::set_x),
                             "y", sol::property(&simo::vec3::y, &simo::vec3::set_y),
                             "z", sol::property(&simo::vec3::z, &simo::vec3::set_z),
                             sol::meta_function::to_string, &vec3_ToString);
  state.new_usertype<simo::rgba>("Rgba",
                             "new", sol::no_constructor,
                             "Custom", sol::factories(&Color),
                             "White", sol::factories(&White),
                             "Black", sol::factories(&Black));
}
