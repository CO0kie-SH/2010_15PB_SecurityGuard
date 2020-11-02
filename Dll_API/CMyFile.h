#pragma once
#include "CInclude.h"

#define		gdefRBFunctions_MAX		0x03


enum _Enum垃圾功能区
{
	gdsz_系统垃圾清理		= 0x00,
	gdsz_IE垃圾清理		= 0x01,
	gdsz_VS垃圾清理		= 0x02
};

constexpr	PTCHAR	gszRBFunctions[] = {
	(PTCHAR)_T("系统垃圾清理"),
	(PTCHAR)_T("IE垃圾清理"),
	(PTCHAR)_T("VS垃圾清理"),
};

constexpr	char* gszRBcmd[] = {
	(char*)"echo 按任意键清除系统垃圾文件。&&pause&&",
	(char*)"pause&&ping 127.0.0.2",
	(char*)"pause&&ping 127.0.0.3"
};

typedef struct _FILEINFO
{
	WIN32_FIND_DATA data = {};
	__int64			Size = 0;
	TCHAR			path[MAX_PATH];
} FILEINFO, * LPFILEINFO;


typedef struct _RubbishFunction
{
	LPVOID			TreeRoot[gdefRBFunctions_MAX];
	__int64			Size = 0;
	vector<FILEINFO>	Fils;
} RubbishFunction, * LPRubbishFunction;


class DLLexp CMyFile
{
public:
	CMyFile();
	~CMyFile();

	BOOL SearchPaths(vector<FILEINFO>& FLs, PTCHAR Path);
private:

};

