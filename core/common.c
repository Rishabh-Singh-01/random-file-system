#include "common.h"
#include "dir.h"
#include "inode.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void ExecuteCbOnInodeValidDirectPtrs(void *disk, Inode *curDir,
                                     CbInodeValidDirectPtrs cb) {
  uint8_t i = 0;
  while (i < MAX_DIRECT_DATA_REGION_LINK_COUNT &&
         curDir->InodeBlock.DirectPtr[i] != 0) {
    cb(disk, curDir, curDir->InodeBlock.DirectPtr[i]);
    i++;
  }
}

uint32_t ExecuteInt32CbOnInodeValidDirectPtrs(void *disk, Inode *curDir,
                                              const char *path,
                                              Int32CbInodeValidDirectPtrs cb) {
  uint8_t i = 0;
  while (i < MAX_DIRECT_DATA_REGION_LINK_COUNT &&
         curDir->InodeBlock.DirectPtr[i] != 0) {
    uint32_t temp = cb(path, curDir, curDir->InodeBlock.DirectPtr[i]);
    if (temp != -1)
      return temp;
    i++;
  }
  return -1;
}

void AssertDirConfigs(void *disk, const char *path) {
  assert(disk != NULL && "Root Directory MetaData Not found");
  assert(strlen(path) <= PATH_NAME_MAX_LENGTH &&
         "Path name should be less than 255 chars");
  assert(*path == '/' &&
         "Directory Path Should always start with root directory");
}

Inode *TravelToDirFromPathName(void *disk, const char *pathPtr,
                               CbTravelToDirMatchCondition cb) {
  Inode *dirInode = FindFirstInode(disk);
  uint32_t matchedInodeBlockIdx = 0;

  char tempStr[PATH_NAME_MAX_LENGTH];
  strcpy(tempStr, pathPtr);
  const char delimiter[] = PATH_NAME_DELIMITER;
  char *splitedStr;
  splitedStr = strtok(tempStr, delimiter);
  while (splitedStr != NULL) {
    DumpAnyDirectory(dirInode);
    matchedInodeBlockIdx = findDirInodeWithValidName(splitedStr, dirInode);
    splitedStr = strtok(NULL, delimiter);
    dirInode = cb(splitedStr, matchedInodeBlockIdx, dirInode);
  }

  return dirInode;
}

const char *PathNameEndPart(const char *pathPtr) {
  size_t pathLenOrg = strlen(pathPtr);
  uint8_t delimiterCount = 0;
  const char *temp = pathPtr;
  size_t pathLen = strlen(pathPtr);
  while (pathLen > 0) {
    if (*temp == '/')
      delimiterCount++;
    temp++;
    pathLen--;
  }
  assert(delimiterCount >= 0 && "Delimiter Count should be greater than 0");

  temp = pathPtr;
  while (delimiterCount > 0) {
    if (*temp == '/')
      delimiterCount--;
    temp++;
  }
  return temp;
}
