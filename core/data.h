#ifndef DATA_H
#define DATA_H

#include "disk.h"
#include "inode.h"
#include <stdint.h>
#define DATA_BLOCKS_COUNT BLOCKS_COUNT - PREDEFINED_BLOCKS_COUNT
struct DataBitMap {
  uint32_t Count;
  uint64_t Map;
};

typedef struct DataBitMap DataBitMap;

struct DirectoryDataItem {
  uint8_t INum;
  uint8_t RecLen;
  uint8_t StrLen;
  char Str[255];
};

typedef struct DirectoryDataItem DirectoryDataItem;

uint32_t WriteNewDirectoryDataItem(Inode *curDir, uint32_t inodeBlockIdx,
                                   const char *path);
DirectoryDataItem *FindNthDataRegion(uint32_t dataRegionIdx);
void WriteCurrentDirectoryItemData(uint32_t dataRegionIdx, uint32_t iNodeIdx);
DataBitMap *FindDataBitMap();
uint32_t FindFirstFreeDataIdx(DataBitMap *dBitMap);
void ReadDirectoryDataItem(Inode *curDir);
uint32_t UpdateDirectoryDataItem(Inode *curDir, uint32_t curDataIdx,
                                 uint32_t inodeBlockIdx, char *path);
void UpdateDataBitMapNthNodeToOccupied(uint32_t bitToSetFromMSB);

#endif // !DATA_H
