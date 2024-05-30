#include <stdlib.h>
#include <stdint.h>

union reg {
  int64_t x;
  int32_t w;
} r[31], zr, pc, sp;

enum {
  N = -1, Z = 0, C = 1, V = 2
} pstate = Z;

int main(int argc, char **argv) {
  return EXIT_SUCCESS;
}
