// 2010_15PB_SecurityGuard.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include "..\Dll_API\CApi.h"
#pragma comment(lib,"..\\x64\\Debug\\Dll_API.lib")

typedef void(*FILECALLBACK)(LPFILEINFO);


void Print(LPFILEINFO fIle) {

}

int main() {
    FILECALLBACK f = &Print;
    f(nullptr);  //调用函数
    return 0;
}



////定义带参回调函数
//void FileCallBack(LPFILEINFO pFileInfo)
//{
//	
//}​
//
////在main函数中实现带参的函数回调
//int main()
//{
//	setlocale(LC_ALL, "chs");
//	//CHeap cHeap;
//	//PTCHAR path = (PTCHAR)L"D:\\15pb\\";
//	//EnumFilePaths(FileCallBack, path, &cHeap);
//	//TCHAR buff[MAX_PATH];
//	//swprintf_s(buff, MAX_PATH, L"%s\n文件夹\t%llu\n文件\t%llu\n大小",
//	//	path, iDirNum, iFileNum);
//	//OutputDebugString(buff);
//	//StrFormatByteSize64(iSize, buff, MAX_PATH);
//	//OutputDebugString(buff);
//	return 0;
//}