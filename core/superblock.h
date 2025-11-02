#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

#include <stdint.h>
struct SuperBlock {
  uint32_t MagicId;
  uint32_t BlocksCount;
  uint32_t BlockSize;
  uint32_t InodesCount;
  uint32_t FreeBlocksCount;
  uint32_t FreeInodesCount;
  uint32_t InodeTableStartPtr;
  uint32_t DataRegionStartPtr;
  uint32_t InodeBitmapStartPtr;
  uint32_t DataBitmapStartPtr;
};

typedef struct SuperBlock SuperBlock;

SuperBlock *FindSuperBlock();
SuperBlock *InitializeSuperBlock();
SuperBlock *UpdateSuperBlockInodeOnly(uint32_t iBlockIdx,
                                      uint32_t dataBlockIdx);
SuperBlock *UpdateSuperBlockDataOnly(uint32_t dataBlockIdx);

#endif // !SUPERBLOCK_H
