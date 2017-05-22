#pragma once

#include "VSImageInfoDlg.h"
#include "VSSliceSliderCtrl.h"

// CVSBottomBarWnd

class CVSCurrentSliceEdit;

class CVSBottomBarWnd : public CWnd
{
	DECLARE_DYNAMIC(CVSBottomBarWnd)

public:
	CVSBottomBarWnd();
	virtual ~CVSBottomBarWnd();

public:
	// �ж��Ƿ�Ϊ2Dͼ��
	BOOL Is2DImage();

	// ��ʼ���ײ�����������Ŀؼ�״̬
	void Init(int nCurrentSlice, int nSlice);

	// ���Ի����ƶ�����ͼ���½�
	void MoveImagePropPageDlg();

	// ��ȡͼ������ҳ�Ի���ָ��
	CVSImageInfoDlg* GetImagePropPageDlg()	{ return m_pImageInfoDlg; }

	// ��ȡ��ͼ��ͼ����
	void GetViewScreenRect(CRect &rect);

	// �ؼ���Ӧ����
	BOOL ChangeCurrentSliceEdit();

	// �����һ�㰴ť��Ӧ����
	void ClickPreviewSliceButton();

	// �����һ�㰴ť��Ӧ����
	void ClickNextSliceButton();

	// �����ť��Ӧ����
	void ClickSliceInfoButton();

	// �ڵ�ǰ��Ƭ�������
	void ClickAddSliceButton();

	// ɾ����ǰ��Ƭ
	void ClickDeleteSliceButton();

	// ���ȫ����ť��Ӧ����
	void ClickFullScreenButton();

	// �����ԭ�󡱰�ťʱ��Ӧ����
	void ClickOriginSizeButton();

	// �ж��û�����������Ƭ����
	BOOL IsInputingCurrentSlice()			{ return m_bInputingCurrentSlice; }

	// �ж�����ҳ����ʾ״̬
	BOOL IsImageInfoDlgVisible()			{ return m_pImageInfoDlg->IsWindowVisible(); }

	// ���õ�ǰ��ʾ����Ƭ������ˢ����ͼ
	void SetCurrentSlice(int nSlice);

	// ���õ�ǰ��Ƭ�༭������ʾ����Ƭ����
	void SetCurrentSliceEditText(int nSlice);

private:
	int					m_nCurrentSlice;			// ��ǰ��Ƭ
	int					m_nSlice;					// �ܵ���Ƭ
	BOOL				m_bInputingCurrentSlice;	// �û�����������Ƭ����
	CVSCurrentSliceEdit*m_pCurrentSlice;			// ��ǰ��Ƭ
	CStatic*			m_pTotalSlice;				// ��Ƭ����
	CStatic*			m_pPreviewSlice;			// ��һ����Ƭ
	CVSSliceSliderCtrl*	m_pSliceSlider;				// ��Ƭ������
	CStatic*			m_pNextSlice;				// ��һ����Ƭ
	CStatic*			m_pAddSlice;				// ������Ƭ
	CStatic*			m_pDeleteSlice;				// ɾ����Ƭ
	CStatic*			m_pSliceInfo;				// ��Ƭ��Ϣ
	CStatic*			m_pFullScreen;				// ȫ����ʾ
	CStatic*			m_pOriginSize;				// ��ԭ����ʾ
	CVSImageInfoDlg*	m_pImageInfoDlg;			// ͼ������ҳ
	CFont*				m_pFont;					// �ؼ�����
	CToolTipCtrl		m_ToolTip;					// ������ʾ��

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMoveSliceSlider(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};