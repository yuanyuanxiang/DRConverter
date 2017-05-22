// Page1.cpp : 实现文件
//

#include "stdafx.h"
#include "DRConverter.h"
#include "Page1.h"
#include "functions.h"
#include "DlgProcess.h"

// CPage1 对话框

IMPLEMENT_DYNAMIC(CPage1, CPageParent)

CPage1::CPage1(CWnd* pWnd) : CPageParent(CPage1::IDD, pWnd)
{
}

CPage1::~CPage1()
{
	ResetMapping();
}


BEGIN_MESSAGE_MAP(CPage1, CPageParent)
	ON_BN_CLICKED(IDC_CHOOSING, &CPage1::OnBnClickedChoosing)
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPage1 消息处理程序


void CPage1::OnBnClickedChoosing()
{
	// 存放选择的目录路径 
	wchar_t szPath[MAX_PATH];

	ZeroMemory(szPath, sizeof(szPath));

	BROWSEINFO bi;
	bi.hwndOwner = m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = _T("请选择目录");
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;

	// 弹出选择目录对话框
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);
	if (lp == NULL)
		return;
	if (SHGetPathFromIDList(lp, szPath) == TRUE)
	{
		m_strSelected.Format(_T("%s"), szPath);
		m_EditSelected.SetWindowText(m_strSelected);
		m_vStrAllSlices = ScanDiskFile(m_strSelected);
		m_nSlice = m_vStrAllSlices.size();
		m_nCurrentSlice = -1;
		if (m_nSlice == 0)
			return;
		// 读取第0张dr图像
		CFile file;
		if (!file.Open(m_vStrAllSlices[0], CFile::modeRead))
			return;
		m_strFilePath = m_vStrAllSlices[0];
		// 获取图像基本信息
		file.Read(&m_DataHeader, sizeof(CVSDataHeader));
		m_nWidth = m_DataHeader.Width;
		m_nHeight = m_DataHeader.Height;
		if (m_nWidth * m_nHeight == 0)
		{
			MessageBox(_T("当前文件是错误的dr图像！"), _T("错误"), MB_OK | MB_ICONERROR);
			file.Close();
			return;
		}
		file.Close();
		LoadDR(0, TRUE);
	}
	CPageParent::OnBnClickedChoosing();
}

void CPage1::OnBnClickedApply()
{
	if (m_nSlice < 2)
		return;

	ResetMapping();

	// 假定一张图像长度能用int表示
	int nSliceLength = GetSliceBytesNum();
	// 合并后长度可能大于4G
	m_i64FileSize = m_nSlice * nSliceLength;

	// 文件对话框
	CFileDialog dlgFile(FALSE, NULL, NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
		_T("DR文件 (*.dr)|*.dr|所有文件 (*.*)|*.*||"), NULL);
	if (dlgFile.DoModal() != IDOK)
		return;

	// 创建文件对象
	CString strMergedFilePath = dlgFile.GetPathName();
	if (GetFileExt(strMergedFilePath).CompareNoCase(_T("dr")) != 0)
		strMergedFilePath += _T(".dr");
	m_hFile = CreateFile(strMergedFilePath, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		TRACE("创建文件对象失败,错误代码:%d\n", GetLastError());
		return;
	}
	// 创建文件映射对象
	m_hFileMap = CreateFileMapping(m_hFile, 0, PAGE_READWRITE,
		(DWORD)(m_i64FileSize >> 32), (DWORD)(m_i64FileSize & 0xFFFFFFFF), _T("Merging"));
	if (m_hFileMap == NULL)
	{
		TRACE("创建文件映射对象失败,错误代码:%d\n", GetLastError());
		CloseHandle(m_hFile);
		m_hFile = NULL;
		DeleteFile(strMergedFilePath);
		return;
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
	m_kbGranularity = dwGran / 1024;
	// 块大小
	m_dwBlockBytes = m_kbGranularity * nSliceLength;
	BeginWaitCursor();
	// 偏移地址
	m_i64FileOffset = 0;
	int j = 0;
	CDlgProcess *pProcessDlg = new CDlgProcess;
	pProcessDlg->Create(IDD_PROCESS_DLG, this);
	pProcessDlg->ShowWindow(SW_SHOW);
	pProcessDlg->m_ProcessCtrl.SetRange(0, m_nSlice - 1);
	CString strTitle;
	while (m_i64FileSize >= m_dwBlockBytes)
	{
		// 映射视图
		m_pMapAddress = (BYTE*)MapViewOfFile(m_hFileMap, FILE_MAP_ALL_ACCESS,
			(DWORD)(m_i64FileOffset >> 32), (DWORD)(m_i64FileOffset & 0xFFFFFFFF),
			m_dwBlockBytes);
		if (m_pMapAddress == NULL)
		{
			TRACE("映射文件映射失败,错误代码:%d\n", GetLastError());
			CloseHandle(m_hFile);
			CloseHandle(m_hFileMap);
			m_hFile = NULL;
			m_hFileMap = NULL;
			DeleteFile(strMergedFilePath);
			EndWaitCursor();
			return;
		}
		// 对映射的视图进行访问
		for (int i = 0; i < m_kbGranularity; i++)
		{
			// 逐个读取
			CFile file;
			if (!file.Open(m_vStrAllSlices[j], CFile::modeRead))
			{
				MessageBox(_T("打开") + m_vStrAllSlices[j] + _T("遇到错误！"),
					_T("错误"), MB_OK | MB_ICONERROR);
				CloseHandle(m_hFile);
				CloseHandle(m_hFileMap);
				m_hFile = NULL;
				m_hFileMap = NULL;
				DeleteFile(strMergedFilePath);
				EndWaitCursor();
				return;
			}
			// 检查当前图像的大小没问题
			CVSDataHeader DataHeader;
			file.Read(&DataHeader, sizeof(CVSDataHeader));
			int nWidth = DataHeader.Width;
			int nHeight = DataHeader.Height;
			// 修改层数
			// DataHeader.Slice = j;
			if (nWidth != m_nWidth || nHeight != m_nHeight || file.GetLength() != nSliceLength)
			{
				MessageBox(_T("图像") + m_vStrAllSlices[j] + _T("尺寸不对！"),
					_T("错误"), MB_OK | MB_ICONERROR);
				file.Close();
				CloseHandle(m_hFile);
				CloseHandle(m_hFileMap);
				m_hFile = NULL;
				m_hFileMap = NULL;
				DeleteFile(strMergedFilePath);
				EndWaitCursor();
				return;
			}
			BYTE* pCur = m_pMapAddress + i * nSliceLength;
			// 文件头
			memcpy(pCur, &DataHeader, sizeof(CVSDataHeader));
			// 数据项
			file.Seek(DR_HEADER_SIZE, CFile::begin);
			file.Read(pCur + DR_HEADER_SIZE, nSliceLength - DR_HEADER_SIZE);
			file.Close();
			pProcessDlg->m_ProcessCtrl.SetPos(j);
			strTitle.Format(_T("进度(%d/%d)"), j + 1, m_nSlice);
			pProcessDlg->SetWindowText(strTitle);
			j++;
		}
		// 撤消文件映像
		UnmapViewOfFile(m_pMapAddress);
		m_pMapAddress = NULL;
		// 修正参数
		m_i64FileOffset += m_dwBlockBytes;
		m_i64FileSize -= m_dwBlockBytes;
	}
	// 写剩余的几帧图像
	if (m_i64FileSize)
	{
		m_dwBlockBytes = m_i64FileSize;
		int nLeft = m_nSlice - j;
		// 映射视图
		m_pMapAddress = (BYTE*)MapViewOfFile(m_hFileMap, FILE_MAP_ALL_ACCESS,
			(DWORD)(m_i64FileOffset >> 32), (DWORD)(m_i64FileOffset & 0xFFFFFFFF),
			m_dwBlockBytes);
		if (m_pMapAddress == NULL)
		{
			TRACE("映射文件映射失败,错误代码:%d\n", GetLastError());
			CloseHandle(m_hFile);
			CloseHandle(m_hFileMap);
			m_hFile = NULL;
			m_hFileMap = NULL;
			DeleteFile(strMergedFilePath);
			EndWaitCursor();
			return;
		}
		// 对映射的视图进行访问
		for (int i = 0; i < nLeft; i++)
		{
			// 逐个读取
			CFile file;
			if (!file.Open(m_vStrAllSlices[j], CFile::modeRead))
			{
				MessageBox(_T("打开") + m_vStrAllSlices[j] + _T("遇到错误！"),
					_T("错误"), MB_OK | MB_ICONERROR);
				CloseHandle(m_hFile);
				CloseHandle(m_hFileMap);
				m_hFile = NULL;
				m_hFileMap = NULL;
				DeleteFile(strMergedFilePath);
				EndWaitCursor();
				return;
			}
			// 检查当前图像的大小没问题
			CVSDataHeader DataHeader;
			file.Read(&DataHeader, sizeof(CVSDataHeader));
			int nWidth = DataHeader.Width;
			int nHeight = DataHeader.Height;
			// 修改层数
			// DataHeader.Slice = j;
			if (nWidth != m_nWidth || nHeight != m_nHeight || file.GetLength() != nSliceLength)
			{
				MessageBox(_T("图像") + m_vStrAllSlices[j] + _T("尺寸不对！"),
					_T("错误"), MB_OK | MB_ICONERROR);
				file.Close();
				CloseHandle(m_hFile);
				CloseHandle(m_hFileMap);
				m_hFile = NULL;
				m_hFileMap = NULL;
				DeleteFile(strMergedFilePath);
				EndWaitCursor();
				return;
			}
			BYTE* pCur = m_pMapAddress + i * nSliceLength;
			// 文件头
			memcpy(pCur, &DataHeader, sizeof(CVSDataHeader));
			// 数据项
			file.Seek(DR_HEADER_SIZE, CFile::begin);
			file.Read(pCur + DR_HEADER_SIZE, nSliceLength - DR_HEADER_SIZE);
			file.Close();
			pProcessDlg->m_ProcessCtrl.SetPos(j);
			strTitle.Format(_T("进度(%d/%d)"), j + 1, m_nSlice);
			pProcessDlg->SetWindowText(strTitle);
			j++;
		}
		// 撤消文件映像
		UnmapViewOfFile(m_pMapAddress);
		m_pMapAddress = NULL;
		// 修正参数
		m_i64FileOffset = 0;
		m_i64FileSize = m_nSlice * nSliceLength;
	}

	EndWaitCursor();

	ResetMapping();

	delete pProcessDlg;

	CPageParent::OnBnClickedApply();
}

BOOL CPage1::LoadDR(int nCurSlice, BOOL bReLoad)
{
	if (bReLoad == FALSE && (m_nCurrentSlice == nCurSlice || m_nSlice == 0))
		return TRUE;

	if (!m_Image.IsNull())
	{
		m_Image.Destroy();
	}

	m_nCurrentSlice = nCurSlice;

	// 读取第m_nCurrentSlice张dr图像
	CFile file;
	if (!file.Open(m_vStrAllSlices[m_nCurrentSlice], CFile::modeRead))
		return FALSE;
	m_strFilePath = m_vStrAllSlices[m_nCurrentSlice];
	file.Read(&m_DataHeader, sizeof(CVSDataHeader));
	if (m_nWidth != m_DataHeader.Width || m_nHeight != m_DataHeader.Height || file.GetLength() < GetSliceBytesNum())
	{
		MessageBox(_T("当前文件的尺寸不正确！"), _T("错误"), MB_OK | MB_ICONERROR);
		file.Close();
		return FALSE;
	}
	float *temp = new float[m_nWidth * m_nHeight];
	file.Seek(DR_HEADER_SIZE, CFile::begin);
	file.Read(temp, m_nWidth * m_nHeight * sizeof(float));
	file.Close();
	// 拷贝浮点数据到CImage用来显示
	Normalized(temp, m_nWidth, m_nHeight);
	m_Image.Create(m_nWidth, m_nHeight, 24, 0UL);
	BYTE *pDst = (BYTE*)m_Image.GetBits() + m_Image.GetPitch() * (m_Image.GetHeight() - 1);
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

// 在当前位置后面插入切片
BOOL CPage1::AddSlice()
{
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("DR文件 (*.dr)|*.dr|所有文件 (*.*)|*.*||"), NULL);

	if (dlgFile.DoModal() == IDOK)
	{
		CString strInsertedFile = dlgFile.GetPathName();
		// 创建另外一个文件内核对象
		HANDLE hFile = CreateFile(strInsertedFile, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			TRACE("创建文件对象失败,错误代码:%d\n", GetLastError());
			return FALSE;
		}
		// 以实际数据长度创建另外一个文件映射内核对象
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
		DWORD dwFileSizeHigh = 0;
		__int64 nFileSize = GetFileSize(hFile, &dwFileSizeHigh);
		nFileSize |= (((__int64)dwFileSizeHigh) << 32);
		// 匹配才可以插入新切片
		if (nWidth == m_nWidth && nHeight == m_nHeight && nFileSize == GetSliceBytesNum())
		{
			CString strCurFile = GetFileName(m_vStrAllSlices[m_nCurrentSlice]/*, FALSE*/);
			int nCur = _ttoi(strCurFile.Right(4));
			CString strInsertedId;
			strInsertedId.Format(_T("%.4d"), nCur + 1);
			strCurFile = strCurFile.Left(strCurFile.GetLength() - 4);
			CString strInsertedFile = m_strSelected + _T("\\") + strCurFile + strInsertedId + _T(".dr");
			CFile file;
			if (!file.Open(strInsertedFile, CFile::modeCreate | CFile::modeWrite))
				return FALSE;
			file.Write(pMapAddress, GetSliceBytesNum());
			file.Close();
			// 由于insert是在元素前面插入
			if (m_nCurrentSlice == m_nSlice - 1)
			{
				m_vStrAllSlices.push_back(strInsertedFile);
			}
			else
			{
				if (strInsertedFile != m_vStrAllSlices[m_nCurrentSlice + 1])
				{
					m_vStrAllSlices.insert(m_vStrAllSlices.begin() + m_nCurrentSlice + 1, strInsertedFile);
				}
				else
				{
					SetTimer(NULL, 3000, NULL);
					MessageBox(_T("插入切片将覆盖现有的dr图像！"), _T("警告"), MB_OK | MB_ICONWARNING);
				}
			}
			m_nSlice++;
			LoadDR(m_nCurrentSlice, TRUE);
			return TRUE;
		}
		//从进程的地址空间撤消文件数据映像
		UnmapViewOfFile(pMapAddress);
		CloseHandle(hFileMap);
		CloseHandle(hFile);
		return FALSE;
	}
	return FALSE;
}

// 删除当前切片
BOOL CPage1::DeleteSlice()
{
	if (MessageBox(_T("您将要删除当前切片？"), _T("提示"), MB_YESNO | MB_ICONINFORMATION) != IDYES)
		return FALSE;
	DeleteFile(m_vStrAllSlices[m_nCurrentSlice]);
	m_vStrAllSlices.erase(m_vStrAllSlices.begin() + m_nCurrentSlice);
	m_nSlice--;
	if (m_nCurrentSlice == m_nSlice)
		m_nCurrentSlice--;
	LoadDR(m_nCurrentSlice, TRUE);
	m_wndBottomBar.ShowWindow(SW_HIDE);
	return TRUE;
}

int CPage1::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

void CPage1::DoDataExchange(CDataExchange* pDX)
{
	// TODO:  在此添加专用代码和/或调用基类

	CPageParent::DoDataExchange(pDX);
}

BOOL CPage1::OnInitDialog()
{
	CPageParent::OnInitDialog();

	m_ChooseTips.SetWindowText(_T("选择目录:"));
	m_BnApply.SetWindowText(_T("合并"));

	return TRUE;
}

void CPage1::OnTimer(UINT_PTR nIDEvent)
{
	// 查找指定标题的窗口，返回窗口句柄
	HWND hWnd = ::FindWindow(NULL, _T("警告"));
	if (hWnd)
	{
		::SendMessage(hWnd, WM_SYSCOMMAND, SC_CLOSE, NULL);
		//keybd_event(13, 0, 0, 0); //模拟回车键按下
	}
	KillTimer(nIDEvent);
	CPageParent::OnTimer(nIDEvent);
}

void CPage1::OnClose()
{
	m_vStrAllSlices.clear();
	Reset();
	// 更新底部工具栏
	m_wndBottomBar.Init(m_nCurrentSlice, m_nSlice);
	SetImage(NULL);
	Invalidate(TRUE);
}

void CPage1::ResetMapping()
{
	if (m_hFile)
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}
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
}