#ifndef SIMO_VIEW_HPP
#define SIMO_VIEW_HPP

#include "wx.h"
#include <wx/glcanvas.h>

#include "mainframe.h"

class View : public wxGLCanvas {
  friend class MainFrame;

 public:
  View(wxWindow* parent, Data* data);

  ~View();

  void OnPaint(wxPaintEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnEraseBackground(wxEraseEvent& event);

  void OnLeftDown(wxMouseEvent& e);
  void OnLeftUp(wxMouseEvent& e);
  void OnMiddleDown(wxMouseEvent& e);
  void OnMiddleUp(wxMouseEvent& e);
  void OnRightDown(wxMouseEvent& e);
  void OnRightUp(wxMouseEvent& e);
  void OnMotion(wxMouseEvent& e);
  void OnWheel(wxMouseEvent& e);

  void Invalidate();

 private:
  float rotX;
  float rotY;
  float distance;
  long lastx;
  long lasty;
  bool down;
  Data* mData;

  wxGLContext rc;
  DECLARE_EVENT_TABLE()
};

#endif
