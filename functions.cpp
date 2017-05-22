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
	// 支持文件格式列表
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

// 从文件夹扫描某一类型的文件，存放到容器中
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
		// 如果是"." 则不扫描;如果文件名称小于4
		// 正确的dr序列命名方式一般为...xxxx.dr
		if (find.IsDots() || strFileName.GetLength() < 4)
		{
			continue;
		}
		// 文件
		else
		{
			vStrAllFiles.push_back(strCurFile);
		}
	}
}

// 比较两个CString的大小
bool CStringCompare(CString &s1, CString &s2)
{
	return s1 < s2;
}

// 从file path获取file name
CString GetFileName(CString strFilePath, BOOL bHasPostfix)
{
	int nLength = strFilePath.GetLength();
	int nPos = strFilePath.ReverseFind('\\');
	CString name = strFilePath.Right(nLength - nPos - 1);
	// 获取没有后缀的文件名
	if (bHasPostfix == FALSE)
	{
		int nDotPos = name.ReverseFind('.');
		name = name.Left(nDotPos);
	}
	return name;
}

// 从file path获取file ext
CString GetFileExt(CString strFilePath)
{
	int nLength = strFilePath.GetLength();
	int nPos = strFilePath.ReverseFind('.');
	CString ext = strFilePath.Right(nLength - nPos - 1);
	return ext;
}