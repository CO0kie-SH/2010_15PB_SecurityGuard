#pragma once
#include "CInclude.h"

#define		gdefPEFunctions_MAX		0x0B
#define		gdefNTHeadInfos_MAX		0x10

enum _EnumPE功能区
{
	gdsz_PE文件头,
	gdsz_PE区段信息
};



constexpr	PTCHAR	gszPEFunctions[] = {
	(PTCHAR)_T("PE文件头"),
	(PTCHAR)_T("区段信息"),
	(PTCHAR)_T("目录信息"),
	(PTCHAR)_T("导 入  表"),
	(PTCHAR)_T("导 出  表"),
	(PTCHAR)_T("资 源  表"),
	(PTCHAR)_T("重定位表"),
	(PTCHAR)_T("延 迟  表"),
	(PTCHAR)_T("T L S 表"),
	(PTCHAR)_T("地址转换"),
	(PTCHAR)_T("时间转换")
};

constexpr	PTCHAR gszNTHeadInfos[]	= {
	(PTCHAR)_T("入口点："),
	(PTCHAR)_T("镜像地址："),
	(PTCHAR)_T("镜像大小："),
	(PTCHAR)_T("代码基址："),
	(PTCHAR)_T("数据基址："),
	(PTCHAR)_T("快对齐："),
	(PTCHAR)_T("文件对齐："),
	(PTCHAR)_T("标志字："),
	(PTCHAR)_T("子系统："),
	(PTCHAR)_T("区段数目："),
	(PTCHAR)_T("日期时间标志："),
	(PTCHAR)_T("首部大小："),
	(PTCHAR)_T("特征值："),
	(PTCHAR)_T("校验和："),
	(PTCHAR)_T("可选头部大小："),
	(PTCHAR)_T("RVA数及大小：")
};


/*
#define	defszPE_00入口点			(char*)"入口点："
#define	defszPE_01镜像地址		(char*)"镜像地址："
#define	defszPE_02镜像大小		(char*)"镜像大小："
#define	defszPE_03代码基址		(char*)"代码基址："
#define	defszPE_04数据基址		(char*)"数据基址："
#define	defszPE_05快对齐			(char*)"快对齐："
#define	defszPE_06文件对齐		(char*)"文件对齐："
#define	defszPE_07标志字			(char*)"标志字："
#define	defszPE_08子系统			(char*)"子系统："
#define	defszPE_09区段数目		(char*)"区段数目："
#define	defszPE_10日期时间标志	(char*)"日期时间标志："
#define	defszPE_11首部大小		(char*)"首部大小："
#define	defszPE_12特征值			(char*)"特征值："
#define	defszPE_13校验和			(char*)"校验和："
#define	defszPE_14可选头部大小	(char*)"可选头部大小："
#define	defszPE_15RVA数及大小		(char*)"RVA数及大小："
*/

enum IDXPEINFO
{
	Idx00入口点 = 0,
	Idx01镜像地址 = 1,
	Idx02镜像大小 = 2,
	Idx03代码基址 = 3,
	Idx04数据基址 = 4,
	Idx05快对齐 = 5,
	Idx06文件对齐 = 6,
	Idx07标志字 = 7,
	Idx08子系统 = 8,
	Idx09区段数目 = 9,
	Idx10日期时间标志 = 10,
	Idx11首部大小 = 11,
	Idx12特征值 = 12,
	Idx13校验和 = 13,
	Idx14可选头部大小 = 14,
	Idx15RVA数及大小 = 15,
};



typedef struct _NTHead_INFO
{
	DWORD	dwPEHead[16];
	//char* szPEHead[16] = {}
	ULONGLONG   x64ImageBase;
}NTHead_INFO, * LPNTHead_INFO;

class DLLexp CPE
{
public:
	CPE();
	~CPE();
	BOOL	Init(char* pFilePath);
	BOOL	Init(PTCHAR pFilePath);
	BOOL	IsPE(char* lpImage);
	BOOL	GetNTHeadInfo();
	DWORD	RvaToFoa(DWORD dwRva, bool isPrint = false);
	//PTCHAR	GetNTHeadTCHAR(BYTE i);
public:
	ULONGLONG		FOA;
	BYTE			is32o64;
	NTHead_INFO		NTHead_Info;
private:
	char*				_pFile = nullptr;
	__int64				_FileSize = 0;
	PIMAGE_NT_HEADERS	_pNt = nullptr;
};