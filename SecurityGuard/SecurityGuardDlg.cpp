
// SecurityGuardDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "SecurityGuard.h"
#include "SecurityGuardDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSecurityGuardDlg 对话框



CSecurityGuardDlg::CSecurityGuardDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SECURITYGUARD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSecurityGuardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSecurityGuardDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CSecurityGuardDlg::OnNMDblclkTree1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CSecurityGuardDlg::OnNMRClickList1)
	//ON_COMMAND(ID_32774, &CSecurityGuardDlg::OnClickMenu)
	ON_COMMAND_RANGE(ID_32772,ID_32777, &CSecurityGuardDlg::OnClickMenu)
END_MESSAGE_MAP()


// CSecurityGuardDlg 消息处理程序

BOOL CSecurityGuardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//this->ShowWindow(SW_HIDE);
	gView.Init(this);
	gCtrl.Init(this->GetSafeHwnd());
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSecurityGuardDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSecurityGuardDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSecurityGuardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSecurityGuardDlg::OnNMDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	if (pNMHDR->idFrom == IDC_TREE1) {
		CTreeCtrl* pTree = (CTreeCtrl*)GetDlgItem(IDC_TREE1);
		ASSERT_VALID(pTree);
		HTREEITEM hLeaf = pTree->GetSelectedItem();
		bool bHasLeaf = pTree->ItemHasChildren(hLeaf), bExpand = false;
		if (bHasLeaf) {
			bExpand = TVIS_EXPANDED &
				pTree->GetItemState(hLeaf, TVIS_EXPANDED);
		}
		OutputDebugString(pTree->GetItemText(hLeaf));
		if (!bHasLeaf || TVIS_EXPANDED &
			pTree->GetItemState(hLeaf, TVIS_EXPANDED))
			gView.DoSomeThingTree(hLeaf);
	}
	*pResult = 0;
}



void CSecurityGuardDlg::OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	if (pNMItemActivate->iItem == -1)	return;
	gCtrl.DoSomeThingList(pNMItemActivate);
}


void CSecurityGuardDlg::OnClickMenu()
{
	// TODO: 在此添加命令处理程序代码
}


void CSecurityGuardDlg::OnClickMenu(UINT nID)
{
	// TODO: 在此添加命令处理程序代码
	gCtrl.DoSomeMenu(nID);
}
