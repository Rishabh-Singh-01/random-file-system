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
  uint32_t numBits = sizeof(uint64_t) * 8; // numBits = 64

  for (uint32_t i = 0; i < numBits; ++i) {
    // Create a 64-bit mask
    uint64_t mask = 1ULL << (numBits - 1 - i);
    // Check if the bit at the current position is 0
    if ((num & mask) == 0) {
      // The bit at position 63 is 1
      // The bits at positions 62 down to 0 are all 0
      return i; // i = 1 (position of the first 0, from the MSB)
    }
  }

  assert(1 == 2 && "Unable to find the first free inode idx");
  return 0;
}

void UpdateInodeBitMapNthNodeToOccupied(void *disk, uint32_t bitToSetFromMSB) {
  // Calculate the 0-indexed bit position from the LSB
  // assuming n=0 is the MSB, n=1 is the next, etc.
  unsigned int bit_position = 63 - bitToSetFromMSB;

  // Create a mask with only the target bit set to 1
  uint64_t mask = 1ULL << bit_position;

  // Set the nth MSB to 1 using a bitwise OR operation
  InodeBitMap *inodeBitMap = FindInodeBitMap(disk);
  uint64_t num = inodeBitMap->Map;
  num |= mask;
}
