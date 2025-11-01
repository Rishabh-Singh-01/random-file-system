#include "data.h"
#include <stdint.h>

void WriteCurrentDirectoryItemData(void *disk, uint32_t dataRegionIdx,
                                   uint32_t iNodeIdx) {
  DirectoryDataItem *data = disk + (dataRegionIdx * BLOCK_SIZE_BYTES);
  data->INum = iNodeIdx;
  data->RecLen = 255;
  data->StrLen = 2;
  data->Str[0] = '.';
  data->Str[1] = '\n';
}
