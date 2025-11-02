#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

#include "core/dir.h"
#include "core/disk.c"
#include "core/file.c"
#include "core/file.h"

int main() {
  void *disk = InitializeDisk();
  MakeDirectory("/hello1");
  MakeDirectory("/hello2");
  MakeDirectory("/hello2/hello3");
  ListDirectory("/hello2/hello3/../.././hello2");
  CreateFile("/hello2/hello3/../.././hello2/yo.txt");
  WriteToFile("/hello2/yo.txt", "Hi there 2");
  ReadFile("/hello2/yo.txt");
  ListDirectory("/.");
  return 0;
}
