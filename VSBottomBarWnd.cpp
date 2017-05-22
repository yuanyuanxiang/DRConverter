// VSBottomBarWnd.cpp : ʵ���ļ�
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
	// ������ɾ���봰����ص�10��ָ��
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

// CVSBottomBarWnd ��Ϣ�������

int CVSBottomBarWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// ����
	m_pFont = new CFont;
	m_pFont->CreatePointFont(100, _T("������"));

	// ������ʾ
	EnableToolTips(true);
	m_ToolTip.Create(this);
	m_ToolTip.Activate(true);
	m_ToolTip.SetDelayTime(TTDT_INITIAL, 200); //�ӳ�200ms����ʾ
	m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 3000);//��ʾ����ʱ��3000msʱ��

	// �½��༭��
	m_pCurrentSlice = new CVSCurrentSliceEdit;
	// �����༭���ĸ���������Ϊ���ڷ����Ͽ�λ�á������ڡ�ID��
	m_pCurrentSlice->Create(WS_CHILD | ES_CENTER | ES_WANTRETURN | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_CURRENT_SLICE);
	m_pCurrentSlice->SetWindowText(_T("1"));
	// ��������
	m_pCurrentSlice->SetFont(m_pFont);
	m_ToolTip.AddTool(m_pCurrentSlice, _T("��ǰ��Ƭ"));

	// ��Ƭ����
	m_pTotalSlice = new CStatic;
	m_pTotalSlice->Create(NULL, WS_CHILD | SS_NOTIFY | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_TOTAL_SLICE);
	m_pTotalSlice->SetFont(m_pFont);
	m_ToolTip.AddTool(m_pTotalSlice, _T("�ܵ���Ƭ"));

	// ��һ����Ƭ
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
		m_pPreviewSlice->SetWindowText(_T("ǰ"));
		m_pPreviewSlice->SetFont(m_pFont);
	}
	m_ToolTip.AddTool(m_pPreviewSlice, _T("��һ��"));

	// ��Ƭ������
	m_pSliceSlider = new CVSSliceSliderCtrl;
	m_pSliceSlider->Create(WS_CHILD | TBS_HORZ | TBS_AUTOTICKS | WS_TABSTOP, CRect(), this, ID_BOTTOMBAR_SLICE_SLIDER);
	m_pSliceSlider->SetFont(m_pFont);
	m_ToolTip.AddTool(m_pSliceSlider, _T("ѡ����Ƭ����"));

	// ��һ����Ƭ
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
		m_pNextSlice->SetWindowText(_T("��"));
		m_pNextSlice->SetFont(m_pFont);
	}
	m_ToolTip.AddTool(m_pNextSlice, _T("��һ��"));

	// ������Ƭ
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
	m_ToolTip.AddTool(m_pAddSlice, _T("����"));

	// ɾ����Ƭ
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
	m_ToolTip.AddTool(m_pDeleteSlice, _T("ɾ��"));

	// ȫ����ť
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
	m_ToolTip.AddTool(m_pFullScreen, _T("ȫ��"));

	// "ԭ��"��ť
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
	m_ToolTip.AddTool(m_pOriginSize, _T("ԭ��"));

	// ��Ƭ��Ϣ��ť
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
	m_ToolTip.AddTool(m_pSliceInfo, _T("��Ƭ����"));

	// ͼ������
	m_pImageInfoDlg = new CVSImageInfoDlg;
	m_pImageInfoDlg->Create(IDD_IMAGE_INFO_DLG, this);
	m_pImageInfoDlg->ShowWindow(SW_HIDE);

	return 0;
}

// �ı�༭�����ݵ���Ӧ����
BOOL CVSBottomBarWnd::ChangeCurrentSliceEdit()
{
	// �û��������
	m_bInputingCurrentSlice = FALSE;
	// ʹ�༭��ʧȥ���㣬�ѽ���ת�Ƹ�������
	SetFocus();
	CString strText;
	m_pCurrentSlice->GetWindowText(strText);
	int nNum = _tstoi(strText) - 1;
	if (strText.IsEmpty())
		nNum = m_nCurrentSlice;

	// nNum����[0, m_nSlice)
	if (nNum < 0 || nNum >= m_nSlice)
	{
		// ������Ч����ʾ�û�
		MessageBox(_T("�����������Ƭ������Ч��"), _T("��ʾ"), MB_OK | MB_ICONWARNING);
		SetCurrentSliceEditText(m_nCurrentSlice);
		return FALSE;
	}
	m_nCurrentSlice = nNum;
	m_pSliceSlider->SetPos(nNum);
	SetCurrentSlice(m_nCurrentSlice);
	SetCurrentSliceEditText(m_nCurrentSlice);
	return TRUE;
}

// �϶�������ʱ��Ӧ����
void CVSBottomBarWnd::OnMoveSliceSlider(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_nCurrentSlice = m_pSliceSlider->GetPos();
	SetCurrentSlice(m_nCurrentSlice);
	SetCurrentSliceEditText(m_nCurrentSlice);
	*pResult = 0;
}

// ���϶���������������Ƭ����ʱ����
void CVSBottomBarWnd::SetCurrentSlice(int nSlice)
{
	TRACE("* ����ת�Ƶ����࣬������Ƭ���� *\n");
	((CVSBottomBar*)GetParent())->SetCurrentSlice(nSlice);
}

// ���õ�ǰ��Ƭ�༭������ʾ����Ƭ����
void CVSBottomBarWnd::SetCurrentSliceEditText(int nSlice)
{
	CString strText;
	strText.Format(_T("%d"), nSlice + 1);
	m_pCurrentSlice->SetWindowText(strText);
}

// ��ȡView����Ļ����(��������)
void CVSBottomBarWnd::GetViewScreenRect(CRect &rect)
{
	// ��ȡ�游��ָ�룬���ײ��������ĸ���
	CWnd* pGP = GetParent()->GetParent();
	pGP->GetClientRect(&rect);
	pGP->ClientToScreen(&rect);
}

// �����һ�㰴ť��Ӧ����
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

// �����һ�㰴ť��Ӧ����
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

// �����Ƭ��Ϣ��ť��Ӧ����
void CVSBottomBarWnd::ClickSliceInfoButton()
{
	ChangeCurrentSliceEdit();

	CRect rect;
	GetViewScreenRect(rect);

	// ����Ի��������صģ�����ʾ����������
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
	TRACE("* ����ת�Ƶ����࣬������Ƭ *\n");
	((CVSBottomBar*)GetParent())->ClickAddSliceButton();
}

void CVSBottomBarWnd::ClickDeleteSliceButton()
{
	TRACE("* ����ת�Ƶ����࣬ɾ����Ƭ *\n");
	((CVSBottomBar*)GetParent())->ClickDeleteSliceButton();
}

// ȫ����ť����Ӧ����
void CVSBottomBarWnd::ClickFullScreenButton()
{
	TRACE("* ����ת�Ƶ����࣬ȫ��������ʾ *\n");
	((CVSBottomBar*)GetParent())->ClickFullScreenButton();
}

// ��ԭ�󡱣���ͼ����ԭ��С������ʾ
void CVSBottomBarWnd::ClickOriginSizeButton()
{
	TRACE("* ����ת�Ƶ����࣬ԭ�������ʾ *\n");
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

// ���Ի����ƶ�����ͼ���½�
void CVSBottomBarWnd::MoveImagePropPageDlg()
{
	if (!m_pImageInfoDlg->IsWindowVisible())
		return;

	CRect rect;
	GetViewScreenRect(rect);

	// ���Ի����ƶ�����ͼ���½�
	m_pImageInfoDlg->MoveWindowEx(rect);
}

// �ж��Ƿ�Ϊ2Dͼ��
BOOL CVSBottomBarWnd::Is2DImage()
{
	return (m_nSlice == 1);
}

void CVSBottomBarWnd::Init(int nCurrentSlice, int nSlice)
{
	m_nCurrentSlice = nCurrentSlice;
	m_nSlice = nSlice;
	// ���nSliceΪ1����2Dͼ������һЩ�ؼ�
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

	// ��ʾ��ȫ��������ԭ�󡱡���ͼ����Ϣ����ť
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
	// ��ǰ��Ƭ
	if (pMsg->hwnd == m_pCurrentSlice->m_hWnd)
	{
		switch (pMsg->message)
		{
		case WM_KEYDOWN:
			// �س�����Ӧ
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
	// ��һ��
	else if (pMsg->hwnd == m_pPreviewSlice->m_hWnd)
	{
		switch (pMsg->message)
		{
		case WM_LBUTTONDOWN:
			ClickPreviewSliceButton();
			break;
		case WM_MOUSEMOVE:
			// ����ȡ�ý��������Ӧ��������������Ϣ
			m_pPreviewSlice->SetFocus();
			break;
		default:
			break;
		}
	}
	// ��һ��
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
	// ����
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
	// ɾ��
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
	// ȫ��
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
	// ԭ��
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
	// ��Ƭ��Ϣ
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

// ���϶�������ʱͬ��������Ƭ�����༭�����ֵ
void CVSBottomBarWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nTic = m_pSliceSlider->GetPos();
	SetCurrentSliceEditText(nTic);
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}