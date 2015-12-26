#include "wx.h"
#include "mainframe.h"

#include "gui.pb.h"

#if !wxUSE_GLCANVAS
#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

class SimoApp : public wxApp {
  virtual bool OnInit();
};

IMPLEMENT_APP(SimoApp)

bool SimoApp::OnInit() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  
  const wxString DISPLAY_NAME = "SiMo 3d";
  
  wxAppConsole* app_console = wxAppConsole::GetInstance();
  app_console->SetAppName("ride");
  app_console->SetAppDisplayName(DISPLAY_NAME);
  app_console->SetVendorName("madeso");
  app_console->SetVendorDisplayName("madeso");
  
  MainFrame *frame =
      new MainFrame(DISPLAY_NAME, wxPoint(50, 50), wxSize(450, 350));

  frame->Show(true);
  return true;
}