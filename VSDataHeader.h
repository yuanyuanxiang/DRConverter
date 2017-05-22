
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
	int		Attribute;			// �����ļ���ʶλ,	��define����
	WORD	Width;				// ͼ����
	WORD	Height;				// ͼ��߶�
	WORD	Slice;				// CTͼ��ɨ����������������ݵ���Ƭ��
/*	int		Attribute;			// �ļ�����
	UINT	Width;				// ���
	UINT	nHeight;			// �߶�
	UINT	Slice;				// ����
	int		DataType;			// ��������*/

	// ����Դ
	float	Voltage;			// ��ѹ
	float	Current;			// ����
	float	Frequency;			// Ƶ��
	float	TargetPower;		// �й���
	float	FocusSize;			// Դ����ߴ�

	// ̽����
	int		DetectorType;		// ̽��������
	float	IntegrationTime;	// ̽��������ʱ��
	int		Pixelbinning;		// ���غϲ���
	int		Framebinning;		// ͼ�����֡��
	float	UnitWidth;			// ̽������Ԫ���, ��ʵ��Ӳ������

	// ���β���
	float	SOD;				// ����Դ��ת̨���ľ���
	float	SDD;				// ����Դ��̽��������
	float	RowPixelWidth;		// ���غ���ʵ�ʿ��
	float	ColumnPixelWidth;	// ��������ʵ�ʿ��

	// ɨ�����
	float	DRStartHeight;		// ̽������ʼ�߶�
	float	DREndHeight;		// ̽���������߶�
	// CT
	float	SliceHeight;		// ����
	int		RotateAngel;		// ��ת�Ƕ���
	int		RotaionNum;			// ��תȦ��
	int		RotateCount;		// ������

	// �ؽ�У������
	int		RandomMove;			// ����ģʽ, 0:�޶���, 1:̽��������, 2:ת̨����
	BOOL	OutOfView;			// �Ƿ���Ұ
	float	PanelHeight1;
	float	PanelOffset1;
	float	PanelHeight2;
	float	PanelOffset2;
	float	CenterOffset1;		// ת̨����ƫ��
	float	VertAngAlign;		// ��ֱ�Ƕ�У׼
	float	CenterOffset2;		// ת̨����ƫ��2
};