#ifndef SIMO_SCRIPT_LIBRARY_H
#define SIMO_SCRIPT_LIBRARY_H

#include <string>
#include <map>
#include <wx/DateTime.h>
#include <gmThread.h>

#ifdef _UNICODE
	typedef std::wstring string;
#else
	typedef std::string string;
#endif

class ConsoleDlg;

class ScriptLibrary
{
public:
	ScriptLibrary(gmMachine* m, ConsoleDlg* c);

	bool init();
	bool reload();

	const bool hasErrors() const;
	void clearErrors();
private:
	gmMachine* mMachine;

	bool run(const string& file);
	bool compile(const string& file);

	bool load(const string& file);
	bool loadManifest(const string& name);
	bool run(const string& file, bool insert);

	typedef std::map<string, wxDateTime> EntryMap;
	EntryMap entries;

	ConsoleDlg* mConsole;
	bool mHasErrors;
};

#endif