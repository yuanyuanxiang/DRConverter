// DRTable.cpp : 实现文件
//

#include "stdafx.h"
#include "DRConverter.h"
#include "DRTable.h"
#include "DRConverterDlg.h"


// CDRTable

IMPLEMENT_DYNAMIC(CDRTable, CTabCtrl)

CDRTable::CDRTable()
{

}

CDRTable::~CDRTable()
{
}


BEGIN_MESSAGE_MAP(CDRTable, CTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, &CDRTable::OnTcnSelchange)
END_MESSAGE_MAP()


// CDRTable 消息处理程序


void CDRTable::OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 获取父窗口指针
	CDRConverterDlg *pParent = (CDRConverterDlg *)GetParent();
	// 把当前的页面隐藏起来
	CPageParent **pDialogArray = pParent->GetDialogArray();
	int nCurSelTab = pParent->GetCurSelPageId();
	pDialogArray[nCurSelTab]->ShowWindow(SW_HIDE);
	// 得到新的页面索引
	nCurSelTab = GetCurSel();
	pParent->SetCurSelPage(nCurSelTab);
	// 把新的页面显示出来
	pDialogArray[nCurSelTab]->ShowWindow(SW_SHOW);
	*pResult = 0;
}