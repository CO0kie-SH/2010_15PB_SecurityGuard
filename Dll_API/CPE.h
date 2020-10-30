#pragma once

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

typedef struct _PEHead_INFO
{
	DWORD	dwPEHead[16] = { 0 };
	char* szPEHead[16] = {
		defszPE_00入口点,
		defszPE_01镜像地址,
		defszPE_02镜像大小,
		defszPE_03代码基址,
		defszPE_04数据基址,
		defszPE_05快对齐,
		defszPE_06文件对齐,
		defszPE_07标志字,
		defszPE_08子系统,
		defszPE_09区段数目,
		defszPE_10日期时间标志,
		defszPE_11首部大小,
		defszPE_12特征值,
		defszPE_13校验和,
		defszPE_14可选头部大小,
		defszPE_15RVA数及大小
	};
	ULONGLONG   x64ImageBase;
}PEHead_INFO, * LPPEHead_INFO;

class _declspec(dllexport) CPE
{
public:
	CPE(char* pFile, ULONGLONG fileSize) ;
	~CPE();

	BOOL	isPE(char* lpImage);
	BOOL	GetNTHeadInfo(bool isPrint);
	DWORD	RvaToFoa(DWORD dwRva, bool isPrint = false);
public:
	SIZE_T			FOA;
	WORD			is32o64;
	PEHead_INFO		PEHead_Info;
private:
	PIMAGE_NT_HEADERS	_pNt;
};