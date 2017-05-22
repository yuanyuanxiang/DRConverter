#pragma once
#include "afxwin.h"
#include "PageParent.h"
#include <vector>
using namespace std;

// CPage1 对话框

class CPage1 : public CPageParent
{
	DECLARE_DYNAMIC(CPage1)

public:
	CPage1(CWnd* pWnd = NULL);   // 标准构造函数
	virtual ~CPage1();

	// 加载某一帧图像
	virtual BOOL LoadDR(int nCurSlice = 0, BOOL bReLoad = FALSE);

	// 在当前切片后面插入切片
	virtual BOOL AddSlice();

	// 删除当前切片
	virtual BOOL DeleteSlice();

	// 重置mapping
	virtual void ResetMapping();

	enum { IDD = IDD_PAGE1_DLG };

private:
	vector<CString>		m_vStrAllSlices;	// 所有切片
	CVSBottomBar		m_wndBottomBar;		// 底部工具栏

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