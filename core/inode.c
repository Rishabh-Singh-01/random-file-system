#include "inode.h"
#include "common.h"
#include "disk.h"
#include "stdlib.h"
#include "superblock.h"
#include <assert.h>
#include <limits.h>
#include <stdint.h>

InodeBitMap *InitializeInodeBitMap() {
  SuperBlock *superBlock = (SuperBlock *)DiskPtr;
  uint32_t iBitMapIdx = superBlock->InodeBitmapStartPtr;
  InodeBitMap *iBitMap = DiskPtr + (iBitMapIdx * BLOCK_SIZE_BYTES);
  iBitMap->Count = INODE_COUNT;
  iBitMap->Map = 0;
  return iBitMap;
}

uint32_t FindInodeBlockIdx(Inode *inode) {
  Inode *rootDirInode = FindFirstInode();
  return inode - rootDirInode;
}

Inode *FindFirstInode() {
  SuperBlock *superBlock = (SuperBlock *)DiskPtr;
  return DiskPtr + (superBlock->InodeTableStartPtr * BLOCK_SIZE_BYTES);
}

Inode *FindNthInode(uint32_t iBlockIdx) {
  SuperBlock *superBlock = (SuperBlock *)DiskPtr;
  Inode *rootDirInode =
      DiskPtr + (superBlock->InodeTableStartPtr * BLOCK_SIZE_BYTES);
  return rootDirInode + iBlockIdx;
}

InodeBitMap *FindInodeBitMap() {
  SuperBlock *superBlock = (SuperBlock *)DiskPtr;
  uint32_t iBitMapIdx = superBlock->InodeBitmapStartPtr;
  InodeBitMap *iBitMap = DiskPtr + (iBitMapIdx * BLOCK_SIZE_BYTES);
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
