#ifndef DIR_H
#define DIR_H

#include "inode.h"

#define MAX_DIRECT_DATA_REGION_LINK_COUNT 12
#define PATH_NAME_MAX_LENGTH 255
#define PATH_NAME_DELIMITER "/"
void DumpRootDirectory();

Inode *FindImmediateParent(Inode *childDir);
void SelectAndResetDirDataItem(Inode *curDir, uint32_t dataRegionIdx,
                               const char *dirPathToReset);
void RemoveAllChildDir(Inode *curDir);
Inode *MakeDirTravelMatchConditionCb(const char *splitedStr,
                                     uint32_t matchedInodeBlockIdx,
                                     Inode *dirInode);

Inode *ListDirTravelMatchConditionCb(const char *splitedStr,
                                     uint32_t matchedInodeBlockIdx,
                                     Inode *dirInode);

uint32_t findDirInodeWithValidName(const char *path, Inode *dirNode);
void MakeDirectory(const char *pathPtr);
Inode *CreateDefaultDirectory();
Inode *createDirectoryInode(uint32_t dirIdx, uint32_t dataRegionIdx);
void LinkParentDirWithChildDir(Inode *childDir, Inode *parentDir,
                               const char *path);
void LinkChildDirWithParentDir(Inode *childDir, Inode *parentDir);
void LinkDirectoryWithParentDir(Inode *curDir, Inode *parentDir);
void ListDirectory(const char *pathPtr);
void DumpAnyDirectory(Inode *rootDirInode);

#endif // !DIR_H
