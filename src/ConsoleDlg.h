#ifndef ConsoleDlg_h
#define ConsoleDlg_h

#include "generated.h"
#include <gmThread.h>

class ConsoleDlg : public ConsoleDlgBase
{
public:
	ConsoleDlg(wxWindow* parent, gmMachine* m);

	void addLog(const wxString& str);
	bool run(const wxString& str);

	// @return true on no errors
	bool parseErrors(const wxString& file);
protected:
	void OnInputEnter(wxCommandEvent& e);
private:
	gmMachine* mMachine;
};

#endif
