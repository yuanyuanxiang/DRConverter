// VSImageInfoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "VSImageInfoDlg.h"
#include "afxdialogex.h"
#include "VSBottomBar.h"
#include "functions.h"
#include "DRConverterDlg.h"
#include "Page1.h"


// CVSImageInfoDlg �Ի���

IMPLEMENT_DYNAMIC(CVSImageInfoDlg, CDialogEx)

CVSImageInfoDlg::CVSImageInfoDlg(CWnd* pParent)
	: CDialogEx(CVSImageInfoDlg::IDD, pParent)
	, m_bIs2DImage(TRUE)
	, m_DataHeader(NULL)
	, m_pImage(NULL)
	, m_strFilePath(_T(""))
	, m_fZoomRate(1.f)
	, m_nSlices(1)
	, m_nCurSlices(0)
	, m_nDelayTime(200)
	, m_nDlgWidth(100)
	, m_nDlgHeight(100)
	, m_bNotCaptured(TRUE)
{

}

CVSImageInfoDlg::~CVSImageInfoDlg()
{

}

void CVSImageInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVSImageInfoDlg, CDialogEx)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CVSImageInfoDlg ��Ϣ�������

// ����֮ǰ���ȵ���SetImageInfomation
BOOL CVSImageInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = NULL;
	hIcon = AfxGetApp()->LoadIcon(IDI_IMAGE_PROP_PAGE);
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	// GetImageInfomation();
	// ��ʼ��ͼ��Ի����С
	CRect rect;
	GetWindowRect(&rect);
	m_nDlgWidth = rect.Width();
	m_nDlgHeight = 680;

	ShowWindow(SW_HIDE);

	// ���÷ֲ�����:�趨һ���µ���չ���
	SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);
	// ����͸����:crKeyΪ�������ɫ,bAlphaΪ͸����,dwFlagsΪ͸����ʽ(��ȡֵΪLWA_ALPHAʱ,crKey������Ч,bAlpha������Ч)
	SetLayeredWindowAttributes(0, 200, LWA_ALPHA);

	return TRUE;
}

// ���Ϊ2dͼ�񣬷���TRUE
BOOL CVSImageInfoDlg::Is2DImage()
{
	return m_bIs2DImage;
}

// ��������ͷ��ͼ��ָ�롢�ļ�·�������ű�������������Ϣ
void CVSImageInfoDlg::SetImageInfomation(CVSDataHeader* DataHeader, CImage *pImage, CString strFilePath, float fZoomRate, int nSlices, int nCurSlices)
{
	m_DataHeader = DataHeader;
	m_pImage = pImage;
	m_strFilePath = strFilePath;
	m_fZoomRate = fZoomRate;
	m_nSlices = nSlices;
	m_nCurSlices = nCurSlices;
}

// ��ȡ���ڿؼ�������ʾͼ�����Ϣ
void CVSImageInfoDlg::GetImageInfomation()
{
	TRACE("* ����ת�Ƶ�������������ͼ����Ϣ *\n");
	CDRConverterDlg *pMainWindow = (CDRConverterDlg *)AfxGetMainWnd();
	pMainWindow->GetCurSelPage()->GetBottomBar()->SetImageInfomation();

	if (m_pImage == NULL || m_pImage->IsNull())
		return;

	m_bIs2DImage = (m_nSlices == 1);

	// �ļ���Ϣ
	GetDlgItem(IDC_EDIT_FILE_NAME)->SetWindowText(GetFileName(m_strFilePath));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_FILE_NAME), GetFileName(m_strFilePath));
	GetDlgItem(IDC_EDIT_FILE_PATH)->SetWindowText(m_strFilePath);
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_FILE_PATH), m_strFilePath);
	GetDlgItem(IDC_EDIT_FILE_TYPE)->SetWindowText(GetFileExt(m_strFilePath));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_FILE_TYPE), GetFileExt(m_strFilePath));

	CFileStatus fileStatus;
	// ���ԣ���ȡ�ļ���Ϣһ���ɹ�
	ASSERT(CFile::GetStatus(m_strFilePath, fileStatus));
	float fSize = (float)fileStatus.m_size;
	CString strUnit;
	if (fSize > 1024 * 1024 * 1024)
	{
		fSize /= (1.f * 1024 * 1024 * 1024);
		strUnit = _T("Gb");
	}
	else if (fSize > 1024 * 1024)
	{
		fSize /= (1.f * 1024 * 1024);
		strUnit = _T("Mb");
	}
	else if (fSize > 1024)
	{
		fSize /= 1024.f;
		strUnit = _T("Kb");
	}
	else
	{
		strUnit = _T("byte");
	}
	CString strText;
	strText.Format(_T("%.2f ") + strUnit, fSize);
	GetDlgItem(IDC_EDIT_FILE_SIZE)->SetWindowText(strText);
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_FILE_SIZE), strText);
	CString strTimeFormat = _T("%Y��%m��%d��, %H:%M:%S");
	GetDlgItem(IDC_EDIT_FILE_TIME)->SetWindowText(fileStatus.m_ctime.Format(strTimeFormat));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_FILE_TIME), fileStatus.m_ctime.Format(strTimeFormat));
	GetDlgItem(IDC_EDIT_ALTER_TIME)->SetWindowText(fileStatus.m_mtime.Format(strTimeFormat));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_ALTER_TIME), fileStatus.m_mtime.Format(strTimeFormat));

	// ͼ����Ϣ
	strText.Format(_T("%d"), m_pImage->GetBPP());
	GetDlgItem(IDC_EDIT_IMAGE_BPP)->SetWindowText(strText);
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_IMAGE_BPP), strText);
	strText.Format(_T("%d"), m_nSlices);
	GetDlgItem(IDC_EDIT_IMAGE_SLICES)->SetWindowText(strText);
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_IMAGE_SLICES), strText);
	strText.Format(_T("%d"), m_nCurSlices);
	GetDlgItem(IDC_EDIT_CURRENT_SLICES)->SetWindowText(strText);
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_CURRENT_SLICES), strText);
	strText.Format(_T("%.1f"), m_fZoomRate);
	GetDlgItem(IDC_EDIT_ZOOM_RATE)->SetWindowText(strText);
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_ZOOM_RATE), strText);
	strText.Format(_T("%d"), m_pImage->GetWidth());
	GetDlgItem(IDC_EDIT_IMAGE_WIDTH)->SetWindowText(strText);
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_IMAGE_WIDTH), strText);
	strText.Format(_T("%d"), m_pImage->GetHeight());
	GetDlgItem(IDC_EDIT_IMAGE_HEIGHT)->SetWindowText(strText);
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_IMAGE_HEIGHT), strText);
	
	CRect rect;
	GetWindowRect(&rect);
	// �����2dͼ��(��dr)������Ҫ��ʾ�ɼ�������λ����Ϣ
	if (Is2DImage() && GetFileExt(m_strFilePath).CompareNoCase(_T("dr")) != 0)
	{
		m_nDlgWidth = rect.Width();
		m_nDlgHeight = 205;
	}
	else
	{
		m_nDlgWidth = rect.Width();
		m_nDlgHeight = 680;

		/** ֱ�����ô������Ĳ��� **/

		// ����Դ
		strText.Format(_T("%.2f"), m_DataHeader->Voltage);
		GetDlgItem(IDC_EDIT_VOLTAGE)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_VOLTAGE), strText);
		strText.Format(_T("%.2f"), m_DataHeader->Current);
		GetDlgItem(IDC_EDIT_CURRENT)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_CURRENT), strText);
		strText.Format(_T("%.2f"), m_DataHeader->Frequency);
		GetDlgItem(IDC_EDIT_FREQUENCY)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_FREQUENCY), strText);
		strText.Format(_T("%.2f"), m_DataHeader->TargetPower);
		GetDlgItem(IDC_EDIT_TARGET_POWER)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_TARGET_POWER), strText);
		strText.Format(_T("%.2f"), m_DataHeader->FocusSize);
		GetDlgItem(IDC_EDIT_FOCUS_SIZE)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_FOCUS_SIZE), strText);
		// ̽����
		strText.Format(_T("%d"), m_DataHeader->DetectorType);
		GetDlgItem(IDC_EDIT_DETECTOR_TYPE)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_DETECTOR_TYPE), strText);
		strText.Format(_T("%.2f"), m_DataHeader->IntegrationTime);
		GetDlgItem(IDC_EDIT_INTEGRATION_TIME)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_INTEGRATION_TIME), strText);
		strText.Format(_T("%d"), m_DataHeader->Pixelbinning);
		GetDlgItem(IDC_EDIT_PIXEL_BINNING)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_PIXEL_BINNING), strText);
		strText.Format(_T("%d"), m_DataHeader->Framebinning);
		GetDlgItem(IDC_EDIT_FRAME_BINNING)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_FRAME_BINNING), strText);
		strText.Format(_T("%.2f"), m_DataHeader->UnitWidth);
		GetDlgItem(IDC_EDIT_UINT_WIDTH)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_UINT_WIDTH), strText);
		// ���β���
		strText.Format(_T("%.2f"), m_DataHeader->SOD);
		GetDlgItem(IDC_EDIT_SOD)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_SOD), strText);
		strText.Format(_T("%.2f"), m_DataHeader->SDD);
		GetDlgItem(IDC_EDIT_SDD)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_SDD), strText);
		strText.Format(_T("%.2f"), m_DataHeader->RowPixelWidth);
		GetDlgItem(IDC_EDIT_ROW_PIXEL_WIDTH)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_ROW_PIXEL_WIDTH), strText);
		strText.Format(_T("%.2f"), m_DataHeader->ColumnPixelWidth);
		GetDlgItem(IDC_EDIT_COLUMN_PIXEL_WIDTH)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_COLUMN_PIXEL_WIDTH), strText);
		// CT
		strText.Format(_T("%.2f"), m_DataHeader->SliceHeight);
		GetDlgItem(IDC_EDIT_SLICE_HEIGHT)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_SLICE_HEIGHT), strText);
		strText.Format(_T("%d"), m_DataHeader->RotateAngel);
		GetDlgItem(IDC_EDIT_ROTATE_ANGEL)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_ROTATE_ANGEL), strText);
		strText.Format(_T("%d"), m_DataHeader->RotaionNum);
		GetDlgItem(IDC_EDIT_ROTATION_NUM)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_ROTATION_NUM), strText);
		strText.Format(_T("%d"), m_DataHeader->RotateCount);
		GetDlgItem(IDC_EDIT_ROTATE_COUNT)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_ROTATE_COUNT), strText);
		// ɨ�����
		strText.Format(_T("%.2f"), m_DataHeader->DRStartHeight);
		GetDlgItem(IDC_EDIT_DRSTART_HEIGHT)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_DRSTART_HEIGHT), strText);
		strText.Format(_T("%.2f"), m_DataHeader->DREndHeight);
		GetDlgItem(IDC_EDIT_DREND_HEIGHT)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_DREND_HEIGHT), strText);
		//strText.Format(_T("%s"), "������Ա");
		//GetDlgItem(IDC_EDIT_OPERATOR)->SetWindowText(strText);
		//strText.Format(_T("%s"), "����ʱ��");
		//GetDlgItem(IDC_EDIT_OPERATION_TIME)->SetWindowText(strText);
		// У������
		strText.Format(_T("%.2f"), m_DataHeader->RandomMove);
		GetDlgItem(IDC_EDIT_RANDOM_MOVE)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_RANDOM_MOVE), strText);
		strText.Format(_T("%.2f"), m_DataHeader->OutOfView);
		GetDlgItem(IDC_EDIT_OUT_OF_VIEW)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_OUT_OF_VIEW), strText);
		strText.Format(_T("%.2f"), m_DataHeader->CenterOffset1);
		GetDlgItem(IDC_EDIT_CENTER_OFFSET1)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_CENTER_OFFSET1), strText);
		strText.Format(_T("%.2f"), m_DataHeader->CenterOffset2);
		GetDlgItem(IDC_EDIT_CENTER_OFFSET2)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_CENTER_OFFSET2), strText);
	}
	::SetWindowPos(GetSafeHwnd(), HWND_TOP, 0, 0, m_nDlgWidth, m_nDlgHeight, SWP_NOACTIVATE | SWP_NOMOVE);
}

// �������öԻ���λ�ã�ʹ�Ի�������½���rect�����½�
void CVSImageInfoDlg::MoveWindowEx(const CRect& rect, BOOL bRepaint)
{
	MoveWindow(rect.right - m_nDlgWidth, rect.bottom - m_nDlgHeight - BOTTTOM_BAR_SIZE + 1, m_nDlgWidth, m_nDlgHeight, bRepaint);
}

// ��д����ʾ���ڵĺ�����ʹ�����ԡ�����/����(����)����ʽ����
BOOL CVSImageInfoDlg::ShowWindow(int nShow)
{
	if (nShow == SW_SHOW)
	{
		//SetFocus();
		// ʹ�û������ͣ��������ϣ���ʾ����
		AnimateWindow(m_nDelayTime, AW_SLIDE | AW_VER_NEGATIVE | AW_ACTIVATE);
	}
	else if (nShow == SW_HIDE)
	{
		AnimateWindow(m_nDelayTime, AW_SLIDE | AW_VER_POSITIVE | AW_HIDE);
	}
	return CDialogEx::ShowWindow(nShow);
}

// ���ƶ����ڣ�����ʾ����
void CVSImageInfoDlg::ShowWindowEx(const CRect& rect, int nShow)
{
	if (nShow == SW_SHOW)
	{
		MoveWindowEx(rect);
		SetCapture();
		m_bNotCaptured = FALSE;
		// ʹ�û������ͣ��������ϣ���ʾ����
		AnimateWindow(m_nDelayTime, AW_SLIDE | AW_VER_NEGATIVE | AW_ACTIVATE);
	}
	else if (nShow == SW_HIDE)
	{
		AnimateWindow(m_nDelayTime, AW_SLIDE | AW_VER_POSITIVE | AW_HIDE);
	}
	CDialogEx::ShowWindow(nShow);
}

int CVSImageInfoDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// ������ʾ
	EnableToolTips(true);
	m_ToolTip.Create(this);
	m_ToolTip.Activate(true);
	m_ToolTip.SetDelayTime(TTDT_INITIAL, 200); //�ӳ�200ms����ʾ
	m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 3000);//��ʾ����ʱ��3000msʱ��

	return 0;
}

BOOL CVSImageInfoDlg::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);
	if (pMsg->message == WM_LBUTTONDOWN)
	{
		m_bNotCaptured = TRUE;
		ReleaseCapture();

		CPoint pt = pMsg->pt;
		CRect rect;
		GetWindowRect(&rect);
		if (!IsPointInRect(pt, rect, 5))
		{
			ShowWindow(SW_HIDE);
		}
	}
	else if (pMsg->message == WM_MOUSEMOVE)
	{
		CPoint pt = pMsg->pt;
		CRect rect;
		GetWindowRect(&rect);
		if (m_bNotCaptured && IsMouseLeaving(pt, rect, 5))
		{
			m_bNotCaptured = FALSE;
			SetCapture();
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

// �����ھ��ξ����ڲ�
BOOL IsPointInRect(CPoint pt, CRect rt, int dl)
{
	return (rt.left + dl <= pt.x && pt.x < rt.right - dl && rt.top + dl <= pt.y && pt.y < rt.bottom - dl);
}

// Ԥ����꽫�뿪��������
BOOL IsMouseLeaving(CPoint pt, CRect rt, int dl)
{
	return (IsPointInRect(pt, rt, dl) == FALSE);
}