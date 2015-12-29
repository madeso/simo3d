#ifndef SIMO_FUNCTIONS_HPP
#define SIMO_FUNCTIONS_HPP

#include <string>

class Script;

void LoadFunctions(Script* script);

void AddLog(const std::string& str);
void AddLogWithoutEndline(const std::string& str);

#endif
