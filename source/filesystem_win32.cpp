
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
