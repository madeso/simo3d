#include "ConsoleDlg.h"
#include "ScriptLibrary.h"
#include <boost/foreach.hpp>
#include <vector>
#include <wx/Dir.h>
#include <wx/arrstr.h>
#include <fstream>
#include <wx/stdpaths.h>
#include <wx/filesys.h>
#include "simopython.h"
#include "ptree.h"

ScriptLibrary::ScriptLibrary(ConsoleDlg* c)
	: mConsole(c)
	, mHasErrors(false)
{
}

bool ScriptLibrary::run(const string& file)
{
	if( compile(file) )
	{
		return true;
	}
	else return false;
}

bool ScriptLibrary::compile(const string& filename)
{
	std::ifstream file(filename.c_str());
	if (!file)
		return false;

	std::string fileString = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	file.close();

	const int errors = PyRun_SimpleString(fileString.c_str());

	const bool detectedErrors = errors != 0;
	if( detectedErrors ) mHasErrors = true;
	return false==detectedErrors;
}

const bool ScriptLibrary::hasErrors() const
{
	return mHasErrors;
}

void ScriptLibrary::clearErrors()
{
	mHasErrors = false;
}

namespace // local
{
	wxDateTime GetModificationDateFor(const string& file)
	{
		wxFileSystem fs;
		wxFSFile* f = fs.OpenFile(file.c_str());
		if( f )
		{
			const wxDateTime d = f->GetModificationTime();
			delete f;
			return d;
		}
		else return wxDateTime::Now();
	}
}

bool ScriptLibrary::reload()
{
	bool allok = true;
	// foreach active script, run it
	BOOST_FOREACH(const EntryMap::value_type& p, entries)
	{
		const wxDateTime d = p.second;
		const string file = p.first;
		const wxDateTime modified = GetModificationDateFor(file);
		if( d < modified )
		{
			if( false == run(file, true) )
			{
				allok = false;
			}
		}
	}
	

	return allok;
}

bool ScriptLibrary::init()
{
	ptree doc;
	const wxString file((wxStandardPaths::Get().GetPluginsDir() + "/plugins/plugins.info").c_str());

	ifstream f;
	f.open(WX_CSTR(file));
	if( f.good() == false )
	{
		mConsole->addLog("failed loading: " + file + "\n");
		return false;
	}

	try
	{
		read_info(f, doc);
		const ptree& root = doc.get_child(_T("plugins"));

		bool allok = true;

		BOOST_FOREACH(const ptree::value_type &e, root)
		{
			if( e.first == _T("plugin") )
			{
				const wxString name = e.second.get<string>(_T("name"));
				if( false == loadManifest(WX_CSTR(name)) )
				{
					allok = false;
				}
			}
		}
		return allok;
	}
	catch(const std::exception& e)
	{
		mConsole->addLog("failed loading: " + file + ", " + e.what() + "\n");
		return false;
	}
}

bool ScriptLibrary::loadManifest(const string& name)
{
	ptree doc;
	const wxString file((wxStandardPaths::Get().GetPluginsDir() + "/plugins/" + name + "/main.info").c_str());
	ifstream f;
	f.open(WX_CSTR(file));
	if( false == f.good() )
	{
		mConsole->addLog("failed loading manifest: " + file + "\n");
		return false;
	}
	try
	{
		read_info(f, doc);

		const ptree& root = doc.get_child(_T("main"));

		const bool dev = name[0] == '_';

		bool allok = true;

		BOOST_FOREACH(const ptree::value_type& e, root)
		{
			if( e.first == _T("script") )
			{
				const wxString sname = e.second.get<string>(_T("name"));
				const wxString sfile((wxStandardPaths::Get().GetPluginsDir() + "/plugins/" + name + "/" + sname ).c_str());
				if( false == run(WX_CSTR(sfile), dev) )
				{
					allok = false;
				}
			}
		}
		return allok;
	}
	catch(const std::exception& e)
	{
		mConsole->addLog("failed loading manifest: " + file + ", " + e.what() + "\n");
		return false;
	}
	
}

bool ScriptLibrary::load(const string& dir)
{
	// foreach new file in dir, run and addit it..
	wxArrayString files;
	wxDir::GetAllFiles(dir, &files, wxEmptyString, wxDIR_FILES);

	bool allok = true;
	for(size_t i = 0; i<files.GetCount(); ++i)
	{
		const wxString file = files[i];
		if( entries.find(WX_CSTR(file)) == entries.end() )
		{
			if( false == run(WX_CSTR(file)) )
			{
				allok = false;
			}
		}
	}

	return allok;
}

bool ScriptLibrary::run(const string& file, bool insert)
{
	const bool result = run(file.c_str());

	if( result ) mConsole->addLog(file + " has been sucessfully loaded\n");
	else mConsole->addLog(file + " failed to load\n");;

	if( insert )
	{
		entries[file.c_str()] = wxDateTime::Now();
	}

	return result;
}
