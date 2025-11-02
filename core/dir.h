#ifndef DIR_H
#define DIR_H

#include "inode.h"

#define MAX_DIRECT_DATA_REGION_LINK_COUNT 12
#define PATH_NAME_MAX_LENGTH 255
#define PATH_NAME_DELIMITER "/"
void DumpRootDirectory(void *disk);

uint32_t findDirInodeWithValidName(const char *path, Inode *dirNode);
void MakeDirectory(const char *pathPtr);
Inode *CreateDefaultDirectory(void *disk);
Inode *createDirectoryInode(void *disk, uint32_t dirIdx,
                            uint32_t dataRegionIdx);
void LinkParentDirWithChildDir(Inode *childDir, Inode *parentDir,
                               const char *path);
void LinkChildDirWithParentDir(Inode *childDir, Inode *parentDir);
void LinkDirectoryWithParentDir(Inode *curDir, Inode *parentDir);
void ListDirectory(const char *pathPtr);
void DumpAnyDirectory(Inode *rootDirInode);

#endif // !DIR_H
