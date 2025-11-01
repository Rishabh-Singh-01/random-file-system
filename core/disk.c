#include "disk.h"
#include "stdlib.h"
#include <assert.h>
#include <stdint.h>

#include "./../utils/logger.h"
#include "data.h"
#include "dir.c"
#include "inode.c"
#include "superblock.c"
#include <inttypes.h>
#include <stdio.h>

DataBitMap *initializeDiskBitMap(void *disk) {
  SuperBlock *superBlock = (SuperBlock *)disk;
  uint32_t dBitMapIdx = superBlock->DataBitmapStartPtr;
  DataBitMap *dBitMap = disk + (dBitMapIdx * BLOCK_SIZE_BYTES);
  dBitMap->Count = DATA_BLOCKS_COUNT;
  dBitMap->Map = 0;
  return dBitMap;
}

void *InitializeDisk() {
  void *disk = malloc(DISK_SIZE_BYTES);
  InitializeSuperBlock(disk);
  InitializeInodeBitMap(disk);
  initializeDiskBitMap(disk);
  InitializeRootDirectory(disk);
  return disk;
}

void DumpDiskMetaData(void *disk) {
  LOG_INFO("---------- DUMPING META INFO FROM DISK STARTED ------------");
  LOG_INFO("Disk: Pointer:                    %p", disk);
  LOG_INFO("----- SUPER BLOCK -----");
  SuperBlock *superBlock = (SuperBlock *)disk;
  LOG_INFO("SuperBlock: Pointer:              %p", superBlock);
  LOG_INFO("SuperBlock: Magic ID:             %u (0x%X)", superBlock->MagicId,
           superBlock->MagicId);
  LOG_INFO("SuperBlock: Blocks Count:         %u", superBlock->BlocksCount);
  LOG_INFO("SuperBlock: Block Size (bytes):   %u", superBlock->BlockSize);
  LOG_INFO("SuperBlock: Inodes Count:         %u", superBlock->InodesCount);
  LOG_INFO("SuperBlock: Free Blocks Count:    %u", superBlock->FreeBlocksCount);
  LOG_INFO("SuperBlock: Free Inodes Count:    %u", superBlock->FreeInodesCount);
  LOG_INFO("SuperBlock: Inode Table Start:    %u",
           superBlock->InodeTableStartPtr);
  LOG_INFO("SuperBlock: Inode Bitmap Start:   %u",
           superBlock->InodeBitmapStartPtr);
  LOG_INFO("SuperBlock: Data Bitmap Start:    %u",
           superBlock->DataBitmapStartPtr);

  LOG_INFO("----- INODE BIT MAP -----");
  uint32_t iBitMapIdx = superBlock->InodeBitmapStartPtr;
  InodeBitMap *iBitMap = disk + (iBitMapIdx * BLOCK_SIZE_BYTES);
  LOG_INFO("Inode BitMap: Pointer:            %p", iBitMap);
  LOG_INFO("Inode BitMap: Blocks Count:       %u", iBitMap->Count);
  LOG_INFO("Inode BitMap: Block Size (bytes): %" PRIu64, iBitMap->Map);

  LOG_INFO("----- DATA BIT MAP -----");
  uint32_t dBitMapIdx = superBlock->DataBitmapStartPtr;
  DataBitMap *dBitMap = disk + (dBitMapIdx * BLOCK_SIZE_BYTES);
  LOG_INFO("Data BitMap: Pointer:             %p", dBitMap);
  LOG_INFO("Data BitMap: Blocks Count:        %u", dBitMap->Count);
  LOG_INFO("Data BitMap: Block Size (bytes):  %" PRIu64, dBitMap->Map);
  LOG_INFO("---------- DUMPING META INFO FROM DISK ENDED ------------");
}
