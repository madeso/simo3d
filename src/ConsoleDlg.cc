#include "ConsoleDlg.h"
#include <boost/algorithm/string.hpp>
#include <sstream>
#include "simopython.h"
#include "emb.h"

#include <wx/font.h>

ConsoleDlg* ConsoleDlg::sInstance = 0;

ConsoleDlg::ConsoleDlg(wxWindow* parent)
	: ConsoleDlgBase(parent)
{
	assert(sInstance == 0);
	sInstance = this;

	int size = 10;

	wxTextAttr style;
	font = wxFont::New(size, wxFONTFAMILY_TELETYPE, wxFONTFLAG_DEFAULT, _T(""), wxFONTENCODING_DEFAULT);
	style.SetFont(*font);
	dLog->SetDefaultStyle(style);
	delete font;
}

ConsoleDlg::~ConsoleDlg()
{
	sInstance = 0;
}

ConsoleDlg* ConsoleDlg::Instance()
{
	assert(sInstance);
	return sInstance;
}

void ConsoleDlg::addLog(const wxString& str)
{
	dLog->AppendText(str);
	//dLog->AppendText("\n");
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
	addLog(">> ");
	addLog(s);
	addLog("\n");
	const int errors = PyRun_SimpleString(s);
	return errors == 0;
}

void Console_out(const std::string& o)
{
	ConsoleDlg::Instance()->addLog(o);
}

void Console_Begin()
{
	emb::set_stdout(Console_out);
	emb::set_stderr(Console_out);
}

void Console_End()
{
	emb::reset_stdout();
	emb::reset_stderr();
}