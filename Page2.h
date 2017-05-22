#pragma once
#include "afxwin.h"
#include "PageParent.h"

// CPage2 对话框

class CPage2 : public CPageParent
{
	DECLARE_DYNAMIC(CPage2)

public:
	CPage2(CWnd* pWnd = NULL);   // 标准构造函数
	virtual ~CPage2();

	// 加载某一帧图像
	virtual BOOL LoadDR(int nCurSlice = 0, BOOL bReLoad = FALSE);

	// 在当前切片后面插入切片
	virtual BOOL AddSlice();

	// 删除当前切片
	virtual BOOL DeleteSlice();

	// 重置mapping
	virtual void ResetMapping();

	// 更新切片层数
	void UpdateDataHeader(BYTE* pCur, int nSlice);

	// 创建映射
	BOOL CreateMapping(int nExtraSize = 0);

	// 映射视图
	BYTE* MappingView(int nCurSlice = 0);

	enum { IDD = IDD_PAGE2_DLG };

private:
	// 底部工具栏
	CVSBottomBar m_wndBottomBar;

	// 是否更新
	BOOL m_bModified;

	// 当前文件块
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