#pragma once

extern "C" _declspec(dllexport) void* PrintInfo();

typedef struct _MsgBoxWHookInfo
{
	ULONG_PTR	OldMsgBoxW;
	LPVOID		MyMsgBoxW;
	LPVOID		pIAT;
	BOOL		isHook;
}MsgBoxWHookInfo, * LPMsgBoxWHookInfo;


typedef struct _OpenProHookInfo
{
	ULONG_PTR	OldOpenPro;
	LPVOID		MyOpenPro;
	LPVOID		pIAT;
	BOOL		isHook;
}OpenProHookInfo, * LPOpenProHookInfo;



class _declspec(dllexport) CHook
{
public:
	CHook(HMODULE hModule);
	~CHook();

	LPVOID GetThis() { return this; };
	BOOL DoHook() { 
		LPVOID iat = _OpenPro.pIAT;
		if (!iat)	return false;
		//LPVOID iat = _BoxW.pIAT;
		LPVOID pwr = (LPVOID)_OpenPro.MyOpenPro;

		memcpy(iat, &pwr, 8);
		LPVOID	val;
		memcpy(&val, iat, 8);
		//printf_s("关闭.请观察IAT 0x%p\t%p ==%p\n",
		//	iat, val, pwr);
		_OpenPro.isHook = true;
		return true;
	};
	BOOL UnHook() { 
		//if (_BoxW.isHook) {
		if (_OpenPro.isHook) {
			//LPVOID iat = _BoxW.pIAT;
			LPVOID iat = _OpenPro.pIAT;
			LPVOID pwr = (LPVOID)_OpenPro.OldOpenPro;

			memcpy(iat, &pwr, 8);
			LPVOID	val;
			memcpy(&val, iat, 8);
			//printf_s("关闭.请观察IAT 0x%p\t%p ==%p\n",
			//	iat, val, pwr);
			_OpenPro.isHook = false;
			return true;
		}
		return 0; };
	void Print() {
		//printf_s("MsgW\t%016llX\nMyW\t%p\nIat\t%p\t%d\n",
		//	_BoxW.OldMsgBoxW, _BoxW.MyMsgBoxW, _BoxW.pIAT, _BoxW.isHook);
	};
private:
	//MsgBoxWHookInfo _BoxW;
	OpenProHookInfo _OpenPro;
	TCHAR path[MAX_PATH];
};


static CHook* dgHook = nullptr;
static PTCHAR dgPath = nullptr;