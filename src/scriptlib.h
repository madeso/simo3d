#ifndef SIMO_SCRIPTLIB_HPP
#define SIMO_SCRIPTLIB_HPP

#include <vector>
#include <string>

#include "wx.h"

class Script;

class ScriptEntry {
 public:
  ScriptEntry(const std::string& file, const wxDateTime& date);

  const std::string& file() const;
  const wxDateTime& date() const;

  void set_file(const std::string& f);
  void set_date(const wxDateTime& d);

 private:
  std::string file_;
  wxDateTime date_;
};

class ScriptLib {
 public:
  explicit ScriptLib(Script* script);

  bool load(const std::string& file);
  bool reload();

 private:
  bool compile(const std::string& file);
  std::vector<ScriptEntry> entries_;
  Script* script_;
};

#endif
