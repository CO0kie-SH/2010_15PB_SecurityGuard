#pragma once
#include <shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

#include <vector>
using std::vector;


typedef struct _FILEINFO
{
	WIN32_FIND_DATA data = {};
	__int64			Size = 0;
} FILEINFO, * LPFILEINFO;


class _declspec(dllexport) CMyFile
{
public:
	CMyFile();
	~CMyFile();

	BOOL SearchPaths(vector<FILEINFO>& FLs, PTCHAR Path);
private:

};

