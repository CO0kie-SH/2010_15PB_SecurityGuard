#pragma once
#include "CInclude.h"

#define		gdefRBFunctions_MAX		0x03
#define		gdefRBVS_MAX			0x08


enum _Enum����������
{
	gdsz_ϵͳ��������		= 0x00,
	gdsz_IE��������		= 0x01,
	gdsz_VS��������		= 0x02
};

constexpr	PTCHAR	gszRBFunctions[] = {
	(PTCHAR)_T("ϵͳ��������"),
	(PTCHAR)_T("IE��������"),
	(PTCHAR)_T("VS��������"),
};

constexpr	char* gszRBcmd[] = {
	(char*)"echo ����������ϵͳ�����ļ���&&pause&&",
	(char*)"pause&&ping 127.0.0.2",
	(char*)"pause&&ping 127.0.0.3"
};

constexpr	PTCHAR gszRBVS[] = {
	(PTCHAR)_T(".ilk"),
	(PTCHAR)_T(".pdb"),
	(PTCHAR)_T(".obj"),
	(PTCHAR)_T(".log"),
	(PTCHAR)_T(".pch"),
	(PTCHAR)_T(".tlog"),
	(PTCHAR)_T(".lastbuildstate"),
	(PTCHAR)_T(".db")
};

typedef struct _FILEINFO
{
	WIN32_FIND_DATA data;
	__int64			Size;
	bool			isDir;
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

	BOOL SearchPaths(vector<FILEINFO>& FLs, PTCHAR Path,
		PTCHAR pFileType = nullptr, bool isClear = true);
private:

};

