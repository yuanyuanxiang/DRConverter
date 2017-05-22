#pragma once
#include "Resource.h"
#include "VSDataHeader.h"

// CVSImageInfoDlg �Ի���

class CVSImageInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CVSImageInfoDlg)

public:
	CVSImageInfoDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CVSImageInfoDlg();

public:
	// �ж�ͼ���Ƿ�Ϊ2Dͼ��
	BOOL Is2DImage();

	// ��������ͷ��ͼ��ָ�롢�ļ�·�������ű�������������Ϣ
	void SetImageInfomation(CVSDataHeader *DataHeader, CImage *pImage, CString strFilePath, float fZoomRate, int nSlices, int nCurSlices);

	// ��ȡͼ����Ϣ������ʾ���ؼ�����
	void GetImageInfomation();

	// �������öԻ���λ�ã�ʹ�Ի�������½���rect�����½�
	void MoveWindowEx(const CRect& rect, BOOL bRepaint = TRUE);

	// ��ʾ����
	BOOL ShowWindow(int nShow);

	// ���ƶ����ڣ�����ʾ����
	void ShowWindowEx(const CRect& rect, int nShow);

private:
	// �Ƿ�2Dͼ��
	BOOL m_bIs2DImage;

	// ����ͷ
	CVSDataHeader *m_DataHeader;

	// ͼ��ָ��
	CImage *m_pImage;

	// �ļ�path
	CString m_strFilePath;
	
	// ���ű���
	float m_fZoomRate;
	
	// ��Ƭ����
	int m_nSlices;

	// ��ǰ����
	int m_nCurSlices;

	// �Ի�����ʱ��ʾʱ��
	int m_nDelayTime;

	// �Ի�����
	int m_nDlgWidth;
	
	// �Ի���߶�
	int m_nDlgHeight;

	// δ���ò���
	BOOL m_bNotCaptured;

	// ������ʾ��
	CToolTipCtrl m_ToolTip;

public:
// �Ի�������
	enum { IDD = IDD_IMAGE_INFO_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

BOOL IsPointInRect(CPoint pt, CRect rt, int dl = 2);

BOOL IsMouseLeaving(CPoint pt, CRect rt, int dl = 2);