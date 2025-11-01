#include "data.h"
#include "./../utils/logger.h"
#include "dir.h"
#include "inode.h"
#include "superblock.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

void ReadDirectoryDataItem(void *disk, Inode *curDir) {
  uint8_t i = 0;
  while (i < MAX_DIRECT_DATA_REGION_LINK_COUNT &&
         curDir->InodeBlock.DirectPtr[i] != 0) {
    uint32_t dataRegionIdx = curDir->InodeBlock.DirectPtr[i];
    DirectoryDataItem *item = FindNthDataRegion(disk, dataRegionIdx);
    i++;
    LOG_INFO(
        "Parent Inode Idx: %d\n, Child Data Region Idx: %d\n, Child Inode Idx: "
        "%d\n, Child Rec Len: %d\n, Child Str Len: %d\n, Child Str Name: %s",
        FindInodeBlockIdx(disk, curDir), dataRegionIdx, item->INum,
        item->RecLen, item->StrLen, item->Str);
  }
}

uint32_t WriteNewDirectoryDataItem(void *disk, Inode *curDir,
                                   uint32_t inodeBlockIdx, const char *path) {
  uint8_t i = 0;
  while (i < MAX_DIRECT_DATA_REGION_LINK_COUNT &&
         curDir->InodeBlock.DirectPtr[i] != 0) {
    i++;
  }
  LOG_DEBUG("something");
  LOG_DEBUG("something curDir %d", FindInodeBlockIdx(disk, curDir));
  uint32_t dataIdx = FindFirstFreeDataIdx(FindDataBitMap(disk));
  LOG_DEBUG("something again free %d", dataIdx);
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

uint32_t UpdateDirectoryDataItem(void *disk, Inode *curDir, uint32_t curDataIdx,
                                 uint32_t inodeBlockIdx, char *path) {
  uint8_t i = 0;
  while (i < MAX_DIRECT_DATA_REGION_LINK_COUNT &&
         curDir->InodeBlock.DirectPtr[i] != 0) {
    i++;
  }
  assert(i > 0 && "During update i should be greater than 0");
  LOG_DEBUG("something curDir %d", FindInodeBlockIdx(disk, curDir));
  curDir->InodeBlock.DirectPtr[i] = curDataIdx;
  DirectoryDataItem *data =
      FindNthDataRegion(disk, curDir->InodeBlock.DirectPtr[i]);
  data->INum = inodeBlockIdx;
  data->RecLen = PATH_NAME_MAX_LENGTH;
  data->StrLen = strlen(path);
  strcpy(data->Str, path);
  LOG_DEBUG("something again free %d", curDataIdx);
  return curDataIdx;
}

void WriteCurrentDirectoryItemData(void *disk, uint32_t dataRegionIdx,
                                   uint32_t iNodeIdx) {
  DirectoryDataItem *data = disk + (dataRegionIdx * BLOCK_SIZE_BYTES);
  data->INum = iNodeIdx;
  data->RecLen = 255;
  data->StrLen = 2;
  data->Str[0] = '.';
  data->Str[1] = '\n';
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
  uint32_t numBits = sizeof(uint64_t) * 8; // numBits = 64

  for (uint32_t i = 0; i < numBits; ++i) {
    // Create a 64-bit mask
    uint64_t mask = 1ULL << (numBits - 1 - i);
    // Check if the bit at the current position is 0
    if ((num & mask) == 0) {
      // The bit at position 63 is 1
      // The bits at positions 62 down to 0 are all 0
      return i + 8; // i = 1 (position of the first 0, from the MSB)
    }
  }
  assert(1 == 2 && "Unable to find the first free data idx");
  return 0;
}

void UpdateDataBitMapNthNodeToOccupied(void *disk, uint32_t bitToSetFromMSB) {
  // Calculate the 0-indexed bit position from the LSB
  // assuming n=0 is the MSB, n=1 is the next, etc.
  unsigned int bit_position = 63 - bitToSetFromMSB - 8;

  // Create a mask with only the target bit set to 1
  uint64_t mask = 1ULL << bit_position;

  // Set the nth MSB to 1 using a bitwise OR operation
  DataBitMap *dataBitMap = FindDataBitMap(disk);
  uint64_t num = dataBitMap->Map;
  num |= mask;
}
