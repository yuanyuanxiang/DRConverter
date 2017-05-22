// VSSliceSliderCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "VSSliceSliderCtrl.h"
#include "VSBottomBarWnd.h"
#include "VSBottomBar.h"


// CVSSliceSliderCtrl

IMPLEMENT_DYNAMIC(CVSSliceSliderCtrl, CWnd)

CVSSliceSliderCtrl::CVSSliceSliderCtrl()
{

}

CVSSliceSliderCtrl::~CVSSliceSliderCtrl()
{
}


BEGIN_MESSAGE_MAP(CVSSliceSliderCtrl, CWnd)
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CVSSliceSliderCtrl 消息处理程序


BOOL CVSSliceSliderCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int nTic = GetPos();
	if (zDelta > 0) nTic--;
	else nTic++;
	if (nTic < GetRangeMin()) nTic = GetRangeMin();
	else if (nTic > GetRangeMax()) nTic = GetRangeMax();
	CVSBottomBarWnd* pParent = (CVSBottomBarWnd*)GetParent();
	pParent->SetCurrentSlice(nTic);
	pParent->SetCurrentSliceEditText(nTic);
	SetPos(nTic);
	return TRUE;
	// 2015-10-9 修改，使得滚动鼠标时滑动条的位置更准确
	//return CSliderCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

void CVSSliceSliderCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	SetFocus();
	CSliderCtrl::OnMouseMove(nFlags, point);
}