#include "superblock.h"
#include "data.h"
#include "disk.h"
#include "inode.h"

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

SuperBlock *UpdateSuperBlock(void *disk) {
  SuperBlock *superBlock = (SuperBlock *)disk;

  superBlock->FreeInodesCount--;
  superBlock->FreeBlocksCount--;
  InodeBitMap *inodeTablePtr =
      disk + (superBlock->InodeBitmapStartPtr * BLOCK_SIZE_BYTES);
  uint32_t inodeMap = inodeTablePtr->Map;
  uint32_t inodeMapMask = 1 << 31;
  inodeTablePtr->Map = inodeMap ^ inodeMapMask;
  DataBitMap *dataTablePtr =
      disk + (superBlock->DataBitmapStartPtr * BLOCK_SIZE_BYTES);
  uint32_t dataMap = dataTablePtr->Map;
  uint32_t dataMapMask = 1 << 31;
  dataTablePtr->Map = dataMap ^ dataMapMask;

  return superBlock;
}
