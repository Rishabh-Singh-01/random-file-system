#include "data.h"
#include "./../utils/logger.h"
#include "common.c"
#include "dir.h"
#include "inode.h"
#include "superblock.h"
#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>

void readDirDataItem(Inode *curDir, uint32_t dataRegionIdx) {
  DirectoryDataItem *item = FindNthDataRegion(dataRegionIdx);
  while (item->RecLen == PATH_NAME_MAX_LENGTH) {
    LOG_INFO(
        "Here is the info for data items: \n> Parent Inode Idx: %d,\n> Child "
        "Data Region Idx: %d,\n> Child Inode Idx: "
        "%d,\n> Child Rec Len: %d,\n> Child Str Len: %d,\n> Child Str Name: %s",
        FindInodeBlockIdx(curDir), dataRegionIdx, item->INum, item->RecLen,
        item->StrLen, item->Str);
    item++;
  }
}

void ReadDirectoryDataItem(Inode *curDir) {
  ExecuteCbOnInodeValidDirectPtrs(curDir, readDirDataItem);
}

uint32_t WriteNewDirectoryDataItem(Inode *curDir, uint32_t inodeBlockIdx,
                                   const char *path) {
  uint32_t dataIdx = curDir->InodeBlock.DirectPtr[0];
  DirectoryDataItem *data = FindNthDataRegion(dataIdx);
  while (data->RecLen == PATH_NAME_MAX_LENGTH) {
    data++;
  }
  data->INum = inodeBlockIdx;
  data->RecLen = PATH_NAME_MAX_LENGTH;
  data->StrLen = strlen(path);
  strcpy(data->Str, path);
  UpdateSuperBlockDataOnly(dataIdx);
  return dataIdx;
}

DirectoryDataItem *FindNthDataRegion(uint32_t dataRegionIdx) {
  assert(dataRegionIdx >= 8);
  return DiskPtr + (dataRegionIdx * BLOCK_SIZE_BYTES);
}

DataBitMap *FindDataBitMap() {
  SuperBlock *superBlock = (SuperBlock *)DiskPtr;
  uint32_t dBitMapIdx = superBlock->DataBitmapStartPtr;
  DataBitMap *dBitMap = DiskPtr + (dBitMapIdx * BLOCK_SIZE_BYTES);
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
