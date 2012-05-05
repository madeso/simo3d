#include "ConsoleDlg.h"
#include <boost/algorithm/string.hpp>
#include <sstream>

class GM_PrintCallback
{
public:
	ConsoleDlg* cd;

	GM_PrintCallback(ConsoleDlg* c)
		: cd(c)
	{
	}

	void operator()(gmMachine * a_machine, const char * a_string)
	{
		cd->addLog(a_string);
	}
};

ConsoleDlg::ConsoleDlg(wxWindow* parent, gmMachine* m)
	: ConsoleDlgBase(parent)
	, mMachine(m)
{
	gmMachine::s_printCallback = GM_PrintCallback(this);
}

void ConsoleDlg::addLog(const wxString& str)
{
	dLog->AppendText(str);
	dLog->AppendText("\n");
}

void ConsoleDlg::OnInputEnter(wxCommandEvent& event)
{
	const wxString s = dInput->GetValue();
	run(s);
}

bool ConsoleDlg::run(const wxString& s)
{
	addLog(s);
	const int errors =mMachine->ExecuteString(s);
	const bool gotErrors = parseErrors("");
	dInput->SelectAll();
	dInput->SetFocus();

	const bool noErrors = errors == 0 && gotErrors == false;
	return noErrors;
}

bool ConsoleDlg::parseErrors(const wxString& file)
{
	gmLog &log = mMachine->GetLog();
	bool firstError = true;
	const char *err = log.GetEntry( firstError );
	bool got = false;
	const wxString kLine = "-----------------------------------------";
	while ( err )
	{
		if( got == false )
		{
			addLog(kLine);
			std::stringstream ss;
			ss << "Compilation error: ";
			if( false == file.empty() )
			{
				ss << "(" << file << ")";
			}
			addLog(ss.str());
			got = true;
		}
		//*dLog << "Compilation error: -" << err << std::endl;
		//std::string s = err;
		//boost::trim(s);
		//addLog(s);
		dLog->AppendText(err);
		err = log.GetEntry( firstError );
	}
	if(got) addLog(kLine);
	log.Reset();

	return got;
}