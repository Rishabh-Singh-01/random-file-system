#ifndef INODE_H
#define INODE_H

#include "stdint.h"
#define inode_size_byte 256
#define INODE_COUNT_SINGLE_BLOCK 16
#define INODE_BLOCK_COUNT 5
#define INODE_COUNT INODE_COUNT_SINGLE_BLOCK *INODE_BLOCK_COUNT

// NOTE: We have around 80 inodes
// But we are always going to initialize 64 only
// because our data region is < 64 (~56)
// thus, practically our Inode should be < 64 (~56)
struct InodeBitMap {
  uint32_t Count;
  uint64_t Map;
};

typedef struct InodeBitMap InodeBitMap;

struct InodeBlock {
  uint32_t DirectPtr[12];
  uint32_t IndirectPtr[3];
};

typedef struct InodeBlock InodeBlock;

enum IMode { MODE_NORMAL_FILE, MODE_DIRECTORY };
typedef enum IMode IMode;

enum IFlags { FLAGS_IMMUTABLE, FLAGS_ALL };
typedef enum IFlags IFlags;

enum IOSD { OSD_GENERIC };
typedef enum IOSD IOSD;

enum IGENERATION { GENERATION_GENERIC };
typedef enum IGENERATION IGENERATION;

struct Inode {
  uint16_t Mode;
  uint16_t Uid;
  uint32_t Size;
  uint32_t Time;
  uint32_t CTime;
  uint32_t MTime;
  uint32_t DTime;
  uint16_t Gid;
  uint16_t Links;
  uint32_t Blocks;
  uint32_t Flags;
  uint32_t Osd1;
  InodeBlock InodeBlock;
  uint32_t Generation;
  uint32_t File;
  uint32_t Dir;
  uint8_t reservedSpace[142];
};
typedef struct Inode Inode;

InodeBitMap *InitializeInodeBitMap(void *disk);
#endif // !INODE_H
