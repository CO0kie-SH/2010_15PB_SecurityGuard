// 2010_15PB_SecurityGuard.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>

#include "..\Dll_API\CApi.h"
#include "..\Dll_API\CPE.h"
#pragma comment(lib,"..\\x64\\debug\\Dll_API.lib")



int main()
{
	std::cout << "Hello World!\n";
	char md5ch[33];
	//char* filepath = (char*)"D:\\Videos\\20200923_脱口秀大会第三季.mp4";
	//char* filepath = (char*)
	//	"D:\\Videos\\七宗罪.Se7en.1995.BD-1080p.1920X1080.中英双语-电波字幕组.mkv";
	//DWORD time = GetMd5_ByCertutil((char*)
	//	"D:\\Videos\\20200923_脱口秀大会第三季.mp4", md5ch);
	char* filepath = (char*)"D:\\15pb\\课堂\\20Q3\\0930_DLL\\Debug\\0930_DLL.dll";
	__int64 fSize = 0;
	char* pFile = (char*)GetFilePtr(filepath, fSize, md5ch);
	printf_s("文件名：%s\n", filepath);
	CPE cPE = CPE(pFile, fSize);
	//cPE.GetNTHeadInfo(true);
	cPE.RvaToFoa(0);
	cPE.~CPE();
	filepath = (char*)"D:\\15pb\\课堂\\20Q3\\0930_DLL\\x64\\Debug\\0930_DLL.dll";
	pFile = (char*)GetFilePtr(filepath, fSize, md5ch);
	printf_s("文件名：%s\n", filepath);
	cPE = CPE(pFile, fSize);
	//cPE.GetNTHeadInfo(true);
	cPE.RvaToFoa(0);
	cPE.~CPE();
	HeapFree(GetProcessHeap(), 0, pFile);
	return 0;
}