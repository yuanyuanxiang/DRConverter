
#pragma once

typedef unsigned int			UINT;
typedef unsigned short			WORD;
typedef unsigned char			BYTE;
typedef int						BOOL;

#define DATA_HEADER_SIZE		((size_t)1024)

class CVSDataHeader
{
public:
	CVSDataHeader()
	{
		memset(this, 0, sizeof(CVSDataHeader));
	}

public:
	int		Attribute;			// 数据文件标识位,	见define定义
	WORD	Width;				// 图像宽度
	WORD	Height;				// 图像高度
	WORD	Slice;				// CT图像扫描层数或者是体数据的切片数
/*	int		Attribute;			// 文件类型
	UINT	Width;				// 宽度
	UINT	nHeight;			// 高度
	UINT	Slice;				// 层数
	int		DataType;			// 数据类型*/

	// 射线源
	float	Voltage;			// 电压
	float	Current;			// 电流
	float	Frequency;			// 频率
	float	TargetPower;		// 靶功率
	float	FocusSize;			// 源焦点尺寸

	// 探测器
	int		DetectorType;		// 探测器类型
	float	IntegrationTime;	// 探测器积分时间
	int		Pixelbinning;		// 像素合并数
	int		Framebinning;		// 图像叠加帧数
	float	UnitWidth;			// 探测器单元宽度, 由实际硬件决定

	// 几何参数
	float	SOD;				// 射线源到转台中心距离
	float	SDD;				// 射线源到探测器距离
	float	RowPixelWidth;		// 像素横向实际宽度
	float	ColumnPixelWidth;	// 像素纵向实际宽度

	// 扫描参数
	float	DRStartHeight;		// 探测器起始高度
	float	DREndHeight;		// 探测器结束高度
	// CT
	float	SliceHeight;		// 层厚度
	int		RotateAngel;		// 旋转角度数
	int		RotaionNum;			// 旋转圈数
	int		RotateCount;		// 采样数

	// 重建校正参数
	int		RandomMove;			// 抖动模式, 0:无抖动, 1:探测器抖动, 2:转台抖动
	BOOL	OutOfView;			// 是否超视野
	float	PanelHeight1;
	float	PanelOffset1;
	float	PanelHeight2;
	float	PanelOffset2;
	float	CenterOffset1;		// 转台中心偏移
	float	VertAngAlign;		// 垂直角度校准
	float	CenterOffset2;		// 转台中心偏移2
};