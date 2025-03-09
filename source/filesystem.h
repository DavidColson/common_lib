// Copyright David Colson. All rights reserved.

// general filesystem
bool FileExists(String filePath);
bool FolderExists(String folderPath);
bool MakeDirectory(String path);


// file IO
struct File;

enum FileMode {
    FM_READ,            // Open existing file for read-only access
    FM_WRITE,           // Create or open file for writing (truncate)
    FM_APPEND,          // Open existing file for writing (append)
    FM_READ_WRITE,      // Open existing file for both reading and writing
    FM_CREATE_NEW,      // Create a new file, fail if it exists
    FM_CREATE_OR_OPEN   // Open file if it exists, create if it doesn't
};

File OpenFile(String filename, FileMode mode);
bool WriteToFile(File file, void* pData, i64 size);
bool ReadFromFile(File file, void* pOutData, i64 sizeToRead);
i64 GetFileSize(File file);
void CloseFile(File file);
bool IsValid(File file);

// convenience functions
char* ReadWholeFile(String filename, i64* outSize, Arena* pArena);
bool WriteWholeFile(String filename, const void* pData, i64 size);

