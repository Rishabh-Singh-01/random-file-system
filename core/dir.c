#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "./../utils/logger.h"
#include "data.c"
#include "data.h"
#include "dir.h"
#include "disk.h"
#include "inode.h"
#include "superblock.h"

void *DiskPtr;

uint32_t findDirInodeWithValidName(const char *path, Inode *dirNode) {
  size_t len = strlen(path);
  uint8_t i = 0;
  while (i < MAX_DIRECT_DATA_REGION_LINK_COUNT &&
         dirNode->InodeBlock.DirectPtr[i] != 0) {
    uint32_t dataRegionIdx = dirNode->InodeBlock.DirectPtr[i];
    DirectoryDataItem *item = FindNthDataRegion(DiskPtr, dataRegionIdx);
    i++;
    if (strcmp(item->Str, path) != 0) {
      continue;
    }
    return item->INum;
  }
  return 0;
}

void InitializeRootDirectory(void *disk) {
  DiskPtr = disk;
  CreateDefaultDirectory(disk);
}

void MakeDirectory(const char *pathPtr) {
  LOG_DEBUG("Mkae directory starting here");
  assert(strlen(pathPtr) <= PATH_NAME_MAX_LENGTH &&
         "Path name should be less than 255 chars");
  assert(DiskPtr != NULL && "Root Directory MetaData Not found");
  assert(*pathPtr == '/' &&
         "Directory Path Should always start with root directory");

  Inode *dirInode = FindFirstInode(DiskPtr);
  uint32_t matchedInodeBlockIdx = 0;

  char tempStr[PATH_NAME_MAX_LENGTH];
  strcpy(tempStr, pathPtr);
  const char delimiter[] = PATH_NAME_DELIMITER;
  char *splitedStr;
  char copiedStr[PATH_NAME_MAX_LENGTH];
  splitedStr = strtok(tempStr, delimiter);
  while (splitedStr != NULL) {
    matchedInodeBlockIdx = findDirInodeWithValidName(splitedStr, dirInode);
    strcpy(copiedStr, splitedStr);
    splitedStr = strtok(NULL, delimiter);
    if (splitedStr != NULL) {
      assert(matchedInodeBlockIdx != 0 &&
             "Unable to find any matching path name");
      dirInode = FindNthInode(DiskPtr, matchedInodeBlockIdx);
    } else {
      LOG_DEBUG("God knows Update: %d", FindInodeBlockIdx(DiskPtr, dirInode));
      assert(matchedInodeBlockIdx == 0 &&
             "New Directory to make already exists");
    }
  }
  Inode *newDir = CreateDefaultDirectory(DiskPtr);
  DumpAnyDirectory(newDir);
  LOG_DEBUG("--------------God knows Parent: %d",
            FindInodeBlockIdx(DiskPtr, dirInode));
  LOG_DEBUG("God knows: %d", FindInodeBlockIdx(DiskPtr, newDir));
  LinkParentDirWithChildDir(newDir, dirInode, copiedStr);
  LinkChildDirWithParentDir(newDir, dirInode);
  /*LinkDirectoryWithParentDir(newDir, dirInode);*/
}

void LinkParentDirWithChildDir(Inode *childDir, Inode *parentDir, char *path) {
  LOG_DEBUG("Child Dir Node: %d, Parent Dir Node %d",
            FindInodeBlockIdx(DiskPtr, childDir),
            FindInodeBlockIdx(DiskPtr, parentDir));
  uint32_t childDirInodeBlockIdx = FindInodeBlockIdx(DiskPtr, childDir);
  /*UpdateDirectoryDataItem(DiskPtr, parentDir,
   * childDir->InodeBlock.DirectPtr[0],*/
  /*                        childDirInodeBlockIdx, path);*/
  WriteNewDirectoryDataItem(DiskPtr, parentDir, childDirInodeBlockIdx, path);
}

void LinkChildDirWithParentDir(Inode *childDir, Inode *parentDir) {
  LOG_DEBUG("Child Dir Node: %d, Parent Dir Node %d",
            FindInodeBlockIdx(DiskPtr, childDir),
            FindInodeBlockIdx(DiskPtr, parentDir));
  uint32_t childDirInodeBlockIdx = FindInodeBlockIdx(DiskPtr, parentDir);
  WriteNewDirectoryDataItem(DiskPtr, childDir, childDirInodeBlockIdx, "..");
}

void LinkDirectoryWithParentDir(Inode *childDir, Inode *parentDir) {
  LOG_DEBUG("New Dir Node: %d, Parent Dir Node %d",
            FindInodeBlockIdx(DiskPtr, childDir),
            FindInodeBlockIdx(DiskPtr, parentDir));
  uint32_t parentDirInodeBlockIdx = FindInodeBlockIdx(DiskPtr, parentDir);
  WriteNewDirectoryDataItem(DiskPtr, childDir, parentDirInodeBlockIdx, "..");
}

Inode *CreateDefaultDirectory(void *disk) {
  InodeBitMap *iBitMap = FindInodeBitMap(disk);
  LOG_DEBUG("inode bit map %" PRIu64, iBitMap->Map);
  uint32_t firstFreeInodeBlockIdx = FindFirstFreeInodeIdx(iBitMap);
  LOG_DEBUG("This is the first free inode block %d", firstFreeInodeBlockIdx);

  DataBitMap *dBitMap = FindDataBitMap(disk);
  uint32_t firstFreeDataIdx = FindFirstFreeDataIdx(dBitMap);
  LOG_DEBUG("This is the first free data block %d", firstFreeDataIdx);
  Inode *curDir = FindNthInode(disk, firstFreeDataIdx);

  Inode *inode =
      createDirectoryInode(disk, firstFreeInodeBlockIdx, firstFreeDataIdx);
  LOG_DEBUG("FOund the new inode free: %d", FindInodeBlockIdx(DiskPtr, inode));
  uint32_t dataIdx =
      WriteNewDirectoryDataItem(disk, curDir, firstFreeInodeBlockIdx, ".");
  LOG_DEBUG("FOund the data index free: %d", dataIdx);

  LOG_DEBUG("Before update: inode bit map %" PRIu64,
            FindInodeBitMap(disk)->Map);
  LOG_DEBUG("Before update: data bit map %" PRIu64, FindDataBitMap(disk)->Map);
  SuperBlock *superBlock =
      UpdateSuperBlock(disk, firstFreeInodeBlockIdx, firstFreeDataIdx);
  LOG_DEBUG("After update: inode bit map %" PRIu64, FindInodeBitMap(disk)->Map);
  LOG_DEBUG("After update: data bit map %" PRIu64, FindDataBitMap(disk)->Map);

  /*uint32_t inodeBlockIdx = FindInodeBlockIdx(DiskPtr, inode);*/
  /*LOG_DEBUG("updating the following values: inode %d, data: %d",
   * inodeBlockIdx,*/
  /*          dataIdx);*/
  /*UpdateInodeBitMapNthNodeToOccupied(DiskPtr, inodeBlockIdx);*/
  /*UpdateDataBitMapNthNodeToOccupied(DiskPtr, dataIdx);*/

  return inode;
}

Inode *createDirectoryInode(void *disk, uint32_t iBlockIdx,
                            uint32_t dataRegionIdx) {
  LOG_DEBUG("From inside of directory node %d", iBlockIdx);
  LOG_DEBUG("From inside of directory node %d", dataRegionIdx);
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
  return dirInode;
}

void ListDirectory(const char *pathPtr) {
  assert(strlen(pathPtr) <= PATH_NAME_MAX_LENGTH &&
         "Path name should be less than 255 chars");
  assert(DiskPtr != NULL && "Root Directory MetaData Not found");
  assert(*pathPtr == '/' &&
         "Directory Path Should always start with root directory");

  Inode *dirInode = FindFirstInode(DiskPtr);
  uint32_t matchedInodeBlockIdx = 0;

  char tempStr[PATH_NAME_MAX_LENGTH];
  strcpy(tempStr, pathPtr);
  const char delimiter[] = PATH_NAME_DELIMITER;
  char *splitedStr;
  splitedStr = strtok(tempStr, delimiter);
  while (splitedStr != NULL) {
    matchedInodeBlockIdx = findDirInodeWithValidName(splitedStr, dirInode);
    assert(matchedInodeBlockIdx != 0 && "Required Directory does not exists");
    dirInode = FindNthInode(DiskPtr, matchedInodeBlockIdx);
    splitedStr = strtok(NULL, delimiter);
    /*if (splitedStr != NULL) {*/
    /*  assert(matchedInodeBlockIdx == 0 &&*/
    /*         "Unable to find any matching path name");*/
    /*} else {*/
    /*}*/
  }

  LOG_DEBUG("Parent here; %d", FindInodeBlockIdx(DiskPtr, dirInode));
  ReadDirectoryDataItem(DiskPtr, dirInode);
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
