#pragma once
#include "CInclude.h"

#include "CMyFile.h"
#include "CPE.h"
#include "CMyProcess.h"
#include "CMyHwnd.h"
#include "CMyServer.h"


#define defExitID程序正常退出			0x0000;
#define defExitID无法解析大文件		0x00F1


extern "C" _declspec(dllexport)		
DWORD	GetMd5_ByCertutil(char* pPath, char md5[33]);
extern "C" _declspec(dllexport)
void*	GetFilePtr(char* pPath, __int64& pSize, char* strSize = nullptr);

class CApi
{
};

