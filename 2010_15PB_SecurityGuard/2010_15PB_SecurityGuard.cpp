// 2010_15PB_SecurityGuard.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include "..\Dll_API\CApi.h"
#pragma comment(lib,"..\\x64\\Debug\\Dll_API.lib")

static __int64 iDirNum = 0, iFileNum = 0, iSize = 0;
//定义带参回调函数
void FileCallBack(LPFILEINFO pFileInfo)
{
	bool& isDir = pFileInfo->isDir;
	if (isDir)
		++iDirNum;
	else {
		++iFileNum;
		//iSize += pFileInfo->Size;

		
		PTCHAR pName = pFileInfo->data.cFileName;
		PTCHAR pKind = PathFindExtension(pName);
		for (BYTE i = 0; i < gdefRBVS_MAX; i++)
		{
			if (0 == lstrcmp(gszRBVS[i], pKind)) {
				char chMd5[33];
				ZeroMemory(chMd5, 33);
				//PTCHAR fpath = (PTCHAR)L"D:\\15pb\\del.bat";
				TCHAR buff[MAX_PATH];
				_tcscpy_s(buff, MAX_PATH, pFileInfo->path);
				_tcscat_s(buff, MAX_PATH, pFileInfo->data.cFileName);
				GetMd5_ByCertutil(buff, chMd5);
				printf_s("%32s\t", chMd5);
				wprintf_s(L"%llu\t%9llX\t%-9s\t%s\n", iFileNum,
					pFileInfo->Size, pKind,
					pName);
				iSize += pFileInfo->Size;

				break;
			}
		}
	}
}


//在main函数中实现带参的函数回调
int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "chs");

	CHeap cHeap;
	PTCHAR path = (PTCHAR)L"D:\\15pb\\";
	EnumFilePaths(FileCallBack, path, &cHeap);
	TCHAR buff[MAX_PATH];
	swprintf_s(buff, MAX_PATH, L"%s\n文件夹\t%llu\n文件\t%llu\n大小",
		path, iDirNum, iFileNum);
	OutputDebugString(buff);
	StrFormatByteSize64(iSize, buff, MAX_PATH);
	OutputDebugString(buff);
	return 0;
}