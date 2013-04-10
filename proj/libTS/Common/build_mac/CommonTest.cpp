#include <stdio.h>
#include <unistd.h>
#include "libTS/Common/Common.h"

#ifndef TS_PLATFORM_MAC
#error "undefined TS_PLATFORM_MAC"
#endif


int main(int ac, char* av[]) {

  printf("Hello. I am MAC!\n");
  return 0;
}
