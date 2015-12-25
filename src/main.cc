#include "wx.h"
#include "mainframe.h"

#if !wxUSE_GLCANVAS
#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

class SimoApp : public wxApp {
  virtual bool OnInit();
};

IMPLEMENT_APP(SimoApp)

bool SimoApp::OnInit() {
  MainFrame *frame =
      new MainFrame("SiMo 3d", wxPoint(50, 50), wxSize(450, 350));

  frame->Show(true);
  SetTopWindow(frame);
  return true;
}