#ifndef ConsoleDlg_h
#define ConsoleDlg_h

#include "generated.h"

class ConsoleDlg : public ConsoleDlgBase {
 public:
  explicit ConsoleDlg(wxWindow* parent);
  ~ConsoleDlg();

  void addLog(const wxString& str);
  bool run(const wxString& str);

  static ConsoleDlg* Instance();

 protected:
  void OnInputEnter(wxCommandEvent& e);

 private:
  wxFont* font;
  static ConsoleDlg* sInstance;
};

void Console_Begin();
void Console_End();

#endif
