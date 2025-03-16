// Copyright David Colson. All rights reserved.

// general filesystem
bool FileExists(String filePath);
bool FolderExists(String folderPath);
bool MakeDirectory(String path);
bool RemoveFileOrDirectory(String path);
bool CopyFile(String from, String to);
bool MoveFile(String from, String to);

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
bool IsValid(File file);
bool WriteToFile(File file, void* pData, i64 size);
bool ReadFromFile(File file, void* pOutData, i64 sizeToRead);
i64 GetFileSize(File file);
void CloseFile(File file);

// convenience functions
char* ReadWholeFile(String filename, i64* outSize, Arena* pArena);
bool WriteWholeFile(String filename, const void* pData, i64 size);

// Directory watcher
struct FileWatcher;

enum FileChangeEvent : i32 {
	FC_NONE = 0,
	FC_ADDED = 1 << 1,
	FC_MODIFIED = 1 << 2,
	FC_MOVED = 1 << 3,
	FC_MOVED_FROM = 1 << 4,
	FC_MOVED_TO = 1 << 5,
	FC_REMOVED = 1 << 6,
};

struct FileChange {
	String path;
	FileChangeEvent event;
};
typedef void (*FileWatcherCallback)(FileChange, void*);

FileWatcher* FileWatcherCreate(FileWatcherCallback callback, i32 eventsToWatch, void* pUserData, bool isRecursive);
bool FileWatcherAddDirectory(FileWatcher* pWatcher, String path);
void FileWatcherProcessChanges(FileWatcher* pWatcher);
