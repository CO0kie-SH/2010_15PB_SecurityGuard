#include "pch.h"
#include "CMyFile.h"




#pragma region 类内函数
CMyFile::CMyFile()
{
}

CMyFile::~CMyFile()
{
}

/*
	函数：搜索文件夹
	返回：成功返回数量，否则为0
	作者：CO0kie丶
	时间：2020-10-31_14-00
*/
BOOL CMyFile::SearchPaths(vector<FILEINFO>& FLs, PTCHAR Path,
	PTCHAR pFileType /*= nullptr*/, bool isClear /*= true*/)
{
	if (nullptr == Path)		return false;		//如果路径不存在则返回
	if (!PathFileExists(Path))	return false;		//如果文件不存在则返回
	if (!PathIsDirectory(Path))	return false;		//如果目录不存在则返回
	if (PathIsDirectoryEmpty(Path))	return false;	//如果空目录则返回

	TCHAR buff[MAX_PATH];
	wsprintf(buff, _T("%s"), Path);
	if (nullptr != pFileType)
		_tcscat_s(buff, MAX_PATH, pFileType);
	

	WIN32_FIND_DATA FindData;	FILEINFO fl;		//初始化文件信息
	HANDLE FindHandle = FindFirstFile(buff, &FindData);
	if (FindHandle == INVALID_HANDLE_VALUE)	return false;
	if (isClear) FLs.clear();
	do {
		//ZeroMemory(&fl, sizeof(FILEINFO));
		if (0 == lstrcmp(_T("."), FindData.cFileName) ||
			0 == lstrcmp(_T(".."), FindData.cFileName))
			continue;
		fl = FILEINFO{ FindData };
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			fl.isDir = true;
		else {
			fl.Size = FindData.nFileSizeLow;		//获取文件大小
			fl.Size |= (((__int64)FindData.nFileSizeHigh) << 32);
		}
		FLs.push_back(fl);
		wsprintf(buff, _T("%s\t%s\n"), fl.isDir ?
			_T("文件夹") : _T("文件"), FindData.cFileName);
		OutputDebugString(buff);
	} while (FindNextFile(FindHandle, &FindData));
	FindClose(FindHandle);
	return (int)FLs.size();
}
#pragma endregion
