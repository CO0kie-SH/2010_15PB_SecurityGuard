#include "pch.h"
#include "CMyFile.h"




#pragma region ���ں���
CMyFile::CMyFile()
{
}

CMyFile::~CMyFile()
{
}

/*
	�����������ļ���
	���أ��ɹ���������������Ϊ0
	���ߣ�CO0kieؼ
	ʱ�䣺2020-10-31_14-00
*/
BOOL CMyFile::SearchPaths(vector<FILEINFO>& FLs, PTCHAR Path,
	PTCHAR pFileType /*= nullptr*/, bool isClear /*= true*/)
{
	if (nullptr == Path)		return false;		//���·���������򷵻�
	if (!PathFileExists(Path))	return false;		//����ļ��������򷵻�
	if (!PathIsDirectory(Path))	return false;		//���Ŀ¼�������򷵻�
	if (PathIsDirectoryEmpty(Path))	return false;	//�����Ŀ¼�򷵻�

	TCHAR buff[MAX_PATH];
	wsprintf(buff, _T("%s"), Path);
	if (nullptr != pFileType)
		_tcscat_s(buff, MAX_PATH, pFileType);
	

	WIN32_FIND_DATA FindData;	FILEINFO fl;		//��ʼ���ļ���Ϣ
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
			fl.Size = FindData.nFileSizeLow;		//��ȡ�ļ���С
			fl.Size |= (((__int64)FindData.nFileSizeHigh) << 32);
		}
		FLs.push_back(fl);
		wsprintf(buff, _T("%s\t%s\n"), fl.isDir ?
			_T("�ļ���") : _T("�ļ�"), FindData.cFileName);
		OutputDebugString(buff);
	} while (FindNextFile(FindHandle, &FindData));
	FindClose(FindHandle);
	return (int)FLs.size();
}
#pragma endregion
