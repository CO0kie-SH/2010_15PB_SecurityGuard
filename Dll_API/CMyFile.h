#pragma once
#include <vector>
using std::vector;


typedef struct _FILEINFO
{
	DWORD	isDIR = 0;
	WIN32_FIND_DATA data = {};
} FILEINFO, * LPFILEINFO;


class _declspec(dllexport) CMyFile
{
public:
	CMyFile();
	~CMyFile();

private:

};

