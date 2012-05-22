#include "mainframe.hpp"
#include "view.hpp"

#include <string>
#include <boost/foreach.hpp>

#include <wx/stdpaths.h>
#include "ConsoleDlg.h"
#include "ScriptLibrary.h"

#include "ptree.h"

#include "simopython.h"
#include "emb.h"

enum
{
	ID_Quit=1,
	ID_About
};

namespace
{
	namespace simodetail
	{
		void msg(const std::string& msg, const std::string& title)
		{
			wxMessageBox(msg, title, wxOK|wxICON_INFORMATION, MainFrame::Get());
		}

		void closemain()
		{
			MainFrame::Get()->Close(true);
		}

		void showconsole()
		{
			MainFrame::Get()->ShowHideConsole();
		}

		void reloadscripts()
		{
			//Get()->reload();
		}

		Data& currentfile()
		{
			return MainFrame::Get()->getData();
		}
	}
}

BOOST_PYTHON_MODULE(simocore)
{
	using namespace boost::python;
	using namespace simodetail;

#define DEF(x) def(#x, x)
	DEF(msg);
	DEF(closemain);
	DEF(showconsole);
	DEF(reloadscripts);
	def("currentfile", make_function(currentfile, return_value_policy<reference_existing_object>()));

	class_<Data>("Data")
		.def("importmesh", &Data::import);
		
}

void Data::import(const std::string& path)
{
	meshes.clear();
}

void Data::render()
{
	BOOST_FOREACH(const Mesh& m, meshes)
	{
		BOOST_FOREACH(const Face& f, m.faces)
		{
			int type = GL_POINTS;

			switch(f.indices.size())
			{
				case 1: type = GL_POINTS; break;
				case 2: type = GL_LINES; break;
				case 3: type = GL_TRIANGLES; break;
				default: type = GL_POLYGON; break;
			}

			glBegin(type);
			BOOST_FOREACH(int i, f.indices)
			{
				if( m.normals.empty() == false )
				{
					glNormal3fv(m.normals[i].data());
				}
				glVertex3fv( m.vertices[i].data() );
			}
			glEnd();
		}
	}
}

MainFrame* MainFrame::Get()
{
	assert(sInstance);
	return sInstance;
}

Data& MainFrame::getData()
{
	return mData;
}
MainFrame* MainFrame::sInstance = 0;

MainFrame::~MainFrame()
{
	Console_End();
	sInstance =  0;
	mConsole->Destroy();
	
	delete mScripts;
	delete mConsole;

	// should this be done? boost documentation says no, but doing this means less memleaks
	Py_Finalize();
}

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame( NULL, -1, title, pos, size )
{
	CreateStatusBar();
	SetStatusText( _("Welcome to SiMo!") );

	assert(sInstance==0);
	sInstance = this;

	PyImport_AppendInittab("simocore", PyInit_simocore);
	PyImport_AppendInittab("emb", emb::PyInit_emb);
	Py_Initialize();
	PyImport_ImportModule("emb");

	Console_Begin();


	wxString file = wxStandardPaths::Get().GetPluginsDir() + "/gui/default.info";
	mConsole = new ConsoleDlg(0);
	mScripts = new ScriptLibrary(mConsole);

	mScripts->init();

	if( mScripts->hasErrors() )
	{
		wxMessageBox("Failed to compile all scripts", "SiMo error", wxOK|wxICON_ERROR, this);
		Close();
	}

	if( false == loadGui(WX_CSTR(file)) )
	{
		wxMessageBox("Failed to load gui from " + string(WX_CSTR(file)), "SiMo error", wxOK|wxICON_ERROR, this);
		Close();
	}

	mView = new View(this, &mData, wxID_ANY, wxDefaultPosition, wxDefaultSize);

	Connect(wxEVT_ACTIVATE, wxActivateEventHandler(MainFrame::OnActivated));
}

void MainFrame::addLog(const wxString& str)
{
	mConsole->addLog(str);
	mConsole->addLog("\n");
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

bool MainFrame::loadGui(const string& file)
{
	IdGenerator idg;

	try
	{
		ifstream f;
		f.open(file.c_str());

		if( f.good() == false )
		{
			addLog("Unable to open " + file);
			return false;
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
		return true;
	}
	catch(const std::exception& e)
	{
		addLog("Unable to parse " + file + ", : " + e.what());
		return false;
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