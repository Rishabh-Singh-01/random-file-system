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
  MakeDirectory("/hello2");
  CreateFile("/yo.txt");
  WriteToFile("/yo.txt", "Hi there");
  ListDirectory("/");
  ReadFile("/yo.txt");
  WriteToFile("/yo.txt", "Hi there 2");
  ReadFile("/yo.txt");
  /*MakeDirectory("/hello3");*/
  /*ListDirectory("/hello3/./../hello2/../hello3/..");*/
  /*ListDirectory("/.");*/
  /*MakeDirectory("/hello3/hello2");*/
  /*MakeDirectory("/hello3/hello2/hello3");*/
  /*ListDirectory("/hello3/hello2/hello3");*/
  /*CreateFile("/hello3/hello2/hello3/hellomoto.txt");*/

  // TODO: add file at root dir
  // Read dir content to ensure file exists
  // Also make sure cannot cd into file
  return 0;
}
