#ifndef SIMO_VIEW_HPP
#define SIMO_VIEW_HPP

#include "wx.h"
#include <wx/glcanvas.h>

// x11 header included by the gtk canvas defines Bool and that interfers with
// other code and is also ugly
#ifdef Bool
#undef Bool
#endif

#include "mainframe.h"

class View : public wxGLCanvas {
  friend class MainFrame;

 public:
  View(wxWindow* parent, MainFrame* main, Data* data);

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

  void ClearBackground(const simo::rgba& c);
  void ClearDepth();
  void Invalidate();
  void DrawNormals(const simo::rgba& c);
  void DrawEdges(const simo::rgba& c, float width);
  void DrawPoints(const simo::rgba& c, float size);
  void DrawFacesShaded(const simo::material& c);
  void DrawFacesPlain(const simo::rgba& c);

 private:
  float rotX;
  float rotY;
  float distance;
  long lastx;
  long lasty;
  bool down;
  Data* mData;
  MainFrame* main_;

  wxGLContext rc;
  DECLARE_EVENT_TABLE()
};

#endif
