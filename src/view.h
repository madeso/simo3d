#ifndef SIMO_VIEW_HPP
#define SIMO_VIEW_HPP

//#include <wx/wx.h>
#include <wx/glcanvas.h>

#include "mainframe.hpp"

class View : public wxGLCanvas
{
	friend class MainFrame;
public:
	View(wxWindow *parent, Data* data, wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0, const wxString& name = _T("View") );

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
	DECLARE_EVENT_TABLE()
};

#endif
