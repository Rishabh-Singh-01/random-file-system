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
  return 0;
}
