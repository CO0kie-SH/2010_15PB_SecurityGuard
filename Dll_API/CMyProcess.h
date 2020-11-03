#pragma once
#include "CInclude.h"

#define		gdefPCFunctions_MAX		0x01//3
#define		gdefMDFunctions_MAX		0x01//3
#define		gdefTHFunctions_MAX		0x01//5


#define		gdsz_枚举进程				0x00
#define		gdsz_枚举线程				0x00
#define		gdsz_枚举模块				0x00
constexpr	PTCHAR	gszPCFunctions[] = {
	(PTCHAR)_T("枚举进程"),
	(PTCHAR)_T("结束进程"),
	(PTCHAR)_T("强制结束进程")
};

constexpr	PTCHAR	gszMDFunctions[] = {
	(PTCHAR)_T("枚举模块"),
	(PTCHAR)_T("加载模块"),
	(PTCHAR)_T("卸载模块")
};

constexpr	PTCHAR	gszTHFunctions[] = {
	(PTCHAR)_T("枚举线程"),
	(PTCHAR)_T("结束线程"),
	(PTCHAR)_T("挂起线程"),
	(PTCHAR)_T("恢复线程"),
	(PTCHAR)_T("强制结束线程")
};


typedef struct _THREADINFO {	//结构体[线程信息]
	DWORD   th32ThreadID;       //线程ID
	DWORD   th32OwnerProcessID; //进程PID
	LONG    tpBasePri;			//进程优先级
	SYSTEMTIME thCreatTime;		//线程创建时间
}THREADINFO, * LPTHREADINFO;


typedef struct _MODULEINFO {	//结构体[模块信息]
	DWORD	pPID;				//所属进程ID
	DWORD	size;				//模块的大小
	LPVOID	modBaseAddr;		//模块的加载基地址
	WCHAR   name[MAX_PATH];		//模块名
	WCHAR   path[MAX_PATH];		//所在路径
}MODULEINFO, * LPMODULEINFO;


typedef struct _PROCESSINFO {	//结构体[进程信息]
	DWORD	tPID;				//本PID
	DWORD	pPID;				//父PID
	DWORD	tTHs;				//线程数
	DWORD	tMDs;				//模块数
	BYTE	is32;				//进程位数
	//LPMODULEINFO pMD;			//模块信息
	TCHAR	name[MAX_PATH];		//进程名
	//TCHAR	path[MAX_PATH];
}PROCESSINFO, * LPPROCESSINFO;


class DLLexp CMyProcess
{
public:
	CMyProcess();
	~CMyProcess();

	BOOL EnumProcess(vector<PROCESSINFO>* proInfos,
		bool bGetMod = false, bool bCleanMem = false);
	BOOL EnumThread(vector<THREADINFO>& threadInfos, DWORD dwPid = 0);
	BOOL EnumModule(vector<MODULEINFO>& moduleInfos, DWORD dwPid = 0);
private:

};

