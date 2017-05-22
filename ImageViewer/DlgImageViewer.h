#pragma once
#include "ImageScrollView.h"
#include "..\Resource.h"


// CDlgImageViewer 对话框

// 工具栏响应函数的ID
#define ID_VIEW_ZOOM_IN 1024
#define ID_VIEW_ZOOM_OUT 1025
#define ID_VIEW_DEFAULT 1026

#define ID_FLIP_VERTICAL 1027
#define ID_FLIP_HORIZONTAL 1028

#define ID_IMAGE_INFO 1029
#define ID_IMAGE_HIST 1030

#define ID_COPY_IMAGE 1031
#define ID_SAVE_IMAGE 1032

#define ID_ABOUT 1033
#define ID_QUIT 1034

// 单次缩放比例
#define ZOOM_RATE 1.1f

// 安全删除指针
#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if ( (p) != NULL ) { delete [] (p); (p) = NULL; }
#endif

// 传入数据类型

#define PASS_TYPE_DEFAULT	0
#define PASS_TYPE_IMAGE		0
#define PASS_TYPE_BYTE		1
#define PASS_TYPE_CHAR		2
#define PASS_TYPE_INT		3
#define PASS_TYPE_FLOAT		4
#define PASS_TYPE_DOUBLE	5

class CDlgImageViewer : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgImageViewer)

public:
	CImageScrollView*	m_pScrollView;			// 滚动视图
	CImage*				m_pImage;				// 图像指针
	CImageList			m_ImageList;			// 图标列表
	CToolBar			m_ToolBar;				// 工具栏
	float				m_fZoomRate;			// 放大倍率
	int					m_nPassType;			// 传入数据类型

	CDlgImageViewer(UINT nIDTemplate, CWnd* pParent = NULL);
	CDlgImageViewer(CImage* pImage, UINT nIDTemplate, CWnd* pParent = NULL);
	CDlgImageViewer(BYTE* ptr, int width, int height, int rowlen, UINT nIDTemplate, CWnd* pParent = NULL);
	CDlgImageViewer(char* ptr, int width, int height, int rowlen, UINT nIDTemplate, CWnd* pParent = NULL);
	CDlgImageViewer(int* ptr, int width, int height, int rowlen, UINT nIDTemplate, CWnd* pParent = NULL);
	CDlgImageViewer(float* ptr, int width, int height, int rowlen, UINT nIDTemplate, CWnd* pParent = NULL);
	CDlgImageViewer(double* ptr, int width, int height, int rowlen, UINT nIDTemplate, CWnd* pParent = NULL);

	template <typename Type> void Initialize(Type* ptr, int width, int height, int rowlen);
	template <typename Type> float FindMaxData(Type* ptr, int width, int height);
	template <typename Type> float FindMinData(Type* ptr, int width, int height);
	template <typename Type> void CopyData(BYTE* pDst, Type* ptr, int width, int height, int rowlen);
	
	virtual ~CDlgImageViewer();					// 析构函数
	void SetImage(CImage *pImage);				// 设置显示的图像
	BYTE* GetImageHeadAddress();				// 获取图像首地址
	BYTE* GetImageHeadAddress(CImage* pImage);	// 获取图像首地址
	BYTE* GetImageLineAddress(int LineID);		// 获取图像某行地址
	void GetImageInfomation(int &width, int &height);
	void GetImageInfomation(int &width, int &height, int &rowlen);
	void GetImageInfomation(int &width, int &height, int &rowlen, int &bpp);
	void GetImageInfomation(int &width, int &height, int &rowlen, int &bpp, int &channel);
	template <typename Type> void CreateImage(Type* ptr, int width, int height, int rowlen);
	void SetColorTableForImage();

	// 对话框数据
	// enum { IDD = IDD_DLG_IMAGE_VIEWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnViewZoomDefault();
	afx_msg void OnUpdateViewZoomIn(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewZoomOut(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewZoomDefault(CCmdUI *pCmdUI);
	afx_msg void OnFlipVertical();
	afx_msg void OnFlipHorizontal();
	afx_msg void OnImageInfo();
	afx_msg void OnImageHist();
	afx_msg void OnCopyImage();
	afx_msg void OnSaveImage();
	afx_msg void OnAbout();
	afx_msg virtual void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual BOOL ContinueModal();
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
};