
// SecurityGuardDlg.h: 头文件
//

#pragma once
#include "CController.h"

// CSecurityGuardDlg 对话框
class CSecurityGuardDlg : public CDialogEx
{
// 构造
public:
	CSecurityGuardDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SECURITYGUARD_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickMenu();
	afx_msg void OnClickMenu(UINT nID);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg void OnNMRClickTree1(NMHDR* pNMHDR, LRESULT* pResult);
};
