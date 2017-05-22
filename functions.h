#pragma once;

#include <vector>
using namespace std;

// ���������ݱ�׼����ĳ������
void Normalized(float *pSrc, int nWidth, int nHeight, float Min = 0.f, float Max = 255.f);

// ɨ��ĳ��Ŀ¼�����DR�ļ�
vector<CString> ScanDiskFile(const CString strPath);

// ���ļ���ɨ��ĳһ���͵��ļ�����ŵ�������
void ScanFormatFile(CFileFind &find, vector<CString> &vStrAllFiles, CString strPath, CString strFormat);

// �Ƚ�����CString�Ĵ�С/ǰ��
bool CStringCompare(CString &s1, CString &s2);

// ��file path��ȡfile name
CString GetFileName(CString strFilePath, BOOL bHasPostfix = TRUE);

// ��file path��ȡfile ext
CString GetFileExt(CString strFilePath);

// �ж�CPoint��CRect�ڲ�
BOOL IsPointInRect(CPoint pt, CRect rt, int dl);

// �ж���꽫Ҫ�뿪
BOOL IsMouseLeaving(CPoint pt, CRect rt, int dl);