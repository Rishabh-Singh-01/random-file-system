#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

#include "core/dir.h"
#include "core/disk.c"

int main() {
  void *disk = InitializeDisk();
  DumpDiskMetaData(disk);
  DumpRootDirectory(disk);
  // Create a new sub dir in Root Dir (done)
  ListDirectory("/");
  MakeDirectory("/hello");
  ListDirectory("/");
  ListDirectory("/hello");
  // Read all the dir in Root Dir (done)
  // Create a new file in Root Dir
  // Create a new file in Sub Dir
  return 0;
}
