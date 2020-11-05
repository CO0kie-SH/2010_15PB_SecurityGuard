#include "pch.h"
#include "CPE.h"

#include "CApi.h"

#pragma region 类内函数区

CPE::CPE() :FOA(0), is32o64(0)
{
	ZeroMemory(&this->NTHead_Info, sizeof(NTHead_INFO));
	ZeroMemory(&this->ZONE_Info, sizeof(ZONE_INFO));
	ZeroMemory(&this->ExportTable, sizeof(PEExportTable));
}


CPE::~CPE()
{
	if (_pFile)
		HeapFree(GetProcessHeap(), 0, _pFile);
}

BOOL CPE::Init(char* pFilePath)
{
	FOA = 0;	is32o64 = 0;
	ZeroMemory(&this->NTHead_Info, sizeof(NTHead_INFO));
	ZeroMemory(&this->ZONE_Info, sizeof(ZONE_INFO));
	ZeroMemory(&this->ExportTable, sizeof(PEExportTable));
	if (_pFile)
		HeapFree(GetProcessHeap(), 0, _pFile);
	_pFile = (char*)GetFilePtr(pFilePath, this->_FileSize);
	if (_pFile == nullptr)	return false;
	if (IsPE(_pFile))
		return true;
	HeapFree(GetProcessHeap(), 0, _pFile);
	_pFile = nullptr;
	return false;
}

BOOL CPE::Init(PTCHAR pFilePath)
{
	char buff[512];
	size_t len = 0;
	wcstombs_s(&len, buff, pFilePath, 512);
	if (len == 0)
		return false;
	return Init(buff);
}

/*
	函数：获取判断文件是否为PE文件
	返回：返回TRUE或FALSE
	作者：CO0kie丶
	时间：2020-10-30_15-20
*/
BOOL CPE::IsPE(char* lpImage)
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
			_pNt = pNt;	FOA = (ULONGLONG)lpImage;
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
BOOL CPE::GetNTHeadInfo()
{
	if (FOA == 0)	return false;		//如果FOA为0则失败
	if (this->NTHead_Info.dwPEHead[0])	//非0值表示以及获取过了
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
	LPDWORD dwInfos = (LPDWORD)&this->NTHead_Info;
	dwInfos[Idx00入口点] = pOptiHeader->AddressOfEntryPoint;
	dwInfos[Idx01镜像地址] = pOptiHeader->ImageBase;
	dwInfos[Idx02镜像大小] = pOptiHeader->SizeOfImage;
	dwInfos[Idx03代码基址] = pOptiHeader->BaseOfCode;
	dwInfos[Idx04数据基址] = pOptiHeader->BaseOfData;
	dwInfos[Idx05快对齐] = pOptiHeader->SectionAlignment;
	dwInfos[Idx06文件对齐] = pOptiHeader->FileAlignment;
	dwInfos[Idx07标志字] = pOptiHeader->Magic;
	dwInfos[Idx08子系统] = pOptiHeader->Subsystem;
	dwInfos[Idx09区段数目] = pFileHeader->NumberOfSections;
	dwInfos[Idx10日期时间标志] = pFileHeader->TimeDateStamp;
	dwInfos[Idx11首部大小] = pOptiHeader->SizeOfHeaders;
	dwInfos[Idx12特征值] = pFileHeader->Characteristics;
	dwInfos[Idx13校验和] = pOptiHeader->CheckSum;
	dwInfos[Idx14可选头部大小] = pFileHeader->SizeOfOptionalHeader;
	dwInfos[Idx15RVA数及大小] = pOptiHeader->NumberOfRvaAndSizes;
	if (this->is32o64 == 64) {
		PIMAGE_OPTIONAL_HEADER64 pOptiHeader64 = (PIMAGE_OPTIONAL_HEADER64)
			&_pNt->OptionalHeader;
		this->NTHead_Info.x64ImageBase = pOptiHeader64->ImageBase;
		dwInfos[Idx15RVA数及大小] = pOptiHeader64->NumberOfRvaAndSizes;
	}
	if (this->is32o64 == 0) {
		MessageBoxW(0, L"错误的标志字，请检查。", 0, 0);
		return false;
	}
	return true;
}
DWORD CPE::RvaToFoa(DWORD dwRva, bool isPrint /*= false*/,
	vector<ZONE_INFO>* zoneInfos /*= nullptr*/)
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
	if (dwRva > 400 && dwRva < _pNt->OptionalHeader.SizeOfHeaders)
		return dwRva;
	else if (dwRva > 0x05 && dwRva < _pNt->OptionalHeader.SizeOfHeaders)
		return dwRva;
	char buff[MAX_PATH];
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
		if (isPrint) {
			wsprintfA(buff, "%lu\tR头%-5lX\tV大%5lX\t长%4lX\t尾%lX\t段Foa%4lX\t",
				i + 1, dwSectionRva, dwSectionVSize,
				dwSectionLen, dwSectionEnd, dwSectionFoa);
			OutputDebugStringA(buff);
			wsprintfA(buff, "标志%lX\t%s\n",
				pHeader[i].Characteristics, pHeader[i].Name);
			OutputDebugStringA(buff);

		}
		if (zoneInfos) {
			wsprintfA(buff, "%s", pHeader[i].Name);

			this->ZONE_Info.dwNums = _pNt->FileHeader.NumberOfSections;
			this->ZONE_Info.dwSectionRva = dwSectionRva;
			this->ZONE_Info.dwSectionLen = dwSectionLen;
			this->ZONE_Info.dwSectionEnd = dwSectionEnd;
			this->ZONE_Info.dwSectionFoa = dwSectionFoa;
			this->ZONE_Info.dwSectionVSize = dwSectionVSize;
			this->ZONE_Info.dwCharacteristics = pHeader[i].Characteristics;
			mbstowcs_s(nullptr, this->ZONE_Info.chName, buff, 8);
			zoneInfos->push_back(this->ZONE_Info);
		}
		if (dwRva >= dwSectionRva && dwRva < dwSectionEnd) {
			return dwFOA;
		}
	}
	return NULL;
}

DWORD CPE::FoaToRva(DWORD dwFoa)
{
	if (FOA == 0)	return false;

	PIMAGE_SECTION_HEADER pHeader = IMAGE_FIRST_SECTION(_pNt);
	if (dwFoa > 0 && dwFoa < _pNt->OptionalHeader.SizeOfHeaders)
		return dwFoa;
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
		//DWORD dwFOA = dwRva - dwSectionRva + dwSectionFoa;
		DWORD dwRva = dwFoa + dwSectionRva - dwSectionFoa;
		if (dwRva >= dwSectionRva && dwRva < dwSectionEnd) {
			return dwRva;
		}

	}

	return 0;
}

/*
	函数：获取目录表
	返回：成功true，失败false
	作者：CO0kie丶
	时间：2020-11-03_09-00
*/
BOOL CPE::GetTableInfo()
{
	if (FOA == 0)	return false;
	PIMAGE_OPTIONAL_HEADER32 pOption32 = (PIMAGE_OPTIONAL_HEADER32)
		&_pNt->OptionalHeader;
	PIMAGE_OPTIONAL_HEADER64 pOption64 = (PIMAGE_OPTIONAL_HEADER64)
		&_pNt->OptionalHeader;

	PIMAGE_DATA_DIRECTORY pTable = this->is32o64 == 32 ? pOption32->DataDirectory :
		this->is32o64 == 64 ? pOption64->DataDirectory : nullptr;
	if (pTable == nullptr)	return false;


	PDWORD pdwRVA = (PDWORD)&Table_Info.dwRVA;
	PDWORD pdwSize = (PDWORD)&Table_Info.dwSize;
	ZeroMemory(&Table_Info, sizeof(PETables_INFO));


	for (BYTE i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; ++i, ++pTable)
	{
		pdwRVA[i] = pTable->VirtualAddress;
		pdwSize[i] = pTable->Size;
		//Table_Info.chName[i] = gszTablesInfos[i];
		//wsprintf(buff, _T("%d\t%08lX\t"),i, pTable->VirtualAddress);
		//OutputDebugString(buff);
		//wsprintf(buff, _T("%08lX\n"), pTable->Size);
		//OutputDebugString(buff);
	}
	return true;
}

/*
	函数：获取导出表
	返回：成功true，失败false
	作者：CO0kie丶
	时间：2020-11-03_14-00
*/
BOOL CPE::GetExportInfo(vector<PEExport_INFO>& exportInfos)
{
	if (FOA == 0)	return false;
	PIMAGE_OPTIONAL_HEADER32 pOption32 = (PIMAGE_OPTIONAL_HEADER32)
		&_pNt->OptionalHeader;
	PIMAGE_OPTIONAL_HEADER64 pOption64 = (PIMAGE_OPTIONAL_HEADER64)
		&_pNt->OptionalHeader;

	PIMAGE_DATA_DIRECTORY pTable = this->is32o64 == 32 ? pOption32->DataDirectory :
		this->is32o64 == 64 ? pOption64->DataDirectory : nullptr;
	if (pTable == nullptr)	return false;


	DWORD dwExportFOA = RvaToFoa(pTable->VirtualAddress);
	PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)(FOA + dwExportFOA);
	//	导出表结构体
	/*
typedef struct _IMAGE_EXPORT_DIRECTORY {
	DWORD   Characteristics;	//1 没用 为0
	DWORD   TimeDateStamp;		//2 没用 和文件头时间一样
	WORD    MajorVersion;		//3 没用 主版本号
	WORD    MinorVersion;		//4 没用 次版本号
	DWORD   Name;				//5[有用]模块DLL名
	DWORD   Base;				//6[有用]函数的起始序号
	DWORD   NumberOfFunctions;	//7【★】函数数量
	DWORD   NumberOfNames;		//8【★】函数名数量
	DWORD   AddressOfFunctions;		//09【★】函数地址表RVA
	DWORD   AddressOfNames;			//10【★】函数名称表RVA
	DWORD   AddressOfNameOrdinals;	//11【★】函数序号表RVA
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;
*/


	DWORD EatFoa = RvaToFoa(pExport->AddressOfFunctions);
	DWORD EntFoa = RvaToFoa(pExport->AddressOfNames);
	DWORD EotFoa = RvaToFoa(pExport->AddressOfNameOrdinals);
	//导出函数地址表
	//导出函数名称表
	//导出函数序号表
	PDWORD	pEat = (PDWORD)(FOA + EatFoa);
	PDWORD	pEnt = (PDWORD)(FOA + EntFoa);
	PWORD	pEot = (PWORD)(FOA + EotFoa);
	DWORD	dwFoa = RvaToFoa(pExport->Name);
	if (dwFoa == 0)	return false;


	this->ExportTable.pDLLName = (char*)FOA + dwFoa;
	//TCHAR buff[MAX_PATH];
	PEExport_INFO tmp;
	for (DWORD i = 0; i < pExport->NumberOfFunctions; i++)
	{
		//ZeroMemory(&tmp, sizeof(PEExport_INFO));
		//wsprintf(buff, _T("\n%lu\t序号表RVA=0x%lX\t"), i + 1, pEat[i]);
		//OutputDebugString(buff);
		if (pEat[i] == 0)	continue;
		tmp = PEExport_INFO{
			i + 1, pEat[i], RvaToFoa(pEat[i]),(char*)"-"
		};
		//OutputDebugString(_T("名称表：\n"));
		for (DWORD j = 0; j < pExport->NumberOfFunctions; j++)
		{
			//wsprintf(buff, _T("%lu\t名称表%lu\n"), j + 1, pEot[j]);
			//OutputDebugString(buff);
			if (i == pEot[j]) {
				dwFoa = RvaToFoa(pEnt[j]);
				tmp.pApiName = (char*)FOA + dwFoa;
				break;
			}
		}
		exportInfos.push_back(tmp);
	}
	return !exportInfos.empty();
}


/*
	函数：获取导出表
	返回：成功true，失败false
	作者：CO0kie丶
	时间：2020-11-03_14-00
*/
BOOL CPE::GetImportInfo(vector<PEImport_INFO>& importInfos)
{

	//结构体说明
	/*
typedef struct _IMAGE_IMPORT_DESCRIPTOR {	//导出表结构体
	union {
		DWORD   Characteristics;
		DWORD   OriginalFirstThunk;		//1【★】指向结构体数组的RVA，结构体叫做
										//输入名称表[INT:Import Name Table]
										//结构体[IMAGE_THUNK_DATA]
	} DUMMYUNIONNAME;
	DWORD   TimeDateStamp;			//当可执行文件不与被输入的DLL进行绑定时,此字段为0
	DWORD   ForwarderChain;			// -1 if no forwarders	转发机制用到
									//第一个被转向的api的索引
	DWORD   Name;					//4[有用]导入的PE文件的名字RVA
	DWORD   FirstThunk;				//5【★】指向一个结构体数组的RVA，结构体叫做
									//输入地址表[IAT:Import Address Table]
									//与INT共用结构体[IMAGE_THUNK_DATA]
									//在磁盘文件中与INT相同,可以看作INT是IAT的备份
} IMAGE_IMPORT_DESCRIPTOR;
typedef IMAGE_IMPORT_DESCRIPTOR UNALIGNED *PIMAGE_IMPORT_DESCRIPTOR;

//3.在有些文件中输入名称表是空的，全零，什么都没有。
//	这说明输入地址表有时没有备份。所以解析输入表时用IAT解析。

typedef struct _IMAGE_THUNK_DATA32 {
	union {
		DWORD ForwarderString;		//转发时用到
		DWORD Function;				//导入函数的地址，在加载到内存后起作用
		DWORD Ordinal;				//假如是序号导入的，用到这里
		DWORD AddressOfData;		//假如是函数名导入的，用这里，
	} u1;							//指向一个结构体[IMAGE_IMPORT_BY_NAME]
} IMAGE_THUNK_DATA32;
typedef IMAGE_THUNK_DATA32 * PIMAGE_THUNK_DATA32;

typedef struct _IMAGE_IMPORT_BY_NAME {
	WORD    Hint;
	CHAR   Name[1];
} IMAGE_IMPORT_BY_NAME, *PIMAGE_IMPORT_BY_NAME;
*/


	if (FOA == 0)	return false;
	PIMAGE_DATA_DIRECTORY dwImportDir =
		(PIMAGE_DATA_DIRECTORY)GetDataDirectory(1);
	if (dwImportDir == nullptr)	return false;

	DWORD dwFOA = RvaToFoa(dwImportDir->VirtualAddress);	//得到区段表
	PIMAGE_IMPORT_DESCRIPTOR pImportTable =					//获取导入表
		(PIMAGE_IMPORT_DESCRIPTOR)(FOA + dwFOA);			//的内存地址






	dwFOA = RvaToFoa(pImportTable->Name);
	PEImport_INFO tmp;
	DWORD dwNum = 0;
	constexpr char* pNULL = (char*)"-";
	while (pImportTable->Name != 0)
	{
		dwFOA = RvaToFoa(pImportTable->Characteristics);
		tmp = { 0,pImportTable->Characteristics,dwFOA,0,0,pNULL };
		char* pDllName = (char*)FOA + dwFOA;
		tmp.pApiName = pDllName;
		importInfos.push_back(tmp);
		tmp = PEImport_INFO{
				dwNum,
				pImportTable->OriginalFirstThunk,
				RvaToFoa(pImportTable->OriginalFirstThunk),
				pImportTable->FirstThunk,
				0,pNULL
		};

		PIMAGE_THUNK_DATA64 pNameTable64 = NULL;
		PIMAGE_THUNK_DATA32 pNameTable = NULL;
		dwFOA = RvaToFoa(pImportTable->FirstThunk);		//使用IAT解析
		pNameTable = (PIMAGE_THUNK_DATA32)(FOA + dwFOA);
		pNameTable64 = (PIMAGE_THUNK_DATA64)(FOA + dwFOA);
		if (this->is32o64 == 32) {
			while (pNameTable->u1.Ordinal != 0)
			{
				tmp.dwIndex = ++dwNum;
				tmp.pApiName = pNULL;
				dwFOA = RvaToFoa(pNameTable->u1.Ordinal);
				if (IMAGE_SNAP_BY_ORDINAL(pNameTable->u1.Ordinal) == 1)
				{
					tmp.Thunk = pNameTable->u1.AddressOfData;
				}
				else
				{
					PIMAGE_IMPORT_BY_NAME pName = (PIMAGE_IMPORT_BY_NAME)(FOA + dwFOA);
					tmp.Thunk = pNameTable->u1.Ordinal;
					tmp.wdHint = pName->Hint;
					tmp.pApiName = pName->Name;
				}
				importInfos.push_back(tmp);
				++pNameTable;
				tmp.dwRVA += 4;	tmp.dwFOA += 4;
			}
		}
		else if (this->is32o64 == 64) {
			while (pNameTable64->u1.Ordinal != 0)
			{
				tmp.dwIndex = ++dwNum;
				tmp.pApiName = pNULL;
				dwFOA = RvaToFoa((DWORD)pNameTable64->u1.Ordinal);
				if (IMAGE_SNAP_BY_ORDINAL(pNameTable64->u1.Ordinal) == 1)
				{
					tmp.Thunk = pNameTable64->u1.AddressOfData;
				}
				else
				{
					PIMAGE_IMPORT_BY_NAME pName = (PIMAGE_IMPORT_BY_NAME)(FOA + dwFOA);
					tmp.Thunk = pNameTable64->u1.Ordinal;
					tmp.wdHint = pName->Hint;
					tmp.pApiName = pName->Name;
				}
				importInfos.push_back(tmp);
				++pNameTable64;
				tmp.dwRVA += 4;	tmp.dwFOA += 4;
			}
		}
		++pImportTable;
	}
	return !importInfos.empty();
}

BOOL CPE::GetTLSInfo(ULONGLONG Info[6])
{

	if (FOA == 0)	return false;
	PIMAGE_DATA_DIRECTORY dwImportDir =
		(PIMAGE_DATA_DIRECTORY)GetDataDirectory(9);
	if (dwImportDir == nullptr)	return false;

	PIMAGE_TLS_DIRECTORY32 pTls = (PIMAGE_TLS_DIRECTORY32)(
		RvaToFoa(dwImportDir->VirtualAddress)
		+ FOA);
	PIMAGE_TLS_DIRECTORY64 pTls64 = (PIMAGE_TLS_DIRECTORY64)(
		RvaToFoa(dwImportDir->VirtualAddress)
		+ FOA);
	//遍历TLS表信息
	TCHAR buff[MAX_PATH];
	if (this->is32o64 == 32) {
		wsprintfW(buff, L"%lX\n", pTls->StartAddressOfRawData);
		Info[0] = pTls->StartAddressOfRawData;
		OutputDebugStringW(buff);
		wsprintfW(buff, L"%lX\n", pTls->EndAddressOfRawData);
		Info[1] = pTls->EndAddressOfRawData;
		OutputDebugStringW(buff);
		wsprintfW(buff, L"%lX\n", pTls->AddressOfIndex);
		Info[2] = pTls->AddressOfIndex;
		OutputDebugStringW(buff);
		wsprintfW(buff, L"%lX\n", pTls->AddressOfCallBacks);
		Info[3] = pTls->AddressOfCallBacks;
		OutputDebugStringW(buff);
		wsprintfW(buff, L"%lX\n", pTls->Characteristics);
		Info[4] = pTls->SizeOfZeroFill;
		Info[5] = pTls->Characteristics;
		OutputDebugStringW(buff);
	}
	else if (this->is32o64 == 64) {
		wsprintfW(buff, L"%llX\n", pTls->StartAddressOfRawData);
		Info[0] = pTls64->StartAddressOfRawData;
		OutputDebugStringW(buff);
		wsprintfW(buff, L"%llX\n", pTls->EndAddressOfRawData);
		Info[1] = pTls64->EndAddressOfRawData;
		OutputDebugStringW(buff);
		wsprintfW(buff, L"%llX\n", pTls->AddressOfIndex);
		Info[2] = pTls64->AddressOfIndex;
		OutputDebugStringW(buff);
		wsprintfW(buff, L"%llX\n", pTls->AddressOfCallBacks);
		Info[3] = pTls64->AddressOfCallBacks;
		OutputDebugStringW(buff);
		wsprintfW(buff, L"%llX\n", pTls->Characteristics);
		Info[4] = pTls64->SizeOfZeroFill;
		Info[5] = pTls64->Characteristics;
		OutputDebugStringW(buff);
	}

	return 0;
}

/*
	函数：获取PE目录表
	时间：11-03_16-05
*/
void* CPE::GetDataDirectory(WORD i)
{
	if (FOA == 0)	return nullptr;
	PIMAGE_OPTIONAL_HEADER32 pOption32 = (PIMAGE_OPTIONAL_HEADER32)
		&_pNt->OptionalHeader;
	PIMAGE_OPTIONAL_HEADER64 pOption64 = (PIMAGE_OPTIONAL_HEADER64)
		&_pNt->OptionalHeader;

	PIMAGE_DATA_DIRECTORY pTable = this->is32o64 == 32 ? pOption32->DataDirectory :
		this->is32o64 == 64 ? pOption64->DataDirectory : nullptr;
	if (pTable == nullptr)	return nullptr;

	pTable += i;
	return (char*)pTable;
}
#pragma endregion
