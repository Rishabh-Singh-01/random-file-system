#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

#include "core/dir.h"
#include "core/disk.c"
#include "core/file.c"
#include "core/file.h"

int main() {
  InitializeDisk();
  MakeDirectory("/hello1");
  MakeDirectory("/hello2");
  MakeDirectory("/hello2/hello3");
  MakeDirectory("/hello2/hello4");
  RemoveDirectory("/hello2/hello3");
  CreateFile("/hello2/yo.txt");
  CreateFile("/hello2/yo2.txt");
  RemoveFile("/hello2/yo2.txt");
  ListDirectory("/hello2");
  return 0;
}
