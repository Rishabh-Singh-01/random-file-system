#include "file.h"
#include "./../utils/logger.h"
#include "common.h"
#include "data.h"
#include "dir.h"
#include "inode.h"
#include "superblock.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

Inode *createBlankFileInode(uint32_t iBlockIdx, uint32_t dataRegionIdx) {
  Inode *inode = FindFirstInode();
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

  UpdateSuperBlockInodeOnly(iBlockIdx, dataRegionIdx);
  UpdateSuperBlockDataOnly(dataRegionIdx);
  return dirInode;
}

Inode *createBlankFile() {
  InodeBitMap *iBitMap = FindInodeBitMap();
  uint32_t firstFreeInodeBlockIdx = FindFirstFreeInodeIdx(iBitMap);

  DataBitMap *dBitMap = FindDataBitMap();
  uint32_t firstFreeDataIdx = FindFirstFreeDataIdx(dBitMap);
  Inode *freeDataRegionInode = FindNthInode(firstFreeDataIdx);

  Inode *inode = createBlankFileInode(firstFreeInodeBlockIdx, firstFreeDataIdx);
  return inode;
}

void linkParentDirWithChildFile(Inode *childFile, Inode *parentDir,
                                const char *path) {
  uint32_t childFileInodeBlockIdx = FindInodeBlockIdx(childFile);
  WriteNewDirectoryDataItem(parentDir, childFileInodeBlockIdx, path);
}

void CreateFile(const char *pathPtr) {
  AssertFileConfigs(pathPtr);

  Inode *dirInode =
      TravelToDirFromPathName(pathPtr, MakeDirTravelMatchConditionCb);
  const char *lastPartStr = PathNameEndPart(pathPtr);

  Inode *newDir = createBlankFile();
  linkParentDirWithChildFile(newDir, dirInode, lastPartStr);
}

// TODO: for now assuming that the file will fit in 4KB
void writeDataToFileOnly(Inode *dirInode, const char *fileData) {
  uint32_t dataRegionIdx = dirInode->InodeBlock.DirectPtr[0];
  // TODO: prev made this method always return DataDirectoryRegion, should have
  // been void *, then caller fn will convert itself
  void *dataRegion = (void *)FindNthDataRegion(dataRegionIdx);
  strcpy(dataRegion, fileData);
}

// TODO: for now assuming that the file will fit in 4KB
void resetDataToFileOnly(Inode *dirInode) {
  uint32_t dataRegionIdx = dirInode->InodeBlock.DirectPtr[0];
  // TODO: prev made this method always return DataDirectoryRegion, should have
  // been void *, then caller fn will convert itself
  void *dataRegion = (void *)FindNthDataRegion(dataRegionIdx);
  memset(dataRegion, 0, BLOCK_SIZE_BYTES);
}

// TODO: for now assuming that the file will fit in 4KB
void readDataFromFile(Inode *dirInode) {
  uint32_t dataRegionIdx = dirInode->InodeBlock.DirectPtr[0];
  // TODO: prev made this method always return DataDirectoryRegion, should have
  // been void *, then caller fn will convert itself
  char *dataRegion = (char *)FindNthDataRegion(dataRegionIdx);
  LOG_INFO("Here is the reading from file:\n%s", dataRegion);
}

void WriteToFile(const char *pathPtr, const char *fileData) {
  AssertFileConfigs(pathPtr);

  Inode *dirInode =
      TravelToDirFromPathName(pathPtr, ListDirTravelMatchConditionCb);

  writeDataToFileOnly(dirInode, fileData);
}

void ReadFile(const char *pathPtr) {
  AssertFileConfigs(pathPtr);

  Inode *dirInode =
      TravelToDirFromPathName(pathPtr, ListDirTravelMatchConditionCb);

  readDataFromFile(dirInode);
}

Inode *listFileTravelMatchConditionCb(const char *splitedStr,
                                      uint32_t matchedInodeBlockIdx,
                                      Inode *dirInode) {
  assert(matchedInodeBlockIdx != -1 && "Required File does not exists");
  if (splitedStr == NULL) {
    return dirInode;
  }
  return FindNthInode(matchedInodeBlockIdx);
}

void RemoveFile(const char *pathPtr) {
  AssertDirConfigs(pathPtr);

  Inode *dirInode =
      TravelToDirFromPathName(pathPtr, listFileTravelMatchConditionCb);
  LOG_DEBUG("Inside current Directory Inode: %d", FindInodeBlockIdx(dirInode));
  const char *lastPartStr = PathNameEndPart(pathPtr);
  LOG_DEBUG("last part str: %s", lastPartStr);

  Inode *fileInode =
      TravelToDirFromPathName(pathPtr, ListDirTravelMatchConditionCb);
  resetDataToFileOnly(fileInode);

  // TODO: purposely using the fact that the only first direct ptr will contain
  // the details
  uint32_t parentRegion = dirInode->InodeBlock.DirectPtr[0];
  SelectAndResetDirDataItem(dirInode, parentRegion, lastPartStr);
}
