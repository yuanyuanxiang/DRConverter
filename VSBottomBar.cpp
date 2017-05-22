// VSBottomBar.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "VSBottomBar.h"
#include "VSBottomBarWnd.h"
#include "VSImageInfoDlg.h"
#include "PageParent.h"
#include "DRConverterDlg.h"

// CVSBottomBar

IMPLEMENT_DYNAMIC(CVSBottomBar, CToolBar)

// ���캯��
CVSBottomBar::CVSBottomBar()
: m_nDelayTime(200)
, m_pBottomBarWnd(NULL)
{

}

// ��������
CVSBottomBar::~CVSBottomBar()
{
	SAFE_DELETE(m_pBottomBarWnd);
}

BEGIN_MESSAGE_MAP(CVSBottomBar, CToolBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CVSBottomBar ��Ϣ�������

int CVSBottomBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pBottomBarWnd = new CVSBottomBarWnd;
	if (!m_pBottomBarWnd->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(), this, ID_BOTTOM_BAR_WND))
	{
		TRACE("�����ײ�������ʧ�ܣ�\n");
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

// ��ʼ���ײ�������
void CVSBottomBar::Init(int nCurrentSlice, int nSlice)
{
	// �û�����������Ƭ����
	if (m_pBottomBarWnd->IsInputingCurrentSlice())
		return;

	m_pBottomBarWnd->Init(nCurrentSlice, nSlice);
}

// ��д�˺���ʹ��ͼ�����ԶԻ�����ײ�������ͬʱ����
BOOL CVSBottomBar::ShowWindow(int nShow)
{
	CVSImageInfoDlg* pDlg = m_pBottomBarWnd->GetImagePropPageDlg();
	if (pDlg->IsWindowVisible())
		return TRUE;

	if (nShow == SW_HIDE)
	{
		// �û�����������Ƭ����ʱ�����ع�����
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

// �ڵ�ǰ��Ƭ�������
void CVSBottomBar::ClickAddSliceButton()
{
	CPageParent* pParent = (CPageParent*)GetParent();
	pParent->AddSlice();
}

// ɾ����ǰ��Ƭ
void CVSBottomBar::ClickDeleteSliceButton()
{
	CPageParent* pParent = (CPageParent*)GetParent();
	pParent->DeleteSlice();
}

// ���ȫ����ť��Ӧ����
void CVSBottomBar::ClickFullScreenButton()
{
	CDRConverterDlg* pMainWindow = (CDRConverterDlg*)AfxGetMainWnd();
	pMainWindow->EnterFullScreen();
}

// �����ԭ�󡱰�ťʱ��Ӧ����
void CVSBottomBar::ClickOriginSizeButton()
{
	CPageParent* pParent = (CPageParent*)GetParent();
	pParent->OnViewZoomDefault();
}

// ���õ�ǰ��ʾ����Ƭ������ˢ����ͼ
void CVSBottomBar::SetCurrentSlice(int nSlice)
{
	CPageParent* pParent = (CPageParent*)GetParent();
	pParent->LoadDR(nSlice);
}

// ����ͼ����Ϣ
void CVSBottomBar::SetImageInfomation()
{
	CPageParent* pParent = (CPageParent*)GetParent();
	CVSImageInfoDlg *pDlg = m_pBottomBarWnd->GetImagePropPageDlg();
	pDlg->SetImageInfomation(pParent->GetDataHeader(), pParent->GetImage(),
		pParent->GetFilePath(), pParent->GetZoomRate(),
		pParent->GetSlice(), pParent->GetCurrentSlice());
}