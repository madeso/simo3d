#include "script.h"

#include "functions.h"

void ExceptionLog(int sd, std::string message, std::exception_ptr ex) {
  AddLog(message);
}

Script::Script() : state_(true) { state_.HandleExceptionsWith(ExceptionLog); }

bool Script::RunCommand(const std::string& cmd) {
  AddLog("> " + cmd);
  const bool ok = state_(cmd.c_str());

  if (false == ok) {
    AddLog("Failed.");
  }

  return ok;
}

bool Script::RunFile(const std::string& file) {
  AddLog("Running file " + file);
  const bool ok = state_.Load(file);
  if (false == ok) {
    AddLog("Failed.");
  }
  return ok;
}

sel::State& Script::state() { return state_; }

const sel::State& Script::state() const { return state_; }
