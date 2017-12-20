
#ifndef __DXF3GL_H_
#define __DXF3GL_H_

#if 0

#ifdef __cplusplus
extern "C"
{
#endif
	int myGLInit(int argc_, char *FileName, char ** argv);


	int myGLDeal(int w, int h, float rx, float ry, float rz);

#ifdef __cplusplus
}
#endif
#endif
#include <vector>
typedef struct _vect_
{
	float v[3];

}vecf3;
typedef struct _vect2_
{
	float v[2];

}vecf2;

typedef struct _point2_
{
	vecf3 m_Point[2];//抑制过后的线段点对
	vecf3 m_color;//之前的线段颜色（当前版本可不用）
	int isVisal; // 之前版本的 颜色判断可见性的标志位（当前版本可不用）

}myPoint2f;


typedef struct __face_
{
	vecf3 facePoint[3];//一个三角片元的三个顶点的坐标，如x0=facePoint[0].v[0];y0=facePoint[0].v[1];z0=facePoint[0].v[2];
	vecf2 face2DimPoint[3];
	int flag[3];//判断三个顶点对应的三条线段是否抑制的标志位（抑制线已给出可不用）.
	int isEyeSee[3]; // 是否能看见 如果1可以看见，0不能看见
	vecf3 normal;//对应平面的法向量，调用如nx=normal.v[0],nx=normal.v[1],nx=normal.v[2]
	vecf3 * linePoint[3];
	vecf2 * line2DPoint[3];
	int * flagEveryPoint[3];
	int linePointNum[3];
	float lineLen[3];


}MyFaceList;



typedef struct __camerInfo_
{
	float rotation[3]; // 相机旋转角度
	float translate[3]; // 相机平移大小
	float Sigma; // 抑制角度
	float fovy;//视角角度
	float zNear;//焦距
	float zFar;

}CamerInfo;


typedef struct __glInfo_
{
	CamerInfo mCamerInfo;
	int win_w;
	int win_h;

	int outW;
	int outH;
	unsigned char * data;
	unsigned char * tempData;

	float * floatData;
	float * tempFloatData;

	std::vector<myPoint2f> mLineVector;

	int numPtrLines;
	myPoint2f * ptrLines;
	
	float ax;
	float ay;
	float u0;
	float v0;

	int NumFaceList;
	MyFaceList * MyFaceListPtr;

}GLInfo;



#ifdef MYLIBDLL
#define MYLIBDLL extern "C" _declspec(dllimport)
#else
#define MYLIBDLL extern "C" _declspec(dllexport)
#endif

// 初始化一次
 int myGLInit(int argc_, char *FileName, char ** argv, int width, int height);


 GLInfo * myGLDeal(CamerInfo * ptrCamerInfo);

 GLInfo * myEyeDeal(float rx, float ry, float rz, float tx, float ty, float tz);


#endif