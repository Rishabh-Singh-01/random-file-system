#ifndef FILE_H
#define FILE_H

void CreateFile(const char *pathPtr);
void WriteToFile(const char *pathPtr, const char *fileData);
void ReadFile(const char *pathPtr);
void RemoveFile(const char *pathPtr);

#endif // !FILE_H
