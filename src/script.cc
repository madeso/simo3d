#include "script.h"
#include <chaiscript/chaiscript_stdlib.hpp>

#include "functions.h"

void ExceptionLog(int sd, std::string message, std::exception_ptr ex) {
  AddLog(message);
}

Script::Script() : state_(true), chai_(chaiscript::Std_Lib::library()) {
  state_.HandleExceptionsWith(ExceptionLog);
}

bool Script::RunCommand(const std::string& cmd) {
  AddLog("> " + cmd);
  return state_(cmd.c_str());
}

bool Script::RunFile(const std::string& file) {
  AddLog("Running file " + file);
  return state_.Load(file);
}

sel::State& Script::state() { return state_; }

const sel::State& Script::state() const { return state_; }
