#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <functional>
#include <string>
#include <memory>
#include <Windows.h>

using namespace std;


inline wstring GetErrorText(int errorCode)
{
	wchar_t buffer[256];

	::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,	NULL, errorCode, MAKELANGID(LANG_SYSTEM_DEFAULT, SUBLANG_NEUTRAL), buffer, 255, NULL);

	return buffer;
}

inline wstring GetTemporaryFileName()
{
	wchar_t buffer[MAX_PATH];

	GetTempFileName(L".", L"RAM", 0, buffer);

	return buffer;
}