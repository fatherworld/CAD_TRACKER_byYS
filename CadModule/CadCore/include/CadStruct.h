#ifndef __CADSTRUCT_H_
#define __CADSTRUCT_H_

#include "DXF3GL.h"

enum {
	mStart = -1,
	mBgrBuffer,
	mGrayBuffer,
	mNormalizedBuffer, // 归一化
	mGaussianKernal,   // 高斯核
	mGaussianBuffer,   // 高斯滤波后的数据
	mGradientBuffer,   // 梯度数据
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
	float angle[3];    // 相机旋转 x, y, z
	float transform[3]; // 相机移动 x, y, z

	float RT[4 * 4];  // 外参矩阵
	float InRef[3 * 4]; // 内参矩阵

	float RT_[4 * 4]; // 相机的位姿 即RT的逆

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
	int numIter; // 寻找次数50


	int Frame = 0;


}CadInfo;

#endif