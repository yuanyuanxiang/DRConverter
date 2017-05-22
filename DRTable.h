#pragma once


// CDRTable

class CDRTable : public CTabCtrl
{
	DECLARE_DYNAMIC(CDRTable)

public:
	CDRTable();
	virtual ~CDRTable();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult);
};