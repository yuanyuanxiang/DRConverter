// VSSliceEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "VSCurrentSliceEdit.h"
#include "VSBottomBarWnd.h"
#include "VSImageInfoDlg.h"
#include "VSBottomBar.h"


// CVSCurrentSliceEdit

IMPLEMENT_DYNAMIC(CVSCurrentSliceEdit, CEdit)

CVSCurrentSliceEdit::CVSCurrentSliceEdit()
{

}

CVSCurrentSliceEdit::~CVSCurrentSliceEdit()
{
}


BEGIN_MESSAGE_MAP(CVSCurrentSliceEdit, CEdit)
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()


// CVSCurrentSliceEdit 消息处理程序


// 当编辑框失去焦点时的响应函数
void CVSCurrentSliceEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	CVSBottomBarWnd* pParent = (CVSBottomBarWnd*)GetParent();
	CVSBottomBar* pGP = (CVSBottomBar*)pParent->GetParent();
	pParent->ChangeCurrentSliceEdit();
	pGP->ShowWindow(SW_HIDE);
}