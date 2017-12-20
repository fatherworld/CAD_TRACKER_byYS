#ifndef __CADSTRUCT_H_
#define __CADSTRUCT_H_

#include "DXF3GL.h"

enum {
	mStart = -1,
	mBgrBuffer,
	mGrayBuffer,
	mNormalizedBuffer, // ��һ��
	mGaussianKernal,   // ��˹��
	mGaussianBuffer,   // ��˹�˲��������
	mGradientBuffer,   // �ݶ�����
	mGradientLenBuffer,
	mGradientEdge,
	mGradientAng,
	mMakeGrayBuffer,
	mMakeGraySmallBuffer,
	mMakeSmallNormalizeBuffer,
	mMakeSmallGradientBuffer,
	mMakeNoZeroIndexBuffer,
	mMakeNoZeroGradientBuffer,

	BufferNum

};
typedef struct _Size_1_
{
	int width;
	int height;
}mSize;

typedef struct _Size_2_
{
	int x;
	int y;
}Coord;

typedef struct _CameraInfo_
{
	float angle[3];    // �����ת x, y, z
	float transform[3]; // ����ƶ� x, y, z

	float RT[4 * 4];  // ��ξ���
	float InRef[3 * 4]; // �ڲξ���

	float RT_[4 * 4]; // �����λ�� ��RT����

}CameraInfo;

typedef struct __Just_Use_
{
	vecf3 d3Point;
	vecf2 d2Point;
	vecf2 d2FindPoint;
	vecf2 d2Normal;

}ForMartix;

typedef struct All_Martix
{
	float G[6][16];
	
	float M[4 * 4];

	float U[6];

	float U_pre[6];

}AllMartix;

typedef struct cadInfo_
{
	float GetPointStep;
	int NumFaceList;
	MyFaceList * MyFaceListPtr;

	int NumForMartixCap;
	int NumForMartixNow;
	ForMartix * mForMartix;
	AllMartix mAllMartix;

	Coord edgeMin;
	Coord edgeMax;

	int noZeroNum;
	float similaryThreosh;
	int StatusFlag; // Detect or Track

	Coord DetecResult;

	void * bufferList[BufferNum];
	mSize  bufferSize[BufferNum];

	CameraInfo mCameraInfo;


	float mGrayTheshold; // 0.4f;
	float mAngleTol; // 1.01f;
	int numIter; // Ѱ�Ҵ���50


	int Frame = 0;


}CadInfo;

#endif