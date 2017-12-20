
// Microsoft Windows/NT openGL include files
#include <windows.h>

// disable data conversion warnings

#pragma warning(disable : 4244)     // MIPS
#pragma warning(disable : 4136)     // X86
#pragma warning(disable : 4051)     // ALPHA


#include "GL/glew.h"
#include <math.h>
#include <vector>
// local object include files
#include "faccess.h"
#include "globj.h"
#include "glface.h"
//#include "BMP.h"
//#include "AboutLog.h"
#include <GL/freeglut.h>
#include "DXF3GL.h"

#include <iostream>
#define DEBUG 0
using namespace std;
static FaceList *CurObj;
#define  THRESHOLD 0.518f

//#include "opencv2/opencv.hpp"
//using namespace cv;


static float SpinAngle = 0;       // spin angle, in degrees
static float SpinAngleStart[3] = { 0.0f, 0.0f, 0.0f };
static float SpinAngleEnd[3] = { 50.0f, 20.0f, 360.0f };
static float SpinAngleStart_back[3] = { SpinAngleStart[0], SpinAngleStart[1], SpinAngleStart[2] };

static float minPoint[3] = { 0.0f, 0.0f, 0.0f };
static float maxPoint[3] = { 0.0f, 0.0f, 0.0f };

static float CenterOfGra[3] = { 0.0f, 0.0f, 0.0f };
static float Zvalue = 0.0f;
static float Zstep = 10.0f;
static GLInfo g_GLInfo;
//
// The glRotatef function takes four arguments, the angle of rotation,
// and a 3-D point.  The axis of rotation defined by the line from (0,0,0)
// to the 3-D point.  So to rotate on the X-axis, the point would be
// (1, 0, 0), to rotate on the Y-axis (0, 1, 0) and to rotate on the
// Z-axis (0, 0, 1).
//
const int On = 1;
const int Off = 0;
// Rotate first on the X-axis
static int AxisPoint[3] = { On, On, On };
static int AxisIx = 0;
static int AxisIxFlag[3] = { 0, 0, 0 };

void SetObj(FaceList *obj)
{
	CurObj = obj;
}


//
// Get the view values (e.g., the min and max values for the X, Y and Z
// axies.
//
typedef struct {
	float min;
	float max;
} AxisMinMax;
static AxisMinMax Axis;
typedef struct __ColorFlag_
{


}LineColorFlag;

typedef struct _Point_2_
{
	float x0;
	float y0;
	float z0;
	float x1;
	float y1;
	float z1;

	float grey;

	FaceList::vect aveColor;

	int flag;

	int faceListIndex0;
	int faceListIndex1;

	int isVisible; // 1 可见， 0 不可见
	FaceList::vect line[2];

	FaceList::vect faceFirst[3];
	FaceList::vect faceSec[3];
	FaceList::vect RGB_color;


	std::vector<FaceList::vect> mlineVec[2];

	std::vector<FaceList::vect> mPointVec[2];


}mPoint2;
typedef struct _RGB
{
	float R;
	float G;
	float B;
}mColorInfo;

typedef struct _lineInfo
{
	std::vector<mPoint2> mVectorPoint;
	std::vector<mColorInfo> mVectorColor;
	int isfirst = 0;

}LineInfo;
static LineInfo g_LineInfo;
int num0 = 0;
int num1 = 0;
static int FrameNum = 0;
void init_minmax(GL_object &GL_Obj)
{
	Axis.min = GL_Obj.PointRange.GetMin();
	Axis.max = GL_Obj.PointRange.GetMax();
	Axis.min = (float)((int)Axis.min + 1);
	Axis.max = (float)((int)Axis.max + 1);
}


void print_minmax(void)
{
	//printf("axis range = %2.2f to %2.2f\n", Axis.min, Axis.max );
}

#undef clamp_g
#define clamp_g(x, minValue, maxValue) ((x) < (minValue) ? (minValue) : ((x) > (maxValue) ? (maxValue) : (x)))


//
// Feed the point list to openGL as a polygon
//

void disp_gl_face(FaceList::face *face_ptr)
{
	int NumPts = face_ptr->point_cnt;
	int i;
	//glColor3f(0.0f, 1.0f, 0.0f);
	/*norm.x = va.y * vb.z - va.z * vb.y

	norm.y = va.z * vb.x - va.x * vb.z

	norm.z = va.x * vb.y - va.y * vb.x*/

	FaceList::vect *f = face_ptr->f;

	FaceList::vect vect_t1, vect_t2;

	vect_t1.v[0] = f[0].v[0] - f[1].v[0];
	vect_t1.v[1] = f[0].v[1] - f[1].v[1];
	vect_t1.v[2] = f[0].v[2] - f[1].v[2];

	vect_t2.v[0] = f[2].v[0] - f[1].v[0];
	vect_t2.v[1] = f[2].v[1] - f[1].v[1];
	vect_t2.v[2] = f[2].v[2] - f[1].v[2];



	float r1 = vect_t1.v[1] * vect_t2.v[2] - vect_t1.v[2] * vect_t2.v[1];
	float g1 = vect_t1.v[2] * vect_t2.v[0] - vect_t1.v[0] * vect_t2.v[2];
	float b1 = vect_t1.v[0] * vect_t2.v[1] - vect_t1.v[1] * vect_t2.v[0];

	float sss = 0.0f;
	 r1 = r1 / (pow((r1 * r1 + g1 * g1 + b1 * b1), 0.5) + sss);
	 g1 = g1 / (pow((r1 * r1 + g1 * g1 + b1 * b1), 0.5) + sss);
	 b1 = b1 / (pow((r1 * r1 + g1 * g1 + b1 * b1), 0.5) + sss);

	face_ptr->normal.v[0] = r1;
	face_ptr->normal.v[1] = g1;
	face_ptr->normal.v[2] = b1;
	r1 =abs(r1);
	g1 =abs(g1);
	b1 =abs(b1);
	
	/*glColor4f(r1, g1, b1, 1.0f);*/


	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	mColorInfo m_ColorInfo;
	m_ColorInfo.R = r1;
	m_ColorInfo.G = g1;
	m_ColorInfo.B = b1;
	g_LineInfo.mVectorColor.push_back(m_ColorInfo);


	glEnable(GL_DEPTH_TEST);
	glBegin(GL_TRIANGLES);

	glNormal3d(r1, g1, b1);
	for (i = 0; i < NumPts; i++) {
		glVertex3fv(face_ptr->f[i].v);
	}
	glEnd();
} // disp_gl_face
static int imgRotationNew(unsigned char *img, unsigned char *destImg, int * m_width, int * m_height, int m_cammer, int m_angle)
{
	int width = *m_width;
	int height = *m_height;
	int channels = 3;

	unsigned char * srcImagUV = (unsigned char *)((int)img + height * width);
	unsigned char * destImgUV = (unsigned char *)((int)destImg + width * height);

	int dx = 0;
	int dy = 0;
	if (0 == m_cammer)
	{
		// 图像矩阵需要向右旋转90度
		if (0 == m_angle)
		{
			*m_width = height;
			*m_height = width;
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					dx = height - 1 - i;
					dy = j;
					destImg[(dy*height + dx) * 3 + 0] = img[(i*width + j) * 3 + 0];
					destImg[(dy*height + dx) * 3 + 1] = img[(i*width + j) * 3 + 1];
					destImg[(dy*height + dx) * 3 + 2] = img[(i*width + j) * 3 + 2];

				}
			}
		}
		// 不用旋转
		else if (90 == m_angle)
		{
			memcpy(destImg, img, width * height * sizeof(unsigned char) * 3 / 2);
		}
		// 旋转180度
		else if (270 == m_angle)
		{

			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					dx = width - 1 - j;
					dy = height - 1 - i;
					destImg[(dy*height + dx) * 3 + 0] = img[(i*width + j) * 3 + 0];
					destImg[(dy*height + dx) * 3 + 1] = img[(i*width + j) * 3 + 1];
					destImg[(dy*height + dx) * 3 + 2] = img[(i*width + j) * 3 + 2];


				}
			}

		}
		// 不做处理，任其错误
		else if (180 == m_angle)
		{
			memcpy(destImg, img, width * height * sizeof(unsigned char) * 3);
		}

	}
	else if (1 == m_cammer) // 前置摄像头
	{
		// 左旋90度，左右镜像
		if (0 == m_angle)
		{
			*m_width = height;
			*m_height = width;
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					dx = height - 1 - i;
					dy = width - 1 - j;
					destImg[(dy*height + dx) * 3 + 0] = img[(i*width + j) * 3 + 0];
					destImg[(dy*height + dx) * 3 + 1] = img[(i*width + j) * 3 + 1];
					destImg[(dy*height + dx) * 3 + 2] = img[(i*width + j) * 3 + 2];
				}
			}

		}
		// 左右镜像
		else if (90 == m_angle)
		{

			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					dx = width - 1 - j;
					dy = i;
					destImg[(dy*height + dx) * 3 + 0] = img[(i*width + j) * 3 + 0];
					destImg[(dy*height + dx) * 3 + 1] = img[(i*width + j) * 3 + 1];
					destImg[(dy*height + dx) * 3 + 2] = img[(i*width + j) * 3 + 2];
				}
			}

		}
		// 上下镜像
		else if (270 == m_angle)
		{
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					dx = j;
					dy = height - 1 - i;
					destImg[(dy*width + dx) * 3 + 0] = img[(i*width + j) * 3 + 0];
					destImg[(dy*width + dx) * 3 + 1] = img[(i*width + j) * 3 + 1];
					destImg[(dy*width + dx) * 3 + 2] = img[(i*width + j) * 3 + 2];
				}
			}

		}
		// 不做处理
		else if (180 == m_angle)
		{
			memcpy(destImg, img, width * height * sizeof(unsigned char));
		}

	}
	return 0;
}

#if 0
bool screenshot(const char* filename)
//从显示窗口中截屏，得到图像数据
{
	GLenum lastBuffer;
	GLbyte* pBits = 0; // 图像数据	
	unsigned long lImageSize;
	GLint iViewport[4]; // 视图大小	
	glGetIntegerv(GL_VIEWPORT, iViewport);
	lImageSize = iViewport[2] * iViewport[3] * 3;
	pBits = (GLbyte*)new unsigned char[lImageSize];
	if (!pBits)		return false;	// 从color buffer中读取数据	
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);	//	
	glGetIntegerv(GL_READ_BUFFER, (GLint*)&lastBuffer);
	glReadBuffer(GL_FRONT);	glReadPixels(0, 0, iViewport[2], iViewport[3], GL_BGR_EXT, GL_UNSIGNED_BYTE, pBits);

	glReadBuffer(GL_FRONT);	glReadPixels(0, 0, iViewport[2], iViewport[3], GL_BGR_EXT, GL_FLOAT, g_GLInfo.floatData);
	glReadBuffer(lastBuffer);
	//printf("iV %d %d \n", iViewport[2], iViewport[3]);
	//if (writeBMP(filename, (unsigned char*)pBits, iViewport[2], iViewport[3]))
	//	return true;
	
// 	for (int i = 0; i < iViewport[2] * iViewport[3]; i++)
// 	{
// 		if (g_GLInfo.floatData[i * 3 + 2] != 0)
// 		{
// 			printf("find red color  %d %f\n", i, g_GLInfo.floatData[i * 3 + 2]);
// 		}
// 	}
	
	int m_width = iViewport[2];
	int m_height = iViewport[3];
	
	IMAGE mImage;
	memcpy(g_GLInfo.tempData, pBits, iViewport[2] * iViewport[3] * 3);
	imgRotationNew(g_GLInfo.tempData, g_GLInfo.data, &m_width, &m_height, 1, 270);
	
	g_GLInfo.outW = m_width;
	g_GLInfo.outH = m_height;
	mImage.width = m_width;
	mImage.height = m_height;
	mImage.data = g_GLInfo.data;
	mImage.channels = 3;
	//WriteBMP(filename, &mImage, BIT24);
	free(pBits);

	return false;
}
#endif
//
// Display the wire frame described by the face list pointed to by
// CurObj.
//

int comparePoint(FaceList::face *m_curFace, FaceList::face *m_curFace1, mPoint2 *t_mPoint2, int * index_, int * index1)
{
	FaceList::vect m_vect[3];
	FaceList::vect m_vect1[3];
	int index[2] = { 9, 9 };
	int indexOther[2] = { 9, 9 };

	m_vect[0] = m_curFace->f[0];
	m_vect[1] = m_curFace->f[1];
	m_vect[2] = m_curFace->f[2];

	m_vect1[0] = m_curFace1->f[0];
	m_vect1[1] = m_curFace1->f[1];
	m_vect1[2] = m_curFace1->f[2];

	memcpy(&t_mPoint2->faceFirst[0], &m_curFace->f[0], sizeof(float) * 3);

	memcpy(&t_mPoint2->faceFirst[1], &m_curFace->f[1], sizeof(float) * 3);

	memcpy(&t_mPoint2->faceFirst[2], &m_curFace->f[2], sizeof(float) * 3);

	memcpy(&t_mPoint2->faceSec[0], &m_curFace1->f[0], sizeof(float) * 3);

	memcpy(&t_mPoint2->faceSec[1], &m_curFace1->f[1], sizeof(float) * 3);

	memcpy(&t_mPoint2->faceSec[2], &m_curFace1->f[2], sizeof(float) * 3);

	int flag[3] = { 0, 0, 0 };
	int count = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			//if (flag[j] == 0)
			{
				if (m_vect[i].v[0] == m_vect1[j].v[0] && m_vect[i].v[1] == m_vect1[j].v[1] && m_vect[i].v[2] == m_vect1[j].v[2])
				{
					if (count == 0)
					{
						t_mPoint2->x0 = m_vect[i].v[0];
						t_mPoint2->y0 = m_vect[i].v[1];
						t_mPoint2->z0 = m_vect[i].v[2];

						t_mPoint2->line[0].v[0] = m_vect[i].v[0];
						t_mPoint2->line[0].v[1] = m_vect[i].v[1];
						t_mPoint2->line[0].v[2] = m_vect[i].v[2];

						index[count] = i;
						indexOther[count] = j;
					}
					else if (count == 1)
					{
						t_mPoint2->x1 = m_vect[i].v[0];
						t_mPoint2->y1 = m_vect[i].v[1];
						t_mPoint2->z1 = m_vect[i].v[2];

						t_mPoint2->line[1].v[0] = m_vect[i].v[0];
						t_mPoint2->line[1].v[1] = m_vect[i].v[1];
						t_mPoint2->line[1].v[2] = m_vect[i].v[2];

						index[count] = i;
						indexOther[count] = j;
					}

					count++;
					break;
					flag[j] = 1;



				}
			}

		}

	}
	if (count == 2)
	{
		if ((index[0] == 0 && index[1] == 1) || (index[0] == 1 && index[1] == 0))
		{
			*index_ = 0;
		}
		else if ((index[0] == 1 && index[1] == 2) || (index[0] == 2 && index[1] == 1))
		{
			*index_ = 1;
		}
		else if ((index[0] == 0 && index[1] == 2) || (index[0] == 2 && index[1] == 0))
		{
			*index_ = 2;
		}


		if ((indexOther[0] == 0 && indexOther[1] == 1) || (indexOther[0] == 1 && indexOther[1] == 0))
		{
			*index1 = 0;
		}
		else if ((indexOther[0] == 1 && indexOther[1] == 2) || (indexOther[0] == 2 && indexOther[1] == 1))
		{
			*index1 = 1;
		}
		else if ((indexOther[0] == 0 && indexOther[1] == 2) || (indexOther[0] == 2 && indexOther[1] == 0))
		{
			*index1 = 2;
		}

		return 1;
	}
	else
	{
		return 0;
	}

}
// 判断是否是同一个点
// 返回1 是同一个点
// 返回0 不是同一个点
int isSamePoint(FaceList::vect a, FaceList::vect b)
{
	if (a.v[0] == b.v[0] && a.v[1] == b.v[1] && a.v[2] == b.v[2])
	{
		return 1;
	}
	else
	{
		return 0;
	}

}
// 判断是否是同一条线
// 返回1 是同一条线
// 返回0 不是同一条线
int isSameLine(mPoint2 * ptrPoint, mPoint2 * ptrVecPoint)
{
	
	if (
		(isSamePoint(ptrPoint->line[0], ptrVecPoint->line[0]) && isSamePoint(ptrPoint->line[1], ptrVecPoint->line[1])) ||
		(isSamePoint(ptrPoint->line[0], ptrVecPoint->line[1]) && isSamePoint(ptrPoint->line[1], ptrVecPoint->line[0]))
		)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}
void compareLine(CamerInfo * ptrCamerInfo, FaceList::face *m_curFace, FaceList::face *m_curFace1, int n, int m)
{
	mPoint2 t_mPoint2;
	int m_index = 0;
	int m_indexOther = 0;
	int ret = comparePoint(m_curFace, m_curFace1, &t_mPoint2, &m_index, &m_indexOther);
	//float thresh = 0.5f;
	if (ret == 1) // 相交
	{
		float A = pow(g_LineInfo.mVectorColor[m].R - g_LineInfo.mVectorColor[n].R, 2) + pow(g_LineInfo.mVectorColor[m].G - g_LineInfo.mVectorColor[n].G, 2) +
			pow(g_LineInfo.mVectorColor[m].B - g_LineInfo.mVectorColor[n].B, 2);
		A = pow(A, 0.5f);

		t_mPoint2.faceListIndex0 = n;
		t_mPoint2.faceListIndex1 = m;

		t_mPoint2.aveColor.v[0] = (g_LineInfo.mVectorColor[m].R + g_LineInfo.mVectorColor[n].R) / 2;
		t_mPoint2.aveColor.v[1] = (g_LineInfo.mVectorColor[m].G + g_LineInfo.mVectorColor[n].G) / 2;
		t_mPoint2.aveColor.v[2] = (g_LineInfo.mVectorColor[m].B + g_LineInfo.mVectorColor[n].B) / 2;

		//printf("n m m_index A = %d %d %d %f ", n, m, m_index, A);

		float t_threshold = 2 * sinf(ptrCamerInfo->Sigma * 3.1415926f / 360);

		if (A <= t_threshold)
		{
			t_mPoint2.grey = 0.0;
			t_mPoint2.flag = 0;
			m_curFace->isFlag[m_index] = 0;
			num0++;
		}
		else
		{
			t_mPoint2.grey = A;
			t_mPoint2.flag = 1;
			m_curFace->isFlag[m_index] = 1;
			m_curFace1->isFlag[m_indexOther] = 0;

			num1++;
			int flag = 0;
			for (int k = 0; k < g_LineInfo.mVectorPoint.size(); k++)
			{
				if (isSameLine(&t_mPoint2, &(g_LineInfo.mVectorPoint[k])))
				{
					flag = 1;
					break;
				}

			}
			if (flag == 0)
			{
				g_LineInfo.mVectorPoint.push_back(t_mPoint2);
			}
			
		}
		//printf(" [] %d \n", m_curFace->isFlag[m_index]);
		

	}

}

void  display_new_my(CamerInfo * ptrCamerInfo)
{

	//g_GLInfo.mLineVector.clear();
	if (g_GLInfo.ptrLines != NULL)
	{
		free(g_GLInfo.ptrLines);
		g_GLInfo.ptrLines = NULL;
		g_GLInfo.ptrLines = 0;
		g_GLInfo.numPtrLines = 0;
	}

	

	float rx = ptrCamerInfo->rotation[0];
	float ry = ptrCamerInfo->rotation[1];
	float rz = ptrCamerInfo->rotation[2];
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f,0.0f, ptrCamerInfo->translate[2],0.0f, 0.0f, 0.0, 0.0, 1.0, 0.0);

	SpinAngleStart[2] = rz;
	SpinAngleStart[1] = ry;
	SpinAngleStart[0] = rx;


	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CenterOfGra[0] = 0.0f;
	CenterOfGra[1] = 0.0f;
	CenterOfGra[2] = 0.0f;

	int faceNum = 0;

	if (CurObj != NULL)
	{
		FaceList::ListHandle FaceList = NULL;
		faceNum = 0;
		if (g_LineInfo.isfirst == 0)
		{
			FaceList = CurObj->GetListStart();
			FaceList::face * m_curFaceFirst = CurObj->GetFace(FaceList);
			minPoint[0] = m_curFaceFirst->f->v[0];
			minPoint[1] = m_curFaceFirst->f->v[1];
			minPoint[2] = m_curFaceFirst->f->v[2];

			maxPoint[0] = m_curFaceFirst->f->v[0];
			maxPoint[1] = m_curFaceFirst->f->v[1];
			maxPoint[2] = m_curFaceFirst->f->v[2];

			
			FaceList = CurObj->GetListStart();
			while (FaceList != NULL)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				FaceList::face * m_curFace = CurObj->GetFace(FaceList);

				disp_gl_face(m_curFace);
				
				//CenterOfGra[0] += m_curFace->f[0].v[0];
				//CenterOfGra[0] += m_curFace->f[1].v[0];
				//CenterOfGra[0] += m_curFace->f[2].v[0];
				//
				//CenterOfGra[1] += m_curFace->f[0].v[1];
				//CenterOfGra[1] += m_curFace->f[1].v[1];
				//CenterOfGra[1] += m_curFace->f[2].v[1];
				//
				//CenterOfGra[2] += m_curFace->f[0].v[2];
				//CenterOfGra[2] += m_curFace->f[1].v[2];
				//CenterOfGra[2] += m_curFace->f[2].v[2];

				for (int i = 0; i < 3; i++)
				{
					if (m_curFace->f[i].v[0] < minPoint[0])
					{
						minPoint[0] = m_curFace->f[i].v[0];
					}
					else if (m_curFace->f[i].v[0] > maxPoint[0])
					{
						maxPoint[0] = m_curFace->f[i].v[0];
					}

					if (m_curFace->f[i].v[1] < minPoint[1])
					{
						minPoint[1] = m_curFace->f[i].v[1];
					}
					else if (m_curFace->f[i].v[1] > maxPoint[1])
					{
						maxPoint[1] = m_curFace->f[i].v[1];
					}

					if (m_curFace->f[i].v[2] < minPoint[2])
					{
						minPoint[2] = m_curFace->f[i].v[2];
					}
					else if (m_curFace->f[i].v[2] > maxPoint[2])
					{
						maxPoint[2] = m_curFace->f[i].v[2];
					}
				}
				
				//printf("[%d] = normal = %f %f %f \n", faceNum, m_curFace->normal.v[0], m_curFace->normal.v[1], m_curFace->normal.v[2]);
				faceNum++;

				FaceList = CurObj->GetNextFace(FaceList);
			} // 
			

			//CenterOfGra[0] /= kk * 3;
			//CenterOfGra[1] /= kk * 3;
			//CenterOfGra[2] /= kk * 3;

			CenterOfGra[0] = (minPoint[0] + maxPoint[0]) / 2;
			CenterOfGra[1] = (minPoint[1] + maxPoint[1]) / 2;
			CenterOfGra[2] = (minPoint[2] + maxPoint[2]) / 2;

			//printf("%f %f %f [] \n", CenterOfGra[0], CenterOfGra[1], CenterOfGra[2]);
			
			FaceList = CurObj->GetListStart();
			while (FaceList != NULL)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				FaceList::face * m_curFace = CurObj->GetFace(FaceList);

				FaceList = CurObj->GetNextFace(FaceList);


				m_curFace->f[0].v[0] -= CenterOfGra[0];
				m_curFace->f[1].v[0] -= CenterOfGra[0];
				m_curFace->f[2].v[0] -= CenterOfGra[0];
				m_curFace->f[0].v[1] -= CenterOfGra[1];
				
				m_curFace->f[1].v[1] -= CenterOfGra[1];
				m_curFace->f[2].v[1] -= CenterOfGra[1];
				m_curFace->f[0].v[2] -= CenterOfGra[2];
				
				m_curFace->f[1].v[2] -= CenterOfGra[2];
				m_curFace->f[2].v[2] -= CenterOfGra[2];

			
			} // 

			g_GLInfo.NumFaceList = faceNum;
			g_GLInfo.MyFaceListPtr = (MyFaceList *)calloc(sizeof(MyFaceList), faceNum);
			int kk = 0;
			// 把三维数据保存
			FaceList = CurObj->GetListStart();
			while (FaceList != NULL)
			{
				FaceList::face * m_curFace = CurObj->GetFace(FaceList);

				FaceList = CurObj->GetNextFace(FaceList);


				g_GLInfo.MyFaceListPtr[kk].facePoint[0].v[0] = m_curFace->f[0].v[0];
				g_GLInfo.MyFaceListPtr[kk].facePoint[1].v[0] = m_curFace->f[1].v[0];
				g_GLInfo.MyFaceListPtr[kk].facePoint[2].v[0] = m_curFace->f[2].v[0];

				g_GLInfo.MyFaceListPtr[kk].facePoint[0].v[1] = m_curFace->f[0].v[1];
				g_GLInfo.MyFaceListPtr[kk].facePoint[1].v[1] = m_curFace->f[1].v[1];
				g_GLInfo.MyFaceListPtr[kk].facePoint[2].v[1] = m_curFace->f[2].v[1];

				g_GLInfo.MyFaceListPtr[kk].facePoint[0].v[2] = m_curFace->f[0].v[2];
				g_GLInfo.MyFaceListPtr[kk].facePoint[1].v[2] = m_curFace->f[1].v[2];
				g_GLInfo.MyFaceListPtr[kk].facePoint[2].v[2] = m_curFace->f[2].v[2];

				g_GLInfo.MyFaceListPtr[kk].normal.v[0] = m_curFace->normal.v[0];
				g_GLInfo.MyFaceListPtr[kk].normal.v[1] = m_curFace->normal.v[1];
				g_GLInfo.MyFaceListPtr[kk].normal.v[2] = m_curFace->normal.v[2];

				g_GLInfo.MyFaceListPtr[kk].flag[0] = m_curFace->isFlag[0];
				g_GLInfo.MyFaceListPtr[kk].flag[1] = m_curFace->isFlag[1];
				g_GLInfo.MyFaceListPtr[kk].flag[2] = m_curFace->isFlag[2];

				kk++;

			} // 
			g_LineInfo.isfirst = 1;

		}

		static float zz = 0.0f;
		zz = CenterOfGra[0];
		//glTranslatef(-CenterOfGra[0], -CenterOfGra[1], -CenterOfGra[2]);

		glTranslatef(ptrCamerInfo->translate[0],ptrCamerInfo->translate[1], Zvalue);

		glRotatef((float)SpinAngleStart[2], 0.0f, 0.0f, 1.0f);
	
		glRotatef((float)SpinAngleStart[1], 0.0f, 1.0f, 0.0f);
		
		glRotatef((float)SpinAngleStart[0], 1.0f, 0.0f, 0.0f);
		FaceList = CurObj->GetListStart();

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1., 1.);
		int tempKK = 0;
		while (FaceList != NULL) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			FaceList::face * m_curFace = CurObj->GetFace(FaceList);


			disp_gl_face(m_curFace);
			
			m_curFace->isFlag[0] = 1;
			m_curFace->isFlag[1] = 1;
			m_curFace->isFlag[2] = 1;

			FaceList = CurObj->GetNextFace(FaceList);

			tempKK++;

		} // 
		//printf("init face = %d \n", tempKK);
		g_LineInfo.mVectorPoint.clear();
		FaceList = CurObj->GetListStart();
		int n = 0;

		num0 = 0;
		num1 = 0;
		int temp_0 = 0;
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		while (FaceList != NULL) {

			FaceList::ListHandle FaceList1 = CurObj->GetListStart();

			FaceList::face * m_curFace = CurObj->GetFace(FaceList);

			int m = 0;
			while (FaceList1 != NULL)
			{
				if (FaceList != FaceList1)
				{
					FaceList::face * m_curFace1 = CurObj->GetFace(FaceList1);
					compareLine(ptrCamerInfo, m_curFace, m_curFace1, n, m);
				}

				FaceList1 = CurObj->GetNextFace(FaceList1);
				m++;
			}

			FaceList = CurObj->GetNextFace(FaceList);
			n++;
		} // 


		FaceList = CurObj->GetListStart();
		n = 0;
		int lineNum = 0;
		while (FaceList != NULL) {

			FaceList::ListHandle FaceList1 = CurObj->GetListStart();

			FaceList::face * m_curFace = CurObj->GetFace(FaceList);


			g_GLInfo.MyFaceListPtr[n].flag[0] = m_curFace->isFlag[0];
			g_GLInfo.MyFaceListPtr[n].flag[1] = m_curFace->isFlag[1];
			g_GLInfo.MyFaceListPtr[n].flag[2] = m_curFace->isFlag[2];

			g_GLInfo.MyFaceListPtr[n].isEyeSee[0] = 1;
			g_GLInfo.MyFaceListPtr[n].isEyeSee[1] = 1;
			g_GLInfo.MyFaceListPtr[n].isEyeSee[2] = 1;

			//printf(" gl [%d] = %d %d %d \n", n, m_curFace->isFlag[0], m_curFace->isFlag[1], m_curFace->isFlag[2]);
			
			int temp = m_curFace->isFlag[0] + m_curFace->isFlag[1] + m_curFace->isFlag[2];
			lineNum += temp;
			//printf("line num = %d \n", lineNum);
			GLuint one_query;
			GLint result = 0;
#if 1
			{
				//坐标轴
				glColor3f(1.0f, 0.0f, 0.0f);
				glBegin(GL_LINE_LOOP);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(10.0f, 0.0f, 0.0f);
				glEnd();

				glColor3f(0.0f, 1.0f, 0.0f);
				glBegin(GL_LINE_LOOP);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(0.0f, 10.0f, 0.0f);
				glEnd();

				glColor3f(0.0f, 0.0f, 1.0f);
				glBegin(GL_LINE_LOOP);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(0.0f, 0.0f, 10.0f);
				glEnd();



			}
#endif
			glColor3f(0.0f, 1.0f, 0.0f);

			glGenQueries(1, &one_query);
			glBeginQuery(GL_SAMPLES_PASSED, one_query);

			glBegin(GL_LINE_LOOP);
			if (m_curFace->isFlag[0] == 1)
			{
				glVertex3fv(m_curFace->f[0].v);
				glVertex3fv(m_curFace->f[1].v);
				temp_0++;
			}
			glEnd();

			glEndQuery(GL_SAMPLES_PASSED);
			glGetQueryObjectiv(one_query, GL_QUERY_RESULT, &result);
			if (result > 5)
			{
				g_GLInfo.MyFaceListPtr[n].isEyeSee[0] = 1;
			}
			else
			{
				g_GLInfo.MyFaceListPtr[n].isEyeSee[0] = 0;
			}


			glGenQueries(1, &one_query);
			glBeginQuery(GL_SAMPLES_PASSED, one_query);

			glBegin(GL_LINE_LOOP);
			if (m_curFace->isFlag[1] == 1)
			{
				glVertex3fv(m_curFace->f[1].v);
				glVertex3fv(m_curFace->f[2].v);
				temp_0++;
			}
			glEnd();

			glEndQuery(GL_SAMPLES_PASSED);
			glGetQueryObjectiv(one_query, GL_QUERY_RESULT, &result);
			if (result > 5)
			{
				g_GLInfo.MyFaceListPtr[n].isEyeSee[1] = 1;
			}
			else
			{
				g_GLInfo.MyFaceListPtr[n].isEyeSee[1] = 0;
			}

			glGenQueries(1, &one_query);
			glBeginQuery(GL_SAMPLES_PASSED, one_query);

			glBegin(GL_LINE_LOOP);
			if (m_curFace->isFlag[2] == 1)
			{
				glVertex3fv(m_curFace->f[2].v);
				glVertex3fv(m_curFace->f[0].v);
				temp_0++;
			}
			glEnd();

			glEndQuery(GL_SAMPLES_PASSED);
			glGetQueryObjectiv(one_query, GL_QUERY_RESULT, &result);
			if (result > 5)
			{
				g_GLInfo.MyFaceListPtr[n].isEyeSee[2] = 1;
			}
			else
			{
				g_GLInfo.MyFaceListPtr[n].isEyeSee[2] = 0;
			}

			FaceList = CurObj->GetNextFace(FaceList);

			n++;
		}


		//cv::Mat image = cv::Mat::zeros(win_h, win_w, CV_8UC3);

#if 0
		for (int i = 0; i < g_LineInfo.mVectorPoint.size(); i++)
		{
			int temp = i / 255;
			int temp1 = i % 255;

			float G_ = temp / 255.0f;
			float B_ = temp1 / 255.0f;

			GLuint one_query;

			glColor3f(1.0f, G_, B_);


			glGenQueries(1, &one_query);


			glBeginQuery(GL_SAMPLES_PASSED, one_query);

			glBegin(GL_LINE_LOOP);

			glVertex3fv(g_LineInfo.mVectorPoint[i].line[0].v);
			glVertex3fv(g_LineInfo.mVectorPoint[i].line[1].v);
	
			glEnd();

			glEndQuery(GL_SAMPLES_PASSED);

			GLint result;

			glGetQueryObjectiv(one_query, GL_QUERY_RESULT, &result);
// 			g_LineInfo.mVectorPoint[i].RGB_color.v[0] = 1.0f;
// 			g_LineInfo.mVectorPoint[i].RGB_color.v[1] = G_;
// 			g_LineInfo.mVectorPoint[i].RGB_color.v[2] = B_;
			if (result > 5)
			{
				g_LineInfo.mVectorPoint[i].isVisible = 1;

				/*cv::line(image, g_LineInfo.mVectorPoint[i].line[0].v
				, g_LineInfo.mVectorPoint[i].line[1].v, cv::Scalar(255, 0, 0), 3, 8, 0);*/
			}
			else
			{
				g_LineInfo.mVectorPoint[i].isVisible = 0;
			}
			/*std::cout << result << std::endl;*/
		}
#endif

		//glBegin(GL_LINE_LOOP);
		//glColor3f(0.0f, 0.0f, 1.0f);
		//glVertex3f(0.0f,0.0f,0.0f);
		//glVertex3f(0.0f, 0.0f, 100.0f);
		//glEnd();
		//glBegin(GL_LINE_LOOP);
		//glColor3f(0.0f, 1.0f, 0.0f);
		//glVertex3f(0.0f, 0.0f, 0.0f);
		//glVertex3f(0.0f, 100.0f, 0.0f);
		//glEnd();
		//glBegin(GL_LINE_LOOP);
		//glColor3f(1.0f, 0.0f, 0.0f);
		//glVertex3f(0.0f, 0.0f, 0.0f);
		//glVertex3f(100.0f, 0.0f, 0.0f);
		//glEnd();
		//printf("num0 = %d num1 = %d temp_0 = %d \n", num0, num1, temp_0);

	}

	glutSwapBuffers();

	{
		char fName[300];
		static int count = 0;
		sprintf_s(fName, 300, "Image/write_%d.bmp", count);
		//screenshot(fName);
		count++;
	}

	int xxxx = 0;

	//printf("mVectorPoint.size = %d \n", g_LineInfo.mVectorPoint.size());

	g_GLInfo.ptrLines = (myPoint2f *)calloc(sizeof(myPoint2f), g_LineInfo.mVectorPoint.size());
	g_GLInfo.numPtrLines = g_LineInfo.mVectorPoint.size();

	
	
	for (int i = 0; i < g_LineInfo.mVectorPoint.size(); i++)
	{
#if 0
		g_LineInfo.mVectorPoint[i].isVisible = 0;
		for (int n = 0; n < g_GLInfo.outH; n++)
		{
			for (int m = 0; m < g_GLInfo.outW; m++)
			{
				float b = g_GLInfo.floatData[(n * g_GLInfo.outW + m) * 3 + 0];
				float g = g_GLInfo.floatData[(n * g_GLInfo.outW + m) * 3 + 1];
				float r = g_GLInfo.floatData[(n * g_GLInfo.outW + m) * 3 + 2];


				if (b == g_LineInfo.mVectorPoint[i].RGB_color.v[2] && g == g_LineInfo.mVectorPoint[i].RGB_color.v[1] && r == g_LineInfo.mVectorPoint[i].RGB_color.v[0])
				{
					g_LineInfo.mVectorPoint[i].isVisible = 1;
					xxxx++;

					goto APPOINT;
				}

			}

		}
	APPOINT:;
#endif



		myPoint2f m_point2f;
		m_point2f.isVisal = 1;

		m_point2f.isVisal = g_LineInfo.mVectorPoint[i].isVisible; //2017.11.11添加

		m_point2f.m_Point[0].v[0] = g_LineInfo.mVectorPoint[i].line[0].v[0];
		m_point2f.m_Point[0].v[1] = g_LineInfo.mVectorPoint[i].line[0].v[1];
		m_point2f.m_Point[0].v[2] = g_LineInfo.mVectorPoint[i].line[0].v[2];

		m_point2f.m_Point[1].v[0] = g_LineInfo.mVectorPoint[i].line[1].v[0];
		m_point2f.m_Point[1].v[1] = g_LineInfo.mVectorPoint[i].line[1].v[1];
		m_point2f.m_Point[1].v[2] = g_LineInfo.mVectorPoint[i].line[1].v[2];

// 		m_point2f.m_color.v[0] = g_LineInfo.mVectorPoint[i].RGB_color.v[0];
// 		m_point2f.m_color.v[1] = g_LineInfo.mVectorPoint[i].RGB_color.v[1];
// 		m_point2f.m_color.v[2] = g_LineInfo.mVectorPoint[i].RGB_color.v[2];

		//g_GLInfo.mLineVector.push_back(m_point2f);

		g_GLInfo.ptrLines[i] = m_point2f;
		//mLineVector.push_back(g_LineInfo.mVectorPoint[i].isVisible);
		//g_GLInfo.mLineVector[i].isVisal = g_LineInfo.mVectorPoint[i].isVisible;

	}
	

	//printf("xxxxx = %d \n", xxxx);


#if 0
	for (int i = 0; i < g_LineInfo.mVectorPoint.size(); i++)
	{

		if (g_LineInfo.mVectorPoint[i].isVisible)
		{

			glColor3f(0.0f, 0.0f, 1.0f);

			glBegin(GL_LINE_LOOP);

			glVertex3f(g_LineInfo.mVectorPoint[i].line[0].v[0], g_LineInfo.mVectorPoint[i].line[0].v[1], g_LineInfo.mVectorPoint[i].line[0].v[2] + 5.0f );
			glVertex3f(g_LineInfo.mVectorPoint[i].line[1].v[0], g_LineInfo.mVectorPoint[i].line[1].v[1], g_LineInfo.mVectorPoint[i].line[1].v[2] + 5.0f);

			glEnd();

		}
	}
#endif

}

void  myReshape_new(int w, int h, CamerInfo * ptrCamerInfo)
{
	glViewport(0, 0, w, h);
#if 0
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w <= h) {
		float AdjY = Axis.max * (GLfloat)h / (GLfloat)w;
		glOrtho(-Axis.max, Axis.max,  // x-axis
			-AdjY, AdjY,          // y-axis
			-Axis.max, Axis.max); // z-axis
	}
	else {
		float AdjX = Axis.max * (GLfloat)h / (GLfloat)w;
		glOrtho(-AdjX, AdjX,          // x-axis
			-Axis.max, Axis.max,  // y-axis
			-Axis.max, Axis.max); // z-axis
	}
#endif
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(ptrCamerInfo->fovy, (GLfloat)w / (GLfloat)h, ptrCamerInfo->zNear, ptrCamerInfo->zFar);
}



	int myGLInit(int argc_, char *FileName, char ** argv, int width, int height) // 调用一次
	{
		FileAccess FileObj(FileName);
		int faceNum = 0;
		if (FileObj.FileOK())
		{
			GL_object GL_Obj(&FileObj);
			FaceList *CurObj = GL_Obj.GL_GetFaceList();  // Get the DXF object
			SetObj(CurObj);

			init_minmax(GL_Obj);  // save the min and the max axis values

			int argc = argc_;
			glutInit(&argc, argv);
			//创建窗口  
			glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); //set up the double buffering  
			glutInitWindowSize(width, height);
			glutInitWindowPosition(0, 0);
			glutCreateWindow("A basic OpenGL Window");
			//初始化GLEW  
			glewInit();

		}


		CamerInfo mCamerInfo;
		mCamerInfo.rotation[0] = 0.0f;
		mCamerInfo.rotation[1] = 0.0f;
		mCamerInfo.rotation[2] = 0.0f;

		mCamerInfo.translate[0] = 0.0f;
		mCamerInfo.translate[1] = 0.0f;
		mCamerInfo.translate[2] = 0.0f;

		mCamerInfo.Sigma = 30.0f;

		g_GLInfo.win_w = width;
		g_GLInfo.win_h = height;

		g_GLInfo.data = (unsigned char *)calloc(sizeof(char), (g_GLInfo.win_h + 20) * (g_GLInfo.win_w + 20) * 3);
		g_GLInfo.tempData = (unsigned char *)calloc(sizeof(char), (g_GLInfo.win_h + 20) * (g_GLInfo.win_w + 20) * 3);

		g_GLInfo.floatData = (float *)calloc(sizeof(float), (g_GLInfo.win_h + 20) * (g_GLInfo.win_w + 20) * 3);
		g_GLInfo.tempFloatData = (float *)calloc(sizeof(float), (g_GLInfo.win_h + 20) * (g_GLInfo.win_w + 20) * 3);

	//	g_GLInfo.NumFaceList = 

		myGLDeal(&mCamerInfo);
		return 0;
	}

	GLInfo * myGLDeal(CamerInfo * ptrCamerInfo)
	{

		myReshape_new(g_GLInfo.win_w, g_GLInfo.win_h, ptrCamerInfo);
		float aspect = g_GLInfo.win_w * 1.0f/ g_GLInfo.win_h;
		float temp = ptrCamerInfo->fovy * 3.1415926f / 360;
		//float h = 2 * ptrCamerInfo->zNear * sqrtf(1 - aspect * aspect * sinf(temp) * sinf(temp));

		float h = 2 * ptrCamerInfo->zNear * tanf(temp);
		float w = h * aspect;

		float dx = w / g_GLInfo.win_w;
		float dy = h / g_GLInfo.win_h;

		float ax = ptrCamerInfo->zNear / dx;
		float ay = ptrCamerInfo->zNear / dy;

		float u0 = g_GLInfo.win_w / 2;
		float v0 = g_GLInfo.win_h / 2;

		g_GLInfo.ax = -ax;
		g_GLInfo.ay = -ay;
		g_GLInfo.u0 = u0;
		g_GLInfo.v0 = v0;
		display_new_my(ptrCamerInfo);
		g_GLInfo.mCamerInfo = *ptrCamerInfo;
		//printf("%f %f %f %f %f %f \n", g_GLInfo.mCamerInfo.rotation[0], g_GLInfo.mCamerInfo.rotation[1], g_GLInfo.mCamerInfo.rotation[2], 
		//	g_GLInfo.mCamerInfo.translate[0], g_GLInfo.mCamerInfo.translate[1], g_GLInfo.mCamerInfo.translate[2]);
		glFlush();

		return &g_GLInfo;
	}

	GLInfo * myEyeDeal(float rx, float ry, float rz, float tx, float ty, float tz)
	{

		CamerInfo mCamerInfo;

		mCamerInfo.rotation[0] = -rx;
		mCamerInfo.rotation[1] = ry;
		mCamerInfo.rotation[2] = -rz;

		mCamerInfo.translate[0] = -tx;
		mCamerInfo.translate[1] = ty;
		mCamerInfo.translate[2] = tz;

		mCamerInfo.fovy = 60.0f;
		mCamerInfo.zNear = 1.0f;
		mCamerInfo.zFar = 500.0f;

		mCamerInfo.Sigma = 30.0f;


		GLInfo * ptr = myGLDeal(&mCamerInfo);

		return ptr;

	}



