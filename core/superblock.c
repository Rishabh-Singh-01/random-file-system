#include "superblock.h"
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
