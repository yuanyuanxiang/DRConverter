// DlgProcess.cpp : 实现文件
//

#include "stdafx.h"
#include "DRConverter.h"
#include "DlgProcess.h"
#include "afxdialogex.h"


// CDlgProcess 对话框

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


// CDlgProcess 消息处理程序


BOOL CDlgProcess::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
