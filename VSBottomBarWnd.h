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
	// 判断是否为2D图像
	BOOL Is2DImage();

	// 初始化底部工具栏上面的控件状态
	void Init(int nCurrentSlice, int nSlice);

	// 将对话框移动到视图右下角
	void MoveImagePropPageDlg();

	// 获取图像属性页对话框指针
	CVSImageInfoDlg* GetImagePropPageDlg()	{ return m_pImageInfoDlg; }

	// 获取视图绘图矩形
	void GetViewScreenRect(CRect &rect);

	// 控件响应函数
	BOOL ChangeCurrentSliceEdit();

	// 点击上一层按钮响应函数
	void ClickPreviewSliceButton();

	// 点击下一层按钮响应函数
	void ClickNextSliceButton();

	// 点击按钮响应函数
	void ClickSliceInfoButton();

	// 在当前切片后面插入
	void ClickAddSliceButton();

	// 删除当前切片
	void ClickDeleteSliceButton();

	// 点击全屏按钮响应函数
	void ClickFullScreenButton();

	// 点击“原大”按钮时响应函数
	void ClickOriginSizeButton();

	// 判断用户正在输入切片层数
	BOOL IsInputingCurrentSlice()			{ return m_bInputingCurrentSlice; }

	// 判断属性页的显示状态
	BOOL IsImageInfoDlgVisible()			{ return m_pImageInfoDlg->IsWindowVisible(); }

	// 设置当前显示的切片层数并刷新视图
	void SetCurrentSlice(int nSlice);

	// 设置当前切片编辑框所显示的切片层数
	void SetCurrentSliceEditText(int nSlice);

private:
	int					m_nCurrentSlice;			// 当前切片
	int					m_nSlice;					// 总的切片
	BOOL				m_bInputingCurrentSlice;	// 用户正在输入切片层数
	CVSCurrentSliceEdit*m_pCurrentSlice;			// 当前切片
	CStatic*			m_pTotalSlice;				// 切片总数
	CStatic*			m_pPreviewSlice;			// 上一层切片
	CVSSliceSliderCtrl*	m_pSliceSlider;				// 切片滑动条
	CStatic*			m_pNextSlice;				// 下一层切片
	CStatic*			m_pAddSlice;				// 增加切片
	CStatic*			m_pDeleteSlice;				// 删除切片
	CStatic*			m_pSliceInfo;				// 切片信息
	CStatic*			m_pFullScreen;				// 全屏显示
	CStatic*			m_pOriginSize;				// “原大”显示
	CVSImageInfoDlg*	m_pImageInfoDlg;			// 图像属性页
	CFont*				m_pFont;					// 控件字体
	CToolTipCtrl		m_ToolTip;					// 工具提示条

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMoveSliceSlider(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};