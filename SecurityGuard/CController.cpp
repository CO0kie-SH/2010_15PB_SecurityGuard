#include "pch.h"
#include "CController.h"

CController gCtrl;

#pragma region 类内函数
CController::CController()
{

}

CController::~CController()
{

}

/*
	函数：列表框处理
	作者：CO0kie丶
	时间：20201102-20-05
*/
void CController::DoSomeThingList(LPNMITEMACTIVATE& pNMItem)
{
	if (gView.m_Statu.tKind == gView.m_tRoot->fService.htTree) {
		POINT xy = { pNMItem->ptAction.x, pNMItem->ptAction.y };
		gView.m_PVList->ClientToScreen(&xy);

		CMenu cMenu;
		cMenu.LoadMenuW(IDR_MENU1);
		CMenu* pSubMenu1 = cMenu.GetSubMenu(2);
		_lpServiceInfo = &gView.m_serviceInfos[pNMItem->iItem];
		pSubMenu1->AppendMenu(MF_GRAYED, MF_POPUP,
			_lpServiceInfo->lpDisplayName);
		pSubMenu1->TrackPopupMenu(TPM_LEFTALIGN, xy.x, xy.y, gView.m_Main);
		
	}
}


/*
	函数：菜单栏处理
	作者：CO0kie丶
	时间：20201102-20-42
*/
void CController::DoSomeMenu(UINT nID)
{
	switch (nID)
	{
	case ID_32774:		//打开服务
		if (this->_lpServiceInfo)
			if (gView.m_CServices.OpenServiceMy(
				this->_lpServiceInfo)) {
				gView.m_PVList->DeleteAllItems();
				MessageBox(gView.m_Main->GetSafeHwnd(),
					_T("启动成功，请刷新页面。"), _T("服务操作成功"), 0);
			}
		break;
	case ID_32775:		//关闭服务
		if (this->_lpServiceInfo)
			if (gView.m_CServices.StopService(
				this->_lpServiceInfo)) {
				gView.m_PVList->DeleteAllItems();
				MessageBox(gView.m_Main->GetSafeHwnd(),
					_T("关闭成功，请刷新页面。"), _T("服务操作成功"), 0);
			}
		break;
	default:
		break;
	}
}
#pragma endregion

