#pragma once
#include "afxwin.h"
#include "PageParent.h"
#include <vector>
using namespace std;

// CPage1 �Ի���

class CPage1 : public CPageParent
{
	DECLARE_DYNAMIC(CPage1)

public:
	CPage1(CWnd* pWnd = NULL);   // ��׼���캯��
	virtual ~CPage1();

	// ����ĳһ֡ͼ��
	virtual BOOL LoadDR(int nCurSlice = 0, BOOL bReLoad = FALSE);

	// �ڵ�ǰ��Ƭ���������Ƭ
	virtual BOOL AddSlice();

	// ɾ����ǰ��Ƭ
	virtual BOOL DeleteSlice();

	// ����mapping
	virtual void ResetMapping();

	enum { IDD = IDD_PAGE1_DLG };

private:
	vector<CString>		m_vStrAllSlices;	// ������Ƭ
	CVSBottomBar		m_wndBottomBar;		// �ײ�������

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg virtual void OnBnClickedChoosing();
	afx_msg virtual void OnBnClickedApply();
	afx_msg virtual void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};