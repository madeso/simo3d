#ifndef SIMO_MAINFRAME_HPP
#define SIMO_MAINFRAME_HPP

#include "wx.h"
#include <string>
#include <map>

#include "data.h"
#include "scriptlib.h"
#include "script.h"

class View;
class ConsoleDlg;

class MainFrame : public wxFrame {
 public:
  MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
  ~MainFrame();

  void RunCommand(wxCommandEvent& event);
  void OnActivated(wxActivateEvent& evt);

  void addResponse(int id, const std::string& cmd);

  Data& getData();

  static MainFrame* Get();

  void AddLog(const std::string& str);

 private:
  static MainFrame* sInstance;

  Data mData;
  View* mView;

  std::string getResponse(int id) const;

  typedef std::map<int, std::string> IdCmdMap;
  IdCmdMap mCommands;

  bool loadGui(const std::string& file);

  Script script_;
  wxTextCtrl* log_;
  ScriptLib library_;
};

#endif
