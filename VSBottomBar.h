#pragma once
#include "VSBottomBarWnd.h"

// CVSBottomBar

class CVSBottomBar : public CToolBar
{
	DECLARE_DYNAMIC(CVSBottomBar)

public:
	// 构造 / 析构
	CVSBottomBar();
	virtual ~CVSBottomBar();

	// 初始化底部工具栏
	void Init(int nCurrentSlice, int nSlice);

	// 重载显示窗口函数
	virtual BOOL ShowWindow(int nShow);

	// 获取吸附在底部工具栏上面的窗口
	CVSBottomBarWnd* GetBottomBarWnd() { return m_pBottomBarWnd; }

	// 设置图像信息
	void SetImageInfomation();

	// 设置当前显示的切片层数并刷新视图
	void SetCurrentSlice(int nSlice);

	// 在当前切片后面插入
	virtual void ClickAddSliceButton();

	// 删除当前切片
	virtual void ClickDeleteSliceButton();

	// 点击全屏按钮响应函数
	virtual void ClickFullScreenButton();

	// 点击“原大”按钮时响应函数
	virtual void ClickOriginSizeButton();

private:
	// 延时显示的时间
	int m_nDelayTime;

	// 吸附在底部工具栏上的窗口
	CVSBottomBarWnd* m_pBottomBarWnd;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};