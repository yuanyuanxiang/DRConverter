// VSBottomBar.cpp : 实现文件
//

#include "stdafx.h"
#include "VSBottomBar.h"
#include "VSBottomBarWnd.h"
#include "VSImageInfoDlg.h"
#include "PageParent.h"
#include "DRConverterDlg.h"

// CVSBottomBar

IMPLEMENT_DYNAMIC(CVSBottomBar, CToolBar)

// 构造函数
CVSBottomBar::CVSBottomBar()
: m_nDelayTime(200)
, m_pBottomBarWnd(NULL)
{

}

// 析构函数
CVSBottomBar::~CVSBottomBar()
{
	SAFE_DELETE(m_pBottomBarWnd);
}

BEGIN_MESSAGE_MAP(CVSBottomBar, CToolBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CVSBottomBar 消息处理程序

int CVSBottomBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pBottomBarWnd = new CVSBottomBarWnd;
	if (!m_pBottomBarWnd->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(), this, ID_BOTTOM_BAR_WND))
	{
		TRACE("创建底部工具栏失败！\n");
		return -1;
	}

	return 0;
}

void CVSBottomBar::OnSize(UINT nType, int cx, int cy)
{
	CToolBar::OnSize(nType, cx, cy);

	if (m_pBottomBarWnd->GetSafeHwnd())
	{
		m_pBottomBarWnd->MoveWindow(0, 0, cx, cy);
	}
}

// 初始化底部工具栏
void CVSBottomBar::Init(int nCurrentSlice, int nSlice)
{
	// 用户正在输入切片层数
	if (m_pBottomBarWnd->IsInputingCurrentSlice())
		return;

	m_pBottomBarWnd->Init(nCurrentSlice, nSlice);
}

// 重写此函数使得图像属性对话框与底部工具栏同时隐藏
BOOL CVSBottomBar::ShowWindow(int nShow)
{
	CVSImageInfoDlg* pDlg = m_pBottomBarWnd->GetImagePropPageDlg();
	if (pDlg->IsWindowVisible())
		return TRUE;

	if (nShow == SW_HIDE)
	{
		// 用户正在输入切片层数时不隐藏工具栏
		if (m_pBottomBarWnd->IsInputingCurrentSlice())
			return TRUE;
		pDlg->ShowWindow(nShow);
		AnimateWindow(m_nDelayTime, AW_SLIDE | AW_VER_POSITIVE | AW_HIDE);
		return CToolBar::ShowWindow(nShow);
	}
	else if (nShow == SW_SHOW)
	{
		AnimateWindow(m_nDelayTime, AW_SLIDE | AW_VER_NEGATIVE | AW_ACTIVATE);
		BOOL bResult = CToolBar::ShowWindow(nShow);
		if (m_pBottomBarWnd->IsImageInfoDlgVisible())
		{
			pDlg->ShowWindow(nShow);
		}
		return bResult;
	}

	return CToolBar::ShowWindow(nShow);
}

// 在当前切片后面插入
void CVSBottomBar::ClickAddSliceButton()
{
	CPageParent* pParent = (CPageParent*)GetParent();
	pParent->AddSlice();
}

// 删除当前切片
void CVSBottomBar::ClickDeleteSliceButton()
{
	CPageParent* pParent = (CPageParent*)GetParent();
	pParent->DeleteSlice();
}

// 点击全屏按钮响应函数
void CVSBottomBar::ClickFullScreenButton()
{
	CDRConverterDlg* pMainWindow = (CDRConverterDlg*)AfxGetMainWnd();
	pMainWindow->EnterFullScreen();
}

// 点击“原大”按钮时响应函数
void CVSBottomBar::ClickOriginSizeButton()
{
	CPageParent* pParent = (CPageParent*)GetParent();
	pParent->OnViewZoomDefault();
}

// 设置当前显示的切片层数并刷新视图
void CVSBottomBar::SetCurrentSlice(int nSlice)
{
	CPageParent* pParent = (CPageParent*)GetParent();
	pParent->LoadDR(nSlice);
}

// 设置图像信息
void CVSBottomBar::SetImageInfomation()
{
	CPageParent* pParent = (CPageParent*)GetParent();
	CVSImageInfoDlg *pDlg = m_pBottomBarWnd->GetImagePropPageDlg();
	pDlg->SetImageInfomation(pParent->GetDataHeader(), pParent->GetImage(),
		pParent->GetFilePath(), pParent->GetZoomRate(),
		pParent->GetSlice(), pParent->GetCurrentSlice());
}