#ifndef SIMO_MAINFRAME_HPP
#define SIMO_MAINFRAME_HPP

#include <wx/wx.h>
#include <string>
#include <map>

#include <cml/cml.h>
#include <vector>

class View;
class ConsoleDlg;

typedef cml::vector3f vec3;

class Face {
 public:
  std::vector<int> indices;
};

class Mesh {
 public:
  std::string name;
  std::vector<vec3> vertices;
  std::vector<vec3> normals;
  std::vector<Face> faces;
};

class Data {
 public:
  std::vector<Mesh> meshes;
  void import(const std::string& path);
  void render();
};

class MainFrame : public wxFrame {
 public:
  MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
  ~MainFrame();

  void RunCommand(wxCommandEvent& event);
  void OnActivated(wxActivateEvent& evt);

  void addResponse(int id, const std::string& cmd);

  void ShowHideConsole();

  void addLog(const wxString& str);

  Data& getData();

  static MainFrame* Get();

 private:
  static MainFrame* sInstance;

  Data mData;
  View* mView;

  std::string getResponse(int id) const;

  typedef std::map<int, std::string> IdCmdMap;
  IdCmdMap mCommands;

  bool loadGui(const std::string& file);

  ConsoleDlg* mConsole;
};

#endif
