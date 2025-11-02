#include "superblock.h"
#include "data.h"
#include "disk.h"
#include "inode.h"
#include <stdint.h>

SuperBlock *InitializeSuperBlock(void *disk) {
  SuperBlock *sb = (SuperBlock *)disk;
  sb->MagicId = 0x00000;
  sb->BlocksCount = BLOCKS_COUNT;
  sb->InodesCount = INODE_COUNT;
  sb->FreeBlocksCount = BLOCKS_COUNT - PREDEFINED_BLOCKS_COUNT;
  sb->FreeInodesCount = INODE_COUNT;
  sb->InodeTableStartPtr = 3;
  sb->DataRegionStartPtr = 8;
  sb->InodeBitmapStartPtr = 1;
  sb->DataBitmapStartPtr = 2;
  return sb;
}

SuperBlock *FindSuperBlock(void *disk) { return (SuperBlock *)disk; }

SuperBlock *UpdateSuperBlockInodeOnly(void *disk, uint32_t iBlockIdx,
                                      uint32_t dataBlockIdx) {
  SuperBlock *superBlock = (SuperBlock *)disk;

  superBlock->FreeInodesCount--;
  InodeBitMap *inodeTablePtr = FindInodeBitMap(disk);
  uint64_t inodeMap = inodeTablePtr->Map;
  uint64_t inodeMapMask = 1l << (63 - iBlockIdx);
  inodeTablePtr->Map = inodeMap ^ inodeMapMask;

  return superBlock;
}

SuperBlock *UpdateSuperBlockDataOnly(void *disk, uint32_t dataBlockIdx) {
  SuperBlock *superBlock = (SuperBlock *)disk;

  superBlock->FreeBlocksCount--;
  // TODO: chek wtf to do here
  DataBitMap *dataTablePtr =
      disk + (superBlock->DataBitmapStartPtr * BLOCK_SIZE_BYTES);
  uint64_t dataMap = dataTablePtr->Map;
  uint64_t dataMapMask = 1l << (63 - (dataBlockIdx - 8));
  dataTablePtr->Map = dataMap ^ dataMapMask;

  return superBlock;
}
