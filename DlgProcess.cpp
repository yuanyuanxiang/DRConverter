// DlgProcess.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DRConverter.h"
#include "DlgProcess.h"
#include "afxdialogex.h"


// CDlgProcess �Ի���

IMPLEMENT_DYNAMIC(CDlgProcess, CDialogEx)

CDlgProcess::CDlgProcess(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgProcess::IDD, pParent)
{

}

CDlgProcess::~CDlgProcess()
{
}

void CDlgProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_ProcessCtrl);
}


BEGIN_MESSAGE_MAP(CDlgProcess, CDialogEx)
END_MESSAGE_MAP()


// CDlgProcess ��Ϣ�������


BOOL CDlgProcess::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}
