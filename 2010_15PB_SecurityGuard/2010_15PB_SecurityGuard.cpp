// 2010_15PB_SecurityGuard.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include "..\Dll_API\CApi.h"
#pragma comment(lib,"..\\x64\\Debug\\Dll_API.lib")

int main()
{
	std::cout << "Hello World!\n";
	CMyServer cSV;
	vector<SERVICEINFO> SVs;
	cSV.EnumServer(SVs);
	
	return 0;
}