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


View::View(wxWindow *parent, wxWindowID id,
		   const wxPoint& pos, const wxSize& size, long style, const wxString& name)
		   : wxGLCanvas(parent, (wxGLCanvas*) NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name )
{
	m_init = false;
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
	if (!m_init)
	{
		InitGL();
		m_init = true;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);
	glMatrixMode(GL_MODELVIEW);

	/* clear color and depth buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*if( m_gllist == 0 )
	{
	m_gllist = glGenLists( 1 );
	glNewList( m_gllist, GL_COMPILE_AND_EXECUTE );
	draw six faces of a cube */
	glBegin(GL_QUADS);
	glNormal3f( 0.0f, 0.0f, 1.0f);
	glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
	glVertex3f(-0.5f,-0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);

	glNormal3f( 0.0f, 0.0f,-1.0f);
	glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
	glVertex3f( 0.5f, 0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);

	glNormal3f( 0.0f, 1.0f, 0.0f);
	glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);
	glVertex3f(-0.5f, 0.5f,-0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);

	glNormal3f( 0.0f,-1.0f, 0.0f);
	glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);
	glVertex3f( 0.5f,-0.5f, 0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);

	glNormal3f( 1.0f, 0.0f, 0.0f);
	glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);
	glVertex3f( 0.5f,-0.5f,-0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);
	glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
	glEnd();/*

			glEndList();
			}
			else
			{
			glCallList(m_gllist);
			}*/

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

void View::InitGL()
{
	SetCurrent();

	/* set viewing projection */
	glMatrixMode(GL_PROJECTION);
	glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);

	/* position viewer */
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(0.0f, 0.0f, -2.0f);

	/* position object */
	glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(30.0f, 0.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}