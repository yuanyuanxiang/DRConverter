
// DRConverterDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "Page1.h"
#include "Page2.h"
#include "DRTable.h"


// CDRConverterDlg 对话框
class CDRConverterDlg : public CDialogEx
{
	// 构造
public:
	CDRConverterDlg(CWnd* pParent = NULL);	// 标准构造函数

	~CDRConverterDlg();

	// 获取当前选择的属性页id
	int GetCurSelPageId() { return m_nCurSelTab; }

	// 获取当前选择的属性页指针
	CPageParent* GetCurSelPage() { return m_pDialog[m_nCurSelTab]; }

	// 设置当前选择的属性页id
	void SetCurSelPage(int nCurSelPage) { m_nCurSelTab = nCurSelPage; }

	// 获取对话框数组的头指针
	CPageParent** GetDialogArray() { return m_pDialog; }

	// 移动属性页使得适合table
	void MovePageFitTable();

	// 全屏
	void EnterFullScreen();

	// 退出全屏
	void QuitFullScreen();

	// 对话框数据
	enum { IDD = IDD_DRCONVERTER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	// 全屏：http://blog.csdn.net/weiwangchao_/article/details/7031338
	// 旧的窗口位置及大小
	RECT m_rcOldWnd;

	// 全屏显示标志
	BOOL m_bFullScreen;

	// 参考:http://www.cnblogs.com/kzloser/archive/2012/11/22/2782459.html
	//Tab Control 控件关联变量
	CDRTable m_Table;

	// 字体
	CFont m_Font;

	//记录当前显示页面的索引
	int m_nCurSelTab;

	//属性页对话框对象
	CPage1 m_Page1;

	CPage2 m_Page2;

	//对话框数组
	CPageParent* m_pDialog[2];

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};