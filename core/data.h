#ifndef DATA_H
#define DATA_H

#include "disk.h"
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

#endif // !DATA_H
