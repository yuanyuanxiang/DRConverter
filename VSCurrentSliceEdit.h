#pragma once


// CVSCurrentSliceEdit£¨µ±Ç°ÇÐÆ¬²ãÊý±à¼­¿ò£©

class CVSCurrentSliceEdit : public CEdit
{
	DECLARE_DYNAMIC(CVSCurrentSliceEdit)

public:
	CVSCurrentSliceEdit();
	virtual ~CVSCurrentSliceEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};