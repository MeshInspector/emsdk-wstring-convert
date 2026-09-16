// Compiled with -fexceptions, linked without: the object carries cleanup landing pads
// (and therefore invoke_* imports) although nothing throws or catches.
#include <string>
int main() {
  std::string a = std::to_string(42);
  std::string b = a + a;
  return b.size();
}
