#include "inode.h"
#include "disk.h"
#include "stdlib.h"
#include "superblock.h"
#include <stdint.h>

InodeBitMap *InitializeInodeBitMap(void *disk) {
  SuperBlock *superBlock = (SuperBlock *)disk;
  uint32_t iBitMapIdx = superBlock->InodeBitmapStartPtr;
  InodeBitMap *iBitMap = disk + (iBitMapIdx * BLOCK_SIZE_BYTES);
  iBitMap->Count = INODE_COUNT;
  iBitMap->Map = 0;
  return iBitMap;
}
