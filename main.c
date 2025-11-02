#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

#include "core/dir.h"
#include "core/disk.c"

int main() {
  void *disk = InitializeDisk();
  MakeDirectory("/hello2");
  MakeDirectory("/hello3");
  ListDirectory("/hello3/./../hello2/../hello3/..");
  ListDirectory("/.");
  MakeDirectory("/hello3/hello2");
  MakeDirectory("/hello3/hello2/hello3");
  ListDirectory("/hello3/hello2/hello3");
  return 0;
}
