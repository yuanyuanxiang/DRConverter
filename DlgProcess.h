#pragma once
#include "afxcmn.h"


// CDlgProcess �Ի���

class CDlgProcess : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProcess)

public:
	CDlgProcess(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgProcess();

	// �Ի�������
	enum { IDD = IDD_PROCESS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_ProcessCtrl;
	virtual BOOL OnInitDialog();
};
