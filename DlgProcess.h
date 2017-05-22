#pragma once
#include "afxcmn.h"


// CDlgProcess 对话框

class CDlgProcess : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProcess)

public:
	CDlgProcess(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgProcess();

	// 对话框数据
	enum { IDD = IDD_PROCESS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_ProcessCtrl;
	virtual BOOL OnInitDialog();
};
