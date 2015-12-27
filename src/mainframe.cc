#include "mainframe.h"
#include "view.h"

#include <string>

#include <wx/stdpaths.h>
#include "ConsoleDlg.h"

#include <assimp/Importer.hpp>   // C++ importer interface
#include <assimp/scene.h>        // Output data structure
#include <assimp/postprocess.h>  // Post processing flags

#include <google/protobuf/message.h>
#include <wx/filename.h>
#include <external/pbjson/src/pbjson.hpp>
#include "gui.pb.h"

enum { ID_Quit = 1, ID_About };

namespace {
namespace simodetail {
void msg(const std::string& msg, const std::string& title) {
  wxMessageBox(msg, title, wxOK | wxICON_INFORMATION, MainFrame::Get());
}

bool yesno(const std::string& msg, const std::string& title) {
  return wxMessageBox(msg, title, wxICON_QUESTION | wxYES_NO,
                      MainFrame::Get()) == wxYES;
}

std::string openfile(const std::string& title, const std::string& pattern) {
  wxFileDialog openFileDialog(MainFrame::Get(), title, "", "", pattern,
                              wxFD_OPEN | wxFD_FILE_MUST_EXIST);

  if (openFileDialog.ShowModal() == wxID_CANCEL)
    return "";
  else
    return openFileDialog.GetPath().c_str().AsChar();
}

void closemain() { MainFrame::Get()->Close(true); }

void showconsole() { MainFrame::Get()->ShowHideConsole(); }

void reloadscripts() {
  // Get()->reload();
}

Data& currentfile() { return MainFrame::Get()->getData(); }
}
}

namespace {
using namespace simodetail;

void vecopy(std::vector<vec3>& t, aiVector3D* s, unsigned int c) {
  t.reserve(c);
  for (unsigned i = 0; i < c; ++i) {
    t.push_back(vec3(s[i].x, s[i].y, s[i].z));
  }
}
}

void Data::import(const std::string& path) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
      path, aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices |
                aiProcess_ValidateDataStructure | aiProcess_FindInvalidData);

  if (!scene) {
    throw std::runtime_error(importer.GetErrorString());
  }

  if (scene->HasMeshes() == false) {
    throw std::runtime_error("non meshes not currently supported");
  }

  meshes.clear();
  meshes.reserve(scene->mNumMeshes);
  for (unsigned int imesh = 0; imesh < scene->mNumMeshes; ++imesh) {
    aiMesh* amesh = scene->mMeshes[imesh];
    Mesh mesh;
    mesh.name = amesh->mName.data;
    vecopy(mesh.vertices, amesh->mVertices, amesh->mNumVertices);
    if (amesh->mNormals) {
      vecopy(mesh.normals, amesh->mNormals, amesh->mNumVertices);
    }

    for (unsigned int iface = 0; iface < amesh->mNumFaces; ++iface) {
      const aiFace& aface = amesh->mFaces[iface];
      Face face;
      for (unsigned int i = 0; i < aface.mNumIndices; ++i) {
        face.indices.push_back(aface.mIndices[i]);
      }
      mesh.faces.push_back(face);
    }

    meshes.push_back(mesh);
  }
}

void Data::render() {
  // glColor3f(1, 0, 0);
  for (const Mesh& m : meshes) {
    for (const Face& f : m.faces) {
      int type = GL_POINTS;

      switch (f.indices.size()) {
        case 1:
          type = GL_POINTS;
          break;
        case 2:
          type = GL_LINES;
          break;
        case 3:
          type = GL_TRIANGLES;
          break;
        default:
          type = GL_POLYGON;
          break;
      }

      glBegin(type);
      for (int i : f.indices) {
        if (m.normals.empty() == false) {
          glNormal3fv(m.normals[i].data());
        }
        glVertex3fv(m.vertices[i].data());
      }
      glEnd();
    }
  }
}

MainFrame* MainFrame::Get() {
  assert(sInstance);
  return sInstance;
}

Data& MainFrame::getData() { return mData; }
MainFrame* MainFrame::sInstance = 0;

MainFrame::~MainFrame() {
  Console_End();
  sInstance = 0;
  mConsole->Destroy();

  delete mConsole;

  // should this be done? boost documentation says no, but doing this means less
  // memleaks
  // Py_Finalize();
}

MainFrame::MainFrame(const wxString& title, const wxPoint& pos,
                     const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size) {
  CreateStatusBar();
  SetStatusText(_("Welcome to SiMo!"));

  assert(sInstance == 0);
  sInstance = this;

  // PyImport_AppendInittab("simocore", PyInit_simocore);
  // PyImport_AppendInittab("emb", emb::PyInit_emb);
  // Py_Initialize();
  // PyImport_ImportModule("emb");

  Console_Begin();

  const wxString file =
      wxStandardPaths::Get().GetResourcesDir() + "/gui/default.info";
  mConsole = new ConsoleDlg(0);
  // mScripts = new ScriptLibrary(mConsole);

  // mScripts->init();

  /*if (mScripts->hasErrors()) {
    wxMessageBox("Failed to compile all scripts", "SiMo error",
                 wxOK | wxICON_ERROR, this);
    Close();
  }*/

  if (false == loadGui(file.c_str().AsChar())) {
    wxMessageBox(
        "Failed to load gui from " + std::string(file.c_str().AsChar()),
        "SiMo error", wxOK | wxICON_ERROR, this);
    Close();
    return;
  }

  mView = new View(this, &mData, wxID_ANY, wxDefaultPosition, wxDefaultSize);

  Connect(wxEVT_ACTIVATE, wxActivateEventHandler(MainFrame::OnActivated));
}

void MainFrame::addLog(const wxString& str) {
  mConsole->addLog(str);
  mConsole->addLog("\n");
}

void MainFrame::ShowHideConsole() { mConsole->Show(); }

class IdGenerator {
 public:
  IdGenerator() : mId(1) {}

  int generate() {
    int temp = id();
    next();
    return temp;
  }

 protected:
  void next() { ++mId; }

  int id() const { return mId; }

 private:
  int mId;
};

wxString LoadProtoJson(google::protobuf::Message* message,
                       const wxString& path) {
  std::string err;
  int load_result = pbjson::json2pb_file(path.c_str().AsChar(), message, err);
  if (load_result < 0) {
    return err.c_str();
  }

  return "";
}

wxString SaveProtoJson(const google::protobuf::Message& t,
                       const wxString& path) {
  bool write_result = pbjson::pb2json_file(&t, path.c_str().AsChar(), true);
  if (write_result == false) {
    return "Unable to write to file";
  }

  return "";
}

wxMenu* loadMenu(MainFrame* mf, const simo::Menu& menuEl, IdGenerator* idg,
                 std::string* title) {
  wxMenu* menu = new wxMenu;

  *title = menuEl.title();

  for (const simo::MenuItem& itemEl : menuEl.items()) {
    if (itemEl.has_button()) {
      auto& button = itemEl.button();
      std::string display = button.display();

      std::string shortcut = button.shortcut();
      if (shortcut.empty() == false) {
        display += "\t" + shortcut;
      }

      std::string cmd = button.cmd();

      int id = idg->generate();

      mf->Connect(id, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction)&MainFrame::RunCommand);
      mf->addResponse(id, cmd);
      menu->Append(id, display);
    } else if (itemEl.has_menu()) {
      std::string display;
      wxMenu* sub = loadMenu(mf, itemEl.menu(), idg, &display);
      if (display.empty()) {
        mf->addLog("missing title in submenu");
        display = "EMPTY";
      }
      menu->Append(0, display, sub);
    } else if (itemEl.has_seperator()) {
      menu->Append(wxID_SEPARATOR, "-", "", wxITEM_SEPARATOR);
    } else {
      mf->addLog("Unrecognized type");
    }
  }

  return menu;
}

bool MainFrame::loadGui(const std::string& file) {
  IdGenerator idg;

  simo::Gui gui;
  wxString r = LoadProtoJson(&gui, file.c_str());
  if (r.IsEmpty() == false) {
    addLog(r);
    return false;
  }

  wxMenuBar* menuBar = nullptr;

  for (const simo::Menu& menu : gui.menu()) {
    if (menuBar == nullptr) menuBar = new wxMenuBar;
    std::string name = "No Title";
    wxMenu* m = loadMenu(this, menu, &idg, &name);
    menuBar->Append(m, name);
  }

  if (menuBar != nullptr) {
    SetMenuBar(menuBar);
  }

  return true;
}

void MainFrame::addResponse(int id, const std::string& cmd) {
  mCommands.insert(std::make_pair(id, cmd));
}

std::string MainFrame::getResponse(int id) const {
  IdCmdMap::const_iterator res = mCommands.find(id);
  if (res == mCommands.end())
    return "";
  else
    return res->second;
}

void MainFrame::RunCommand(wxCommandEvent& ev) {
  const std::string res = getResponse(ev.GetId());
  bool ok = mConsole->run(res);
  if (false == ok) {
    wxMessageBox("Failed to sucessfully run command, see console for details",
                 "SiMo error", wxOK | wxICON_ERROR, this);
    // mScripts->clearErrors();
  }
  mView->Invalidate();
}

void MainFrame::OnActivated(wxActivateEvent& evt) {
  // check if it lost focus
  if (evt.GetActive() == false) return;
  evt.Skip();
  /*
  mScripts->reload();
  if (mScripts->hasErrors()) {
    mScripts->clearErrors();
    wxMessageBox("Failed to reload scripts, see console for details",
                 "SiMo error", wxOK | wxICON_ERROR, this);
  }
  */
  // addLog("app got focus");
}