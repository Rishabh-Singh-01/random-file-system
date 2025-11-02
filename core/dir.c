#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "./../utils/logger.h"
#include "common.h"
#include "data.c"
#include "data.h"
#include "dir.h"
#include "disk.h"
#include "inode.h"
#include "superblock.h"

void *DiskPtr;

Inode *makeDirTravelMatchConditionCb(const char *splitedStr,
                                     uint32_t matchedInodeBlockIdx,
                                     Inode *dirInode) {
  if (splitedStr == NULL) {
    assert(matchedInodeBlockIdx == 0 && "New Directory to make already exists");
    return dirInode;
  }

  assert(matchedInodeBlockIdx != 0 && "Unable to find any matching path name");
  return FindNthInode(DiskPtr, matchedInodeBlockIdx);
}

Inode *listDirTravelMatchConditionCb(const char *splitedStr,
                                     uint32_t matchedInodeBlockIdx,
                                     Inode *dirInode) {
  assert(matchedInodeBlockIdx != 0 && "Required Directory does not exists");
  return FindNthInode(DiskPtr, matchedInodeBlockIdx);
}

uint32_t inodeDirectChildsContainsPathName(const char *path, Inode *dirNode,
                                           uint32_t dataRegionIdx) {
  DirectoryDataItem *item = FindNthDataRegion(DiskPtr, dataRegionIdx);
  if (strcmp(item->Str, path) == 0) {
    return item->INum;
  }
  return -1;
}

uint32_t findDirInodeWithValidName(const char *path, Inode *dirNode) {
  return ExecuteInt32CbOnInodeValidDirectPtrs(
      DiskPtr, dirNode, path, inodeDirectChildsContainsPathName);
}

void InitializeRootDirectory(void *disk) {
  DiskPtr = disk;
  CreateDefaultDirectory(disk);
}

void ListDirectory(const char *pathPtr) {
  AssertDirConfigs(DiskPtr, pathPtr);

  Inode *dirInode =
      TravelToDirFromPathName(DiskPtr, pathPtr, listDirTravelMatchConditionCb);

  ReadDirectoryDataItem(DiskPtr, dirInode);
}

void MakeDirectory(const char *pathPtr) {
  AssertDirConfigs(DiskPtr, pathPtr);

  Inode *dirInode =
      TravelToDirFromPathName(DiskPtr, pathPtr, makeDirTravelMatchConditionCb);
  const char *lastPartStr = PathNameEndPart(pathPtr);

  Inode *newDir = CreateDefaultDirectory(DiskPtr);
  LinkParentDirWithChildDir(newDir, dirInode, lastPartStr);
  LinkChildDirWithParentDir(newDir, dirInode);
}

void LinkParentDirWithChildDir(Inode *childDir, Inode *parentDir,
                               const char *path) {
  uint32_t childDirInodeBlockIdx = FindInodeBlockIdx(DiskPtr, childDir);
  WriteNewDirectoryDataItem(DiskPtr, parentDir, childDirInodeBlockIdx, path);
}

void LinkChildDirWithParentDir(Inode *childDir, Inode *parentDir) {
  uint32_t childDirInodeBlockIdx = FindInodeBlockIdx(DiskPtr, parentDir);
  WriteNewDirectoryDataItem(DiskPtr, childDir, childDirInodeBlockIdx, "..");
}

Inode *CreateDefaultDirectory(void *disk) {
  InodeBitMap *iBitMap = FindInodeBitMap(disk);
  uint32_t firstFreeInodeBlockIdx = FindFirstFreeInodeIdx(iBitMap);

  DataBitMap *dBitMap = FindDataBitMap(disk);
  uint32_t firstFreeDataIdx = FindFirstFreeDataIdx(dBitMap);
  Inode *curDir = FindNthInode(disk, firstFreeDataIdx);

  Inode *inode =
      createDirectoryInode(disk, firstFreeInodeBlockIdx, firstFreeDataIdx);
  uint32_t dataIdx =
      WriteNewDirectoryDataItem(disk, curDir, firstFreeInodeBlockIdx, ".");
  return inode;
}

Inode *createDirectoryInode(void *disk, uint32_t iBlockIdx,
                            uint32_t dataRegionIdx) {
  Inode *inode = FindFirstInode(disk);
  Inode *dirInode = inode + iBlockIdx;

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
  dirInode->Dir = 0;
  dirInode->Generation = GENERATION_GENERIC;

  UpdateSuperBlockInodeOnly(disk, iBlockIdx, dataRegionIdx);
  return dirInode;
}

void DumpAnyDirectory(Inode *rootDirInode) {
  LOG_INFO("---------- DUMPING META INFO FROM ANY DIR STARTED ------------");
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
      DiskPtr + (rootDirInode->InodeBlock.DirectPtr[0] * BLOCK_SIZE_BYTES);
  LOG_INFO("Root Dir: Inode Block 0: INode Num:   %u", dataItem->INum);
  LOG_INFO("Root Dir: Inode Block 0: Rec Leng:    %u", dataItem->RecLen);
  LOG_INFO("Root Dir: Inode Block 0: Str Leng:    %u", dataItem->StrLen);
  LOG_INFO("Root Dir: Inode Block 0: Str Name:    %s", &((dataItem->Str)[0]));
  LOG_INFO("---------- DUMPING META INFO FROM ROOT DIR ENDED ------------");
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
  LOG_INFO("---------- DUMPING META INFO FROM ANY DIR ENDED ------------");
}
