#include <cstdio>
int main() { FILE* f = fopen("/tmp/x.txt", "w"); if (f) { fputs("x", f); fclose(f); } return 0; }
