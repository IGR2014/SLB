#pragma once

#include <windows.h>

// Logging buffer (singleton pattern)
class CLogBuffer {

	private:

		// Buffer data
		LPWSTR						buffer;
		// Buffer data size
		DWORD						bufferSize;

		// Write pointer
		LPWSTR						writePointer;
		// Read pointer (aka Write to file pointer)
		LPWSTR						readPointer;

		// Log file name
		WCHAR						fileName[1024];

		// Time interval between autowrite
		DWORD						writeThreadTime;

		// Log file handle
		HANDLE						hTimerThread;

		// Singleton routine
		static		CLogBuffer*		singletonInstance;

		// File lock
		CRITICAL_SECTION			fileLock;

		// Default c-tor
		CLogBuffer();
		// C-tor
		CLogBuffer(const DWORD&, const DWORD&);

		// No copy for singletones
		CLogBuffer(const CLogBuffer&) {};		
		CLogBuffer& operator= (const CLogBuffer&);

		// D-tor
		~CLogBuffer();


		// Freind functions to access class data
		friend void writeFile(LPCVOID);
		friend void writeFileByTimer(LPCVOID, DWORD);


	public:

		// Buffer lock
		CRITICAL_SECTION			bufferLock;
		
		// Initialize instance
		static void init(const DWORD &size, const DWORD &time) {

			// If first call
			if (!singletonInstance) {
			
				// Create new instance
				singletonInstance = new CLogBuffer(size, time);
			
			}

		}

		// Get instance
		static CLogBuffer& use() {

			// Return instance by reference
			return *singletonInstance;

		}

		// Set name for log file
		void setLogFileName(LPCWSTR);
		// Get name of log file
		LPCWSTR getLogFileName();

		// Write current system time to buffer
		void writeTime();

		// Write some data to buffer
		void write(LPCWSTR, const DWORD&);

};

// Write buffer to file
void writeFile(LPCVOID);

// Write data to file by timer
void writeFileByTimer(LPCVOID, DWORD);
