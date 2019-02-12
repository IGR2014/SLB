#include "LogFunctions.hpp"
#include "LogBuffer.hpp"

// Initialize log buffer
void LogBufferInit(const DWORD &size, const DWORD &writeTime) {

	// Get local time
	SYSTEMTIME time;
	GetLocalTime(&time);
	WCHAR logFileName[25];
	// Form file name as "log_<dd-mm-yyyy>_<hh-mm>.txt"
	wsprintf(logFileName,
			 L"log_%02i-%02i-%04i_%02i-%02i.txt\0",
			 time.wDay,
			 time.wMonth,
			 time.wYear,
			 time.wHour,
			 time.wMinute);

	// Path to current app
	WCHAR filePath[1024];

// WinCE does not have GetCurrentDirectory
//
// Microsoft - I'm lovin` it!
// Para-pa-pa-pa
#ifdef _WIN32_WCE

	// Get path to current app
	GetModuleFileName(NULL, filePath, 1023);
	// Exclude .exe name from path
	LPWSTR it = wcsrchr(filePath, L'\\');
	if (it) {
	
		// Character after last slash
		// in path should be EOL
		*++it = L'\0';

	}

#else

	// Get path to current app
	GetCurrentDirectory(1023, filePath);

#endif	// _WIN32_WCE

	// Add file name to current app`s path
	wcscat(filePath, logFileName);

	// Open log file
	HANDLE hFile = CreateFile(filePath,
							  GENERIC_WRITE,
							  FILE_SHARE_WRITE,
							  NULL,
							  CREATE_ALWAYS,
							  FILE_ATTRIBUTE_NORMAL,
							  0);

	// Error handling
	if (hFile == INVALID_HANDLE_VALUE) {
	
		MessageBox(NULL, L"Unable to create file!", L"LOG Error",MB_OK);
		ExitThread(-1);

	}

	// Counter of writen bytes for WriteFile
	DWORD bytesWriten = 0;
	
	// Add unicode text file header
	WCHAR UTF16Header = 0xFEFF;
	// Save header to file
	WriteFile(hFile, &UTF16Header, sizeof(WCHAR), &bytesWriten, NULL);

	// Close log file
	CloseHandle(hFile);

	// Initialize log buffer
	CLogBuffer::init(size, writeTime);
	// Set log file name
	CLogBuffer::use().setLogFileName(filePath);

	// Add "start logging" message
	CLogBuffer::use().writeTime();
	CLogBuffer::use().write(L"\tLog started.\r\n", 15);
	CLogBuffer::use().write(L"=======================\r\n", 25);

}

// Main log function
void LogBufferWrite(LPCWSTR eventName,
					LPCWSTR valueName,
					LPCWSTR valueData) {

	// Lock buffer
	EnterCriticalSection(&CLogBuffer::use().bufferLock);

	// Write data to buffer
	CLogBuffer::use().writeTime();
	CLogBuffer::use().write(L"\t", 1);
	CLogBuffer::use().write(eventName, wcslen(eventName));
	CLogBuffer::use().write(L":\t", 2);
	CLogBuffer::use().write(valueName, wcslen(valueName));
	CLogBuffer::use().write(L" = ", 3);
	CLogBuffer::use().write(valueData, wcslen(valueData));
	CLogBuffer::use().write(L"\r\n", 2);

	// Free buffer
	LeaveCriticalSection(&CLogBuffer::use().bufferLock);

}

// Log int value
void LogBufferWrite(LPCWSTR eventName,
					LPCWSTR valueName,
					const int &value) {

	// Convert int to unicode string
	WCHAR intValue[11];
	swprintf(intValue, L"%.1i", value);
	// Write to buffer
	LogBufferWrite(eventName, valueName, intValue);

}

// Log long long value
void LogBufferWrite(LPCWSTR eventName,
					LPCWSTR valueName,
					const long long &value) {

	// Convert long to unicode string
	WCHAR longValue[21];
	swprintf(longValue, L"%.1lli", value);
	// Write to buffer
	LogBufferWrite(eventName, valueName, longValue);

}

// Log unsigned int value
void LogBufferWrite(LPCWSTR eventName,
					LPCWSTR valueName,
					const unsigned int &value) {

	// Convert int to unicode string
	WCHAR intValue[11];
	swprintf(intValue, L"%.1u", value);
	// Write to buffer
	LogBufferWrite(eventName, valueName, intValue);

}

// Log unsigned long long value
void LogBufferWrite(LPCWSTR eventName,
					LPCWSTR valueName,
					const unsigned long long &value) {

	// Convert long to unicode string
	WCHAR longValue[21];
	swprintf(longValue, L"%.1llu", value);
	// Write to buffer
	LogBufferWrite(eventName, valueName, longValue);

}

// Log float value
void LogBufferWrite(LPCWSTR eventName,
					LPCWSTR valueName,
					const float &value) {

	// Convert float to unicode string
	WCHAR floatValue[11];
	swprintf(floatValue, L"%.2f", value);
	// Write to buffer
	LogBufferWrite(eventName, valueName, floatValue);

}

// Log double value
void LogBufferWrite(LPCWSTR eventName,
					LPCWSTR valueName,
					const double &value) {

	// Convert double to unicode string
	WCHAR doubleValue[21];
	swprintf(doubleValue, L"%.2f", value);
	// Write to buffer
	LogBufferWrite(eventName, valueName, doubleValue);

}

// Finalize log
void LogBufferFin() {

	// Add "log end" message
	CLogBuffer::use().write(L"=======================\r\n", 25);
	CLogBuffer::use().writeTime();
	CLogBuffer::use().write(L"\tLog ended.\r\n", 13);

	// Create thread to write
	HANDLE hThread = CreateThread(NULL,
								  NULL,
								  (LPTHREAD_START_ROUTINE)&writeFile,
								  (LPVOID)&CLogBuffer::use(),
								  NULL,
								  NULL);

	// Checl if thread created
	if (hThread == INVALID_HANDLE_VALUE) {

		MessageBox(NULL, L"Unable to create thread!", L"LOG Error", MB_OK);

	}

	// Wait for thread
	WaitForSingleObject(hThread, INFINITE);

}
