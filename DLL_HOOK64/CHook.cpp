#include "pch.h"
#include "CHook.h"

CHook gHook;
int WINAPI MyMessageBoxW(			//函数原型
	_In_opt_ HWND hWnd,						//父窗口句柄
	_In_opt_ LPCWSTR lpText,				//内容
	_In_opt_ LPCWSTR lpCaption,				//标题
	_In_ UINT uType)						//按钮信息等
{
	lpText = (wchar_t*)L"这是修改内容";		//修改内容
	//gHook.UnHook();					//还原User32.BoxW
	int dwReturnCode = MessageBoxW(hWnd, lpText, lpCaption, uType);
	//gHook.DoHook();					//继续钩BoxW
	return dwReturnCode;					//返回弹窗按钮
};

CHook::CHook()
{
}

CHook::~CHook()
{
}
