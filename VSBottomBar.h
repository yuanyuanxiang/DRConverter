#pragma once
#include "VSBottomBarWnd.h"

// CVSBottomBar

class CVSBottomBar : public CToolBar
{
	DECLARE_DYNAMIC(CVSBottomBar)

public:
	// ���� / ����
	CVSBottomBar();
	virtual ~CVSBottomBar();

	// ��ʼ���ײ�������
	void Init(int nCurrentSlice, int nSlice);

	// ������ʾ���ں���
	virtual BOOL ShowWindow(int nShow);

	// ��ȡ�����ڵײ�����������Ĵ���
	CVSBottomBarWnd* GetBottomBarWnd() { return m_pBottomBarWnd; }

	// ����ͼ����Ϣ
	void SetImageInfomation();

	// ���õ�ǰ��ʾ����Ƭ������ˢ����ͼ
	void SetCurrentSlice(int nSlice);

	// �ڵ�ǰ��Ƭ�������
	virtual void ClickAddSliceButton();

	// ɾ����ǰ��Ƭ
	virtual void ClickDeleteSliceButton();

	// ���ȫ����ť��Ӧ����
	virtual void ClickFullScreenButton();

	// �����ԭ�󡱰�ťʱ��Ӧ����
	virtual void ClickOriginSizeButton();

private:
	// ��ʱ��ʾ��ʱ��
	int m_nDelayTime;

	// �����ڵײ��������ϵĴ���
	CVSBottomBarWnd* m_pBottomBarWnd;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};