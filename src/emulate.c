#include <stdlib.h>

union reg {
  int64_t x;
  int32_t w;
} r[31], zr = {0}, pc, sp;

int64_t x(int id) {
  return r[id].x;
}

int32_t w(int id) {
  return r[id].w;
}

enum {
  N = -1, Z = 0, C = 1, V = 2
} pstate = Z;

int main(int argc, char **argv) {
  
  return EXIT_SUCCESS;
}
