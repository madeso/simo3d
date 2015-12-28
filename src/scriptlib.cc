#include "scriptlib.h"

#include "script.h"

#include <wx/filesys.h>

namespace {
wxDateTime GetModificationDateFor(const std::string& file) {
  wxFileSystem fs;
  wxFSFile* f = fs.OpenFile(file.c_str());
  if (f) {
    const wxDateTime d = f->GetModificationTime();
    delete f;
    return d;
  } else
    return wxDateTime::Now();
}
}

//////////////////////////////////////////////////////////////////////////

ScriptEntry::ScriptEntry(const std::string& file, const wxDateTime& date)
    : file_(file), date_(date) {}

const std::string& ScriptEntry::file() const { return file_; }

const wxDateTime& ScriptEntry::date() const { return date_; }

void ScriptEntry::set_file(const std::string& f) { file_ = f; }

void ScriptEntry::set_date(const wxDateTime& d) { date_ = d; }

//////////////////////////////////////////////////////////////////////////

ScriptLib::ScriptLib(Script* script) : script_(script) {}

bool ScriptLib::load(const std::string& file) {
  if (compile(file)) {
    entries_.push_back(ScriptEntry(file, GetModificationDateFor(file)));
    return true;
  } else
    return false;
}

bool ScriptLib::reload() {
  bool ret = true;

  for (auto& entry : entries_) {
    const auto newdate = GetModificationDateFor(entry.file());
    if (entry.date() < newdate) {
      const bool compiled = compile(entry.file());
      if (compiled) {
        entry.set_date(newdate);
      } else {
        ret = false;
      }
    }
  }

  return ret;
}

bool ScriptLib::compile(const std::string& file) {
  return script_->RunFile(file);
}
