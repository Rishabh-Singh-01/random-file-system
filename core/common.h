#ifndef COMMON_H
#define COMMON_H

#include "inode.h"
#include <stdint.h>

extern void *DiskPtr;
typedef void (*CbInodeValidDirectPtrs)(Inode *curDir, uint32_t dataRegionIdx);

void ExecuteCbOnInodeValidDirectPtrs(Inode *curDir, CbInodeValidDirectPtrs cb);

typedef uint32_t (*Int32CbInodeValidDirectPtrs)(const char *path, Inode *curDir,
                                                uint32_t dataRegionIdx);
uint32_t ExecuteInt32CbOnInodeValidDirectPtrs(Inode *curDir, const char *path,
                                              Int32CbInodeValidDirectPtrs cb);

typedef Inode *(*CbTravelToDirMatchCondition)(const char *splitedStr,
                                              uint32_t matchedInodeBlockIdx,
                                              Inode *dirInode);
Inode *TravelToDirFromPathName(const char *pathPtr,
                               CbTravelToDirMatchCondition);

void AssertFileConfigs(const char *path);
void AssertDirConfigs(const char *path);
const char *PathNameEndPart(const char *pathPtr);
#endif // !COMMON_Hk
