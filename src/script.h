#ifndef SIMO_SCRIPT_HPP
#define SIMO_SCRIPT_HPP

#include "selene.h"

#include <string>

class Script {
 public:
  Script();
  bool RunCommand(const std::string& cmd);

 private:
  sel::State state_;
};

#endif
