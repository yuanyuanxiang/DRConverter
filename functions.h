#pragma once;

#include <vector>
using namespace std;

// 将浮点数据标准化到某个区间
void Normalized(float *pSrc, int nWidth, int nHeight, float Min = 0.f, float Max = 255.f);

// 扫描某个目录下面的DR文件
vector<CString> ScanDiskFile(const CString strPath);

// 从文件夹扫描某一类型的文件，存放到容器中
void ScanFormatFile(CFileFind &find, vector<CString> &vStrAllFiles, CString strPath, CString strFormat);

// 比较两个CString的大小/前后
bool CStringCompare(CString &s1, CString &s2);

// 从file path获取file name
CString GetFileName(CString strFilePath, BOOL bHasPostfix = TRUE);

// 从file path获取file ext
CString GetFileExt(CString strFilePath);

// 判断CPoint在CRect内部
BOOL IsPointInRect(CPoint pt, CRect rt, int dl);

// 判断鼠标将要离开
BOOL IsMouseLeaving(CPoint pt, CRect rt, int dl);