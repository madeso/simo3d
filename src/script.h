#ifndef SIMO_SCRIPT_HPP
#define SIMO_SCRIPT_HPP

#include "selene.h"

#include <string>

class Script {
 public:
  Script();

  bool RunCommand(const std::string& cmd);
  bool RunFile(const std::string& file);

  sel::State& state();
  const sel::State& state() const;

 private:
  sel::State state_;
};

#endif
