// VSBottomBarWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "VSBottomBarWnd.h"
#include "VSImageInfoDlg.h"
#include "VSCurrentSliceEdit.h"
#include "VSBottomBar.h"

// CVSBottomBarWnd

IMPLEMENT_DYNAMIC(CVSBottomBarWnd, CWnd)

CVSBottomBarWnd::CVSBottomBarWnd()
: m_nCurrentSlice(0)
, m_nSlice(1)
, m_bInputingCurrentSlice(FALSE)
, m_pCurrentSlice(NULL)
, m_pTotalSlice(NULL)
, m_pPreviewSlice(NULL)
, m_pSliceSlider(NULL)
, m_pNextSlice(NULL)
, m_pAddSlice(NULL)
, m_pDeleteSlice(NULL)
, m_pSliceInfo(NULL)
, m_pFullScreen(NULL)
, m_pOriginSize(NULL)
, m_pImageInfoDlg(NULL)
, m_pFont(NULL)
{

}

CVSBottomBarWnd::~CVSBottomBarWnd()
{
	// 析构：删除与窗口相关的10个指针
	SAFE_DELETE(m_pCurrentSlice);
	SAFE_DELETE(m_pTotalSlice);
	SAFE_DELETE(m_pPreviewSlice);
	SAFE_DELETE(m_pSliceSlider);
	SAFE_DELETE(m_pNextSlice);
	SAFE_DELETE(m_pAddSlice);
	SAFE_DELETE(m_pDeleteSlice);
	SAFE_DELETE(m_pSliceInfo);
	SAFE_DELETE(m_pFullScreen);
	SAFE_DELETE(m_pOriginSize);
	if (m_pImageInfoDlg)
	{
		m_pImageInfoDlg->DestroyWindow();
		SAFE_DELETE(m_pImageInfoDlg);
	}
	SAFE_DELETE(m_pFont);
}

BEGIN_MESSAGE_MAP(CVSBottomBarWnd, CWnd)
	ON_WM_CREATE()
	ON_NOTIFY(NM_RELEASEDCAPTURE, ID_BOTTOMBAR_SLICE_SLIDER, &CVSBottomBarWnd::OnMoveSliceSlider)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

// CVSBottomBarWnd 消息处理程序

int CVSBottomBarWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 字体
	m_pFont = new CFont;
	m_pFont->CreatePointFont(100, _T("新宋体"));

	// 工具提示
	EnableToolTips(true);
	m_ToolTip.Create(this);
	m_ToolTip.Activate(true);
	m_ToolTip.SetDelayTime(TTDT_INITIAL, 200); //延迟200ms后显示
	m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 3000);//显示持续时间3000ms时间

	// 新建编辑框
	m_pCurrentSlice = new CVSCurrentSliceEdit;
	// 创建编辑框，四个参数依次为窗口风格、组合框位置、父窗口、ID号
	m_pCurrentSlice->Create(WS_CHILD | ES_CENTER | ES_WANTRETURN | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_CURRENT_SLICE);
	m_pCurrentSlice->SetWindowText(_T("1"));
	// 设置字体
	m_pCurrentSlice->SetFont(m_pFont);
	m_ToolTip.AddTool(m_pCurrentSlice, _T("当前切片"));

	// 切片总数
	m_pTotalSlice = new CStatic;
	m_pTotalSlice->Create(NULL, WS_CHILD | SS_NOTIFY | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_TOTAL_SLICE);
	m_pTotalSlice->SetFont(m_pFont);
	m_ToolTip.AddTool(m_pTotalSlice, _T("总的切片"));

	// 上一层切片
	HICON hIcon = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_PREVIEW_SLICE), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	//HICON hIcon = (HICON)::LoadImage(AfxGetInstanceHandle(), _T("res\\PreviewSlice.ico"), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
	m_pPreviewSlice = new CStatic;
	if (hIcon)
	{
		m_pPreviewSlice->Create(NULL, WS_CHILD | SS_ICON | SS_CENTERIMAGE | SS_NOTIFY | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_PREVIEW_SLICE);
		m_pPreviewSlice->SetIcon(hIcon);
	}
	else
	{
		m_pPreviewSlice->Create(NULL, WS_CHILD | SS_CENTER | SS_NOTIFY | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_PREVIEW_SLICE);
		m_pPreviewSlice->SetWindowText(_T("前"));
		m_pPreviewSlice->SetFont(m_pFont);
	}
	m_ToolTip.AddTool(m_pPreviewSlice, _T("上一张"));

	// 切片滑动条
	m_pSliceSlider = new CVSSliceSliderCtrl;
	m_pSliceSlider->Create(WS_CHILD | TBS_HORZ | TBS_AUTOTICKS | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_SLICE_SLIDER);
	m_pSliceSlider->SetFont(m_pFont);
	m_ToolTip.AddTool(m_pSliceSlider, _T("选择切片层数"));

	// 下一层切片
	hIcon = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_NEXT_SLICE), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	//hIcon = (HICON)::LoadImage(AfxGetInstanceHandle(), _T("res\\NextSlice.ico"), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
	m_pNextSlice = new CStatic;
	if (hIcon)
	{
		m_pNextSlice->Create(NULL, WS_CHILD | SS_ICON | SS_CENTERIMAGE | SS_NOTIFY | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_NEXT_SLICE);
		m_pNextSlice->SetIcon(hIcon);
	}
	else
	{
		m_pNextSlice->Create(NULL, WS_CHILD | SS_CENTER | SS_NOTIFY | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_NEXT_SLICE);
		m_pNextSlice->SetWindowText(_T("后"));
		m_pNextSlice->SetFont(m_pFont);
	}
	m_ToolTip.AddTool(m_pNextSlice, _T("下一张"));

	// 增加切片
	m_pAddSlice = new CStatic;
	hIcon = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ADD_SLICE), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	if (hIcon)
	{
		m_pAddSlice->Create(NULL, WS_CHILD | SS_ICON | SS_CENTERIMAGE | SS_NOTIFY | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_ADD_SLICE);
		m_pAddSlice->SetIcon(hIcon);
	}
	else
	{
		m_pAddSlice->Create(NULL, WS_CHILD | SS_CENTER | SS_NOTIFY | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_ADD_SLICE);
		m_pAddSlice->SetWindowText(_T("+"));
		m_pAddSlice->SetFont(m_pFont);
	}
	m_ToolTip.AddTool(m_pAddSlice, _T("增加"));

	// 删除切片
	m_pDeleteSlice = new CStatic;
	hIcon = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_DELETE_SLICE), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	if (hIcon)
	{
		m_pDeleteSlice->Create(NULL, WS_CHILD | SS_ICON | SS_CENTERIMAGE | SS_NOTIFY | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_DELETE_SLICE);
		m_pDeleteSlice->SetIcon(hIcon);
	}
	else
	{
		m_pDeleteSlice->Create(NULL, WS_CHILD | SS_CENTER | SS_NOTIFY | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_DELETE_SLICE);
		m_pDeleteSlice->SetWindowText(_T("-"));
		m_pDeleteSlice->SetFont(m_pFont);
	}
	m_ToolTip.AddTool(m_pDeleteSlice, _T("删除"));

	// 全屏按钮
	hIcon = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_FULL_SCREEN_VIEW), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	//hIcon = (HICON)::LoadImage(AfxGetInstanceHandle(), _T("res\\FullScreenView.ico"), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
	m_pFullScreen = new CStatic;
	if (hIcon)
	{
		m_pFullScreen->Create(NULL, WS_CHILD | SS_ICON | SS_CENTERIMAGE | SS_NOTIFY | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_FULL_SCREEN);
		m_pFullScreen->SetIcon(hIcon);
	}
	else
	{
		m_pFullScreen->Create(NULL, WS_CHILD | SS_CENTER | SS_NOTIFY | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_FULL_SCREEN);
		m_pFullScreen->SetWindowText(_T("F"));
		m_pFullScreen->SetFont(m_pFont);
	}
	m_ToolTip.AddTool(m_pFullScreen, _T("全屏"));

	// "原大"按钮
	hIcon = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ORIGIN_SIZE_VIEW), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	//hIcon = (HICON)::LoadImage(AfxGetInstanceHandle(), _T("res\\OriginSizeView.ico"), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
	m_pOriginSize = new CStatic;
	if (hIcon)
	{
		m_pOriginSize->Create(NULL, WS_CHILD | SS_ICON | SS_CENTERIMAGE | SS_NOTIFY | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_ORIGIN_SIZE);
		m_pOriginSize->SetIcon(hIcon);
	}
	else
	{
		m_pOriginSize->Create(NULL, WS_CHILD | SS_CENTER | SS_NOTIFY | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_ORIGIN_SIZE);
		m_pOriginSize->SetWindowText(_T("O"));
		m_pOriginSize->SetFont(m_pFont);
	}
	m_ToolTip.AddTool(m_pOriginSize, _T("原大"));

	// 切片信息按钮
	hIcon = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_IMAGE_PROP_PAGE), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	//hIcon = (HICON)::LoadImage(AfxGetInstanceHandle(), _T("res\\DlgImagePropPage.ico"), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
	m_pSliceInfo = new CStatic;
	if (hIcon)
	{
		m_pSliceInfo->Create(NULL, WS_CHILD | SS_ICON | SS_CENTERIMAGE | SS_NOTIFY | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_SLICE_INFO);
		m_pSliceInfo->SetIcon(hIcon);
	}
	else
	{
		m_pSliceInfo->Create(NULL, WS_CHILD | SS_CENTER | SS_NOTIFY | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_SLICE_INFO);
		m_pSliceInfo->SetWindowText(_T("I"));
		m_pSliceInfo->SetFont(m_pFont);
	}
	m_ToolTip.AddTool(m_pSliceInfo, _T("切片属性"));

	// 图像属性
	m_pImageInfoDlg = new CVSImageInfoDlg;
	m_pImageInfoDlg->Create(IDD_IMAGE_INFO_DLG, this);
	m_pImageInfoDlg->ShowWindow(SW_HIDE);

	return 0;
}

// 改变编辑框内容的响应函数
BOOL CVSBottomBarWnd::ChangeCurrentSliceEdit()
{
	// 用户完成输入
	m_bInputingCurrentSlice = FALSE;
	// 使编辑框失去焦点，把焦点转移给父窗口
	SetFocus();
	CString strText;
	m_pCurrentSlice->GetWindowText(strText);
	int nNum = _tstoi(strText) - 1;
	if (strText.IsEmpty())
		nNum = m_nCurrentSlice;

	// nNum属于[0, m_nSlice)
	if (nNum < 0 || nNum >= m_nSlice)
	{
		// 输入无效，提示用户
		MessageBox(_T("您所输入的切片层数无效！"), _T("提示"), MB_OK | MB_ICONWARNING);
		SetCurrentSliceEditText(m_nCurrentSlice);
		return FALSE;
	}
	m_nCurrentSlice = nNum;
	m_pSliceSlider->SetPos(nNum);
	SetCurrentSlice(m_nCurrentSlice);
	SetCurrentSliceEditText(m_nCurrentSlice);
	return TRUE;
}

// 拖动滑动条时响应函数
void CVSBottomBarWnd::OnMoveSliceSlider(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_nCurrentSlice = m_pSliceSlider->GetPos();
	SetCurrentSlice(m_nCurrentSlice);
	SetCurrentSliceEditText(m_nCurrentSlice);
	*pResult = 0;
}

// 当拖动滑动条或输入切片层数时更新
void CVSBottomBarWnd::SetCurrentSlice(int nSlice)
{
	TRACE("* 功能转移到父类，设置切片层数 *\n");
	((CVSBottomBar*)GetParent())->SetCurrentSlice(nSlice);
}

// 设置当前切片编辑框所显示的切片层数
void CVSBottomBarWnd::SetCurrentSliceEditText(int nSlice)
{
	CString strText;
	strText.Format(_T("%d"), nSlice + 1);
	m_pCurrentSlice->SetWindowText(strText);
}

// 获取View的屏幕坐标(矩形区域)
void CVSBottomBarWnd::GetViewScreenRect(CRect &rect)
{
	// 获取祖父的指针，即底部工具栏的父亲
	CWnd* pGP = GetParent()->GetParent();
	pGP->GetClientRect(&rect);
	pGP->ClientToScreen(&rect);
}

// 点击上一层按钮响应函数
void CVSBottomBarWnd::ClickPreviewSliceButton()
{
	if (--m_nCurrentSlice == -1)
	{
		m_nCurrentSlice = 0;
		return;
	}
	SetCurrentSlice(m_nCurrentSlice);
	m_pSliceSlider->SetPos(m_nCurrentSlice);
	SetCurrentSliceEditText(m_nCurrentSlice);
}

// 点击下一层按钮响应函数
void CVSBottomBarWnd::ClickNextSliceButton()
{
	if (++m_nCurrentSlice >= m_nSlice)
	{
		m_nCurrentSlice = m_nSlice - 1;
		return;
	}
	SetCurrentSlice(m_nCurrentSlice);
	m_pSliceSlider->SetPos(m_nCurrentSlice);
	SetCurrentSliceEditText(m_nCurrentSlice);
}

// 点击切片信息按钮响应函数
void CVSBottomBarWnd::ClickSliceInfoButton()
{
	ChangeCurrentSliceEdit();

	CRect rect;
	GetViewScreenRect(rect);

	// 如果对话框是隐藏的，则显示，否则隐藏
	if (m_pImageInfoDlg->IsWindowVisible())
	{
		m_pImageInfoDlg->ShowWindowEx(rect, SW_HIDE);
	}
	else
	{
		m_pImageInfoDlg->ShowWindowEx(rect, SW_SHOW);
	}
}

void CVSBottomBarWnd::ClickAddSliceButton()
{
	TRACE("* 功能转移到父类，增加切片 *\n");
	((CVSBottomBar*)GetParent())->ClickAddSliceButton();
}

void CVSBottomBarWnd::ClickDeleteSliceButton()
{
	TRACE("* 功能转移到父类，删除切片 *\n");
	((CVSBottomBar*)GetParent())->ClickDeleteSliceButton();
}

// 全屏按钮的响应函数
void CVSBottomBarWnd::ClickFullScreenButton()
{
	TRACE("* 功能转移到父类，全屏进行显示 *\n");
	((CVSBottomBar*)GetParent())->ClickFullScreenButton();
}

// “原大”：将图像以原大小进行显示
void CVSBottomBarWnd::ClickOriginSizeButton()
{
	TRACE("* 功能转移到父类，原大进行显示 *\n");
	((CVSBottomBar*)GetParent())->ClickOriginSizeButton();
}

void CVSBottomBarWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(&rect);

	if (m_pCurrentSlice->GetSafeHwnd())
	{
		m_pCurrentSlice->MoveWindow(10, 4, 30, 15);
	}
	if (m_pTotalSlice->GetSafeHwnd())
	{
		m_pTotalSlice->MoveWindow(40, 4, 40, 15);
	}
	if (m_pPreviewSlice->GetSafeHwnd())
	{
		m_pPreviewSlice->MoveWindow(90, rect.bottom - BOTTTOM_BAR_SIZE + 1, BOTTTOM_BAR_SIZE - 2, BOTTTOM_BAR_SIZE - 2);
	}
	if (m_pSliceSlider->GetSafeHwnd())
	{
		m_pSliceSlider->MoveWindow(100 + BOTTTOM_BAR_SIZE - 2, 1, 150, BOTTTOM_BAR_SIZE - 2);
	}
	if (m_pNextSlice->GetSafeHwnd())
	{
		m_pNextSlice->MoveWindow(260 + BOTTTOM_BAR_SIZE - 2, rect.bottom - BOTTTOM_BAR_SIZE + 1, BOTTTOM_BAR_SIZE - 2, BOTTTOM_BAR_SIZE - 2);
	}
	if (m_pAddSlice->GetSafeHwnd())
	{
		m_pAddSlice->MoveWindow(rect.right - 5 * (BOTTTOM_BAR_SIZE - 2) - 50, rect.bottom - BOTTTOM_BAR_SIZE + 1, BOTTTOM_BAR_SIZE - 2, BOTTTOM_BAR_SIZE - 2);
	}
	if (m_pDeleteSlice->GetSafeHwnd())
	{
		m_pDeleteSlice->MoveWindow(rect.right - 4 * (BOTTTOM_BAR_SIZE - 2) - 40, rect.bottom - BOTTTOM_BAR_SIZE + 1, BOTTTOM_BAR_SIZE - 2, BOTTTOM_BAR_SIZE - 2);
	}
	if (m_pFullScreen->GetSafeHwnd())
	{
		m_pFullScreen->MoveWindow(rect.right - 3 * (BOTTTOM_BAR_SIZE - 2) - 30, rect.bottom - BOTTTOM_BAR_SIZE + 1, BOTTTOM_BAR_SIZE - 2, BOTTTOM_BAR_SIZE - 2);
	}
	if (m_pOriginSize->GetSafeHwnd())
	{
		m_pOriginSize->MoveWindow(rect.right - 2 * (BOTTTOM_BAR_SIZE - 2) - 20, rect.bottom - BOTTTOM_BAR_SIZE + 1, BOTTTOM_BAR_SIZE - 2, BOTTTOM_BAR_SIZE - 2);
	}
	if (m_pSliceInfo->GetSafeHwnd())
	{
		m_pSliceInfo->MoveWindow(rect.right - (BOTTTOM_BAR_SIZE - 2) - 10, rect.bottom - BOTTTOM_BAR_SIZE + 1, BOTTTOM_BAR_SIZE - 2, BOTTTOM_BAR_SIZE - 2);
	}
	if (m_pImageInfoDlg->GetSafeHwnd())
	{
		MoveImagePropPageDlg();
	}
}

// 将对话框移动到视图右下角
void CVSBottomBarWnd::MoveImagePropPageDlg()
{
	if (!m_pImageInfoDlg->IsWindowVisible())
		return;

	CRect rect;
	GetViewScreenRect(rect);

	// 将对话框移动到视图右下角
	m_pImageInfoDlg->MoveWindowEx(rect);
}

// 判断是否为2D图像
BOOL CVSBottomBarWnd::Is2DImage()
{
	return (m_nSlice == 1);
}

void CVSBottomBarWnd::Init(int nCurrentSlice, int nSlice)
{
	m_nCurrentSlice = nCurrentSlice;
	m_nSlice = nSlice;
	// 如果nSlice为1，即2D图像，隐藏一些控件
	if (m_nSlice == 1)
	{
		m_pCurrentSlice->ShowWindow(SW_HIDE);
		m_pTotalSlice->ShowWindow(SW_HIDE);
		m_pPreviewSlice->ShowWindow(SW_HIDE);
		m_pNextSlice->ShowWindow(SW_HIDE);
		m_pSliceSlider->ShowWindow(SW_HIDE);
		m_pAddSlice->ShowWindow(SW_HIDE);
		m_pDeleteSlice->ShowWindow(SW_HIDE);
	}
	else if (m_nSlice > 1)
	{
		m_pCurrentSlice->ShowWindow(SW_SHOW);
		m_pTotalSlice->ShowWindow(SW_SHOW);
		m_pPreviewSlice->ShowWindow(SW_SHOW);
		m_pNextSlice->ShowWindow(SW_SHOW);
		m_pSliceSlider->ShowWindow(SW_SHOW);
		m_pSliceSlider->SetRangeMax(m_nSlice - 1);
		m_pSliceSlider->SetTicFreq(m_nSlice / 10);
		m_pAddSlice->ShowWindow(SW_SHOW);
		m_pDeleteSlice->ShowWindow(SW_SHOW);
	}

	// 显示“全屏”、“原大”、“图像信息”按钮
	m_pFullScreen->ShowWindow(SW_SHOW);
	m_pOriginSize->ShowWindow(SW_SHOW);
	m_pSliceInfo->ShowWindow(SW_SHOW);

	m_pSliceSlider->SetPos(m_nCurrentSlice);
	SetCurrentSliceEditText(m_nCurrentSlice);

	CString text;
	text.Format(_T("/%d"), m_nSlice);
	m_pTotalSlice->SetWindowText(text);
	m_pImageInfoDlg->GetImageInfomation();
	MoveImagePropPageDlg();
}

BOOL CVSBottomBarWnd::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);
	// 当前切片
	if (pMsg->hwnd == m_pCurrentSlice->m_hWnd)
	{
		switch (pMsg->message)
		{
		case WM_KEYDOWN:
			// 回车键响应
			if (pMsg->wParam == VK_RETURN)
			{
				m_bInputingCurrentSlice = FALSE;
				ChangeCurrentSliceEdit();
			}
			break;
		case WM_LBUTTONDOWN:
			m_bInputingCurrentSlice = TRUE;
			m_pCurrentSlice->SetWindowText(_T(""));
			break;
		default:
			break;
		}
	}
	// 上一层
	else if (pMsg->hwnd == m_pPreviewSlice->m_hWnd)
	{
		switch (pMsg->message)
		{
		case WM_LBUTTONDOWN:
			ClickPreviewSliceButton();
			break;
		case WM_MOUSEMOVE:
			// 必须取得焦点才能响应鼠标左键单击的消息
			m_pPreviewSlice->SetFocus();
			break;
		default:
			break;
		}
	}
	// 下一层
	else if (pMsg->hwnd == m_pNextSlice->m_hWnd)
	{
		switch (pMsg->message)
		{
		case WM_LBUTTONDOWN:
			ClickNextSliceButton();
			break;
		case WM_MOUSEMOVE:
			m_pNextSlice->SetFocus();
			break;
		default:
			break;
		}
	}
	// 增加
	else if (pMsg->hwnd == m_pAddSlice->m_hWnd)
	{
		switch (pMsg->message)
		{
		case WM_LBUTTONDOWN:
			ClickAddSliceButton();
			break;
		case WM_MOUSEMOVE:
			m_pAddSlice->SetFocus();
			break;
		default:
			break;
		}
	}
	// 删除
	else if (pMsg->hwnd == m_pDeleteSlice->m_hWnd)
	{
		switch (pMsg->message)
		{
		case WM_LBUTTONDOWN:
			ClickDeleteSliceButton();
			break;
		case WM_MOUSEMOVE:
			m_pDeleteSlice->SetFocus();
			break;
		default:
			break;
		}
	}
	// 全屏
	else if (pMsg->hwnd == m_pFullScreen->m_hWnd)
	{
		switch (pMsg->message)
		{
		case WM_LBUTTONDOWN:
			ClickFullScreenButton();
			break;
		case WM_MOUSEMOVE:
			m_pFullScreen->SetFocus();
			break;
		default:
			break;
		}
	}
	// 原大
	else if (pMsg->hwnd == m_pOriginSize->m_hWnd)
	{
		switch (pMsg->message)
		{
		case WM_LBUTTONDOWN:
			ClickOriginSizeButton();
			break;
		case WM_MOUSEMOVE:
			m_pOriginSize->SetFocus();
			break;
		default:
			break;
		}
	}
	// 切片信息
	else if (pMsg->hwnd == m_pSliceInfo->m_hWnd)
	{
		switch (pMsg->message)
		{
		case WM_LBUTTONDOWN:
			ClickSliceInfoButton();
			break;
		case WM_MOUSEMOVE:
			m_pSliceInfo->SetFocus();
			break;
		default:
			break;
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

// 当拖动滑动条时同步更新切片层数编辑框的数值
void CVSBottomBarWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nTic = m_pSliceSlider->GetPos();
	SetCurrentSliceEditText(nTic);
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}