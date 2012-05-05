#ifndef SIMO_VIEW_HPP
#define SIMO_VIEW_HPP

//#include <wx/wx.h>
#include <wx/glcanvas.h>

#include "mainframe.hpp"

class View : public wxGLCanvas
{
	friend class MainFrame;
public:
	View( wxWindow *parent, wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0, const wxString& name = _T("View") );

	~View();

	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnEraseBackground(wxEraseEvent& event);

	void Render();
	void InitGL();
private:
	bool m_init;
	DECLARE_EVENT_TABLE()
};

#endif
