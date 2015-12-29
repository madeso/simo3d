#include "script.h"

#include "functions.h"

// capture print functions
#define fwrite(str, one, size, output) AddLogWithoutEndline(str)
#define puts(str) AddLog(str)

#include <chaiscript/chaiscript_stdlib.hpp>

Script::Script() : chai_(chaiscript::Std_Lib::library()) {}

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
    chai_.eval(cmd.c_str());
    return true;
  } catch (...) {
    AddErrorLog();
    return false;
  }
}

bool Script::RunFile(const std::string& file) {
  AddLog("Running file " + file);
  try {
    chai_.eval_file(file);
  } catch (...) {
    AddErrorLog();
    return false;
  }
  return true;
}

chaiscript::ChaiScript& Script::chai() { return chai_; }

const chaiscript::ChaiScript& Script::chai() const { return chai_; }
