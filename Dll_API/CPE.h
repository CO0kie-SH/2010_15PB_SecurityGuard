#pragma once
#include "CInclude.h"

#define		gdefPEFunctions_MAX		0x0B
#define		gdefNTHeadInfos_MAX		0x10
#define		gdefZONEInfos_MAX		0x07
#define		gdefTableInfos_MAX		0x10

enum _EnumPE功能区
{
	gdsz_PE文件头,
	gdsz_PE区段信息,
	gdsz_PE目录信息,
	gdsz_PE导入表信息,
	gdsz_PE导出表信息
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

constexpr	PTCHAR	gszZONEInfos[] = {
	(PTCHAR)_T("区段名"),
	(PTCHAR)_T("起始RVA"),
	(PTCHAR)_T("区段大小"),
	(PTCHAR)_T("RVA尾部"),
	(PTCHAR)_T("区段FOA"),
	(PTCHAR)_T("VOffset"),
	(PTCHAR)_T("标志")
};


constexpr	PTCHAR	gszTablesInfos[] = {
	(PTCHAR)_T("输出表"),
	(PTCHAR)_T("输入表"),
	(PTCHAR)_T("资源"),
	(PTCHAR)_T("例外"),
	(PTCHAR)_T("安全"),
	(PTCHAR)_T("重定位"),
	(PTCHAR)_T("调试"),
	(PTCHAR)_T("版权"),
	(PTCHAR)_T("全局指针"),
	(PTCHAR)_T("TLS表"),
	(PTCHAR)_T("载入配置"),
	(PTCHAR)_T("载入范围"),
	(PTCHAR)_T("IAT"),
	(PTCHAR)_T("延迟输入"),
	(PTCHAR)_T("COM"),
	(PTCHAR)_T("保留")
};

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

typedef struct _ZONE_INFO
{
	DWORD	dwNums;
	DWORD	dwSectionRva;
	DWORD	dwSectionLen;
	DWORD	dwSectionEnd;
	DWORD	dwSectionFoa;
	DWORD	dwSectionVSize;
	DWORD	dwCharacteristics;
	WCHAR	chName[9];
}ZONE_INFO, * LPZONE_INFO;


typedef struct _PETables_INFO
{
	DWORD	dwRVA	[gdefTableInfos_MAX];
	DWORD	dwSize	[gdefTableInfos_MAX];
}PETables_INFO, * LPPETables_INFO;



typedef struct _PEExport_INFO
{
	DWORD	dwIndex;
	DWORD	dwRVA;
	DWORD	dwFOA;
	char*	pApiName;
}PEExport_INFO, * LPPEExport_INFO;


typedef struct _PEExportTable
{
	DWORD	dwRVA;
	DWORD	dwNums;
	DWORD	dwNumsName;
	char*	pDLLName;
}PEExportTable, * LPPEExportTable;



class DLLexp CPE
{
public:
	CPE();
	~CPE();
	BOOL	Init(char* pFilePath);
	BOOL	Init(PTCHAR pFilePath);
	BOOL	IsPE(char* lpImage);
	BOOL	GetNTHeadInfo();
	DWORD	RvaToFoa(DWORD dwRva, bool isPrint = false,
		vector<ZONE_INFO>* zoneInfos = nullptr);


	BOOL	GetTableInfo();
	BOOL	GetExportInfo(vector<PEExport_INFO>& exportInfos);
	//PTCHAR	GetNTHeadTCHAR(BYTE i);
public:
	ULONGLONG		FOA;
	BYTE			is32o64;
	NTHead_INFO		NTHead_Info;
	ZONE_INFO		ZONE_Info;
	PETables_INFO	Table_Info;
	PEExportTable	ExportTable;
private:
	char*				_pFile = nullptr;
	__int64				_FileSize = 0;
	PIMAGE_NT_HEADERS	_pNt = nullptr;
};