#ifndef DIR_H
#define DIR_H

#include "inode.h"
void DumpRootDirectory(void *disk);

Inode *createDefaultDirectory(void *disk, uint32_t dirIdx,
                              uint32_t dataRegionIdx);
#endif // !DIR_H
