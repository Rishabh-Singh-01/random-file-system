#ifndef COMMON_H
#define COMMON_H

#include "inode.h"
#include <stdint.h>
typedef void (*CbInodeValidDirectPtrs)(void *disk, Inode *curDir,
                                       uint32_t dataRegionIdx);

void ExecuteCbOnInodeValidDirectPtrs(void *disk, Inode *curDir,
                                     CbInodeValidDirectPtrs cb);

typedef uint32_t (*Int32CbInodeValidDirectPtrs)(const char *path, Inode *curDir,
                                                uint32_t dataRegionIdx);
uint32_t ExecuteInt32CbOnInodeValidDirectPtrs(void *disk, Inode *curDir,
                                              const char *path,
                                              Int32CbInodeValidDirectPtrs cb);

typedef Inode *(*CbTravelToDirMatchCondition)(const char *splitedStr,
                                              uint32_t matchedInodeBlockIdx,
                                              Inode *dirInode);
Inode *TravelToDirFromPathName(void *disk, const char *pathPtr,
                               CbTravelToDirMatchCondition);

void AssertDirConfigs(void *disk, const char *path);
const char *PathNameEndPart(const char *pathPtr);

#endif // !COMMON_H
