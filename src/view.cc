#include <sol.hpp>
#include "view.h"

#include <exception>

#if wxUSE_GLCANVAS
#pragma message("wxGL is included")
#else
#error "No wxGL"
#endif

BEGIN_EVENT_TABLE(View, wxGLCanvas)
EVT_SIZE(View::OnSize)
EVT_PAINT(View::OnPaint)
EVT_ERASE_BACKGROUND(View::OnEraseBackground)

EVT_LEFT_DOWN(View::OnLeftDown)
EVT_LEFT_UP(View::OnLeftUp)
EVT_MIDDLE_DOWN(View::OnMiddleDown)
EVT_MIDDLE_UP(View::OnMiddleUp)
EVT_RIGHT_DOWN(View::OnRightDown)
EVT_RIGHT_UP(View::OnRightUp)
EVT_MOTION(View::OnMotion)
EVT_MOUSEWHEEL(View::OnWheel)
END_EVENT_TABLE()

int wx_gl_args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};

View::View(wxWindow* parent, MainFrame* main, Data* data)
    : wxGLCanvas(parent, wxID_ANY, wx_gl_args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE),
      main_(main), mData(data),
      rotX(0),
      rotY(0),
      down(false),
      distance(100),
      rc(this) {
  Invalidate();
}

View::~View() {}

void View::OnLeftDown(wxMouseEvent& e) {
  lastx = e.m_x;
  lasty = e.m_y;
  down = true;
}

void View::OnLeftUp(wxMouseEvent& e) { down = false; }

void View::OnMiddleDown(wxMouseEvent& e) {}

void View::OnMiddleUp(wxMouseEvent& e) {}

void View::OnRightDown(wxMouseEvent& e) {}

void View::OnRightUp(wxMouseEvent& e) {}

void View::OnMotion(wxMouseEvent& e) {
  if (down) {
    float dx = lastx - e.m_x;
    float dy = lasty - e.m_y;

    rotX -= dx;
    rotY -= dy;

    lastx = e.m_x;
    lasty = e.m_y;

    Invalidate();
  }
}

void View::Invalidate() { Refresh(false); }

void View::OnWheel(wxMouseEvent& e) {
  const float move = (float)e.m_wheelRotation / e.m_wheelDelta;
  distance -= move * 10;
  Invalidate();
}

void View::OnPaint(wxPaintEvent& WXUNUSED(event)) {
  wxPaintDC dc(this);
  SetCurrent(rc);

  /*
  #ifndef __WXMOTIF__
    if (!GetContext()) return;
  #endif
    */

  // SetCurrent();
  // Init OpenGL once, but after SetCurrent
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_CULL_FACE);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  int width = 0;
  int height = 0;
  GetClientSize(&width, &height);
  if (height == 0) height = 1;
  const double aspectRatio = (float) width / height;
  const double fieldOfView = 45.0;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  const double zNear = 1.0;
  const double zFar = 1000.0;
  GLfloat aspect = float(width) / float(height);
  GLfloat fH = tan(float(fieldOfView / 360.0f * 3.14159f)) * zNear;
  GLfloat fW = fH * aspect;
  glFrustum(-fW, fW, -fH, fH, zNear, zFar);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // position viewer
  glTranslatef(0.0f, 0.0f, -distance);
  glRotatef(rotY, 1.0f, 0.0f, 0.0f);
  glRotatef(rotX, 0.0f, 1.0f, 0.0f);

  /* clear color and depth buffers */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  std::string m;
  try {
    sol::optional<std::function<void()>> render_func = main_->script().state()["render_func"];
    if (render_func) {
      (*render_func)();
    }
  }
  catch(std::exception& x) {
    m = x.what();
  }
  main_->SetRenderError(m);

  glFlush();
  SwapBuffers();
}

void View::OnSize(wxSizeEvent& event) {
  if (!IsShownOnScreen()) return;
  SetCurrent(rc);

  const auto size = event.GetSize();
  glViewport(0, 0, size.x, size.y);
}

void View::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {
  // Do nothing, to avoid flashing.
}

void ClearBackground(const simo::rgba& c);
void ClearDepth();

void Apply(const simo::rgba& color) {
  glColor4f(color.r(), color.g(), color.b(), color.a());
}

namespace {
void SetGlMaterialf(int face, int id, const simo::rgba& c) {
  GLfloat params[4] = {c.r(), c.g(), c.b(), c.a()};
  glGetMaterialfv(face, id, params);
}
void glVertex3fv(const simo::vec3& v) {
  glVertex3f(v.x(), v.y(), v.z());
}
void glNormal3fv(const simo::vec3& v) {
  glNormal3f(v.x(), v.y(), v.z());
}
}

void Apply(const simo::material& m) {
  int face = GL_FRONT_AND_BACK;
  SetGlMaterialf(face, GL_AMBIENT, m.ambient());
  SetGlMaterialf(face, GL_DIFFUSE, m.diffuse());
  SetGlMaterialf(face, GL_SPECULAR, m.specular());
  SetGlMaterialf(face, GL_EMISSION, m.emission());
  GLfloat p = m.shininess();
  glMaterialfv(face, GL_SHININESS, &p);
}

void View::DrawNormals(const simo::rgba& c) {
  glDisable(GL_LIGHTING);
  Apply(c);
  for (const simo::mesh& m : mData->data.meshes()) {
    for (const simo::face& f : m.faces()) {
      if(f.indices().size() < 3 ) continue;

      glBegin(GL_LINES);
      for (auto i : f.indices()) {
        simo::vec3 e = m.vertices(i.normal()) + ( m.normals(i.normal()) * 10.0f );
        glVertex3fv(e);
        glVertex3fv(m.vertices(i.normal()));
      }
      glEnd();
    }
  }
  glEnable(GL_LIGHTING);
}

void View::DrawEdges(const simo::rgba& c, float width) {
  glDisable(GL_LIGHTING);
  Apply(c);
  glLineWidth(width);
  for (const simo::mesh& m : mData->data.meshes()) {
    for (const simo::face& f : m.faces()) {
      if(f.indices().size() < 3 ) continue;

      glBegin(GL_LINE_LOOP);
      for (auto i : f.indices()) {
        glVertex3fv(m.vertices(i.vertex()));
      }
      glEnd();
    }
  }
  glEnable(GL_LIGHTING);
}

void View::DrawPoints(const simo::rgba& c, float size) {
  glDisable(GL_LIGHTING);
  Apply(c);
  glPointSize(size);
  glBegin(GL_POINTS);
  for (const simo::mesh& m : mData->data.meshes()) {
    for (const simo::vec3& v : m.vertices()) {
      glVertex3fv(v);
    }
  }
  glEnd();
  glEnable(GL_LIGHTING);
}

void View::DrawFacesPlain(const simo::rgba& c) {
  glDisable(GL_LIGHTING);
  Apply(c);
  for (const simo::mesh& m : mData->data.meshes()) {
    for (const simo::face& f : m.faces()) {
      if(f.indices().size() < 3) {
        continue;
      }

      glBegin(GL_POLYGON);
      for (auto i : f.indices()) {
        if (m.normals().empty() == false) {
          glNormal3fv(m.normals(i.normal()));
        }
        glVertex3fv(m.vertices(i.vertex()));
      }
      glEnd();
    }
  }
  glEnable(GL_LIGHTING);
}

void View::DrawFacesShaded(const simo::material& c) {
  Apply(c);
  for (const simo::mesh& m : mData->data.meshes()) {
    for (const simo::face& f : m.faces()) {
      if(f.indices().size() < 3) {
        continue;
      }

      glBegin(GL_POLYGON);
      for (auto i : f.indices()) {
        if (m.normals().empty() == false) {
          glNormal3fv(m.normals(i.normal()));
        }
        glVertex3fv(m.vertices(i.vertex()));
      }
      glEnd();
    }
  }
}
