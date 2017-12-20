
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
	vecf3 m_Point[2];//���ƹ�����߶ε��
	vecf3 m_color;//֮ǰ���߶���ɫ����ǰ�汾�ɲ��ã�
	int isVisal; // ֮ǰ�汾�� ��ɫ�жϿɼ��Եı�־λ����ǰ�汾�ɲ��ã�

}myPoint2f;


typedef struct __face_
{
	vecf3 facePoint[3];//һ������ƬԪ��������������꣬��x0=facePoint[0].v[0];y0=facePoint[0].v[1];z0=facePoint[0].v[2];
	vecf2 face2DimPoint[3];
	int flag[3];//�ж����������Ӧ�������߶��Ƿ����Ƶı�־λ���������Ѹ����ɲ��ã�.
	int isEyeSee[3]; // �Ƿ��ܿ��� ���1���Կ�����0���ܿ���
	vecf3 normal;//��Ӧƽ��ķ�������������nx=normal.v[0],nx=normal.v[1],nx=normal.v[2]
	vecf3 * linePoint[3];
	vecf2 * line2DPoint[3];
	int * flagEveryPoint[3];
	int linePointNum[3];
	float lineLen[3];


}MyFaceList;



typedef struct __camerInfo_
{
	float rotation[3]; // �����ת�Ƕ�
	float translate[3]; // ���ƽ�ƴ�С
	float Sigma; // ���ƽǶ�
	float fovy;//�ӽǽǶ�
	float zNear;//����
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

// ��ʼ��һ��
 int myGLInit(int argc_, char *FileName, char ** argv, int width, int height);


 GLInfo * myGLDeal(CamerInfo * ptrCamerInfo);

 GLInfo * myEyeDeal(float rx, float ry, float rz, float tx, float ty, float tz);


#endif