#pragma once
#include "CInclude.h"

#define		gdefHWFunctions_MAX		0x01

#define		gdsz_枚举窗口				0x00
constexpr	PTCHAR	gszHWFunctions[] = {
	(PTCHAR)_T("枚举窗口")
};

static BOOL CALLBACK EnumWindowsV(
	_In_ HWND   hwnd,
	_In_ LPARAM lParam) {
	vector<HWND>* hwndInfos = (vector<HWND>*)lParam;
	hwndInfos->push_back(hwnd);
	return true;
};
class DLLexp CMyHwnd
{
public:
	BOOL EnumWindow(vector<HWND>* hwndInfos) {
		EnumWindows(EnumWindowsV, (LPARAM)hwndInfos);
		return !hwndInfos->empty();
	};
};

