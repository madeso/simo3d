#ifndef ConsoleDlg_h
#define ConsoleDlg_h

#include "generated.h"

class ConsoleDlg : public ConsoleDlgBase
{
public:
	explicit ConsoleDlg(wxWindow* parent);

	void addLog(const wxString& str);
	bool run(const wxString& str);
protected:
	void OnInputEnter(wxCommandEvent& e);
};

#endif
