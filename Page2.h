#pragma once
#include "afxwin.h"
#include "PageParent.h"

// CPage2 �Ի���

class CPage2 : public CPageParent
{
	DECLARE_DYNAMIC(CPage2)

public:
	CPage2(CWnd* pWnd = NULL);   // ��׼���캯��
	virtual ~CPage2();

	// ����ĳһ֡ͼ��
	virtual BOOL LoadDR(int nCurSlice = 0, BOOL bReLoad = FALSE);

	// �ڵ�ǰ��Ƭ���������Ƭ
	virtual BOOL AddSlice();

	// ɾ����ǰ��Ƭ
	virtual BOOL DeleteSlice();

	// ����mapping
	virtual void ResetMapping();

	// ������Ƭ����
	void UpdateDataHeader(BYTE* pCur, int nSlice);

	// ����ӳ��
	BOOL CreateMapping(int nExtraSize = 0);

	// ӳ����ͼ
	BYTE* MappingView(int nCurSlice = 0);

	enum { IDD = IDD_PAGE2_DLG };

private:
	// �ײ�������
	CVSBottomBar m_wndBottomBar;

	// �Ƿ����
	BOOL m_bModified;

	// ��ǰ�ļ���
	int m_nCurBlock;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg virtual void OnBnClickedChoosing();
	afx_msg virtual void OnBnClickedApply();
	afx_msg virtual void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
};