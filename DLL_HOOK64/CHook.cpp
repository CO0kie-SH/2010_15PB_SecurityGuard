#include "pch.h"
#include "CHook.h"

CHook gHook;
int WINAPI MyMessageBoxW(			//����ԭ��
	_In_opt_ HWND hWnd,						//�����ھ��
	_In_opt_ LPCWSTR lpText,				//����
	_In_opt_ LPCWSTR lpCaption,				//����
	_In_ UINT uType)						//��ť��Ϣ��
{
	lpText = (wchar_t*)L"�����޸�����";		//�޸�����
	//gHook.UnHook();					//��ԭUser32.BoxW
	int dwReturnCode = MessageBoxW(hWnd, lpText, lpCaption, uType);
	//gHook.DoHook();					//������BoxW
	return dwReturnCode;					//���ص�����ť
};

CHook::CHook()
{
}

CHook::~CHook()
{
}
