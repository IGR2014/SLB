#include <iostream>
#include "LogFunctions.hpp"
#include "LogBuffer.hpp"

#define THREAD_COUNT 40

static int intVal = 0;
static float floatVal = 0.5;

void threadFunc(LPVOID lpParam) {

	// Get counter from function parameter
	int number = *(int*)lpParam;
	// Array for thread ID
	WCHAR text[255];
	// Form thread ID
	swprintf(text, L"%s #%i", L"Поток", number);

	// Log some data
	LogBufferWrite(text, L"A", ++intVal);
	LogBufferWrite(text, L"B", ++intVal);
	LogBufferWrite(text, L"C", ++floatVal);
	LogBufferWrite(text, L"D", ++floatVal);
	LogBufferWrite(text, L"E", ++floatVal);
	LogBufferWrite(text, L"F", ++floatVal);
	LogBufferWrite(text, L"G", ++intVal);
	LogBufferWrite(text, L"H", ++intVal);
	LogBufferWrite(text, L"I", ++intVal);
	LogBufferWrite(text, L"J", ++intVal);
	LogBufferWrite(text, L"K", ++floatVal);
	LogBufferWrite(text, L"L", ++floatVal);
	LogBufferWrite(text, L"M", ++floatVal);
	LogBufferWrite(text, L"N", ++floatVal);
	LogBufferWrite(text, L"O", ++intVal);
	LogBufferWrite(text, L"P", ++intVal);
	LogBufferWrite(text, L"Q", ++intVal);
	LogBufferWrite(text, L"R", ++intVal);
	LogBufferWrite(text, L"S", ++floatVal);
	LogBufferWrite(text, L"T", ++floatVal);
	LogBufferWrite(text, L"U", ++floatVal);
	LogBufferWrite(text, L"V", ++floatVal);
	LogBufferWrite(text, L"W", ++intVal);
	LogBufferWrite(text, L"X", ++intVal);
	LogBufferWrite(text, L"Y", ++intVal);
	LogBufferWrite(text, L"Z", ++intVal);

	// Exit thread
	ExitThread(0);

}

int _tmain() {

	// Init logging
	LogBufferInit(4096, 3000);
	
	// Handles for threads
	HANDLE hThread[THREAD_COUNT];
	// Array of thread numbers
	int threadNumber[THREAD_COUNT];

	// Create threads
	for (int i = 0; i < THREAD_COUNT; ++i) {
	
		// "Calc" thread number
		threadNumber[i] = i;
		// Call threadFunc`s
		hThread[i] = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&threadFunc, &threadNumber[i], NULL, NULL);

		// Error handling
		if(hThread[i] == NULL) {
		
			MessageBox(NULL, L"ERROR!", L"Thread was not created!", MB_OK);

		}

	}

// In WinCE WaitFoMultipleObjects(..) waits
// !SURPRISE! only till first signalled event
//
// Microsoft - I'm lovin` it!
// Para-pa-pa-pa
#ifdef _WIN32_WCE

	// Wait for threadFunc`s
	for (int i = 0; i < THREAD_COUNT; ++i) {
	
		// Wait for all threads to end execution
		WaitForSingleObject(hThread[i], INFINITE);
	
	}

#else
	
	// Wait for threadFunc`s
	WaitForMultipleObjects(THREAD_COUNT, hThread, TRUE, INFINITE);
	
#endif	// _WIN32_WCE

	// End logging
	LogBufferFin();

	return 0;

}

