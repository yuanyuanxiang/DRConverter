#pragma once
#include "VSBottomBar.h"
#include "afxwin.h"
#include "ImageViewer\DlgImageViewer.h"


// CPageParent 对话框

class CPageParent : public CDlgImageViewer
{
	DECLARE_DYNAMIC(CPageParent)

public:
	CPageParent(UINT nIDTemplate, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageParent();

	// 加载某一帧图像
	virtual BOOL LoadDR(int nCurSlice = 0, BOOL bReLoad = FALSE) { return 1; }

	// 在当前切片后面插入切片
	virtual BOOL AddSlice()  { return 1; }

	// 删除当前切片
	virtual BOOL DeleteSlice()  { return 1; }

	// 重置mapping
	virtual void ResetMapping(){ }

	// 显示/隐藏底部工具栏
	BOOL ShowBottomBar(const CPoint& point);

	// 获取底部工具栏指针
	CVSBottomBar* GetBottomBar() { return m_pBottomBar; }

	// 获取数据头
	CVSDataHeader* GetDataHeader() { return &m_DataHeader; }

	// 获取图像指针
	CImage* GetImage() { return &m_Image; }

	// 设置图像指针
	// void SetImage(CImage* pImage) { Invalidate(TRUE); }

	// 获取切片总数
	int GetSlice() { return m_nSlice; }

	// 获取当前切片
	int GetCurrentSlice() { return m_nCurrentSlice; }

	// 获取文件路径
	CString GetFilePath() { return m_strFilePath; }

	// 获取缩放比例
	float GetZoomRate() { return m_fZoomRate; }

	// 重置所有
	void Reset();

	// 获取文件字节数
	DWORD GetFileBytesNum(){ return m_nSlice * (DR_HEADER_SIZE + m_nWidth * m_nHeight * sizeof(float)); }

	// 获取每帧图像字节数
	int GetSliceBytesNum() { return (DR_HEADER_SIZE + m_nWidth * m_nHeight * sizeof(float)); }

public:
	CVSDataHeader		m_DataHeader;		// 数据头
	CImage				m_Image;			// 当前图像
	int					m_nWidth;			// 图像宽度
	int					m_nHeight;			// 图像高度
	int					m_nSlice;			// 切片层数
	int					m_nCurrentSlice;	// 当前切片
	CString				m_strSelected;		// 所选择的
	CString				m_strFilePath;		// 文件路径
	float				m_fZoomRate;		// 缩放比例
	CRect				m_PaintRect;		// 绘图矩形
	CRect				m_ClientRect;		// 客户区域
	CVSBottomBar*		m_pBottomBar;		// 底部工具栏
	HANDLE				m_hFile;			// 文件对象
	HANDLE				m_hFileMap;			// 文件映射
	BYTE*				m_pMapAddress;		// 映射地址
	__int64				m_i64FileSize;		// 文件大小
	__int64				m_i64FileOffset;	// 文件偏移量
	DWORD				m_dwBlockBytes;		// 块大小
	int					m_kbGranularity;	// 分配粒度(kb)

protected:
	// DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	// 对话框控件
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