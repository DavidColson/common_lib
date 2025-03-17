
// ***********************************************************************

bool FileExists(String filePath) {
	DWORD dwAttrib = GetFileAttributes(filePath.pData);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

// ***********************************************************************

bool FolderExists(String folderPath) {
	DWORD dwAttrib = GetFileAttributes(folderPath.pData);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

// ***********************************************************************

bool MakeDirectory(String path, bool makeAll) {
	if (makeAll) {
		char folderToTest[MAX_PATH]; 
		memset(folderToTest, 0, MAX_PATH * sizeof(char));

		String subPath = path;
		i64 end = subPath.Find("/");
		while(subPath.length > 0) {
			strncpy(folderToTest, path.pData, end);
			if (!CreateDirectory(folderToTest, nullptr)) {
				DWORD err = GetLastError();
				if (err != ERROR_ALREADY_EXISTS) {
					return false;
				}
			}
			subPath = path.SubStr(end, path.length-end);
			end = end + subPath.Find("/");
		}
	}
	else {
		if (!CreateDirectory(path.pData, nullptr)) {
			return false;
		}
	}
	return true;
}

// ***********************************************************************

bool RemoveFileOrDirectory(String path) {
	// remove file or directory
	DWORD dwAttrib = GetFileAttributes(path.pData);
	if (dwAttrib == INVALID_FILE_ATTRIBUTES)
		return false;

	if (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) {
		// recursively delete directory
		WIN32_FIND_DATA findFileData;
		HANDLE hFind;

		StringBuilder builder(g_pArenaFrame);
		builder.Append(path);
		builder.Append("/");
		builder.Append("*");
		hFind = FindFirstFile(builder.CreateString(g_pArenaFrame).pData, &findFileData);
		
		// directory is empty
		if (hFind == INVALID_HANDLE_VALUE) {
			return RemoveDirectory(path.pData) != 0;
		}

		do {
			String item = findFileData.cFileName;

			if (item != "." && item != "..") {
				StringBuilder fullPathBuilder(g_pArenaFrame);
				fullPathBuilder.Append(path);
				fullPathBuilder.Append("/");
				fullPathBuilder.Append(item);
				RemoveFileOrDirectory(fullPathBuilder.CreateString(g_pArenaFrame));
			}
		} while (FindNextFile(hFind, &findFileData));

		FindClose(hFind);
		return RemoveDirectory(path.pData) != 0;
	}
	else {
		// delete file
		return DeleteFile(path.pData) != 0;
	}
	return false;
}

// ***********************************************************************

bool CopyFile(String from, String to) {
	return CopyFileA(from.pData, to.pData, true);
}

// ***********************************************************************

bool MoveFile(String from, String to) {
	return MoveFileA(from.pData, to.pData);
}

// ***********************************************************************

struct File {
	HANDLE handle;
};

// ***********************************************************************

File OpenFile(String filename, FileMode mode) {
	File file;

	DWORD access = 0;
    DWORD shareMode = 0;
    DWORD creationDisposition = OPEN_EXISTING;
	switch (mode) {
        case FM_READ:
            access = GENERIC_READ;
			shareMode = FILE_SHARE_READ;
            creationDisposition = OPEN_EXISTING;
            break;
        case FM_WRITE:
            access = GENERIC_WRITE;
			shareMode = 0;
            creationDisposition = CREATE_ALWAYS;
            break;
        case FM_APPEND:
            access = GENERIC_WRITE;
			shareMode = 0;
            creationDisposition = OPEN_ALWAYS;
            break;
        case FM_READ_WRITE:
            access = GENERIC_READ | GENERIC_WRITE;
			shareMode = 0;
            creationDisposition = OPEN_EXISTING;
            break;
        case FM_CREATE_NEW:
            access = GENERIC_READ | GENERIC_WRITE;
			shareMode = 0;
            creationDisposition = CREATE_NEW;
            break;
        case FM_CREATE_OR_OPEN:
            access = GENERIC_READ | GENERIC_WRITE;
			shareMode = 0;
            creationDisposition = OPEN_ALWAYS;
            break;
    }
	
	// @todo: can't guarantee filename is null terminated here, need to fix that
	file.handle = CreateFile(filename.pData, access, shareMode, nullptr, creationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
	return file;
}

// ***********************************************************************

bool WriteToFile(File file, const void* pData, i64 size) {
	// Note, since we only do synchronous writes, we assume all bytes were written if this returns true
	BOOL success = WriteFile(file.handle, pData, (DWORD)size, nullptr, nullptr);
	return success;
}

// ***********************************************************************

bool ReadFromFile(File file, void* pOutData, i64 sizeToRead) {
	BOOL success = ReadFile(file.handle, pOutData, (DWORD)sizeToRead, nullptr, nullptr);
	return success;
}

// ***********************************************************************

i64 GetFileSize(File file) {
	LARGE_INTEGER size;

	if (GetFileSizeEx(file.handle, &size)) {
		return size.QuadPart;
	}
	return 0;
}

// ***********************************************************************

void CloseFile(File file) {
	CloseHandle(file.handle);
}

// ***********************************************************************

bool IsValid(File file) {
	return file.handle != INVALID_HANDLE_VALUE;
}

// ***********************************************************************

char* ReadWholeFile(String filename, i64* outSize, Arena* pArena) {
	File file = OpenFile(filename, FM_READ);
	defer(CloseFile(file));

	if (!IsValid(file))
		return nullptr;

	*outSize = GetFileSize(file);
    char* pData = New(pArena, char, *outSize);
	if (ReadFromFile(file, pData, *outSize)) {
		return pData;
	}
	return nullptr;
}

// ***********************************************************************

bool WriteWholeFile(String filename, const void* pData, i64 size) {
	File file = OpenFile(filename, FM_WRITE);
	defer(CloseFile(file));

	if (!IsValid(file))
		return false;

	if (WriteToFile(file, pData, size)) {
		return true;
	}
	return false;
}

// ***********************************************************************

struct DirectoryWatchInfo {
	String name;
	OVERLAPPED overlapped;
	HANDLE handle;
	void* notifyBuffer;
};

struct FileWatcher {
	Arena* pArena;
	bool isRecursive;
	i32 eventsToWatch;
	ResizableArray<DirectoryWatchInfo*> directories;
	FileWatcherCallback callback;
	void* pUserData;
};

#define WATCHER_BUFFER_SIZE 8000

// ***********************************************************************

FileWatcher* FileWatcherCreate(FileWatcherCallback callback, i32 eventsToWatch, void* pUserData, bool isRecursive) {
	FileWatcher* pWatcher;
	Arena* pArena = ArenaCreate();
	pWatcher = New(pArena, FileWatcher);
	pWatcher->pArena = pArena;

	pWatcher->pUserData = pUserData;
	pWatcher->isRecursive = isRecursive;
	pWatcher->eventsToWatch = eventsToWatch;
	pWatcher->directories.pArena = pWatcher->pArena;
	pWatcher->callback = callback;
	return pWatcher;
}

// ***********************************************************************

bool FileWatcherIssueRead(FileWatcher* pWatcher, DirectoryWatchInfo* pInfo) {
	i32 notifyFilter = 0;
	if (pWatcher->eventsToWatch & (FC_ADDED | FC_MOVED | FC_REMOVED)) {
		notifyFilter |= FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_CREATION;
	}
	if (pWatcher->eventsToWatch & (FC_MODIFIED)) {
		notifyFilter |= FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE;
	}

	bool result = ReadDirectoryChangesW(pInfo->handle,
									 pInfo->notifyBuffer,
									 WATCHER_BUFFER_SIZE,
									 pWatcher->isRecursive,
									 notifyFilter,
									 nullptr,
									 &pInfo->overlapped,
									 nullptr);

	if (result == false) {
		Log::Info("Failing to watch directory");
	}
	return result;
}

// ***********************************************************************

bool FileWatcherAddDirectory(FileWatcher* pWatcher, String path) {
	String localString = CopyString(path, pWatcher->pArena);
	if (localString[localString.length-1] == '/' || localString[localString.length-1] == '\\') {
		localString.length -= 1;
		localString[localString.length] = 0;

	}

	HANDLE handle = CreateFile(localString.pData, FILE_LIST_DIRECTORY, FILE_SHARE_READ|FILE_SHARE_DELETE|FILE_SHARE_WRITE,
							 nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED, nullptr);

	if (handle == INVALID_HANDLE_VALUE) {
		Log::Info("Failed to open handle to directory");
	}

	DirectoryWatchInfo* pInfo = New(pWatcher->pArena, DirectoryWatchInfo);
	pInfo->name = localString;
	pInfo->notifyBuffer = ArenaAlloc(pWatcher->pArena, WATCHER_BUFFER_SIZE, sizeof(DWORD));
	pInfo->overlapped.hEvent = CreateEventW(nullptr, false, false, nullptr);
	pInfo->overlapped.Offset = 0;
	pInfo->handle = handle;

	bool success = FileWatcherIssueRead(pWatcher, pInfo);

	pWatcher->directories.PushBack(pInfo);
	return success;
}

// ***********************************************************************

void FileWatcherProcessChanges(FileWatcher* pWatcher) {
    for (DirectoryWatchInfo* pInfo : pWatcher->directories) {
		if (!HasOverlappedIoCompleted(&pInfo->overlapped)) continue;

	  	DWORD bytesTransferred = 0;
		bool success = GetOverlappedResult(pInfo->handle, &pInfo->overlapped, &bytesTransferred, FALSE); 

		defer(FileWatcherIssueRead(pWatcher, pInfo));

		if (!success || bytesTransferred == 0) continue;

		FILE_NOTIFY_INFORMATION* pResults = (FILE_NOTIFY_INFORMATION*)pInfo->notifyBuffer;
		while (true) {
			FileChange change;
			switch (pResults->Action) {
				case FILE_ACTION_ADDED: change.event = FC_ADDED; break;
				case FILE_ACTION_MODIFIED: change.event = FC_MODIFIED; break;
				case FILE_ACTION_RENAMED_OLD_NAME: change.event = FileChangeEvent(FC_MOVED | FC_MOVED_FROM); break;
				case FILE_ACTION_RENAMED_NEW_NAME: change.event = FileChangeEvent(FC_MOVED | FC_MOVED_TO); break;
				case FILE_ACTION_REMOVED: change.event = FC_REMOVED; break;
				default: change.event = FC_NONE;
			}

			i32 len = WideCharToMultiByte(CP_UTF8, 0, pResults->FileName, pResults->FileNameLength/2, nullptr, 0, nullptr, nullptr);
			String fileName = AllocString(len, g_pArenaFrame);
			WideCharToMultiByte(CP_UTF8, 0, pResults->FileName, pResults->FileNameLength/2, fileName.pData, len, nullptr, nullptr);

			change.path = StringPrint(pWatcher->pArena, "%S/%S", pInfo->name, fileName);

			// @todo: you will want to coalesce events together, things like nvim do many edits quickly
			pWatcher->callback(change, pWatcher->pUserData);

			// this was the last record
			if (pResults->NextEntryOffset == 0) break;

			pResults = (FILE_NOTIFY_INFORMATION*)(((char*)pResults) + pResults->NextEntryOffset);
		}
	}
}
