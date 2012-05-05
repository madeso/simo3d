#ifndef SIMO_MAINFRAME_HPP
#define SIMO_MAINFRAME_HPP

#include <wx/wx.h>
#include <gmThread.h>
#include <string>
#include <map>

class View;
class ConsoleDlg;
class ScriptLibrary;

#ifdef _UNICODE
	typedef std::wstring string;
#else
	typedef std::string string;
#endif

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	~MainFrame();

	void RunCommand(wxCommandEvent& event);
	void OnActivated(wxActivateEvent& evt);

	void addResponse(int id, const string& cmd);

	void ShowHideConsole();

	void addLog(const wxString& str);
private:
	View* mView;

	string getResponse(int id) const;

	typedef std::map<int, string> IdCmdMap;
	IdCmdMap mCommands;

	void loadGui(const string& file);

	gmMachine mMachine;
	ConsoleDlg* mConsole;

	ScriptLibrary* mScripts;
};

#endif
