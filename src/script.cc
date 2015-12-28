#include "script.h"

Script::Script() : state_(true) {}

bool Script::RunCommand(const std::string& cmd) { return state_(cmd.c_str()); }

bool Script::RunFile(const std::string& file) { return false; }

sel::State& Script::state() { return state_; }

const sel::State& Script::state() const { return state_; }
