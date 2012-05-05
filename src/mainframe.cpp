#include "mainframe.hpp"
#include "view.hpp"

#include <string>
#include <boost/foreach.hpp>

#include <wx/stdpaths.h>
#include "ConsoleDlg.h"
#include "ScriptLibrary.h"

#include "gmMathLib.h"
#include "gmStringLib.h"

#include "ptree.h"

enum
{
	ID_Quit=1,
	ID_About
};

// GM_CDECL
class GM_MessageBox
{
public:
	MainFrame* main;

	GM_MessageBox(MainFrame* mf)
		: main(mf)
	{
	}

	int operator()(gmThread* a_thread)
	{
		GM_CHECK_NUM_PARAMS(2);
		GM_CHECK_STRING_PARAM(msg, 0);
		GM_CHECK_STRING_PARAM(title, 1);
		wxMessageBox(msg, title, wxOK|wxICON_INFORMATION, main);
		return GM_OK;
	}
};

class GM_CloseThis
{
public:
	MainFrame* main;

	GM_CloseThis(MainFrame* mf)
		: main(mf)
	{
	}

	int operator()(gmThread* a_thread)
	{
		main->Close(true);
		return GM_OK;
	}
};

class GM_ShowHideConsole
{
public:
	MainFrame* main;

	GM_ShowHideConsole(MainFrame* mf)
		: main(mf)
	{
	}

	int operator()(gmThread* a_thread)
	{
		main->ShowHideConsole();
		return GM_OK;
	}
};

class GM_ReloadScripts
{
public:
	ScriptLibrary* main;

	GM_ReloadScripts(ScriptLibrary* mf)
		: main(mf)
	{
	}

	int operator()(gmThread* a_thread)
	{
		main->reload();
		return GM_OK;
	}
};

MainFrame::~MainFrame()
{
	mConsole->Destroy();
}

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame( NULL, -1, title, pos, size )
{
	CreateStatusBar();
	SetStatusText( _("Welcome to SiMo!") );

	gmBindStringLib(&mMachine);
	gmBindMathLib(&mMachine);
	mMachine.RegisterLibraryFunction("msg", GM_MessageBox(this) );
	mMachine.RegisterLibraryFunction("quit", GM_CloseThis(this) );
	mMachine.RegisterLibraryFunction("console", GM_ShowHideConsole(this) );
	mMachine.RegisterLibraryFunction("reload", GM_ReloadScripts(mScripts) );

	wxString file = wxStandardPaths::Get().GetPluginsDir() + "/gui/default.info";
	mConsole = new ConsoleDlg(0, &mMachine);
	mScripts = new ScriptLibrary(&mMachine, mConsole);

	mMachine.RegisterLibraryFunction("reload", GM_ReloadScripts(mScripts) );

	mScripts->init();

	if( mScripts->hasErrors() )
	{
		wxMessageBox("Failed to compile all scripts, see console for details", "SiMo error", wxOK|wxICON_ERROR, this);
		mScripts->clearErrors();
	}

	loadGui(WX_CSTR(file));

	mView = new View(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

	Connect(wxEVT_ACTIVATE, wxActivateEventHandler(MainFrame::OnActivated));
}

void MainFrame::addLog(const wxString& str)
{
	mConsole->addLog(str);
}

void MainFrame::ShowHideConsole()
{
	mConsole->Show();
}

class IdGenerator
{
public:
	IdGenerator()
		: mId(1)
	{ }

	int generate()
	{
		int temp = id();
		next();
		return temp;
	}

protected:
	void next()
	{
		++mId;
	}

	int id() const
	{
		return mId;
	}
private:
	int mId;
};

wxMenu* loadMenu(MainFrame* mf, const ptree& menuEl, IdGenerator* idg, string* title)
{
	wxMenu* menu = new wxMenu;

	*title = menuEl.get<string>(_T("title"));

	BOOST_FOREACH(const ptree::value_type &itemEl, menuEl.get_child(_T("childs")))
	{
		const string type = itemEl.first;

		if( type == "button" )
		{
			string display = itemEl.second.get<string>(_T("display"));

			string shortcut = itemEl.second.get<string>(_T("shortcut"), _T(""));
			if( shortcut.empty() == false )
			{
				display += "\t" + shortcut;
			}

			string cmd = itemEl.second.get<string>(_T("cmd"));

			int id = idg->generate();

			mf->Connect(id, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction) &MainFrame::RunCommand );
			mf->addResponse(id, cmd);
			menu->Append( id, display );
		}
		else if ( type == "submenu" )
		{
			string display;
			wxMenu* sub = loadMenu(mf, itemEl.second, idg, &display);
			if( display.empty() )
			{
				mf->addLog("missing title in submenu");
				display = _T("EMPTY");
			}
			menu->Append(0, display, sub);
		}
		else if ( type == "seperator" )
		{
			menu->Append(wxID_SEPARATOR, "-", "", wxITEM_SEPARATOR);
		}
		else
		{
			mf->addLog("Unrecognized type " + type);
		}
	}

	return menu;
}

void MainFrame::loadGui(const string& file)
{
	IdGenerator idg;

	
	try
	{
		ifstream f;
		f.open(file.c_str());

		if( f.good() == false )
		{
			addLog("Unable to open " + file);
			return;
		}

		ptree doc;
		read_info(f, doc);
		
		const ptree& root = doc.get_child(_T("gui"));

		const ptree& menuContainerEl = root.get_child(_T("menus"));
		{
			wxMenuBar *menuBar = new wxMenuBar;
			BOOST_FOREACH(const ptree::value_type &menuEl, menuContainerEl)
			{
				string name = _T("No Title");
				wxMenu* m = loadMenu(this, menuEl.second, &idg, &name);
				menuBar->Append(m, name);
			}
			SetMenuBar(menuBar);
		}
	}
	catch(const std::exception& e)
	{
		addLog("Unable to parse " + file + ", : " + e.what());
	}
}

void MainFrame::addResponse(int id, const string& cmd)
{
	mCommands.insert( std::make_pair(id, cmd) );
}

string MainFrame::getResponse(int id) const
{
	IdCmdMap::const_iterator res = mCommands.find(id);
	if( res == mCommands.end() ) return _T("");
	else return res->second;
}

void MainFrame::RunCommand(wxCommandEvent& ev)
{
	const string res = getResponse(ev.GetId());
	bool ok = mConsole->run(res);
	if( false == ok )
	{
		wxMessageBox("Failed to sucessfully run command, see console for details", "SiMo error", wxOK|wxICON_ERROR, this);
		mScripts->clearErrors();
	}
}


void MainFrame::OnActivated(wxActivateEvent& evt)
{
	// check if it lost focus
	if( evt.GetActive() == false ) return;
	evt.Skip();
	mScripts->reload();
	if( mScripts->hasErrors() )
	{
		mScripts->clearErrors();
		wxMessageBox("Failed to reload scripts, see console for details", "SiMo error", wxOK|wxICON_ERROR, this);
	}
	//addLog("app got focus");
}