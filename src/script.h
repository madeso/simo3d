#ifndef SIMO_SCRIPT_HPP
#define SIMO_SCRIPT_HPP

#include <chaiscript/chaiscript.hpp>

#include <string>
class MainFrame;

class Script {
 public:
  Script();

  bool RunCommand(const std::string& cmd);
  bool RunFile(const std::string& file);

  chaiscript::ChaiScript& chai();
  const chaiscript::ChaiScript& chai() const;

 private:
  chaiscript::ChaiScript chai_;
  MainFrame* main_;
};

#endif
