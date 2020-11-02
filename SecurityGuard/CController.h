#pragma once
#include "CMyView.h"


class CController
{
public:
	CController();
	~CController();
	
	void DoSomeThingList(LPNMITEMACTIVATE& pNMItem);
	void DoSomeMenu(UINT nID);
private:
	LPSERVICEINFO	_lpServiceInfo = nullptr;
};

extern CController gCtrl;