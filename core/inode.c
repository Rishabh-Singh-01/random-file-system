#include "inode.h"
#include "disk.h"
#include "stdlib.h"
#include "superblock.h"
#include <assert.h>
#include <limits.h>
#include <stdint.h>

InodeBitMap *InitializeInodeBitMap(void *disk) {
  SuperBlock *superBlock = (SuperBlock *)disk;
  uint32_t iBitMapIdx = superBlock->InodeBitmapStartPtr;
  InodeBitMap *iBitMap = disk + (iBitMapIdx * BLOCK_SIZE_BYTES);
  iBitMap->Count = INODE_COUNT;
  iBitMap->Map = 0;
  return iBitMap;
}

uint32_t FindInodeBlockIdx(void *disk, Inode *inode) {
  Inode *rootDirInode = FindFirstInode(disk);
  return inode - rootDirInode;
}

Inode *FindFirstInode(void *disk) {
  SuperBlock *superBlock = (SuperBlock *)disk;
  return disk + (superBlock->InodeTableStartPtr * BLOCK_SIZE_BYTES);
}

Inode *FindNthInode(void *disk, uint32_t iBlockIdx) {
  SuperBlock *superBlock = (SuperBlock *)disk;
  Inode *rootDirInode =
      disk + (superBlock->InodeTableStartPtr * BLOCK_SIZE_BYTES);
  return rootDirInode + iBlockIdx;
}

InodeBitMap *FindInodeBitMap(void *disk) {
  SuperBlock *superBlock = (SuperBlock *)disk;
  uint32_t iBitMapIdx = superBlock->InodeBitmapStartPtr;
  InodeBitMap *iBitMap = disk + (iBitMapIdx * BLOCK_SIZE_BYTES);
  return iBitMap;
}

uint32_t FindFirstFreeInodeIdx(InodeBitMap *iBitMap) {
  uint64_t num = iBitMap->Map;
  uint32_t numBits = sizeof(uint64_t) * 8;

  for (uint32_t i = 0; i < numBits; ++i) {
    uint64_t mask = 1ULL << (numBits - 1 - i);
    if ((num & mask) == 0) {
      return i;
    }
  }

  assert(1 == 2 && "Unable to find the first free inode idx");
  return 0;
}
