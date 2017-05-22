
// DRConverterDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DRConverter.h"
#include "DRConverterDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDRConverterDlg �Ի���



CDRConverterDlg::CDRConverterDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CDRConverterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nCurSelTab = 0;
	m_bFullScreen = FALSE;
}

CDRConverterDlg::~CDRConverterDlg()
{
}

void CDRConverterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDRConverterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDRConverterDlg ��Ϣ�������

BOOL CDRConverterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_Font.CreatePointFont(100, _T("����"));

	CRect rc;
	GetClientRect(&rc);
	m_Table.Create(WS_CHILD | WS_VISIBLE, rc, this, ID_TABLE_WND);
	m_Table.SetFont(&m_Font);

	// ���������Ի���,���� CTabCtrl ������Ϊ�丸����
	m_Page1.Create(IDD_PAGE1_DLG, &m_Table);
	m_Page2.Create(IDD_PAGE2_DLG, &m_Table);

	// �ѶԻ�������� CDialog ������ϵ����
	m_pDialog[0] = &m_Page1;
	m_pDialog[1] = &m_Page2;

	// Ϊ Tab Control ��������ҳ��
	m_Table.InsertItem(0, _T("�ϲ�"));
	m_Table.InsertItem(1, _T("���"));

	MovePageFitTable();

	//��ʾ��ʼҳ��,���浱ǰѡ��
	m_pDialog[0]->ShowWindow(SW_SHOW);
	m_pDialog[1]->ShowWindow(SW_HIDE);
	m_nCurSelTab = 0;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CDRConverterDlg::MovePageFitTable()
{
	//��� Tab Control �ͻ������δ�С
	CRect rc;
	m_Table.GetClientRect(&rc);

	// �������δ�С������ҳ��ʾ���ʵ���λ����
	rc.top += 23;
	rc.bottom -= 3;
	rc.left += 3;
	rc.right -= 3;

	// �ƶ��Ի��򴰿ڵ�����������
	m_Page1.MoveWindow(&rc);
	m_Page2.MoveWindow(&rc);
}

void CDRConverterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDRConverterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CDRConverterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDRConverterDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_Table.GetSafeHwnd())
	{
		m_Table.MoveWindow(0, 0, cx, cy);
		MovePageFitTable();
	}
}

// ����ȫ��
void CDRConverterDlg::EnterFullScreen()
{
	//����Ѿ���ȫ��״̬,�򲻴���
	if (m_bFullScreen)
		return;

	//����ɴ�������λ��
	GetWindowRect(&m_rcOldWnd);

	//ModifyStyle(0, WS_POPUP);
	ModifyStyle(WS_CAPTION, 0);
	ModifyStyle(WS_BORDER, 0);
	ModifyStyle(WS_THICKFRAME, 0);

	//�����ڴ�С��Ϊȫ��,
	MoveWindow(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

	//����ͼ��ռ�������ͻ���
	ShowWindow(SW_SHOWMAXIMIZED);

	m_bFullScreen = true;
}

// �˳�ȫ��
void CDRConverterDlg::QuitFullScreen()
{
	//�������ȫ��״̬,�򲻴���
	if (!m_bFullScreen)
		return;

	m_bFullScreen = false;

	ModifyStyle(0, WS_CAPTION);
	ModifyStyle(0, WS_BORDER);
	ModifyStyle(0, WS_THICKFRAME);

	// �ָ�����λ��
	// �������Ӧ����ModifyStyle���棬�����˳�ȫ��ʱ����������
	MoveWindow(&m_rcOldWnd);

	//���Ӵ�����ͼ��������ʾ
	ShowWindow(SW_SHOW);
	Invalidate(TRUE);
}