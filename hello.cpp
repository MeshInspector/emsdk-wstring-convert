#include <cstdio>
#include <string>
#include <emscripten/bind.h>
std::string greet(const std::string& s) {
  FILE* f = fopen("/tmp/x.txt", "w"); if (f) { fputs(s.c_str(), f); fclose(f); }
  return "hi " + s;
}
EMSCRIPTEN_BINDINGS(m) { emscripten::function("greet", &greet); }
