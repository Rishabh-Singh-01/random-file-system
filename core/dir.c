#include "dir.h"
#include "./../utils/logger.h"
#include "data.h"
#include "disk.h"
#include "inode.h"
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "superblock.h"
void InitializeRootDirectory(void *disk) {
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

  uint32_t rootDirIdx = superBlock->InodeTableStartPtr;
  Inode *rootDirInode = disk + (rootDirIdx * BLOCK_SIZE_BYTES);
  rootDirInode->Mode = MODE_DIRECTORY;
  rootDirInode->Uid = getuid();
  rootDirInode->Size = 0;
  uint32_t currTimestamp = (uint32_t)time(NULL);
  rootDirInode->Time = currTimestamp;
  rootDirInode->CTime = currTimestamp;
  rootDirInode->MTime = currTimestamp;
  rootDirInode->DTime = currTimestamp;
  rootDirInode->Gid = getegid();
  rootDirInode->Links = 0;
  rootDirInode->Flags = FLAGS_IMMUTABLE;
  rootDirInode->Blocks = 1;
  rootDirInode->Osd1 = OSD_GENERIC;
  rootDirInode->InodeBlock.DirectPtr[0] = superBlock->DataRegionStartPtr;
  rootDirInode->File = -1;
  rootDirInode->Dir = superBlock->DataRegionStartPtr;
  rootDirInode->Generation = GENERATION_GENERIC;

  uint32_t rootDirDataRegionIdx = rootDirInode->Dir;
  DirectoryDataItem *data = disk + (rootDirDataRegionIdx * BLOCK_SIZE_BYTES);
  data->INum = rootDirIdx;
  data->RecLen = 255;
  data->StrLen = 2;
  data->Str[0] = '.';
  data->Str[1] = '\n';
}

void DumpRootDirectory(void *disk) {
  SuperBlock *superBlock = (SuperBlock *)disk;
  uint32_t rootDirIdx = superBlock->DataBitmapStartPtr + 1;
  Inode *rootDirInode = disk + (rootDirIdx * BLOCK_SIZE_BYTES);
  LOG_INFO("---------- DUMPING META INFO FROM ROOT DIR STARTED ------------");
  LOG_INFO("Disk: Pointer:                        %p", disk);
  LOG_INFO("Root Dir: Pointer:                    %p", rootDirInode);
  LOG_INFO("Root Dir: Mode:                       %u", rootDirInode->Mode);
  LOG_INFO("Root Dir: Uid:                        %u", rootDirInode->Uid);
  LOG_INFO("Root Dir: Size:                       %u", rootDirInode->Size);
  LOG_INFO("Root Dir: Time:                       %u", rootDirInode->Time);
  LOG_INFO("Root Dir: CTime:                      %u", rootDirInode->CTime);
  LOG_INFO("Root Dir: MTime:                      %u", rootDirInode->MTime);
  LOG_INFO("Root Dir: DTime:                      %u", rootDirInode->DTime);
  LOG_INFO("Root Dir: Gid:                        %u", rootDirInode->Gid);
  LOG_INFO("Root Dir: Links:                      %u", rootDirInode->Links);
  LOG_INFO("Root Dir: Flags:                      %u", rootDirInode->Flags);
  LOG_INFO("Root Dir: Blocks:                     %u", rootDirInode->Blocks);
  LOG_INFO("Root Dir: OSD1:                       %u", rootDirInode->Osd1);
  LOG_INFO("Root Dir: File:                       %u", rootDirInode->File);
  LOG_INFO("Root Dir: Blocks:                     %u", rootDirInode->Blocks);
  LOG_INFO("Root Dir: Dir:                        %u", rootDirInode->Dir);
  LOG_INFO("Root Dir: Generation:                 %u",
           rootDirInode->Generation);
  DirectoryDataItem *dataItem =
      disk + (rootDirInode->InodeBlock.DirectPtr[0] * BLOCK_SIZE_BYTES);
  LOG_INFO("Root Dir: Inode Block 0: INode Num:   %u", dataItem->INum);
  LOG_INFO("Root Dir: Inode Block 0: Rec Leng:    %u", dataItem->RecLen);
  LOG_INFO("Root Dir: Inode Block 0: Str Leng:    %u", dataItem->StrLen);
  LOG_INFO("Root Dir: Inode Block 0: Str Name:    %s", &((dataItem->Str)[0]));

  LOG_INFO("---------- DUMPING META INFO FROM ROOT DIR ENDED ------------");
}
