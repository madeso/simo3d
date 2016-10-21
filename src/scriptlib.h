#ifndef SIMO_SCRIPTLIB_HPP
#define SIMO_SCRIPTLIB_HPP

#include <vector>
#include <string>
#include <map>

#include "wx.h"

class Script;

class Paths {
 public:
  Paths();
  std::vector<wxString> allFiles(const wxString& pattern) const;
  wxString find(const wxString& relative) const;
 private:
  std::vector<wxString> directories;
};

typedef std::map <wxString, wxDateTime> StringTimeMap;
typedef std::pair<wxString, wxDateTime> StringTimePair;

class ScriptLib {
 public:
  ScriptLib(Script* script, Paths* paths);

  bool reload();
 private:
  Script& script_;
  Paths& paths_;

  StringTimeMap loadedFiles_;
};

#endif
