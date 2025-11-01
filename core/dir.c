#include <limits.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "./../utils/logger.h"
#include "data.c"
#include "dir.h"
#include "disk.h"
#include "inode.h"
#include "superblock.h"

void InitializeRootDirectory(void *disk) {
  SuperBlock *superBlock = UpdateSuperBlock(disk);

  uint32_t rootDirIdx = superBlock->InodeTableStartPtr;
  uint32_t dataRegionIdx = superBlock->DataRegionStartPtr;
  Inode *rootDirInode = createDefaultDirectory(disk, rootDirIdx, dataRegionIdx);

  uint32_t rootDirDataRegionIdx = rootDirInode->Dir;
  WriteCurrentDirectoryItemData(disk, rootDirDataRegionIdx, rootDirIdx);
}

Inode *createDefaultDirectory(void *disk, uint32_t dirIdx,
                              uint32_t dataRegionIdx) {
  SuperBlock *superBlock = (SuperBlock *)disk;
  Inode *dirInode = disk + (dirIdx * BLOCK_SIZE_BYTES);
  dirInode->Mode = MODE_DIRECTORY;
  dirInode->Uid = getuid();
  dirInode->Size = 0;
  uint32_t currTimestamp = (uint32_t)time(NULL);
  dirInode->Time = currTimestamp;
  dirInode->CTime = currTimestamp;
  dirInode->MTime = currTimestamp;
  dirInode->DTime = currTimestamp;
  dirInode->Gid = getegid();
  dirInode->Links = 0;
  dirInode->Flags = FLAGS_ALL;
  dirInode->Blocks = 1;
  dirInode->Osd1 = OSD_GENERIC;
  dirInode->InodeBlock.DirectPtr[0] = dataRegionIdx;
  dirInode->File = 0;
  dirInode->Dir = dataRegionIdx;
  dirInode->Generation = GENERATION_GENERIC;
  return dirInode;
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
