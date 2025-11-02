#include "data.h"
#include "./../utils/logger.h"
#include "common.c"
#include "dir.h"
#include "inode.h"
#include "superblock.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

void readDirDataItem(void *disk, Inode *curDir, uint32_t dataRegionIdx) {
  DirectoryDataItem *item = FindNthDataRegion(disk, dataRegionIdx);
  LOG_INFO(
      "Here is the info for data items: \n> Parent Inode Idx: %d,\n> Child "
      "Data Region Idx: %d,\n> Child Inode Idx: "
      "%d,\n> Child Rec Len: %d,\n> Child Str Len: %d,\n> Child Str Name: %s",
      FindInodeBlockIdx(disk, curDir), dataRegionIdx, item->INum, item->RecLen,
      item->StrLen, item->Str);
}

void ReadDirectoryDataItem(void *disk, Inode *curDir) {
  ExecuteCbOnInodeValidDirectPtrs(disk, curDir, readDirDataItem);
}

uint32_t WriteNewDirectoryDataItem(void *disk, Inode *curDir,
                                   uint32_t inodeBlockIdx, const char *path) {
  uint8_t i = 0;
  while (i < MAX_DIRECT_DATA_REGION_LINK_COUNT &&
         curDir->InodeBlock.DirectPtr[i] != 0) {
    i++;
  }
  uint32_t dataIdx = FindFirstFreeDataIdx(FindDataBitMap(disk));
  curDir->InodeBlock.DirectPtr[i] = dataIdx;
  DirectoryDataItem *data =
      FindNthDataRegion(disk, curDir->InodeBlock.DirectPtr[i]);
  data->INum = inodeBlockIdx;
  data->RecLen = PATH_NAME_MAX_LENGTH;
  data->StrLen = strlen(path);
  strcpy(data->Str, path);
  UpdateSuperBlockDataOnly(disk, dataIdx);
  return dataIdx;
}

DirectoryDataItem *FindNthDataRegion(void *disk, uint32_t dataRegionIdx) {
  assert(dataRegionIdx >= 8);
  SuperBlock *superBlock = (SuperBlock *)disk;
  return disk + (dataRegionIdx * BLOCK_SIZE_BYTES);
}

DataBitMap *FindDataBitMap(void *disk) {
  SuperBlock *superBlock = (SuperBlock *)disk;
  uint32_t dBitMapIdx = superBlock->DataBitmapStartPtr;
  DataBitMap *dBitMap = disk + (dBitMapIdx * BLOCK_SIZE_BYTES);
  return dBitMap;
}

uint32_t FindFirstFreeDataIdx(DataBitMap *dBitMap) {
  uint64_t num = dBitMap->Map;
  uint32_t numBits = sizeof(uint64_t) * 8;

  for (uint32_t i = 0; i < numBits; ++i) {
    uint64_t mask = 1ULL << (numBits - 1 - i);
    if ((num & mask) == 0) {
      return i + 8;
    }
  }

  assert(1 == 2 && "Unable to find the first free data idx");
  return 0;
}
