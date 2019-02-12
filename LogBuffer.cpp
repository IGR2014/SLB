#include "LogBuffer.hpp"

// C-tor
CLogBuffer::CLogBuffer(const DWORD &size,
					   const DWORD &writeTime) : buffer(new WCHAR[size]),
												 bufferSize(size),
												 writeThreadTime(writeTime) {

	// Create lock
	InitializeCriticalSection(&fileLock);
	InitializeCriticalSection(&bufferLock);
	// Initialize ring buffer
	writePointer = readPointer = buffer;
	// Preset log file name
	wcscpy(fileName, L"log.txt");

	// Start timer thread
	HANDLE hTimerThread = CreateThread(NULL,
									   NULL,
									   (LPTHREAD_START_ROUTINE)&writeFileByTimer,
									   (LPVOID)this,
									   NULL,
									   NULL);

}

// Set name for log file
void CLogBuffer::setLogFileName(LPCWSTR newFileName) {

	wcscpy(fileName, newFileName);

}

// Get name of log file
LPCWSTR CLogBuffer::getLogFileName() {

	return fileName;

}

// Write data to RAM buffer
void CLogBuffer::write(LPCWSTR data, const DWORD &size) {

	// Copy data to buffer
	for (DWORD i = 0; i < size; ++i) {

		// Copy data to buffer
		*writePointer = data[i];

		// When pointer reaches end of ring buffer
		if (++writePointer > (buffer + bufferSize)) {
		
			// it moves to start
			writePointer = buffer;
		
		}

		// When buffer overflowed it's time to write in file
		if (writePointer == readPointer) {

			// Create thread to write
			HANDLE hThread = CreateThread(NULL,
										  NULL,
										  (LPTHREAD_START_ROUTINE)&writeFile,
										  (LPVOID)this,
										  NULL,
										  NULL);

			// Checl if thread created
			if (hThread == INVALID_HANDLE_VALUE) {
			
				MessageBox(NULL, L"Unable to create thread!", L"LOG Error", MB_OK);
			
			}

			// Wait for thread
			WaitForSingleObject(hThread, INFINITE);

		}

	}

}

// Write current system time to buffer
void CLogBuffer::writeTime() {

	// Get system time
	SYSTEMTIME time;
	GetLocalTime(&time);
	WCHAR timeBuff[13];
	// Form log time as "hh:mm:ss.zzz"
	wsprintf(timeBuff,
			 L"%02i:%02i:%02i.%03i\0",
			 time.wHour,
			 time.wMinute,
			 time.wSecond,
			 time.wMilliseconds);

	// Write time to buffer
	write(L"[", 1);
	write(timeBuff, wcslen(timeBuff));
	write(L"]", 1);

}

// D-tor
CLogBuffer::~CLogBuffer() {

	// Destroy lock
	DeleteCriticalSection(&fileLock);
	DeleteCriticalSection(&bufferLock);

	// Delete buffer
	delete [] buffer;

	// Zero pointers and values
	writePointer = readPointer = buffer = NULL;
	bufferSize = 0;

}

// Write data to file
void writeFile(LPCVOID lpParam) {

	// Get pointer to log buffer from param
	CLogBuffer* logBuff = (CLogBuffer*)lpParam;

	// Lock file
	EnterCriticalSection(&logBuff->fileLock);

	// Open log file
	HANDLE hFile = CreateFile(logBuff->fileName,
							  GENERIC_WRITE,
							  FILE_SHARE_WRITE,
							  NULL,
							  OPEN_ALWAYS,
							  FILE_ATTRIBUTE_NORMAL,
							  0);

	// Check if file opened
	if (hFile == INVALID_HANDLE_VALUE) {
	
		MessageBox(NULL, L"Unable to open file!", L"LOG Error", MB_OK);
		ExitThread(-1);

	}

	// Append to the end of file
	SetFilePointer(hFile, 0, NULL, FILE_END);

	// Counter of writen bytes for WriteFile
	DWORD bytesWriten = 0;

	// Count size of data to write
	int writeSize = (logBuff->writePointer - logBuff->readPointer) * sizeof(WCHAR);

	// Check if data to write is linear
	if (logBuff->readPointer < logBuff->writePointer) {

		// Write data to file
		WriteFile(hFile, logBuff->readPointer, writeSize, &bytesWriten, NULL);

	} else {

		// Count size of first part of data
		writeSize = ((logBuff->buffer + logBuff->bufferSize) - logBuff->readPointer + 1) * sizeof(WCHAR);
		// Write data to file
		WriteFile(hFile, logBuff->readPointer, writeSize, &bytesWriten, NULL);
		// Count size of first part of data
		writeSize = (logBuff->writePointer - logBuff->buffer) * sizeof(WCHAR);
		// Write data to file
		WriteFile(hFile, logBuff->buffer, writeSize, &bytesWriten, NULL);

	}

	// Move read pointer
	logBuff->readPointer = logBuff->writePointer;

	// Close file
	CloseHandle(hFile);

	// Free file
	LeaveCriticalSection(&logBuff->fileLock);

	// Exit thread
	ExitThread(0);

}

// Write data to file by timer
void writeFileByTimer(LPCVOID lpParam, DWORD time) {

	CLogBuffer* buffer = (CLogBuffer*)lpParam;

	// Infinite loop
	while (true) {

		// Wait 3 sec.
		Sleep(buffer->writeThreadTime);
		// Write to file
		writeFile(lpParam);

	}

	// Exit thread
	ExitThread(0);

}

// Singleton routine
CLogBuffer* CLogBuffer::singletonInstance = NULL;
