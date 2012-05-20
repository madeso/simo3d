#include "ConsoleDlg.h"
#include <boost/algorithm/string.hpp>
#include <sstream>
#include "simopython.h"

ConsoleDlg::ConsoleDlg(wxWindow* parent)
	: ConsoleDlgBase(parent)
{
}

void ConsoleDlg::addLog(const wxString& str)
{
	dLog->AppendText(str);
	dLog->AppendText("\n");
	//OutputDebugString(str);
}

void ConsoleDlg::OnInputEnter(wxCommandEvent& event)
{
	const wxString s = dInput->GetValue();
	run(s);
	dInput->SelectAll();
	dInput->SetFocus();
}

bool ConsoleDlg::run(const wxString& s)
{
	addLog(s);
	const int errors = PyRun_SimpleString(s);
	return errors == 0;
}
