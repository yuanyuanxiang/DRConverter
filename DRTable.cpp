// DRTable.cpp : ʵ���ļ�
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


// CDRTable ��Ϣ�������


void CDRTable::OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
	// ��ȡ������ָ��
	CDRConverterDlg *pParent = (CDRConverterDlg *)GetParent();
	// �ѵ�ǰ��ҳ����������
	CPageParent **pDialogArray = pParent->GetDialogArray();
	int nCurSelTab = pParent->GetCurSelPageId();
	pDialogArray[nCurSelTab]->ShowWindow(SW_HIDE);
	// �õ��µ�ҳ������
	nCurSelTab = GetCurSel();
	pParent->SetCurSelPage(nCurSelTab);
	// ���µ�ҳ����ʾ����
	pDialogArray[nCurSelTab]->ShowWindow(SW_SHOW);
	*pResult = 0;
}