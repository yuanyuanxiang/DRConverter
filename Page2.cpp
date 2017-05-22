// Page2.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DRConverter.h"
#include "Page2.h"
#include "functions.h"
#include "DlgProcess.h"

// CPage2 �Ի���

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


// CPage2 ��Ϣ�������


// �������䣬����Ҫ������Ƭʱ�봫�������ڴ�ռ�
BOOL CPage2::CreateMapping(int nExtraSize)
{
	// �����ļ�ӳ��
	ResetMapping();

	// �����ļ�����
	m_hFile = CreateFile(m_strFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		TRACE("�����ļ�����ʧ��,�������:%d\n", GetLastError());
		return FALSE;
	}
	// �õ�ϵͳ��������
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	DWORD dwGran = SysInfo.dwAllocationGranularity;
	// ͨ�������dwGran = 2^16����kbGran = 64
	if (dwGran % 1024)
	{
		MessageBox(_T("��֧�ֵ�ǰ����ϵͳ�ķ������ȣ�"), _T("��ʾ"), MB_OK | MB_ICONINFORMATION);
	}
	// �õ��ļ��ߴ�
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
		TRACE("�����ļ�ӳ�����ʧ��,�������:%d\n", GetLastError());
		return FALSE;
	}
	m_nCurBlock = -1;
	return TRUE;
}

// ӳ����ͼ��ȡ��ĳ֡ͼ��ĵ�ַ
BYTE* CPage2::MappingView(int nCurSlice)
{
	// ���С
	m_dwBlockBytes = m_kbGranularity * GetSliceBytesNum();
	// λ����
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
	// �����ļ�ӳ��
	UnmapViewOfFile(m_pMapAddress);
	m_pMapAddress = NULL;
	// ����ӳ����ͼ
	m_pMapAddress = (BYTE*)MapViewOfFile(m_hFileMap, FILE_MAP_ALL_ACCESS,
		(DWORD)(m_i64FileOffset >> 32), (DWORD)(m_i64FileOffset & 0xFFFFFFFF),
		m_dwBlockBytes);
	if (m_pMapAddress == NULL)
	{
		TRACE("ӳ���ļ�ӳ��ʧ��,�������:%d\n", GetLastError());
		return NULL;
	}
	return (BYTE*)m_pMapAddress + nNumber * GetSliceBytesNum();
}

void CPage2::OnBnClickedChoosing()
{
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("DR�ļ� (*.dr)|*.dr|�����ļ� (*.*)|*.*||"), NULL);

	if (dlgFile.DoModal() == IDOK)
	{
		m_strSelected = dlgFile.GetPathName();
		m_EditSelected.SetWindowText(m_strSelected);
		m_strFilePath = m_strSelected;

		CFile file;
		if (!file.Open(m_strFilePath, CFile::modeRead))
		{
			MessageBox(_T("��") + m_strFilePath + _T("��������"),
				_T("����"), MB_OK | MB_ICONERROR);
			return;
		}
		int nDataHeader = sizeof(CVSDataHeader);
		if (file.GetLength() < nDataHeader)
		{
			MessageBox(_T("�ļ�") + m_strFilePath + _T("�ߴ����"),
				_T("����"), MB_OK | MB_ICONERROR);
			file.Close();
			return;
		}
		// ��ȡͼ��Ļ�����Ϣ
		file.Read(&m_DataHeader, nDataHeader);
		m_nWidth = m_DataHeader.Width;
		m_nHeight = m_DataHeader.Height;
		file.Close();

		if (CreateMapping(0) == FALSE)
			return;

		if (m_i64FileSize % GetSliceBytesNum())
		{
			MessageBox(_T("�ļ�") + m_strFilePath + _T("�ߴ粻�ԣ�"),
				_T("����"), MB_OK | MB_ICONERROR);
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
	// ���ѡ���Ŀ¼·�� 
	wchar_t szPath[MAX_PATH];

	ZeroMemory(szPath, sizeof(szPath));

	BROWSEINFO bi;
	bi.hwndOwner = m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = _T("��ѡ��Ŀ¼");
	bi.ulFlags = 0x0040 | BIF_EDITBOX;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;

	// ����ѡ��Ŀ¼�Ի���
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
		// ����֡ͼ��д�뵽ָ���ļ�
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
				MessageBox(_T("��") + strCurFile + _T("��������"),
					_T("����"), MB_OK | MB_ICONERROR);
				EndWaitCursor();
				return;
			}
			BYTE *pCur = MappingView(i);
			file.Write(pCur, nSliceLength);
			file.Close();
			pProcessDlg->m_ProcessCtrl.SetPos(i);
			strTitle.Format(_T("����(%d/%d)"), i + 1, m_nSlice);
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
	// ��ȡ��nCurSlice����Ƭ
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
	// ���µײ�������
	m_wndBottomBar.Init(m_nCurrentSlice, m_nSlice);
	SetImage(&m_Image);

	return TRUE;
}

int CPage2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPageParent::OnCreate(lpCreateStruct) == -1)
		return -1;

	// ��ӵײ�������
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
	// TODO:  �ڴ����ר�ô����/����û���

	CPageParent::DoDataExchange(pDX);
}

BOOL CPage2::OnInitDialog()
{
	CPageParent::OnInitDialog();

	m_ChooseTips.SetWindowText(_T("ѡ���ļ�:"));
	m_BnApply.SetWindowText(_T("���"));

	return TRUE;
}

void CPage2::OnClose()
{
	Reset();
	// ���µײ�������
	m_wndBottomBar.Init(m_nCurrentSlice, m_nSlice);
	SetImage(NULL);
	Invalidate(TRUE);
}

// ������Ƭ����
void CPage2::UpdateDataHeader(BYTE* pCur, int nSlice)
{
	CVSDataHeader DataHeader;
	memcpy(&DataHeader, pCur, sizeof(CVSDataHeader));
	DataHeader.Slice = nSlice;
	memcpy(pCur, &DataHeader, sizeof(CVSDataHeader));
}

// �ڵ�ǰλ�ú��������Ƭ
BOOL CPage2::AddSlice()
{
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY,
		_T("DR�ļ� (*.dr)|*.dr|�����ļ� (*.*)|*.*||"), NULL);

	if (dlgFile.DoModal() == IDOK)
	{
		CString strInsertedFile = dlgFile.GetPathName();
		// ��������һ���ļ��ں˶���
		HANDLE hFile = CreateFile(strInsertedFile, GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			TRACE("�����ļ�����ʧ��,�������:%d\n", GetLastError());
			return FALSE;
		}
		// ��������һ���ļ�ӳ���ں˶���
		HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, GetSliceBytesNum(), _T("NewSlice"));
		if (hFileMap == NULL)
		{
			TRACE("�����ļ�ӳ�����ʧ��,�������:%d\n", GetLastError());
			CloseHandle(hFile);
			return FALSE;
		}
		// ���ļ�����ӳ�䵽���̵ĵ�ַ�ռ�
		BYTE* pMapAddress = (BYTE*)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, GetSliceBytesNum());
		if (pMapAddress == NULL)
		{
			TRACE("ӳ���ļ�ӳ��ʧ��,�������:%d\n", GetLastError());
			CloseHandle(hFileMap);
			CloseHandle(hFile);
			return FALSE;
		}
		// �õ��ļ�ͷ
		CVSDataHeader DataHeader;
		int nDataHeader = sizeof(CVSDataHeader);
		memcpy(&DataHeader, pMapAddress, nDataHeader);
		int nWidth = DataHeader.Width;
		int nHeight = DataHeader.Height;
		int nSliceLength = GetSliceBytesNum();
		// ƥ��ſ��Բ�������Ƭ
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
		// ���浱ǰ��Ƭ
		BYTE* pCur = MappingView(m_nCurrentSlice);
		BYTE *temp = new BYTE[nSliceLength];
		memcpy(temp, pCur, nSliceLength);
		// �滻��ǰ��Ƭ
		memcpy(pCur, pMapAddress, nSliceLength);
		// ��֡ͼ������ƶ�
		BYTE* temp2 = new BYTE[nSliceLength];
		for (int i = m_nCurrentSlice + 1; i < m_nSlice + 1; ++i)
		{
			pCur = MappingView(i);
			// ��������
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

// ɾ����ǰ��Ƭ
BOOL CPage2::DeleteSlice()
{
	if (MessageBox(_T("����Ҫɾ����ǰ��Ƭ��"), _T("��ʾ"), MB_YESNO | MB_ICONINFORMATION) != IDYES)
		return FALSE;

	m_bModified = TRUE;
	int nDataHeader = sizeof(CVSDataHeader);
	int nSliceLength = GetSliceBytesNum();
	CreateMapping(nSliceLength);

	// ��֡ͼ�񿽱���ǰһ֡
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