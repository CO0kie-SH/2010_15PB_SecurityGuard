#pragma once


#include <shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

#define DLLexp						extern _declspec(dllexport)
#define DLLexpC						extern "C" _declspec(dllexport)
#define defExitID���������˳�			0x0000;
#define defExitID�޷��������ļ�		0x00F1


extern "C" _declspec(dllexport)
DWORD	GetMd5_ByCertutil(char* pPath, char* md5);
extern "C" _declspec(dllexport)
void*	GetFilePtr(char* pPath, __int64& pSize, char* str = nullptr);

class CApi
{
};

