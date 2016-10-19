#ifndef SIMO_SCRIPT_HPP
#define SIMO_SCRIPT_HPP

namespace sol {
  class state;
}

#include <string>
#include <memory>
class MainFrame;

struct ScriptImpl;

class Script {
 public:
  Script();
  ~Script();

  bool RunCommand(const std::string& cmd);
  bool RunFile(const std::string& file);

  sol::state& state();
  const sol::state& state() const;

 private:
  std::unique_ptr<ScriptImpl> impl;
  MainFrame* main_;
};

#endif
