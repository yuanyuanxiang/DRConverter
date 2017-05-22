#pragma once
#include "VSBottomBar.h"
#include "afxwin.h"
#include "ImageViewer\DlgImageViewer.h"


// CPageParent �Ի���

class CPageParent : public CDlgImageViewer
{
	DECLARE_DYNAMIC(CPageParent)

public:
	CPageParent(UINT nIDTemplate, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPageParent();

	// ����ĳһ֡ͼ��
	virtual BOOL LoadDR(int nCurSlice = 0, BOOL bReLoad = FALSE) { return 1; }

	// �ڵ�ǰ��Ƭ���������Ƭ
	virtual BOOL AddSlice()  { return 1; }

	// ɾ����ǰ��Ƭ
	virtual BOOL DeleteSlice()  { return 1; }

	// ����mapping
	virtual void ResetMapping(){ }

	// ��ʾ/���صײ�������
	BOOL ShowBottomBar(const CPoint& point);

	// ��ȡ�ײ�������ָ��
	CVSBottomBar* GetBottomBar() { return m_pBottomBar; }

	// ��ȡ����ͷ
	CVSDataHeader* GetDataHeader() { return &m_DataHeader; }

	// ��ȡͼ��ָ��
	CImage* GetImage() { return &m_Image; }

	// ����ͼ��ָ��
	// void SetImage(CImage* pImage) { Invalidate(TRUE); }

	// ��ȡ��Ƭ����
	int GetSlice() { return m_nSlice; }

	// ��ȡ��ǰ��Ƭ
	int GetCurrentSlice() { return m_nCurrentSlice; }

	// ��ȡ�ļ�·��
	CString GetFilePath() { return m_strFilePath; }

	// ��ȡ���ű���
	float GetZoomRate() { return m_fZoomRate; }

	// ��������
	void Reset();

	// ��ȡ�ļ��ֽ���
	DWORD GetFileBytesNum(){ return m_nSlice * (DR_HEADER_SIZE + m_nWidth * m_nHeight * sizeof(float)); }

	// ��ȡÿ֡ͼ���ֽ���
	int GetSliceBytesNum() { return (DR_HEADER_SIZE + m_nWidth * m_nHeight * sizeof(float)); }

public:
	CVSDataHeader		m_DataHeader;		// ����ͷ
	CImage				m_Image;			// ��ǰͼ��
	int					m_nWidth;			// ͼ����
	int					m_nHeight;			// ͼ��߶�
	int					m_nSlice;			// ��Ƭ����
	int					m_nCurrentSlice;	// ��ǰ��Ƭ
	CString				m_strSelected;		// ��ѡ���
	CString				m_strFilePath;		// �ļ�·��
	float				m_fZoomRate;		// ���ű���
	CRect				m_PaintRect;		// ��ͼ����
	CRect				m_ClientRect;		// �ͻ�����
	CVSBottomBar*		m_pBottomBar;		// �ײ�������
	HANDLE				m_hFile;			// �ļ�����
	HANDLE				m_hFileMap;			// �ļ�ӳ��
	BYTE*				m_pMapAddress;		// ӳ���ַ
	__int64				m_i64FileSize;		// �ļ���С
	__int64				m_i64FileOffset;	// �ļ�ƫ����
	DWORD				m_dwBlockBytes;		// ���С
	int					m_kbGranularity;	// ��������(kb)

protected:
	// DDX/DDV ֧��
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	// �Ի���ؼ�
	CEdit m_EditSelected;
	CButton m_BnChoose;
	CButton m_BnApply;
	CStatic m_ChooseTips;
	CToolTipCtrl m_ToolTip;

	afx_msg virtual void OnBnClickedChoosing();
	afx_msg virtual void OnBnClickedApply();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual BOOL ContinueModal();
	virtual void OnPaint();
};