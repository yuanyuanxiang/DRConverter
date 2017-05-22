// VSImageInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "VSImageInfoDlg.h"
#include "afxdialogex.h"
#include "VSBottomBar.h"
#include "functions.h"
#include "DRConverterDlg.h"
#include "Page1.h"


// CVSImageInfoDlg 对话框

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

// CVSImageInfoDlg 消息处理程序

// 调用之前请先调用SetImageInfomation
BOOL CVSImageInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = NULL;
	hIcon = AfxGetApp()->LoadIcon(IDI_IMAGE_PROP_PAGE);
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	// GetImageInfomation();
	// 初始化图像对话框大小
	CRect rect;
	GetWindowRect(&rect);
	m_nDlgWidth = rect.Width();
	m_nDlgHeight = 680;

	ShowWindow(SW_HIDE);

	// 设置分层属性:设定一个新的扩展风格
	SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);
	// 设置透明度:crKey为掩码的颜色,bAlpha为透明度,dwFlags为透明方式(当取值为LWA_ALPHA时,crKey参数无效,bAlpha参数有效)
	SetLayeredWindowAttributes(0, 200, LWA_ALPHA);

	return TRUE;
}

// 如果为2d图像，返回TRUE
BOOL CVSImageInfoDlg::Is2DImage()
{
	return m_bIs2DImage;
}

// 设置数据头、图像指针、文件路径、缩放比例、层数等信息
void CVSImageInfoDlg::SetImageInfomation(CVSDataHeader* DataHeader, CImage *pImage, CString strFilePath, float fZoomRate, int nSlices, int nCurSlices)
{
	m_DataHeader = DataHeader;
	m_pImage = pImage;
	m_strFilePath = strFilePath;
	m_fZoomRate = fZoomRate;
	m_nSlices = nSlices;
	m_nCurSlices = nCurSlices;
}

// 获取并在控件上面显示图像的信息
void CVSImageInfoDlg::GetImageInfomation()
{
	TRACE("* 功能转移到工具栏，设置图像信息 *\n");
	CDRConverterDlg *pMainWindow = (CDRConverterDlg *)AfxGetMainWnd();
	pMainWindow->GetCurSelPage()->GetBottomBar()->SetImageInfomation();

	if (m_pImage == NULL || m_pImage->IsNull())
		return;

	m_bIs2DImage = (m_nSlices == 1);

	// 文件信息
	GetDlgItem(IDC_EDIT_FILE_NAME)->SetWindowText(GetFileName(m_strFilePath));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_FILE_NAME), GetFileName(m_strFilePath));
	GetDlgItem(IDC_EDIT_FILE_PATH)->SetWindowText(m_strFilePath);
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_FILE_PATH), m_strFilePath);
	GetDlgItem(IDC_EDIT_FILE_TYPE)->SetWindowText(GetFileExt(m_strFilePath));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_FILE_TYPE), GetFileExt(m_strFilePath));

	CFileStatus fileStatus;
	// 断言：获取文件信息一定成功
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
	CString strTimeFormat = _T("%Y年%m月%d日, %H:%M:%S");
	GetDlgItem(IDC_EDIT_FILE_TIME)->SetWindowText(fileStatus.m_ctime.Format(strTimeFormat));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_FILE_TIME), fileStatus.m_ctime.Format(strTimeFormat));
	GetDlgItem(IDC_EDIT_ALTER_TIME)->SetWindowText(fileStatus.m_mtime.Format(strTimeFormat));
	m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_ALTER_TIME), fileStatus.m_mtime.Format(strTimeFormat));

	// 图像信息
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
	// 如果是2d图像(非dr)，则不需要显示采集参数及位置信息
	if (Is2DImage() && GetFileExt(m_strFilePath).CompareNoCase(_T("dr")) != 0)
	{
		m_nDlgWidth = rect.Width();
		m_nDlgHeight = 205;
	}
	else
	{
		m_nDlgWidth = rect.Width();
		m_nDlgHeight = 680;

		/** 直接利用传进来的参数 **/

		// 射线源
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
		// 探测器
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
		// 几何参数
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
		// 扫描参数
		strText.Format(_T("%.2f"), m_DataHeader->DRStartHeight);
		GetDlgItem(IDC_EDIT_DRSTART_HEIGHT)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_DRSTART_HEIGHT), strText);
		strText.Format(_T("%.2f"), m_DataHeader->DREndHeight);
		GetDlgItem(IDC_EDIT_DREND_HEIGHT)->SetWindowText(strText);
		m_ToolTip.AddTool(GetDlgItem(IDC_EDIT_DREND_HEIGHT), strText);
		//strText.Format(_T("%s"), "操作人员");
		//GetDlgItem(IDC_EDIT_OPERATOR)->SetWindowText(strText);
		//strText.Format(_T("%s"), "操作时间");
		//GetDlgItem(IDC_EDIT_OPERATION_TIME)->SetWindowText(strText);
		// 校正参数
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

// 重新设置对话框位置，使对话框的右下角是rect的右下角
void CVSImageInfoDlg::MoveWindowEx(const CRect& rect, BOOL bRepaint)
{
	MoveWindow(rect.right - m_nDlgWidth, rect.bottom - m_nDlgHeight - BOTTTOM_BAR_SIZE + 1, m_nDlgWidth, m_nDlgHeight, bRepaint);
}

// 重写了显示窗口的函数，使窗口以“渐入/渐出(滑动)”方式出现
BOOL CVSImageInfoDlg::ShowWindow(int nShow)
{
	if (nShow == SW_SHOW)
	{
		//SetFocus();
		// 使用滑动类型，从下往上，显示窗口
		AnimateWindow(m_nDelayTime, AW_SLIDE | AW_VER_NEGATIVE | AW_ACTIVATE);
	}
	else if (nShow == SW_HIDE)
	{
		AnimateWindow(m_nDelayTime, AW_SLIDE | AW_VER_POSITIVE | AW_HIDE);
	}
	return CDialogEx::ShowWindow(nShow);
}

// 先移动窗口，再显示窗口
void CVSImageInfoDlg::ShowWindowEx(const CRect& rect, int nShow)
{
	if (nShow == SW_SHOW)
	{
		MoveWindowEx(rect);
		SetCapture();
		m_bNotCaptured = FALSE;
		// 使用滑动类型，从下往上，显示窗口
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

	// 工具提示
	EnableToolTips(true);
	m_ToolTip.Create(this);
	m_ToolTip.Activate(true);
	m_ToolTip.SetDelayTime(TTDT_INITIAL, 200); //延迟200ms后显示
	m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 3000);//显示持续时间3000ms时间

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

// 鼠标点在矩形局域内部
BOOL IsPointInRect(CPoint pt, CRect rt, int dl)
{
	return (rt.left + dl <= pt.x && pt.x < rt.right - dl && rt.top + dl <= pt.y && pt.y < rt.bottom - dl);
}

// 预测鼠标将离开矩形区域
BOOL IsMouseLeaving(CPoint pt, CRect rt, int dl)
{
	return (IsPointInRect(pt, rt, dl) == FALSE);
}