#include "script.h"

#include "functions.h"

#include "sol.hpp"

struct ScriptImpl {
  ~ScriptImpl() {}

  sol::state state;
};

Script::Script() : impl(new ScriptImpl()) {
}

Script::~Script() {
}

void AddErrorLog() {
  try {
    throw;
  }
  /*
  catch (const chaiscript::exception::eval_error& eval) {
    eval.
  }*/
  catch (const std::runtime_error& err) {
    AddLog(err.what());
  }
}

bool Script::RunCommand(const std::string& cmd) {
  AddLog("> " + cmd);
  try {
    impl->state.script(cmd);
    return true;
  } catch (...) {
    AddErrorLog();
    return false;
  }
}

bool Script::RunFile(const std::string& file) {
  AddLog("Running file " + file);
  try {
    impl->state.script_file(file);
  } catch (...) {
    AddErrorLog();
    return false;
  }
  return true;
}

sol::state& Script::state() { return impl->state; }
const sol::state& Script::state() const { return impl->state; }

