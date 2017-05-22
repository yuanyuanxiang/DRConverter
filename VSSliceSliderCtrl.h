#pragma once


// CVSSliceSliderCtrl

class CVSSliceSliderCtrl : public CSliderCtrl
{
	DECLARE_DYNAMIC(CVSSliceSliderCtrl)

public:
	CVSSliceSliderCtrl();
	virtual ~CVSSliceSliderCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};