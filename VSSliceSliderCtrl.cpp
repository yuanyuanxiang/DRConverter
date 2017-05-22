// VSSliceSliderCtrl.cpp : ʵ���ļ�
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


// CVSSliceSliderCtrl ��Ϣ�������


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
	// 2015-10-9 �޸ģ�ʹ�ù������ʱ��������λ�ø�׼ȷ
	//return CSliderCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

void CVSSliceSliderCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	SetFocus();
	CSliderCtrl::OnMouseMove(nFlags, point);
}