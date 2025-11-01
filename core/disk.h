#ifndef DISK_H
#define DISK_H

#define BLOCKS_COUNT 64
#define PREDEFINED_BLOCKS_COUNT 8
#define DATA_REGION_COUNT BLOCKS_COUNT - PREDEFINED_BLOCKS_COUNT
#define BLOCK_SIZE_BYTES 4 * 1024
#define DISK_SIZE_BYTES BLOCKS_COUNT *BLOCK_SIZE_BYTES

typedef char Disk[DISK_SIZE_BYTES];

void *InitializeDisk();
void DumpDisk(void *disk);

#endif // !DISK_H
#define DISK_H
