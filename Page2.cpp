// Page2.cpp : 实现文件
//

#include "stdafx.h"
#include "DRConverter.h"
#include "Page2.h"
#include "functions.h"
#include "DlgProcess.h"

// CPage2 对话框

IMPLEMENT_DYNAMIC(CPage2, CPageParent)

CPage2::CPage2(CWnd* pWnd) : CPageParent(CPage2::IDD, pWnd)
{
	m_bModified = FALSE;
	m_nCurBlock = -1;
}

CPage2::~CPage2()
{
	ResetMapping();
}


BEGIN_MESSAGE_MAP(CPage2, CPageParent)
	ON_BN_CLICKED(IDC_CHOOSING, &CPage2::OnBnClickedChoosing)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CPage2 消息处理程序


// 创建隐射，当需要插入切片时请传入额外的内存空间
BOOL CPage2::CreateMapping(int nExtraSize)
{
	// 重置文件映射
	ResetMapping();

	// 创建文件对象
	m_hFile = CreateFile(m_strFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		TRACE("创建文件对象失败,错误代码:%d\n", GetLastError());
		return FALSE;
	}
	// 得到系统分配粒度
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	DWORD dwGran = SysInfo.dwAllocationGranularity;
	// 通常情况下dwGran = 2^16，则kbGran = 64
	if (dwGran % 1024)
	{
		MessageBox(_T("不支持当前操作系统的分配粒度！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
	}
	// 得到文件尺寸
	DWORD dwFileSizeHigh = 0;
	m_i64FileSize = GetFileSize(m_hFile, &dwFileSizeHigh);
	m_i64FileSize |= (((__int64)dwFileSizeHigh) << 32);
	SetFilePointer(m_hFile, nExtraSize, NULL, FILE_END);
	SetEndOfFile(m_hFile);
	m_i64FileSize = m_i64FileSize + nExtraSize;
	m_kbGranularity = dwGran / 1024;
	m_hFileMap = CreateFileMapping(m_hFile, NULL, PAGE_READWRITE, 0, m_i64FileSize, _T("Splitting"));
	if (m_hFileMap == NULL)
	{
		TRACE("创建文件映射对象失败,错误代码:%d\n", GetLastError());
		return FALSE;
	}
	m_nCurBlock = -1;
	return TRUE;
}

// 映射视图并取得某帧图像的地址
BYTE* CPage2::MappingView(int nCurSlice)
{
	// 块大小
	m_dwBlockBytes = m_kbGranularity * GetSliceBytesNum();
	// 位移量
	int nBlocks = nCurSlice / m_kbGranularity;
	int nNumber = nCurSlice % m_kbGranularity;
	if (nBlocks == m_nCurBlock)
	{
		return (BYTE*)m_pMapAddress + nNumber * GetSliceBytesNum();;
	}
	else
	{
		m_nCurBlock = nBlocks;
	}
	m_i64FileOffset = m_nCurBlock * m_dwBlockBytes;
	if (m_i64FileSize - m_i64FileOffset < m_dwBlockBytes)
	{
		m_dwBlockBytes = m_i64FileSize - m_i64FileOffset;
	}
	// 撤消文件映像
	UnmapViewOfFile(m_pMapAddress);
	m_pMapAddress = NULL;
	// 重新映射视图
	m_pMapAddress = (BYTE*)MapViewOfFile(m_hFileMap, FILE_MAP_ALL_ACCESS,
		(DWORD)(m_i64FileOffset >> 32), (DWORD)(m_i64FileOffset & 0xFFFFFFFF),
		m_dwBlockBytes);
	if (m_pMapAddress == NULL)
	{
		TRACE("映射文件映射失败,错误代码:%d\n", GetLastError());
		return NULL;
	}
	return (BYTE*)m_pMapAddress + nNumber * GetSliceBytesNum();
}

void CPage2::OnBnClickedChoosing()
{
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("DR文件 (*.dr)|*.dr|所有文件 (*.*)|*.*||"), NULL);

	if (dlgFile.DoModal() == IDOK)
	{
		m_strSelected = dlgFile.GetPathName();
		m_EditSelected.SetWindowText(m_strSelected);
		m_strFilePath = m_strSelected;

		CFile file;
		if (!file.Open(m_strFilePath, CFile::modeRead))
		{
			MessageBox(_T("打开") + m_strFilePath + _T("遇到错误！"),
				_T("错误"), MB_OK | MB_ICONERROR);
			return;
		}
		int nDataHeader = sizeof(CVSDataHeader);
		if (file.GetLength() < nDataHeader)
		{
			MessageBox(_T("文件") + m_strFilePath + _T("尺寸错误！"),
				_T("错误"), MB_OK | MB_ICONERROR);
			file.Close();
			return;
		}
		// 获取图像的基本信息
		file.Read(&m_DataHeader, nDataHeader);
		m_nWidth = m_DataHeader.Width;
		m_nHeight = m_DataHeader.Height;
		file.Close();

		if (CreateMapping(0) == FALSE)
			return;

		if (m_i64FileSize % GetSliceBytesNum())
		{
			MessageBox(_T("文件") + m_strFilePath + _T("尺寸不对！"),
				_T("错误"), MB_OK | MB_ICONERROR);
			return;
		}
		m_nSlice = m_i64FileSize / GetSliceBytesNum();

		if (GetFileBytesNum() < m_i64FileSize)
			return;

		LoadDR(0, TRUE);
	}
	CPageParent::OnBnClickedChoosing();
}

void CPage2::OnBnClickedApply()
{
	// 存放选择的目录路径 
	wchar_t szPath[MAX_PATH];

	ZeroMemory(szPath, sizeof(szPath));

	BROWSEINFO bi;
	bi.hwndOwner = m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = _T("请选择目录");
	bi.ulFlags = 0x0040 | BIF_EDITBOX;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;

	// 弹出选择目录对话框
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);
	if (lp == NULL)
		return;
	if (SHGetPathFromIDList(lp, szPath) == TRUE)
	{
		CString strFolder, strFileName;
		strFolder.Format(_T("%s"), szPath);
		strFileName = GetFileName(m_strFilePath);
		strFileName = strFileName.Left(strFileName.GetLength() - 3);
		int nDataHeader = sizeof(CVSDataHeader);
		ULONGLONG nSliceLength = GetSliceBytesNum();
		// 将逐帧图像，写入到指定文件
		CDlgProcess *pProcessDlg = new CDlgProcess;
		pProcessDlg->Create(IDD_PROCESS_DLG, this);
		pProcessDlg->ShowWindow(SW_SHOW);
		pProcessDlg->m_ProcessCtrl.SetRange(0, m_nSlice - 1);
		CString strTitle;
		BeginWaitCursor();
		for (int i = 0; i < m_nSlice; ++i)
		{
			CString strCurFile;
			strCurFile.Format(strFolder + _T("\\") + strFileName + _T("_%.4d.dr"), i);
			CFile file;
			if (!file.Open(strCurFile, CFile::modeCreate | CFile::modeWrite))
			{
				MessageBox(_T("打开") + strCurFile + _T("遇到错误！"),
					_T("错误"), MB_OK | MB_ICONERROR);
				EndWaitCursor();
				return;
			}
			BYTE *pCur = MappingView(i);
			file.Write(pCur, nSliceLength);
			file.Close();
			pProcessDlg->m_ProcessCtrl.SetPos(i);
			strTitle.Format(_T("进度(%d/%d)"), i + 1, m_nSlice);
			pProcessDlg->SetWindowText(strTitle);
		}
		EndWaitCursor();
		delete pProcessDlg;
	}
	CPageParent::OnBnClickedApply();
}

BOOL CPage2::LoadDR(int nCurSlice, BOOL bReLoad)
{
	if (bReLoad == FALSE && (m_nCurrentSlice == nCurSlice || m_nSlice == 0))
		return TRUE;

	if (!m_Image.IsNull())
	{
		m_Image.Destroy();
	}
	// 读取第nCurSlice张切片
	m_nCurrentSlice = nCurSlice;
	BYTE *pSrc = MappingView(m_nCurrentSlice);
	if (pSrc == NULL)
		return FALSE;
	m_Image.Create(m_nWidth, m_nHeight, 24, 0UL);
	BYTE *pDst = (BYTE*)m_Image.GetBits() + m_Image.GetPitch() * (m_Image.GetHeight() - 1);
	float *temp = new float[m_nWidth * m_nHeight];
	memcpy(temp, pSrc + DR_HEADER_SIZE, m_nWidth * m_nHeight * sizeof(float));
	Normalized(temp, m_nWidth, m_nHeight);
	int nRowlen = abs(m_Image.GetPitch());
	for (int i = 0; i < m_nHeight; ++i)
	{
		for (int j = 0; j < m_nWidth; ++j)
		{
			pDst[j * 3 + i * nRowlen] = (BYTE)temp[j + i * m_nWidth];
			pDst[1 + j * 3 + i * nRowlen] = (BYTE)temp[j + i * m_nWidth];
			pDst[2 + j * 3 + i * nRowlen] = (BYTE)temp[j + i * m_nWidth];
		}
	}
	delete[] temp;
	// 更新底部工具栏
	m_wndBottomBar.Init(m_nCurrentSlice, m_nSlice);
	SetImage(&m_Image);

	return TRUE;
}

int CPage2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPageParent::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 添加底部工具栏
	if (!m_wndBottomBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
	{
		TRACE0("Failed to create bottombar.\n");
		return FALSE;	    // fail to create
	}

	m_pBottomBar = &m_wndBottomBar;

	return 0;
}

void CPage2::DoDataExchange(CDataExchange* pDX)
{
	// TODO:  在此添加专用代码和/或调用基类

	CPageParent::DoDataExchange(pDX);
}

BOOL CPage2::OnInitDialog()
{
	CPageParent::OnInitDialog();

	m_ChooseTips.SetWindowText(_T("选择文件:"));
	m_BnApply.SetWindowText(_T("拆分"));

	return TRUE;
}

void CPage2::OnClose()
{
	Reset();
	// 更新底部工具栏
	m_wndBottomBar.Init(m_nCurrentSlice, m_nSlice);
	SetImage(NULL);
	Invalidate(TRUE);
}

// 更新切片层数
void CPage2::UpdateDataHeader(BYTE* pCur, int nSlice)
{
	CVSDataHeader DataHeader;
	memcpy(&DataHeader, pCur, sizeof(CVSDataHeader));
	DataHeader.Slice = nSlice;
	memcpy(pCur, &DataHeader, sizeof(CVSDataHeader));
}

// 在当前位置后面插入切片
BOOL CPage2::AddSlice()
{
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY,
		_T("DR文件 (*.dr)|*.dr|所有文件 (*.*)|*.*||"), NULL);

	if (dlgFile.DoModal() == IDOK)
	{
		CString strInsertedFile = dlgFile.GetPathName();
		// 创建另外一个文件内核对象
		HANDLE hFile = CreateFile(strInsertedFile, GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			TRACE("创建文件对象失败,错误代码:%d\n", GetLastError());
			return FALSE;
		}
		// 创建另外一个文件映射内核对象
		HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, GetSliceBytesNum(), _T("NewSlice"));
		if (hFileMap == NULL)
		{
			TRACE("创建文件映射对象失败,错误代码:%d\n", GetLastError());
			CloseHandle(hFile);
			return FALSE;
		}
		// 将文件数据映射到进程的地址空间
		BYTE* pMapAddress = (BYTE*)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, GetSliceBytesNum());
		if (pMapAddress == NULL)
		{
			TRACE("映射文件映射失败,错误代码:%d\n", GetLastError());
			CloseHandle(hFileMap);
			CloseHandle(hFile);
			return FALSE;
		}
		// 得到文件头
		CVSDataHeader DataHeader;
		int nDataHeader = sizeof(CVSDataHeader);
		memcpy(&DataHeader, pMapAddress, nDataHeader);
		int nWidth = DataHeader.Width;
		int nHeight = DataHeader.Height;
		int nSliceLength = GetSliceBytesNum();
		// 匹配才可以插入新切片
		DWORD dwFileSizeHigh = 0;
		__int64 nFileSize = GetFileSize(hFile, &dwFileSizeHigh);
		nFileSize |= (((__int64)dwFileSizeHigh) << 32);
		if (nWidth != m_nWidth || nHeight != m_nHeight || nFileSize != nSliceLength
			|| CreateMapping(nSliceLength) == FALSE)
		{
			UnmapViewOfFile(pMapAddress);
			CloseHandle(hFileMap);
			CloseHandle(hFile);
			return FALSE;
		}
		BeginWaitCursor();
		m_bModified = TRUE;
		// 保存当前切片
		BYTE* pCur = MappingView(m_nCurrentSlice);
		BYTE *temp = new BYTE[nSliceLength];
		memcpy(temp, pCur, nSliceLength);
		// 替换当前切片
		memcpy(pCur, pMapAddress, nSliceLength);
		// 逐帧图像向后移动
		BYTE* temp2 = new BYTE[nSliceLength];
		for (int i = m_nCurrentSlice + 1; i < m_nSlice + 1; ++i)
		{
			pCur = MappingView(i);
			// 拷贝数据
			memcpy(temp2, pCur, nSliceLength);
			memcpy(pCur, temp, nSliceLength);
			memcpy(temp, temp2, nSliceLength);
		}
		delete[] temp2;
		delete[] temp;

		EndWaitCursor();

		UnmapViewOfFile(pMapAddress);
		CloseHandle(hFileMap);
		CloseHandle(hFile);

		m_nSlice++;
		LoadDR(m_nCurrentSlice, TRUE);
		return TRUE;
	}
	return FALSE;
}

// 删除当前切片
BOOL CPage2::DeleteSlice()
{
	if (MessageBox(_T("您将要删除当前切片？"), _T("提示"), MB_YESNO | MB_ICONINFORMATION) != IDYES)
		return FALSE;

	m_bModified = TRUE;
	int nDataHeader = sizeof(CVSDataHeader);
	int nSliceLength = GetSliceBytesNum();
	CreateMapping(nSliceLength);

	// 逐帧图像拷贝到前一帧
	BeginWaitCursor();
	for (int i = m_nCurrentSlice + 1; i < m_nSlice; ++i)
	{
		BYTE* pNext = MappingView(i);
		BYTE* pCur = MappingView(i - 1);
		memcpy(pCur, pNext, nSliceLength);
	}
	EndWaitCursor();

	m_nSlice--;
	if (m_nCurrentSlice == m_nSlice)
		m_nCurrentSlice--;
	LoadDR(m_nCurrentSlice, TRUE);
	m_wndBottomBar.ShowWindow(SW_HIDE);

	return TRUE;
}

void CPage2::ResetMapping()
{
	if (m_hFileMap)
	{
		CloseHandle(m_hFileMap);
		m_hFileMap = NULL;
	}
	if (m_pMapAddress)
	{
		UnmapViewOfFile(m_pMapAddress);
		m_pMapAddress = NULL;
	}
	if (m_hFile)
	{
		SetFilePointer(m_hFile, GetFileBytesNum(), NULL, FILE_BEGIN);
		SetEndOfFile(m_hFile);
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}
}