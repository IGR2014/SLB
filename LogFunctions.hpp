#pragma once

#include <windows.h>

// Initialize log buffer
void LogBufferInit(const DWORD&, const DWORD&);

// Main log function
void LogBufferWrite(LPCWSTR, LPCWSTR, LPCWSTR);

// Log int value
void LogBufferWrite(LPCWSTR, LPCWSTR, const int&);

// Log long long value
void LogBufferWrite(LPCWSTR, LPCWSTR, const long long&);

// Log unsigned int value
void LogBufferWrite(LPCWSTR, LPCWSTR, const unsigned int&);

// Log unsigned long long value
void LogBufferWrite(LPCWSTR, LPCWSTR, const unsigned long long&);

// Log float value
void LogBufferWrite(LPCWSTR, LPCWSTR, const float&);

// Log double value
void LogBufferWrite(LPCWSTR, LPCWSTR, const double&);

// Finalize log
void LogBufferFin();
