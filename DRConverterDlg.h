
// DRConverterDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "Page1.h"
#include "Page2.h"
#include "DRTable.h"


// CDRConverterDlg �Ի���
class CDRConverterDlg : public CDialogEx
{
	// ����
public:
	CDRConverterDlg(CWnd* pParent = NULL);	// ��׼���캯��

	~CDRConverterDlg();

	// ��ȡ��ǰѡ�������ҳid
	int GetCurSelPageId() { return m_nCurSelTab; }

	// ��ȡ��ǰѡ�������ҳָ��
	CPageParent* GetCurSelPage() { return m_pDialog[m_nCurSelTab]; }

	// ���õ�ǰѡ�������ҳid
	void SetCurSelPage(int nCurSelPage) { m_nCurSelTab = nCurSelPage; }

	// ��ȡ�Ի��������ͷָ��
	CPageParent** GetDialogArray() { return m_pDialog; }

	// �ƶ�����ҳʹ���ʺ�table
	void MovePageFitTable();

	// ȫ��
	void EnterFullScreen();

	// �˳�ȫ��
	void QuitFullScreen();

	// �Ի�������
	enum { IDD = IDD_DRCONVERTER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	// ȫ����http://blog.csdn.net/weiwangchao_/article/details/7031338
	// �ɵĴ���λ�ü���С
	RECT m_rcOldWnd;

	// ȫ����ʾ��־
	BOOL m_bFullScreen;

	// �ο�:http://www.cnblogs.com/kzloser/archive/2012/11/22/2782459.html
	//Tab Control �ؼ���������
	CDRTable m_Table;

	// ����
	CFont m_Font;

	//��¼��ǰ��ʾҳ�������
	int m_nCurSelTab;

	//����ҳ�Ի������
	CPage1 m_Page1;

	CPage2 m_Page2;

	//�Ի�������
	CPageParent* m_pDialog[2];

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};