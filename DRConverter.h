
// DRConverter.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDRConverterApp: 
// �йش����ʵ�֣������ DRConverter.cpp
//

class CDRConverterApp : public CWinApp
{
public:
	CDRConverterApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDRConverterApp theApp;