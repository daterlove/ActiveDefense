#include "stdafx.h"

BOOL InstallDriver(const char* lpszDriverName, const char* lpszDriverPath, const char* lpszAltitude);
BOOL StartDriver(const WCHAR * lpszDriverName);
BOOL StopDriver(const WCHAR * lpszDriverName);
BOOL DeleteDriver(const WCHAR* lpszDriverName);