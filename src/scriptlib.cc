#include "scriptlib.h"

#include "script.h"

#include <wx/stdpaths.h>
#include <wx/dir.h>

#include <wx/filesys.h>

namespace {
wxDateTime GetModificationDateFor(const wxString& file) {
  wxFileSystem fs;
  wxFSFile* f = fs.OpenFile(file);
  if (f) {
    const wxDateTime d = f->GetModificationTime();
    delete f;
    return d;
  } else
    return wxDateTime::Now();
}
}

Paths::Paths() {
  directories.push_back(wxGetCwd());
  directories.push_back(wxStandardPaths::Get().GetUserDataDir());
  directories.push_back(wxStandardPaths::Get().GetResourcesDir());
}

std::vector<wxString> Paths::allFiles(const wxString& pattern) const {
  std::vector<wxString> r;

  for(const wxString& d: directories) {
    wxArrayString files;
    wxDir::GetAllFiles(d, &files, pattern);
    for (const wxString &file : files) {
      r.push_back(file);
    }
  }

  return r;
}

wxString Paths::find(const wxString& path) const {
  for(const wxString& d : directories) {
    const wxString cwd = d + path;
    if( wxFileExists(cwd) ) return cwd;
  }

  // couldnt find file, just return the default
  return path;
}

ScriptLib::ScriptLib(Script* script, Paths* paths) : script_(*script), paths_(*paths) {
}

bool ScriptLib::reload() {
  StringTimeMap loaded;
  bool loadStatus = true;
  const auto& files = paths_.allFiles("*.lua");
  for(const wxString& file: files) {
    const wxDateTime m = GetModificationDateFor(file);
    auto found = loadedFiles_.find(file);

    if( found != loadedFiles_.end() ) {
      if( found->second >= m ) {
        loaded.insert(StringTimePair(file, found->second));
        continue;
      }
    }

    if( script_.RunFile(file.c_str().AsChar()) ) {
      loaded.insert(StringTimePair(file, m));
    }
    else {
      loadStatus = false;
    }
  }
  loadedFiles_ = loaded;
  return loadStatus && !loadedFiles_.empty();
}
