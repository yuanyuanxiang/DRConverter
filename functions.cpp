#include "stdafx.h"
#include "functions.h"
#include <algorithm>
#include "Resource.h"

void Normalized(float *pSrc, int nWidth, int nHeight, float Min, float Max)
{
	float SrcMin = *pSrc, SrcMax = *pSrc;
	for (int i = 0; i < nHeight; ++i)
	{
		for (int j = 0; j < nWidth; ++j)
		{
			if (pSrc[j + i * nWidth] < SrcMin)
			{
				SrcMin = pSrc[j + i * nWidth];
			}
			if (pSrc[j + i * nWidth] > SrcMax)
			{
				SrcMax = pSrc[j + i * nWidth];
			}
		}
	}
	if (Min == Max)
		return;
	for (int i = 0; i < nHeight; ++i)
	{
		for (int j = 0; j < nWidth; ++j)
		{
			pSrc[j + i * nWidth] = (pSrc[j + i * nWidth] - SrcMin) / (SrcMax - SrcMin);
			pSrc[j + i * nWidth] = (Max - Min) * pSrc[j + i * nWidth] + Min;
		}
	}
}

vector<CString> ScanDiskFile(const CString strPath)
{
	// ֧���ļ���ʽ�б�
	CString Formats[SUPPORTED_FORMATS_NUM] = { _T("*.DR") };
	CFileFind find;
	vector<CString> vStrAllFiles;
	for (int i = 0; i < SUPPORTED_FORMATS_NUM; ++i)
	{
		ScanFormatFile(find, vStrAllFiles, strPath, Formats[i]);
	}
	find.Close();
	sort(vStrAllFiles.begin(), vStrAllFiles.end(), CStringCompare);
	return vStrAllFiles;
}

// ���ļ���ɨ��ĳһ���͵��ļ�����ŵ�������
void ScanFormatFile(CFileFind &find, vector<CString> &vStrAllFiles, CString strPath, CString strFormat)
{
	if (strFormat.IsEmpty())
		return;
	BOOL IsFind = find.FindFile(strPath + _T("\\") + strFormat);
	while (IsFind)
	{
		IsFind = find.FindNextFile();
		CString strCurFile = find.GetFilePath();
		CString strFileName = GetFileName(strCurFile, FALSE);
		// �����"." ��ɨ��;����ļ�����С��4
		// ��ȷ��dr����������ʽһ��Ϊ...xxxx.dr
		if (find.IsDots() || strFileName.GetLength() < 4)
		{
			continue;
		}
		// �ļ�
		else
		{
			vStrAllFiles.push_back(strCurFile);
		}
	}
}

// �Ƚ�����CString�Ĵ�С
bool CStringCompare(CString &s1, CString &s2)
{
	return s1 < s2;
}

// ��file path��ȡfile name
CString GetFileName(CString strFilePath, BOOL bHasPostfix)
{
	int nLength = strFilePath.GetLength();
	int nPos = strFilePath.ReverseFind('\\');
	CString name = strFilePath.Right(nLength - nPos - 1);
	// ��ȡû�к�׺���ļ���
	if (bHasPostfix == FALSE)
	{
		int nDotPos = name.ReverseFind('.');
		name = name.Left(nDotPos);
	}
	return name;
}

// ��file path��ȡfile ext
CString GetFileExt(CString strFilePath)
{
	int nLength = strFilePath.GetLength();
	int nPos = strFilePath.ReverseFind('.');
	CString ext = strFilePath.Right(nLength - nPos - 1);
	return ext;
}