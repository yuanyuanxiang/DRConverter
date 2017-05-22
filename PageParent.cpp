// PageParent.cpp : 实现文件
//

#include "stdafx.h"
#include "DRConverter.h"
#include "PageParent.h"
#include "afxdialogex.h"
#include "functions.h"
#include "DRConverterDlg.h"

// CPageParent 对话框

IMPLEMENT_DYNAMIC(CPageParent, CDlgImageViewer)

CPageParent::CPageParent(UINT nIDTemplate, CWnd* pParent) 
	: CDlgImageViewer(nIDTemplate, pParent)
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nSlice = 0;
	m_nCurrentSlice = -1;
	m_fZoomRate = 1.f;
	m_pBottomBar = NULL;
	m_hFile = NULL;
	m_hFileMap = NULL;
	m_pMapAddress = NULL;
	m_i64FileSize = 0;
	m_i64FileOffset = 0;
	m_dwBlockBytes = 0;
	m_kbGranularity = 0;
}

CPageParent::~CPageParent()
{
	ResetMapping();
}

void CPageParent::DoDataExchange(CDataExchange* pDX)
{
	CDlgImageViewer::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SELECTED, m_EditSelected);
	DDX_Control(pDX, IDC_CHOOSING, m_BnChoose);
	DDX_Control(pDX, IDC_APPLY, m_BnApply);
	DDX_Control(pDX, IDC_CHOOSE_TIP, m_ChooseTips);
}


BEGIN_MESSAGE_MAP(CPageParent, CDlgImageViewer)
	ON_BN_CLICKED(IDC_CHOOSING, &CPageParent::OnBnClickedChoosing)
	ON_BN_CLICKED(IDC_APPLY, &CPageParent::OnBnClickedApply)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CPageParent 消息处理程序

void CPageParent::OnBnClickedChoosing()
{
	m_ToolTip.AddTool(&m_EditSelected, m_strSelected);
}

BOOL CPageParent::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);

	// 屏蔽ESC关闭窗体
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	// 进入全屏
	if (pMsg->wParam == VK_F2)
	{
		((CDRConverterDlg*)::AfxGetMainWnd())->EnterFullScreen();
		return TRUE;
	}
	// 退出全屏
	if (pMsg->wParam == VK_ESCAPE)
	{
		((CDRConverterDlg*)::AfxGetMainWnd())->QuitFullScreen();
		return TRUE;
	}
	return CDlgImageViewer::PreTranslateMessage(pMsg);
}

void CPageParent::OnOK()
{
}

BOOL CPageParent::OnInitDialog()
{
	CDlgImageViewer::OnInitDialog();

	// 工具提示
	EnableToolTips(true);
	m_ToolTip.Create(this);
	m_ToolTip.Activate(true);
	m_ToolTip.SetDelayTime(TTDT_INITIAL, 200); //延迟200ms后显示
	m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 3000);//显示持续时间3000ms时间

	m_ToolTip.AddTool(&m_BnChoose, _T("选择..."));
	m_ToolTip.AddTool(&m_BnApply, _T("执行操作"));

	return TRUE;
}

void CPageParent::OnSize(UINT nType, int cx, int cy)
{
	CDlgImageViewer::OnSize(nType, cx, cy);

	GetClientRect(&m_ClientRect);

	m_PaintRect.left = BOTTTOM_BAR_SIZE;
	m_PaintRect.right = m_ClientRect.right - BOTTTOM_BAR_SIZE;
	m_PaintRect.top = 80;
	m_PaintRect.bottom = m_ClientRect.bottom - BOTTTOM_BAR_SIZE;

	if (m_pBottomBar != NULL && m_pBottomBar->GetSafeHwnd())
	{
		// 移动工具栏位置
		m_pBottomBar->MoveWindow(m_ClientRect.left, m_ClientRect.bottom - BOTTTOM_BAR_SIZE, m_ClientRect.Width(), BOTTTOM_BAR_SIZE);
	}
}

BOOL CPageParent::ContinueModal()
{
	if (m_pBottomBar != NULL && m_pBottomBar->IsWindowVisible())
	{
		CFrameWnd* pParent = (CFrameWnd*)m_pBottomBar->GetParent();
		if (pParent)
			m_pBottomBar->OnUpdateCmdUI(pParent, (WPARAM)TRUE);
	}
	CMenu* pMainMenu = GetMenu();
	CCmdUI cmdUI;
	if (pMainMenu != NULL)
	{
		for (int n = 0; n < pMainMenu->GetMenuItemCount(); ++n)
		{
			CMenu* pSubMenu = pMainMenu->GetSubMenu(n);
			cmdUI.m_nIndexMax = pSubMenu->GetMenuItemCount();
			for (int i = 0; i < cmdUI.m_nIndexMax; ++i)
			{
				cmdUI.m_nIndex = i;
				cmdUI.m_nID = pSubMenu->GetMenuItemID(i);
				cmdUI.m_pMenu = pSubMenu;
				cmdUI.DoUpdate(this, FALSE);
			}
		}
	}
	return CDlgImageViewer::ContinueModal();
}

void CPageParent::OnMouseMove(UINT nFlags, CPoint point)
{
	ShowBottomBar(point);

	CDlgImageViewer::OnMouseMove(nFlags, point);
}

// 显示/隐藏底部工具栏
BOOL CPageParent::ShowBottomBar(const CPoint& point)
{
	if (!m_pBottomBar || !m_pBottomBar->GetSafeHwnd())
		return FALSE;

	CRect rect;
	GetClientRect(rect);
	if (point.y > rect.bottom - 40)
	{
		m_pBottomBar->ShowWindow(SW_SHOW);
		return TRUE;
	}
	else
	{
		m_pBottomBar->ShowWindow(SW_HIDE);
		return FALSE;
	}
}

void CPageParent::OnBnClickedApply()
{
}

// 重置所有变量
void CPageParent::Reset()
{
	ResetMapping();

	m_nWidth = 0;
	m_nHeight = 0;
	m_nSlice = 0;
	m_nCurrentSlice = -1;
	m_fZoomRate = 1.f;
	m_i64FileSize = 0;

	m_strSelected.Empty();
	m_strFilePath.Empty();
	m_Image.Destroy();
	m_PaintRect = CRect();
	m_ClientRect = CRect();

	m_EditSelected.SetWindowText(_T(""));
	m_ToolTip.UpdateTipText(_T(""), &m_EditSelected);
}

void CPageParent::OnPaint()
{
	CPaintDC dc(this);
	if (!m_Image.IsNull())
	{
		m_Image.Draw(dc.GetSafeHdc(), m_PaintRect, Gdiplus::InterpolationModeBicubic);
	}
}