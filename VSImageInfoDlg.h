#pragma once
#include "Resource.h"
#include "VSDataHeader.h"

// CVSImageInfoDlg 对话框

class CVSImageInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CVSImageInfoDlg)

public:
	CVSImageInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CVSImageInfoDlg();

public:
	// 判断图像是否为2D图像
	BOOL Is2DImage();

	// 设置数据头、图像指针、文件路径、缩放比例、层数等信息
	void SetImageInfomation(CVSDataHeader *DataHeader, CImage *pImage, CString strFilePath, float fZoomRate, int nSlices, int nCurSlices);

	// 获取图像信息，并显示到控件上面
	void GetImageInfomation();

	// 重新设置对话框位置，使对话框的右下角是rect的右下角
	void MoveWindowEx(const CRect& rect, BOOL bRepaint = TRUE);

	// 显示窗口
	BOOL ShowWindow(int nShow);

	// 先移动窗口，再显示窗口
	void ShowWindowEx(const CRect& rect, int nShow);

private:
	// 是否2D图像
	BOOL m_bIs2DImage;

	// 数据头
	CVSDataHeader *m_DataHeader;

	// 图像指针
	CImage *m_pImage;

	// 文件path
	CString m_strFilePath;
	
	// 缩放比例
	float m_fZoomRate;
	
	// 切片层数
	int m_nSlices;

	// 当前层数
	int m_nCurSlices;

	// 对话框延时显示时间
	int m_nDelayTime;

	// 对话框宽度
	int m_nDlgWidth;
	
	// 对话框高度
	int m_nDlgHeight;

	// 未设置捕获
	BOOL m_bNotCaptured;

	// 工具提示条
	CToolTipCtrl m_ToolTip;

public:
// 对话框数据
	enum { IDD = IDD_IMAGE_INFO_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

BOOL IsPointInRect(CPoint pt, CRect rt, int dl = 2);

BOOL IsMouseLeaving(CPoint pt, CRect rt, int dl = 2);