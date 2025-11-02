#include "file.h"
#include "./../utils/logger.h"
#include "common.h"
#include "data.h"
#include "dir.h"
#include "inode.h"
#include "superblock.h"
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

Inode *createBlankFileInode(void *disk, uint32_t iBlockIdx,
                            uint32_t dataRegionIdx) {
  Inode *inode = FindFirstInode(disk);
  Inode *dirInode = inode + iBlockIdx;

  dirInode->Mode = MODE_NORMAL_FILE;
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
  UpdateSuperBlockDataOnly(disk, dataRegionIdx);
  return dirInode;
}

Inode *createBlankFile(void *disk) {
  InodeBitMap *iBitMap = FindInodeBitMap(disk);
  uint32_t firstFreeInodeBlockIdx = FindFirstFreeInodeIdx(iBitMap);

  DataBitMap *dBitMap = FindDataBitMap(disk);
  uint32_t firstFreeDataIdx = FindFirstFreeDataIdx(dBitMap);
  Inode *freeDataRegionInode = FindNthInode(disk, firstFreeDataIdx);

  Inode *inode =
      createBlankFileInode(disk, firstFreeInodeBlockIdx, firstFreeDataIdx);
  return inode;
}

void linkParentDirWithChildFile(Inode *childFile, Inode *parentDir,
                                const char *path) {
  uint32_t childFileInodeBlockIdx = FindInodeBlockIdx(DiskPtr, childFile);
  WriteNewDirectoryDataItem(DiskPtr, parentDir, childFileInodeBlockIdx, path);
}

void CreateFile(const char *pathPtr) {
  AssertFileConfigs(DiskPtr, pathPtr);

  Inode *dirInode =
      TravelToDirFromPathName(DiskPtr, pathPtr, MakeDirTravelMatchConditionCb);
  const char *lastPartStr = PathNameEndPart(pathPtr);

  Inode *newDir = createBlankFile(DiskPtr);
  linkParentDirWithChildFile(newDir, dirInode, lastPartStr);
}

// TODO: for now assuming that the file will fit in 4KB
void writeDataToFileOnly(Inode *dirInode, const char *fileData) {
  uint32_t dataRegionIdx = dirInode->InodeBlock.DirectPtr[0];
  // TODO: prev made this method always return DataDirectoryRegion, should have
  // been void *, then caller fn will convert itself
  void *dataRegion = (void *)FindNthDataRegion(DiskPtr, dataRegionIdx);
  strcpy(dataRegion, fileData);
}

// TODO: for now assuming that the file will fit in 4KB
void readDataFromFile(Inode *dirInode) {
  uint32_t dataRegionIdx = dirInode->InodeBlock.DirectPtr[0];
  // TODO: prev made this method always return DataDirectoryRegion, should have
  // been void *, then caller fn will convert itself
  char *dataRegion = (char *)FindNthDataRegion(DiskPtr, dataRegionIdx);
  LOG_INFO("Here is the reading from file:\n%s", dataRegion);
}

void WriteToFile(const char *pathPtr, const char *fileData) {
  AssertFileConfigs(DiskPtr, pathPtr);

  Inode *dirInode =
      TravelToDirFromPathName(DiskPtr, pathPtr, ListDirTravelMatchConditionCb);

  writeDataToFileOnly(dirInode, fileData);
}

void ReadFile(const char *pathPtr) {
  AssertFileConfigs(DiskPtr, pathPtr);

  Inode *dirInode =
      TravelToDirFromPathName(DiskPtr, pathPtr, ListDirTravelMatchConditionCb);

  readDataFromFile(dirInode);
}
