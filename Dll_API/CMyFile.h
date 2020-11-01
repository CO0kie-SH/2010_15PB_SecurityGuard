#pragma once
#include "CInclude.h"

typedef struct _FILEINFO
{
	WIN32_FIND_DATA data = {};
	__int64			Size = 0;
} FILEINFO, * LPFILEINFO;


class DLLexp CMyFile
{
public:
	CMyFile();
	~CMyFile();

	BOOL SearchPaths(vector<FILEINFO>& FLs, PTCHAR Path);
private:

};

