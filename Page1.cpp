// Page1.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DRConverter.h"
#include "Page1.h"
#include "functions.h"
#include "DlgProcess.h"

// CPage1 �Ի���

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


// CPage1 ��Ϣ�������


void CPage1::OnBnClickedChoosing()
{
	// ���ѡ���Ŀ¼·�� 
	wchar_t szPath[MAX_PATH];

	ZeroMemory(szPath, sizeof(szPath));

	BROWSEINFO bi;
	bi.hwndOwner = m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = _T("��ѡ��Ŀ¼");
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;

	// ����ѡ��Ŀ¼�Ի���
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
		// ��ȡ��0��drͼ��
		CFile file;
		if (!file.Open(m_vStrAllSlices[0], CFile::modeRead))
			return;
		m_strFilePath = m_vStrAllSlices[0];
		// ��ȡͼ�������Ϣ
		file.Read(&m_DataHeader, sizeof(CVSDataHeader));
		m_nWidth = m_DataHeader.Width;
		m_nHeight = m_DataHeader.Height;
		if (m_nWidth * m_nHeight == 0)
		{
			MessageBox(_T("��ǰ�ļ��Ǵ����drͼ��"), _T("����"), MB_OK | MB_ICONERROR);
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

	// �ٶ�һ��ͼ�񳤶�����int��ʾ
	int nSliceLength = GetSliceBytesNum();
	// �ϲ��󳤶ȿ��ܴ���4G
	m_i64FileSize = m_nSlice * nSliceLength;

	// �ļ��Ի���
	CFileDialog dlgFile(FALSE, NULL, NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
		_T("DR�ļ� (*.dr)|*.dr|�����ļ� (*.*)|*.*||"), NULL);
	if (dlgFile.DoModal() != IDOK)
		return;

	// �����ļ�����
	CString strMergedFilePath = dlgFile.GetPathName();
	if (GetFileExt(strMergedFilePath).CompareNoCase(_T("dr")) != 0)
		strMergedFilePath += _T(".dr");
	m_hFile = CreateFile(strMergedFilePath, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		TRACE("�����ļ�����ʧ��,�������:%d\n", GetLastError());
		return;
	}
	// �����ļ�ӳ�����
	m_hFileMap = CreateFileMapping(m_hFile, 0, PAGE_READWRITE,
		(DWORD)(m_i64FileSize >> 32), (DWORD)(m_i64FileSize & 0xFFFFFFFF), _T("Merging"));
	if (m_hFileMap == NULL)
	{
		TRACE("�����ļ�ӳ�����ʧ��,�������:%d\n", GetLastError());
		CloseHandle(m_hFile);
		m_hFile = NULL;
		DeleteFile(strMergedFilePath);
		return;
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
	m_kbGranularity = dwGran / 1024;
	// ���С
	m_dwBlockBytes = m_kbGranularity * nSliceLength;
	BeginWaitCursor();
	// ƫ�Ƶ�ַ
	m_i64FileOffset = 0;
	int j = 0;
	CDlgProcess *pProcessDlg = new CDlgProcess;
	pProcessDlg->Create(IDD_PROCESS_DLG, this);
	pProcessDlg->ShowWindow(SW_SHOW);
	pProcessDlg->m_ProcessCtrl.SetRange(0, m_nSlice - 1);
	CString strTitle;
	while (m_i64FileSize >= m_dwBlockBytes)
	{
		// ӳ����ͼ
		m_pMapAddress = (BYTE*)MapViewOfFile(m_hFileMap, FILE_MAP_ALL_ACCESS,
			(DWORD)(m_i64FileOffset >> 32), (DWORD)(m_i64FileOffset & 0xFFFFFFFF),
			m_dwBlockBytes);
		if (m_pMapAddress == NULL)
		{
			TRACE("ӳ���ļ�ӳ��ʧ��,�������:%d\n", GetLastError());
			CloseHandle(m_hFile);
			CloseHandle(m_hFileMap);
			m_hFile = NULL;
			m_hFileMap = NULL;
			DeleteFile(strMergedFilePath);
			EndWaitCursor();
			return;
		}
		// ��ӳ�����ͼ���з���
		for (int i = 0; i < m_kbGranularity; i++)
		{
			// �����ȡ
			CFile file;
			if (!file.Open(m_vStrAllSlices[j], CFile::modeRead))
			{
				MessageBox(_T("��") + m_vStrAllSlices[j] + _T("��������"),
					_T("����"), MB_OK | MB_ICONERROR);
				CloseHandle(m_hFile);
				CloseHandle(m_hFileMap);
				m_hFile = NULL;
				m_hFileMap = NULL;
				DeleteFile(strMergedFilePath);
				EndWaitCursor();
				return;
			}
			// ��鵱ǰͼ��Ĵ�Сû����
			CVSDataHeader DataHeader;
			file.Read(&DataHeader, sizeof(CVSDataHeader));
			int nWidth = DataHeader.Width;
			int nHeight = DataHeader.Height;
			// �޸Ĳ���
			// DataHeader.Slice = j;
			if (nWidth != m_nWidth || nHeight != m_nHeight || file.GetLength() != nSliceLength)
			{
				MessageBox(_T("ͼ��") + m_vStrAllSlices[j] + _T("�ߴ粻�ԣ�"),
					_T("����"), MB_OK | MB_ICONERROR);
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
			// �ļ�ͷ
			memcpy(pCur, &DataHeader, sizeof(CVSDataHeader));
			// ������
			file.Seek(DR_HEADER_SIZE, CFile::begin);
			file.Read(pCur + DR_HEADER_SIZE, nSliceLength - DR_HEADER_SIZE);
			file.Close();
			pProcessDlg->m_ProcessCtrl.SetPos(j);
			strTitle.Format(_T("����(%d/%d)"), j + 1, m_nSlice);
			pProcessDlg->SetWindowText(strTitle);
			j++;
		}
		// �����ļ�ӳ��
		UnmapViewOfFile(m_pMapAddress);
		m_pMapAddress = NULL;
		// ��������
		m_i64FileOffset += m_dwBlockBytes;
		m_i64FileSize -= m_dwBlockBytes;
	}
	// дʣ��ļ�֡ͼ��
	if (m_i64FileSize)
	{
		m_dwBlockBytes = m_i64FileSize;
		int nLeft = m_nSlice - j;
		// ӳ����ͼ
		m_pMapAddress = (BYTE*)MapViewOfFile(m_hFileMap, FILE_MAP_ALL_ACCESS,
			(DWORD)(m_i64FileOffset >> 32), (DWORD)(m_i64FileOffset & 0xFFFFFFFF),
			m_dwBlockBytes);
		if (m_pMapAddress == NULL)
		{
			TRACE("ӳ���ļ�ӳ��ʧ��,�������:%d\n", GetLastError());
			CloseHandle(m_hFile);
			CloseHandle(m_hFileMap);
			m_hFile = NULL;
			m_hFileMap = NULL;
			DeleteFile(strMergedFilePath);
			EndWaitCursor();
			return;
		}
		// ��ӳ�����ͼ���з���
		for (int i = 0; i < nLeft; i++)
		{
			// �����ȡ
			CFile file;
			if (!file.Open(m_vStrAllSlices[j], CFile::modeRead))
			{
				MessageBox(_T("��") + m_vStrAllSlices[j] + _T("��������"),
					_T("����"), MB_OK | MB_ICONERROR);
				CloseHandle(m_hFile);
				CloseHandle(m_hFileMap);
				m_hFile = NULL;
				m_hFileMap = NULL;
				DeleteFile(strMergedFilePath);
				EndWaitCursor();
				return;
			}
			// ��鵱ǰͼ��Ĵ�Сû����
			CVSDataHeader DataHeader;
			file.Read(&DataHeader, sizeof(CVSDataHeader));
			int nWidth = DataHeader.Width;
			int nHeight = DataHeader.Height;
			// �޸Ĳ���
			// DataHeader.Slice = j;
			if (nWidth != m_nWidth || nHeight != m_nHeight || file.GetLength() != nSliceLength)
			{
				MessageBox(_T("ͼ��") + m_vStrAllSlices[j] + _T("�ߴ粻�ԣ�"),
					_T("����"), MB_OK | MB_ICONERROR);
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
			// �ļ�ͷ
			memcpy(pCur, &DataHeader, sizeof(CVSDataHeader));
			// ������
			file.Seek(DR_HEADER_SIZE, CFile::begin);
			file.Read(pCur + DR_HEADER_SIZE, nSliceLength - DR_HEADER_SIZE);
			file.Close();
			pProcessDlg->m_ProcessCtrl.SetPos(j);
			strTitle.Format(_T("����(%d/%d)"), j + 1, m_nSlice);
			pProcessDlg->SetWindowText(strTitle);
			j++;
		}
		// �����ļ�ӳ��
		UnmapViewOfFile(m_pMapAddress);
		m_pMapAddress = NULL;
		// ��������
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

	// ��ȡ��m_nCurrentSlice��drͼ��
	CFile file;
	if (!file.Open(m_vStrAllSlices[m_nCurrentSlice], CFile::modeRead))
		return FALSE;
	m_strFilePath = m_vStrAllSlices[m_nCurrentSlice];
	file.Read(&m_DataHeader, sizeof(CVSDataHeader));
	if (m_nWidth != m_DataHeader.Width || m_nHeight != m_DataHeader.Height || file.GetLength() < GetSliceBytesNum())
	{
		MessageBox(_T("��ǰ�ļ��ĳߴ粻��ȷ��"), _T("����"), MB_OK | MB_ICONERROR);
		file.Close();
		return FALSE;
	}
	float *temp = new float[m_nWidth * m_nHeight];
	file.Seek(DR_HEADER_SIZE, CFile::begin);
	file.Read(temp, m_nWidth * m_nHeight * sizeof(float));
	file.Close();
	// �����������ݵ�CImage������ʾ
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
	// ���µײ�������
	m_wndBottomBar.Init(m_nCurrentSlice, m_nSlice);
	SetImage(&m_Image);

	return TRUE;
}

// �ڵ�ǰλ�ú��������Ƭ
BOOL CPage1::AddSlice()
{
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("DR�ļ� (*.dr)|*.dr|�����ļ� (*.*)|*.*||"), NULL);

	if (dlgFile.DoModal() == IDOK)
	{
		CString strInsertedFile = dlgFile.GetPathName();
		// ��������һ���ļ��ں˶���
		HANDLE hFile = CreateFile(strInsertedFile, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			TRACE("�����ļ�����ʧ��,�������:%d\n", GetLastError());
			return FALSE;
		}
		// ��ʵ�����ݳ��ȴ�������һ���ļ�ӳ���ں˶���
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
		DWORD dwFileSizeHigh = 0;
		__int64 nFileSize = GetFileSize(hFile, &dwFileSizeHigh);
		nFileSize |= (((__int64)dwFileSizeHigh) << 32);
		// ƥ��ſ��Բ�������Ƭ
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
			// ����insert����Ԫ��ǰ�����
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
					MessageBox(_T("������Ƭ���������е�drͼ��"), _T("����"), MB_OK | MB_ICONWARNING);
				}
			}
			m_nSlice++;
			LoadDR(m_nCurrentSlice, TRUE);
			return TRUE;
		}
		//�ӽ��̵ĵ�ַ�ռ䳷���ļ�����ӳ��
		UnmapViewOfFile(pMapAddress);
		CloseHandle(hFileMap);
		CloseHandle(hFile);
		return FALSE;
	}
	return FALSE;
}

// ɾ����ǰ��Ƭ
BOOL CPage1::DeleteSlice()
{
	if (MessageBox(_T("����Ҫɾ����ǰ��Ƭ��"), _T("��ʾ"), MB_YESNO | MB_ICONINFORMATION) != IDYES)
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

	// ��ӵײ�������
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
	// TODO:  �ڴ����ר�ô����/����û���

	CPageParent::DoDataExchange(pDX);
}

BOOL CPage1::OnInitDialog()
{
	CPageParent::OnInitDialog();

	m_ChooseTips.SetWindowText(_T("ѡ��Ŀ¼:"));
	m_BnApply.SetWindowText(_T("�ϲ�"));

	return TRUE;
}

void CPage1::OnTimer(UINT_PTR nIDEvent)
{
	// ����ָ������Ĵ��ڣ����ش��ھ��
	HWND hWnd = ::FindWindow(NULL, _T("����"));
	if (hWnd)
	{
		::SendMessage(hWnd, WM_SYSCOMMAND, SC_CLOSE, NULL);
		//keybd_event(13, 0, 0, 0); //ģ��س�������
	}
	KillTimer(nIDEvent);
	CPageParent::OnTimer(nIDEvent);
}

void CPage1::OnClose()
{
	m_vStrAllSlices.clear();
	Reset();
	// ���µײ�������
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