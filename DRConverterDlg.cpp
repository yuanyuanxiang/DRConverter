
// DRConverterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DRConverter.h"
#include "DRConverterDlg.h"
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
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDRConverterDlg 对话框



CDRConverterDlg::CDRConverterDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CDRConverterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nCurSelTab = 0;
	m_bFullScreen = FALSE;
}

CDRConverterDlg::~CDRConverterDlg()
{
}

void CDRConverterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDRConverterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDRConverterDlg 消息处理程序

BOOL CDRConverterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	m_Font.CreatePointFont(100, _T("宋体"));

	CRect rc;
	GetClientRect(&rc);
	m_Table.Create(WS_CHILD | WS_VISIBLE, rc, this, ID_TABLE_WND);
	m_Table.SetFont(&m_Font);

	// 创建两个对话框,并将 CTabCtrl 对象作为其父窗口
	m_Page1.Create(IDD_PAGE1_DLG, &m_Table);
	m_Page2.Create(IDD_PAGE2_DLG, &m_Table);

	// 把对话框对象与 CDialog 数组联系起来
	m_pDialog[0] = &m_Page1;
	m_pDialog[1] = &m_Page2;

	// 为 Tab Control 增加两个页面
	m_Table.InsertItem(0, _T("合并"));
	m_Table.InsertItem(1, _T("拆分"));

	MovePageFitTable();

	//显示初始页面,保存当前选择
	m_pDialog[0]->ShowWindow(SW_SHOW);
	m_pDialog[1]->ShowWindow(SW_HIDE);
	m_nCurSelTab = 0;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDRConverterDlg::MovePageFitTable()
{
	//获得 Tab Control 客户区矩形大小
	CRect rc;
	m_Table.GetClientRect(&rc);

	// 调整矩形大小让属性页显示在适当的位置上
	rc.top += 23;
	rc.bottom -= 3;
	rc.left += 3;
	rc.right -= 3;

	// 移动对话框窗口到矩形区域中
	m_Page1.MoveWindow(&rc);
	m_Page2.MoveWindow(&rc);
}

void CDRConverterDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDRConverterDlg::OnPaint()
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
HCURSOR CDRConverterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDRConverterDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_Table.GetSafeHwnd())
	{
		m_Table.MoveWindow(0, 0, cx, cy);
		MovePageFitTable();
	}
}

// 进入全屏
void CDRConverterDlg::EnterFullScreen()
{
	//如果已经是全屏状态,则不处理
	if (m_bFullScreen)
		return;

	//保存旧窗口坐标位置
	GetWindowRect(&m_rcOldWnd);

	//ModifyStyle(0, WS_POPUP);
	ModifyStyle(WS_CAPTION, 0);
	ModifyStyle(WS_BORDER, 0);
	ModifyStyle(WS_THICKFRAME, 0);

	//将窗口大小设为全屏,
	MoveWindow(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

	//将视图区占满整个客户区
	ShowWindow(SW_SHOWMAXIMIZED);

	m_bFullScreen = true;
}

// 退出全屏
void CDRConverterDlg::QuitFullScreen()
{
	//如果不是全屏状态,则不处理
	if (!m_bFullScreen)
		return;

	m_bFullScreen = false;

	ModifyStyle(0, WS_CAPTION);
	ModifyStyle(0, WS_BORDER);
	ModifyStyle(0, WS_THICKFRAME);

	// 恢复到旧位置
	// 本条语句应该在ModifyStyle后面，否则退出全屏时会遇到问题
	MoveWindow(&m_rcOldWnd);

	//将子窗口视图区正常显示
	ShowWindow(SW_SHOW);
	Invalidate(TRUE);
}