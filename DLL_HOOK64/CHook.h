#pragma once

class CHook
{
public:
	CHook();
	~CHook();

	BOOL DoHook();
	BOOL UnHook();

private:
	LPVOID _lpApi = nullptr;
};

extern CHook gHook;