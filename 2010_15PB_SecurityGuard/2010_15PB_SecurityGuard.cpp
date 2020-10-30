// 2010_15PB_SecurityGuard.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>

#include "..\Dll_API\CApi.h"
#include "..\Dll_API\CPE.h"
#include "..\Dll_API\MYINFO.h"
#pragma comment(lib,"..\\x64\\debug\\Dll_API.lib")



int main()
{
	std::cout << "Hello World!\n";
	char md5ch[33];
	char* filepath = DLLP1;
	__int64 fSize = 0;
	char* pFile = (char*)GetFilePtr(filepath, fSize, md5ch);
	CPE cPE = CPE(pFile, fSize);
	//cPE.GetNTHeadInfo(true);
	cPE.RvaToFoa(0);
	cPE.~CPE();
	filepath = DLLP2;
	pFile = (char*)GetFilePtr(filepath, fSize, md5ch);
	cPE = CPE(pFile, fSize);
	//cPE.GetNTHeadInfo(true);
	cPE.RvaToFoa(0);
	cPE.~CPE();
	HeapFree(GetProcessHeap(), 0, pFile);
	return 0;
}