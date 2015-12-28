#include "script.h"

Script::Script() : state_(true) {}

bool Script::RunCommand(const std::string& cmd) { return false; }

bool Script::RunFile(const std::string& file) { return false; }
