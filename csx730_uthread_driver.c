#include "csx730_uthread.h"

int main() {
  // just some assembly code; feel free to get rid of it
  __asm__ ("mov $0, %%rax;"
	   "inc %%rax;"
	   "xor %%rax, %%rax;"
	   "leave;"
	   "ret;"
	   :
	   :
	   : "rax");
} // main
