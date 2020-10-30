#include "pch.h"
#include "CPE.h"
#include <stdio.h>


#pragma region 类内函数区
CPE::CPE(char* pFile, ULONGLONG fileSize)
	:_pNt(nullptr), FOA(0), is32o64(0)
{
	this->PEHead_Info.x64ImageBase = 0;
	if (isPE(pFile)) {
		this->FOA = (SIZE_T)pFile;
	}
}

CPE::~CPE()
{
}

/*
	函数：获取判断文件是否为PE文件
	返回：返回TRUE或FALSE
	作者：CO0kie丶
	时间：2020-10-30_15-20
*/
BOOL CPE::isPE(char* lpImage)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)lpImage;
	if (pDos == nullptr) {
		OutputDebugStringA("检测Dos头失败\n");
		return false;
	}
	if (pDos->e_magic == IMAGE_DOS_SIGNATURE)
	{
		OutputDebugStringA("检测到Dos头，指向PE偏移\n");
		PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + lpImage);
		if (pNt->Signature == IMAGE_NT_SIGNATURE)
		{
			_pNt = pNt;
			return true;
		}
	}
	return false;
}
/*
	函数：获取PE头部信息
	返回：返回TRUE或FALSE
	作者：CO0kie丶
	时间：2020-10-30_17-21
*/
BOOL CPE::GetNTHeadInfo(bool isPrint)
{
	if (FOA == 0)	return false;		//如果FOA为0则失败
	if (this->PEHead_Info.x64ImageBase)	//非0值表示以及获取过了
		return true;		
	//结构体[NT头]说明
	/*
typedef struct _IMAGE_NT_HEADERS {
	DWORD Signature;	//指向NT头标志：0x0000 4550 [PE..]
						//==宏[IMAGE_NT_SIGNATURE]
	IMAGE_FILE_HEADER FileHeader;			//指向[文件头]的结构体[IMAGE_FILE_HEADER]
	IMAGE_OPTIONAL_HEADER32 OptionalHeader;	//指向[扩展头]的结构体[IMAGE_OPTIONAL_HEADER]
} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;
*/
	//结构体[文件头]说明
	/*
	typedef struct _IMAGE_FILE_HEADER {	//[文件头]结构体
		WORD    Machine;				//文件运行的平台[i386/014C]
		WORD    NumberOfSections;		//★区段数量
		DWORD   TimeDateStamp;			//文件创建时间
		DWORD   PointerToSymbolTable;	//符号表偏移
		DWORD   NumberOfSymbols;		//符号个数
		WORD    SizeOfOptionalHeader;	//扩展头的大小[32/00E0][64/00F0]
		WORD    Characteristics;		//PE文件的属性
	} IMAGE_FILE_HEADER, * PIMAGE_FILE_HEADER;
	*/
	PIMAGE_FILE_HEADER pFileHeader = &_pNt->FileHeader;
	//结构体[PE信息头]说明
	/*
	typedef struct _IMAGE_OPTIONAL_HEADER {
		//
		// Standard fields.
		//

		WORD    Magic;	//★文件类型[32/010B][64/020B][ROM镜像/0170]
		BYTE    MajorLinkerVersion;		// 无用 连接器主版本
		BYTE    MinorLinkerVersion;		// 无用 连接器次版本
		DWORD   SizeOfCode;				//[有用]所有代码区段(节)的总大小
		DWORD   SizeOfInitializedData;	//[有用]已初始化数据的总大小
		DWORD   SizeOfUninitializedData;	//[有用]未初始化数据的总大小，
										//在磁盘中不占用空间，在加载进内存之后，
										//会预留这么大的空间，一般存储在.bss区段中
		DWORD   AddressOfEntryPoint;	//【★】程序开始执行的相对虚拟地(RVA),
									//也叫OEP，Orginal，Entry，Point，源入口点
		DWORD   BaseOfCode;			//[有用]起始代码的相对虚拟地址(RVA)
		DWORD   BaseOfData;			//[有用]起始数据的相对虚拟地址(RVA)

		//
		// NT additional fields.
		//

		DWORD   ImageBase;			//【★】默认加载基址(如果没有加载则为重定位)
		DWORD   SectionAlignment;	//【重】块对齐数，一般为0x1000
		DWORD   FileAlignment;		//【重】PE对齐数，一般为0x200
		WORD    MajorOperatingSystemVersion;	// 无用 主操作系统版本号
		WORD    MinorOperatingSystemVersion;	// 无用 次操作系统版本号
		WORD    MajorImageVersion;			// 无用 主映像版本
		WORD    MinorImageVersion;			// 无用 次映像版本
		WORD    MajorSubsystemVersion;		// 无用 主子系统版本
		WORD    MinorSubsystemVersion;		// 无用 次子系统版本
		DWORD   Win32VersionValue;		// 无用 保留值 一般为0
		DWORD   SizeOfImage;			//【重】要把文件加载进内存，
								//所需要的内存大小，注意是进行了块对齐之后
		DWORD   SizeOfHeaders;		//【重】所有头部大小
		DWORD   CheckSum;			// 无用 校验和
		WORD    Subsystem;			//[有用]子系统
		WORD    DllCharacteristics;	//[有用]指示DLL特征的标志
		DWORD   SizeOfStackReserve;	//[有用]表示进程中栈最大值,一般为1MB
		DWORD   SizeOfStackCommit;	//[有用]表示进程中栈初始值,一般为4KB
		DWORD   SizeOfHeapReserve;	//[有用]表示进程中堆最大值,一般为1MB
		DWORD   SizeOfHeapCommit;	//[有用]表示进程中堆初始值
		DWORD   LoaderFlags;
		DWORD   NumberOfRvaAndSizes;	//[有用]数据目录的个数，也就是↓数组元素的个数
										//【★】数据目录表，最大为16
		IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
	} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;
	*/
	PIMAGE_OPTIONAL_HEADER32 pOptiHeader = (PIMAGE_OPTIONAL_HEADER32)
		&_pNt->OptionalHeader;
	if (!pFileHeader || !pOptiHeader)	return false;
	this->is32o64 = pOptiHeader->Magic == 0x10B ? 32 :
		pOptiHeader->Magic == 0x20B ? 64 : 0;
	LPDWORD dwInfos = (LPDWORD)&this->PEHead_Info;
	dwInfos[Idx00入口点]		= pOptiHeader->AddressOfEntryPoint;
	dwInfos[Idx01镜像地址]	= pOptiHeader->ImageBase;
	dwInfos[Idx02镜像大小]	= pOptiHeader->SizeOfImage;
	dwInfos[Idx03代码基址]	= pOptiHeader->BaseOfCode;
	dwInfos[Idx04数据基址]	= pOptiHeader->BaseOfData;
	dwInfos[Idx05快对齐]		= pOptiHeader->SectionAlignment;
	dwInfos[Idx06文件对齐]	= pOptiHeader->FileAlignment;
	dwInfos[Idx07标志字]		= pOptiHeader->Magic;
	dwInfos[Idx08子系统]		= pOptiHeader->Subsystem;
	dwInfos[Idx09区段数目]	= pFileHeader->NumberOfSections;
	dwInfos[Idx10日期时间标志] = pFileHeader->TimeDateStamp;
	dwInfos[Idx11首部大小]	= pOptiHeader->SizeOfHeaders;
	dwInfos[Idx12特征值]		= pFileHeader->Characteristics;
	dwInfos[Idx13校验和]		= pOptiHeader->CheckSum;
	dwInfos[Idx14可选头部大小] = pFileHeader->SizeOfOptionalHeader;
	dwInfos[Idx15RVA数及大小] = pOptiHeader->NumberOfRvaAndSizes;
	this->PEHead_Info.x64ImageBase = dwInfos[Idx01镜像地址];
	if (this->is32o64 == 64) {
		PIMAGE_OPTIONAL_HEADER64 pOptiHeader64 = (PIMAGE_OPTIONAL_HEADER64)
			&_pNt->OptionalHeader;
		this->PEHead_Info.x64ImageBase = pOptiHeader64->ImageBase;
		dwInfos[Idx15RVA数及大小] = pOptiHeader64->NumberOfRvaAndSizes;
	}
	for (BYTE i = 0; i < 16; i++)
	{
		if (i == Idx01镜像地址 && this->is32o64 == 64) {
			printf_s("%-16s%0llX\n", this->PEHead_Info.szPEHead[i],
				this->PEHead_Info.x64ImageBase);
		}
		else {
			printf_s("%-16s%08lX\n", this->PEHead_Info.szPEHead[i],
				this->PEHead_Info.dwPEHead[i]);
		}
	}
	if (this->is32o64 == 0) {
		MessageBoxW(0, L"错误的标志字，请检查。", 0, 0);
		return false;
	}
	return true;
}
DWORD CPE::RvaToFoa(DWORD dwRva, bool isPrint /*= false*/)
{
	if (FOA == 0)	return NULL;	//如果FOA为0则失败
	//结构体[区段表]说明
	/*
typedef struct _IMAGE_SECTION_HEADER {
	BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];	//块名
	union {
			DWORD   PhysicalAddress;	//NULL
			DWORD   VirtualSize;		//2[有用]区段没有对齐前的实际大小
	} Misc;
	DWORD   VirtualAddress;			//3【★】段RVA
	DWORD   SizeOfRawData;			//4【★】段大小	+ RVA =段尾
	DWORD   PointerToRawData;		//5【★】段FOA	段FOA-段RVA=目标FOA-目标RVA
	DWORD   PointerToRelocations;	//6 没用 区段重定位信息的文件偏移
	DWORD   PointerToLinenumbers;	//7 没用 COFF行号信息的偏移
	WORD    NumberOfRelocations;	//8 没用 重定位信息的数目
	WORD    NumberOfLinenumbers;	//9 没用 行号信息的数目，只有COFF行号时才有用
	DWORD   Characteristics;		//10【★】区段属性：可读写执行
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;
*/
	PIMAGE_SECTION_HEADER pHeader = IMAGE_FIRST_SECTION(_pNt);
	//if (dwRva < _pNt->OptionalHeader.SizeOfHeaders)
	//	return dwRva;
	for (DWORD i = 0; i < _pNt->FileHeader.NumberOfSections; ++i)
	{
		DWORD& dwSectionRva = pHeader[i].VirtualAddress;
		DWORD& dwSectionLen = pHeader[i].SizeOfRawData;
		DWORD  dwSectionEnd = dwSectionRva + dwSectionLen;
		DWORD& dwSectionFoa = pHeader[i].PointerToRawData;
		DWORD& dwSectionVSize = pHeader[i].Misc.VirtualSize;
		/*
		目标：FOA=目标RVA-段RVA+段FOA
		变形：段FOA-段RVA=目标FOA-目标RVA
		原型：原始VA - 默认基址   =  新VA -  新基址
		*/
		DWORD dwFOA = dwRva - dwSectionRva + dwSectionFoa;

		printf_s("%lu\tR头%-5lX\tV大%5lX\t长%4lX\t尾%lX\t段Foa%4lX\t", i + 1,
			dwSectionRva, dwSectionVSize, dwSectionLen, dwSectionEnd, dwSectionFoa);
		printf_s("标志%lX\t%s\n", pHeader[i].Characteristics, pHeader[i].Name);
	}
	return NULL;
}
#pragma endregion
