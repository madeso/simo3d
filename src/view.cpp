#include "view.hpp"

#if wxUSE_GLCANVAS
#pragma message("wxGL is included")
#else
#error "No wxGL"
#endif

BEGIN_EVENT_TABLE(View, wxGLCanvas)
	EVT_SIZE(View::OnSize)
	EVT_PAINT(View::OnPaint)
	EVT_ERASE_BACKGROUND(View::OnEraseBackground)
END_EVENT_TABLE()


View::View(wxWindow *parent, Data* data, wxWindowID id,
		   const wxPoint& pos, const wxSize& size, long style, const wxString& name)
		   : wxGLCanvas(parent, (wxGLCanvas*) NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name )
		   , mData(data)
{
}

View::~View()
{
}

void View::Render()
{
	wxPaintDC dc(this);

#ifndef __WXMOTIF__
	if (!GetContext()) return;
#endif

	SetCurrent();
	// Init OpenGL once, but after SetCurrent
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	int width = 0;
	int height = 0;
	GetClientSize(&width, &height);
	if(height==0) height = 1;
	const double aspectRatio = (float) width / height;
	const double fieldOfView = 45.0;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	const double zNear = 1.0;
	const double zFar = 1000.0;
	GLfloat aspect = float(width)/float(height);
	GLfloat fH = tan( float(fieldOfView / 360.0f * 3.14159f) ) * zNear;
	GLfloat fW = fH * aspect;
	glFrustum( -fW, fW, -fH, fH, zNear, zFar );

	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// position viewer
	glTranslatef(0.0f, 0.0f, -200.0f);
	glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(30.0f, 0.0f, 1.0f, 0.0f);

	/* clear color and depth buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mData->render();

	glFlush();
	SwapBuffers();
}

void View::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
	Render();
}

void View::OnSize(wxSizeEvent& event)
{
	// this is also necessary to update the context on some platforms
	wxGLCanvas::OnSize(event);

	// set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
	int w, h;
	GetClientSize(&w, &h);
#ifndef __WXMOTIF__
	if (GetContext())
#endif
	{
		SetCurrent();
		glViewport(0, 0, (GLint) w, (GLint) h);
	}
}

void View::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
	// Do nothing, to avoid flashing.
}