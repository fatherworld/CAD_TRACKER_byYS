#include "Config.h"
#include "Format.h"
#define LOG_TAG "CadInterface"
#include "Log.h"
#include "CadInterface.h"
#include "cadStruct.h"
#include "CadGaosi.h"

#include "faccess.h"
#include "globj.h"
#include "glface.h"

#include "CadMatrix.h"
#include "CadDraw.h"
#include "CadCutImage.h"
#include "CadNormal.h"
#include "CadGradient.h"
#include "CadSimilarity.h"
#include "Dump.h"
#include "CadInhibition.h"
#include "CadMacro.h"
#include "AlvaTimer.h"
#include "CadPoints.h"
#include "CadErodeDilate.h"
#include "CadBuffer.h"
#include "CadCalculateM.h"
#include "Dump.h"
#include "testShow.h"
#include "opencv2/opencv.hpp"
#include "LineTriangleIntersect1.h"
#if DebugInfo_Switch == Debug_Alva
#include "testShow.h"
#endif

#include "LineTriangleIntersect.h"
#include "CadIsSee.h"
#include "CadUpDataRT.h"
#include <vector>
#include "cv_temp.h"

using namespace cv;
using namespace std;

static int firstTrack = 0;



int SetOutRef(void * pCadInfo)
{
	CadInfo * t_ptrCadInfo = (CadInfo *)pCadInfo;


	float t_RT[16];
	float Angle[3] = { 90.0f,45.0f, 0.0f };
	float Transform[3] = { 0.0f, 0.0f, -40.0f };

	memcpy(t_ptrCadInfo->mCameraInfo.angle, Angle, sizeof(float) * 3);
	memcpy(t_ptrCadInfo->mCameraInfo.transform, Transform, sizeof(float) * 3);

	RoAndTranToRT(Angle[0], Angle[1], Angle[2],
		Transform[0], Transform[1], Transform[2], t_RT);

	//RoAndTranToRT_W2C(Angle[0], Angle[1], Angle[2],
	//	Transform[0], Transform[1], Transform[2], t_RT);

	RTobj2RTcam(t_RT, t_ptrCadInfo->mCameraInfo.RT_);

	memcpy(t_ptrCadInfo->mCameraInfo.RT, t_RT, sizeof(float) * 16);

	memset(&t_ptrCadInfo->mAllMartix.G[0][0], 0, sizeof(float) * 6 * 16);

	t_ptrCadInfo->mAllMartix.G[0][3] = 1.0f;

	t_ptrCadInfo->mAllMartix.G[1][7] = 1.0f;

	t_ptrCadInfo->mAllMartix.G[2][11] = 1.0f;

	t_ptrCadInfo->mAllMartix.G[3][6] = -1.0f;
	t_ptrCadInfo->mAllMartix.G[3][9] = 1.0f;

	t_ptrCadInfo->mAllMartix.G[4][2] = 1.0f;
	t_ptrCadInfo->mAllMartix.G[4][8] = -1.0f;

	t_ptrCadInfo->mAllMartix.G[5][1] = -1.0f;
	t_ptrCadInfo->mAllMartix.G[5][4] = 1.0f;

#if 1
	GLInfo * ptr = myEyeDeal(Angle[0], Angle[1], Angle[2],
		Transform[0], Transform[1], Transform[2]);

	for (int i = 0; i < ptr->NumFaceList; i++)
	{
		//memcpy(t_ptrCadInfo->MyFaceListPtr[i].isEyeSee, ptr->MyFaceListPtr[i].isEyeSee, sizeof(int) * 3);
	}
#endif


	return 0;
}

void * initCad(char * modelPath, int width, int height)
{
	int ret = 0;
	CadInfo * ptrCadInfo = (CadInfo *)malloc(sizeof(CadInfo));

	FileAccess FileObj(modelPath);
	int faceNum = 0;
	float CenterOfGra[3];
	float minPoint[3] = { 0.0f, 0.0f, 0.0f };
	float maxPoint[3] = { 0.0f, 0.0f, 0.0f };
	if (FileObj.FileOK())
	{
		GL_object GL_Obj(&FileObj);
		FaceList *CurObj = GL_Obj.GL_GetFaceList();  // Get the DXF object

		CenterOfGra[0] = 0.0f;
		CenterOfGra[1] = 0.0f;
		CenterOfGra[2] = 0.0f;

		int faceNum = 0;

		if (CurObj != NULL)
		{
			FaceList::ListHandle FaceList = NULL;
			faceNum = 0;

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
				FaceList::face * m_curFace = CurObj->GetFace(FaceList);
				// 计算中心值
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
				faceNum++;
				FaceList = CurObj->GetNextFace(FaceList);
			}

			// 把模型文件读入到自己的数据中，同时算出法向量
			CenterOfGra[0] = (minPoint[0] + maxPoint[0]) / 2;
			CenterOfGra[1] = (minPoint[1] + maxPoint[1]) / 2;
			CenterOfGra[2] = (minPoint[2] + maxPoint[2]) / 2;

			ptrCadInfo->NumFaceList = faceNum;
			ptrCadInfo->GetPointStep = 0.5f; // 取点步长先设为1
			ptrCadInfo->MyFaceListPtr = (MyFaceList *)malloc(sizeof(MyFaceList) * faceNum);

			FaceList = CurObj->GetListStart();
			int i = 0;
			int j = 0;
			int k = 0;
			while (FaceList != NULL)
			{
				FaceList::face * m_curFace = CurObj->GetFace(FaceList);

				m_curFace->f[0].v[0] -= CenterOfGra[0];
				m_curFace->f[1].v[0] -= CenterOfGra[0];
				m_curFace->f[2].v[0] -= CenterOfGra[0];

				m_curFace->f[0].v[1] -= CenterOfGra[1];
				m_curFace->f[1].v[1] -= CenterOfGra[1];
				m_curFace->f[2].v[1] -= CenterOfGra[1];

				m_curFace->f[0].v[2] -= CenterOfGra[2];
				m_curFace->f[1].v[2] -= CenterOfGra[2];
				m_curFace->f[2].v[2] -= CenterOfGra[2];

				ptrCadInfo->MyFaceListPtr[i].facePoint[0].v[0] = m_curFace->f[0].v[0];
				ptrCadInfo->MyFaceListPtr[i].facePoint[0].v[1] = m_curFace->f[0].v[1];
				ptrCadInfo->MyFaceListPtr[i].facePoint[0].v[2] = m_curFace->f[0].v[2];

				ptrCadInfo->MyFaceListPtr[i].facePoint[1].v[0] = m_curFace->f[1].v[0];
				ptrCadInfo->MyFaceListPtr[i].facePoint[1].v[1] = m_curFace->f[1].v[1];
				ptrCadInfo->MyFaceListPtr[i].facePoint[1].v[2] = m_curFace->f[1].v[2];

				ptrCadInfo->MyFaceListPtr[i].facePoint[2].v[0] = m_curFace->f[2].v[0];
				ptrCadInfo->MyFaceListPtr[i].facePoint[2].v[1] = m_curFace->f[2].v[1];
				ptrCadInfo->MyFaceListPtr[i].facePoint[2].v[2] = m_curFace->f[2].v[2];

				ptrCadInfo->MyFaceListPtr[i].flag[0] = 1;
				ptrCadInfo->MyFaceListPtr[i].flag[1] = 1;
				ptrCadInfo->MyFaceListPtr[i].flag[2] = 1;

				ptrCadInfo->MyFaceListPtr[i].isEyeSee[0] = 1;
				ptrCadInfo->MyFaceListPtr[i].isEyeSee[1] = 1;
				ptrCadInfo->MyFaceListPtr[i].isEyeSee[2] = 1;

				vecf3 normal;

				getNormal(ptrCadInfo->MyFaceListPtr[i].facePoint, &normal);

				ptrCadInfo->MyFaceListPtr[i].normal = normal;

				
				//printf(" [%d] = %f %f %f ...%f %f %f ...%f %f %f \n", i, ptrCadInfo->MyFaceListPtr[i].facePoint[0].v[0], ptrCadInfo->MyFaceListPtr[i].facePoint[0].v[1], ptrCadInfo->MyFaceListPtr[i].facePoint[0].v[2],
				//	ptrCadInfo->MyFaceListPtr[i].facePoint[1].v[0], ptrCadInfo->MyFaceListPtr[i].facePoint[1].v[1], ptrCadInfo->MyFaceListPtr[i].facePoint[1].v[2], 
				//	ptrCadInfo->MyFaceListPtr[i].facePoint[2].v[0], ptrCadInfo->MyFaceListPtr[i].facePoint[2].v[1], ptrCadInfo->MyFaceListPtr[i].facePoint[2].v[2] );
				//printf("[%d] = normal = %f %f %f \n", i, normal.v[0], normal.v[1], normal.v[2]);
				FaceList = CurObj->GetNextFace(FaceList);
				i++;
			} // 

			//printf("i = %d \n", i);

			int m = 0; int n = 0;
			for (n = 0; n < ptrCadInfo->NumFaceList; n++)
			{
				MyFaceList * current = &(ptrCadInfo->MyFaceListPtr[n]);
				for (m = 0; m < ptrCadInfo->NumFaceList; m++)
				{
					MyFaceList * other = &(ptrCadInfo->MyFaceListPtr[m]);
					if (m != n)
					{
						IsInhibition(current, other, n, m);
						
					}
				}
			//	printf("[%d] = %d %d %d \n", n, current->flag[0], current->flag[1], current->flag[2]);
			//	printf("%f %f %f  \n", current->facePoint->v[0], current->facePoint->v[1], current->facePoint->v[2]);
				
			}

		}

	}


	// 初始化相机的内参
#if 1
	float InRef[3 * 4] = { -415.69220f, 0.0f, 320.0f, 0.0f, 
							0.0f, 415.69220f, 240.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 0.0f };
#else
	//615.52, 0, 320.80, 0, 617.65, 227.10
	float InRef[3 * 4] = { -615.52f, 0.0f, 320.80f, 0.0f,
		0.0f,-617.65f, 227.10f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f };
#endif
	memcpy(ptrCadInfo->mCameraInfo.InRef, InRef, sizeof(float) * 3 * 4);
	// 初始化空间
	initBuffer(ptrCadInfo, width, height);
	// 设置外参
	SetOutRef(ptrCadInfo);
	// 生成一个内参外参下的图像

	// 计算所有的采样点
	FindLinePoint(ptrCadInfo);

#if 1
	
	//Calcs = LTI::CalcFive(ptrCadInfo);
	// 存放可见点
	ptrCadInfo->NumForMartixCap = 1000;
	ptrCadInfo->NumForMartixNow = 0;
	ptrCadInfo->mForMartix = (ForMartix *)calloc(sizeof(ForMartix), ptrCadInfo->NumForMartixCap);


	CadIsSeeInit(ptrCadInfo);

	CadIsSeeDeal(ptrCadInfo);


#endif

	CadDrawAllLine(ptrCadInfo);

	cutImageResult((unsigned char *)ptrCadInfo->bufferList[mMakeGrayBuffer], (unsigned char *)ptrCadInfo->bufferList[mMakeGraySmallBuffer], ptrCadInfo->bufferSize[mMakeGrayBuffer].width, ptrCadInfo->bufferSize[mMakeGrayBuffer].height,
		ptrCadInfo->edgeMin.x, ptrCadInfo->edgeMin.y, ptrCadInfo->edgeMax.x, ptrCadInfo->edgeMax.y, 1);
	

	ptrCadInfo->bufferSize[mMakeGraySmallBuffer].width = ptrCadInfo->edgeMax.x - ptrCadInfo->edgeMin.x + 1;
	ptrCadInfo->bufferSize[mMakeGraySmallBuffer].height = ptrCadInfo->edgeMax.y - ptrCadInfo->edgeMin.y + 1;
#if DebugInfo_Switch == Debug_Alva
	{
		char fName[200];
		int width, height;
		void *ptrBuffer = ptrCadInfo->bufferList[mMakeGraySmallBuffer];
		width = ptrCadInfo->bufferSize[mMakeGraySmallBuffer].width;
		height = ptrCadInfo->bufferSize[mMakeGraySmallBuffer].height;
		sprintf_s(fName, "w_%d_h_%d.bmp", width, height);
		dump_bmp(fName, ptrBuffer, width,height ,1, DUMP_UCHAR);

	}
#endif
	ptrCadInfo->bufferSize[mMakeSmallNormalizeBuffer].width = ptrCadInfo->edgeMax.x - ptrCadInfo->edgeMin.x + 1;
	ptrCadInfo->bufferSize[mMakeSmallNormalizeBuffer].height = ptrCadInfo->edgeMax.y - ptrCadInfo->edgeMin.y + 1;

	ptrCadInfo->bufferSize[mMakeSmallGradientBuffer].width = ptrCadInfo->edgeMax.x - ptrCadInfo->edgeMin.x + 1;
	ptrCadInfo->bufferSize[mMakeSmallGradientBuffer].height = ptrCadInfo->edgeMax.y - ptrCadInfo->edgeMin.y + 1;

	ptrCadInfo->bufferSize[mMakeGraySmallBuffer].width = ptrCadInfo->edgeMax.x - ptrCadInfo->edgeMin.x + 1;
	ptrCadInfo->bufferSize[mMakeGraySmallBuffer].height = ptrCadInfo->edgeMax.y - ptrCadInfo->edgeMin.y + 1;


	


	CadNormal((unsigned char *)ptrCadInfo->bufferList[mMakeGraySmallBuffer], ptrCadInfo->bufferSize[mMakeGraySmallBuffer].width,
		ptrCadInfo->bufferSize[mMakeGraySmallBuffer].height, (int)(Alva_FMT_NV21), (float *)ptrCadInfo->bufferList[mMakeSmallNormalizeBuffer]);
#if DebugInfo_Switch == Debug_Alva
	{
		char fName[200];
		int width, height;
		void *ptrBuffer = ptrCadInfo->bufferList[mMakeSmallNormalizeBuffer];
		width = ptrCadInfo->bufferSize[mMakeSmallNormalizeBuffer].width;
		height = ptrCadInfo->bufferSize[mMakeSmallNormalizeBuffer].height;
		sprintf_s(fName, "Normal_w_%d_h_%d.txt", width, height);
		//dump_bmp(fName, ptrBuffer, width, height, 1, DUMP_FLOAT);

		DumpFloat(fName, (char *)ptrBuffer, width, height);
	}
#endif
	ptrCadInfo->noZeroNum = CadGradient((float *)(ptrCadInfo->bufferList[mMakeSmallNormalizeBuffer]), ptrCadInfo->bufferSize[mMakeSmallNormalizeBuffer].width, ptrCadInfo->bufferSize[mMakeSmallNormalizeBuffer].height,
		(float *)(ptrCadInfo->bufferList[mMakeSmallGradientBuffer]));

	ptrCadInfo->bufferList[mMakeNoZeroIndexBuffer] = (int *)calloc(1, sizeof(int) * ptrCadInfo->noZeroNum * 2);
	ptrCadInfo->bufferList[mMakeNoZeroGradientBuffer] = (float *)calloc(1, sizeof(float) * ptrCadInfo->noZeroNum * 4);

	ret = memcpyNoZeroGradient(ptrCadInfo);
	if (0 == ret)
	{
		LOGE("memcpyNoZeroGridient succeed \n");
	}
	LOGE("noZeroNum = %d ", ptrCadInfo->noZeroNum);

	ptrCadInfo->similaryThreosh = SimilarThreshold;
	ptrCadInfo->mAngleTol = 10.0f;
	ptrCadInfo->mGrayTheshold = 0.2f;
	ptrCadInfo->Frame = 0;
	ptrCadInfo->numIter = 50;

	ptrCadInfo->StatusFlag = 0; 
	return ptrCadInfo;
}

// 识别
int CadDetect(void * ptrInfo, unsigned char * data, int format)
{
	CadInfo * ptrCadInfo = (CadInfo *)ptrInfo;
	int ret = 0;

	int outOffset[2];
	Timer mTimer;
	// 归一化
	startTime(&mTimer);
	CadNormal(data, ptrCadInfo->bufferSize[mGrayBuffer].width, ptrCadInfo->bufferSize[mGrayBuffer].height, format, (float *)(ptrCadInfo->bufferList[mNormalizedBuffer]));
	stopTime(&mTimer);
	//LOGE("CadNormal time = %ld ", diffTime(mTimer));
	// 高斯滤波
	startTime(&mTimer);
	GaussianCon((float *)(ptrCadInfo->bufferList[mNormalizedBuffer]), ptrCadInfo->bufferSize[mGrayBuffer].width, ptrCadInfo->bufferSize[mGrayBuffer].height,
		(float *)(ptrCadInfo->bufferList[mGaussianKernal]), ptrCadInfo->bufferSize[mGaussianKernal].width, 1, (float *)(ptrCadInfo->bufferList[mGaussianBuffer]));
	stopTime(&mTimer);
	//LOGE("GaussianCon time = %ld ", diffTime(mTimer));
	startTime(&mTimer);
	// 梯度计算
	float LenMinMax[2] = { 0.0f, 0.0f };
	CadGradient1((float *)(ptrCadInfo->bufferList[mGaussianBuffer]), ptrCadInfo->bufferSize[mGrayBuffer].width, ptrCadInfo->bufferSize[mGrayBuffer].height,
		(float *)(ptrCadInfo->bufferList[mGradientBuffer]), (float *)(ptrCadInfo->bufferList[mGradientLenBuffer]), LenMinMax);
	stopTime(&mTimer);
	//LOGE("CadGradient time = %ld ", diffTime(mTimer));
#if DebugInfo_Switch == Release_Alva //Release_Alva  Debug_Alva
	{
		char fName[200];
		int width, height;
		void *ptrBuffer = ptrCadInfo->bufferList[mGradientBuffer];
		width = ptrCadInfo->bufferSize[mGradientBuffer].width;
		height = ptrCadInfo->bufferSize[mGradientBuffer].height;
		//sprintf_s(fName, "Gradient_w_%d_h_%d_4.bin", width, height);
		//dump_temp(fName, ptrBuffer, width * height * 4 * sizeof(float));
	
		//convertStretch("GradientX", (float *)ptrBuffer, width, height, 4, 0);
		//convertStretch("GradientY", (float *)ptrBuffer, width, height, 4, 1);
		for (int i = 3; i < 4; i++)
		{
			//convertStretch("Gradientdx", (float *)ptrBuffer, width, height, 4, 0);
		//	convertStretch("Gradientdy", (float *)ptrBuffer, width, height, 4, 3);
		}
		
	}
#endif
	//CadErodeDilate((float *)(ptrCadInfo->bufferList[mGradientLenBuffer]), ptrCadInfo->bufferSize[mGradientLenBuffer].width, ptrCadInfo->bufferSize[mGradientLenBuffer].height, 3, (float *)(ptrCadInfo->bufferList[mGaussianBuffer]));

	//convertStretch("GradientdyLenNor", (float *)(ptrCadInfo->bufferList[mGradientLenBuffer]), ptrCadInfo->bufferSize[mGrayBuffer].width, ptrCadInfo->bufferSize[mGrayBuffer].height, 1, 0);
	// 相似度计算
	startTime(&mTimer);

#if 0
	ret = findMaxSimilar((float *)(ptrCadInfo->bufferList[mGradientBuffer]), ptrCadInfo->bufferSize[mGrayBuffer].width, ptrCadInfo->bufferSize[mGrayBuffer].height,
	(float *)(ptrCadInfo->bufferList[mMakeSmallGradientBuffer]), ptrCadInfo->bufferSize[mMakeSmallGradientBuffer].width, ptrCadInfo->bufferSize[mMakeSmallGradientBuffer].height,
		ptrCadInfo->edgeMin.x, ptrCadInfo->edgeMin.y, 2, 30, ptrCadInfo->noZeroNum, ptrCadInfo->similaryThreosh, outOffset);
#else
	ret = findMaxSimilar_optimiza((float *)(ptrCadInfo->bufferList[mGradientBuffer]), ptrCadInfo->bufferSize[mGrayBuffer].width, ptrCadInfo->bufferSize[mGrayBuffer].height,
		(float *)(ptrCadInfo->bufferList[mMakeNoZeroGradientBuffer]),  (int *)ptrCadInfo->bufferList[mMakeNoZeroIndexBuffer],
		ptrCadInfo->edgeMin.x, ptrCadInfo->edgeMin.y, 2, 50, ptrCadInfo->noZeroNum, ptrCadInfo->similaryThreosh, outOffset);

#endif
	stopTime(&mTimer);
	//LOGE("findMaxSimilar time = %ld ", diffTime(mTimer));

	if (ret == 1)
	{
		ptrCadInfo->DetecResult.x = outOffset[0];
		ptrCadInfo->DetecResult.y = outOffset[1];

		int offsetd[2] = { outOffset[0] - ptrCadInfo->edgeMin.x, outOffset[1] - ptrCadInfo->edgeMin.y };

		ptrCadInfo->StatusFlag = 1;

		LOGE("offsex = %d offsety = %d \n", outOffset[0], outOffset[1]);
	}

	/// 测试隐藏算法
	//drawAllPoint(ptrCadInfo);

	return ret;
}


int CadTrack(void * ptrInfo, unsigned char * data, int format)
{
	CadInfo * ptrCadInfo = (CadInfo *)ptrInfo;
#if 1
	int ret = 0;

	int outOffset[2];
	Timer mTimer;
	// 归一化
	startTime(&mTimer);
	CadNormal(data, ptrCadInfo->bufferSize[mGrayBuffer].width, ptrCadInfo->bufferSize[mGrayBuffer].height, format, (float *)(ptrCadInfo->bufferList[mNormalizedBuffer]));
	stopTime(&mTimer);
	//LOGE("CadNormal time = %ld ", diffTime(mTimer));
	// 高斯滤波
	startTime(&mTimer);
	GaussianCon((float *)(ptrCadInfo->bufferList[mNormalizedBuffer]), ptrCadInfo->bufferSize[mGrayBuffer].width, ptrCadInfo->bufferSize[mGrayBuffer].height,
		(float *)(ptrCadInfo->bufferList[mGaussianKernal]), ptrCadInfo->bufferSize[mGaussianKernal].width, 1, (float *)(ptrCadInfo->bufferList[mGaussianBuffer]));
	stopTime(&mTimer);
	//LOGE("GaussianCon time = %ld ", diffTime(mTimer));
	startTime(&mTimer);
	// 梯度计算
	float LenMinMax[2] = { 0.0f, 0.0f };
	CadGradient1((float *)(ptrCadInfo->bufferList[mGaussianBuffer]), ptrCadInfo->bufferSize[mGrayBuffer].width, ptrCadInfo->bufferSize[mGrayBuffer].height,
		(float *)(ptrCadInfo->bufferList[mGradientBuffer]), (float *)(ptrCadInfo->bufferList[mGradientLenBuffer]), LenMinMax);
	stopTime(&mTimer);

#endif

#if 0
	cv::Mat inputData = cv::Mat(ptrCadInfo->bufferSize[mGrayBuffer].height, ptrCadInfo->bufferSize[mGrayBuffer].width, CV_8UC3, data);
	std::vector<cv::Mat> ImEdge;
	ImEdge = GetImEdge(inputData);
	int width = ptrCadInfo->bufferSize[mGrayBuffer].width;
	int height = ptrCadInfo->bufferSize[mGrayBuffer].height;

	//cv::imshow("imageEdge", ImEdge[0]);
	//cv::imshow("imageAng", ImEdge[1]);

	ImEdge[0].convertTo(ImEdge[0], CV_32F, 1.0f, 0);
	ImEdge[1].convertTo(ImEdge[1], CV_32F, 1.0f, 0);
	//cv::waitKey(0);
	memcpy(ptrCadInfo->bufferList[mGradientEdge], ImEdge[0].data, sizeof(float) * width * height);
	memcpy(ptrCadInfo->bufferList[mGradientAng], ImEdge[1].data, sizeof(float) * width * height);

#endif



	//LOGE("CadGradient time = %ld ", diffTime(mTimer));
#if DebugInfo_Switch == Release_Alva //Release_Alva  Debug_Alva
	{
		char fName[200];
		int width, height;
		void *ptrBuffer = ptrCadInfo->bufferList[mGradientBuffer];
		width = ptrCadInfo->bufferSize[mGradientBuffer].width;
		height = ptrCadInfo->bufferSize[mGradientBuffer].height;
		//sprintf_s(fName, "Gradient_w_%d_h_%d_4.bin", width, height);
		//dump_temp(fName, ptrBuffer, width * height * 4 * sizeof(float));

		for (int i = 3; i < 4; i++)
		{
			//convertStretch("Gradientdx", (float *)ptrBuffer, width, height, 4, 0);
			//convertStretch("Gradientdy", (float *)ptrBuffer, width, height, 4, 3);
		
		}

	}
#endif
	//CadErodeDilate((float *)(ptrCadInfo->bufferList[mGradientLenBuffer]), ptrCadInfo->bufferSize[mGradientLenBuffer].width, ptrCadInfo->bufferSize[mGradientLenBuffer].height, 3, (float *)(ptrCadInfo->bufferList[mGaussianBuffer]));

	//convertStretch("GradientdyLenNor", (float *)(ptrCadInfo->bufferList[mGradientLenBuffer]), ptrCadInfo->bufferSize[mGrayBuffer].width, ptrCadInfo->bufferSize[mGrayBuffer].height, 1, 0);
	
	// 在当前的RT下寻找二维点，

	if (firstTrack == 0)
	{
		int offsetd[2] = { ptrCadInfo->DetecResult.x - ptrCadInfo->edgeMin.x, ptrCadInfo->DetecResult.y - ptrCadInfo->edgeMin.y };
		//FirstTrack(ptrInfo, offsetd, (float *)(ptrCadInfo->bufferList[mGradientLenBuffer]), ptrCadInfo->bufferSize[mGradientLenBuffer].width, ptrCadInfo->bufferSize[mGradientLenBuffer].height);

		FirstTrack_1(ptrInfo, offsetd, (float *)(ptrCadInfo->bufferList[mGradientBuffer]), ptrCadInfo->bufferSize[mGradientBuffer].width, ptrCadInfo->bufferSize[mGradientBuffer].height);
		
		//dump_temp("model1.txt", ptrCadInfo->mForMartix, sizeof(ForMartix) * ptrCadInfo->NumForMartixNow);

		firstTrack = 1;
	}
	else
	{
		//LookForNew2DPoint(ptrInfo, (float *)(ptrCadInfo->bufferList[mGradientLenBuffer]), ptrCadInfo->bufferSize[mGradientLenBuffer].width, ptrCadInfo->bufferSize[mGradientLenBuffer].height);

		LookForNew2DPoint_1(ptrInfo, (float *)(ptrCadInfo->bufferList[mGradientBuffer]), ptrCadInfo->bufferSize[mGradientBuffer].width, ptrCadInfo->bufferSize[mGradientBuffer].height);

		//LookForNew2DPoint_cv(ptrInfo, (float *)(ptrCadInfo->bufferList[mGradientEdge]), (float *)(ptrCadInfo->bufferList[mGradientAng]),  width,  height);
	}
#if 0
	{
		// 保存找到的点
		char fName[200];
		sprintf_s(fName, "test/%d_pointData_[size= %d].txt", ptrCadInfo->Frame, ptrCadInfo->NumForMartixNow);

		dump_temp(fName, ptrCadInfo->mForMartix, sizeof(ForMartix) * ptrCadInfo->NumForMartixNow);

		sprintf_s(fName, "test/%d_Befor_RT.txt", ptrCadInfo->Frame);

		FILE * fp;
		fopen_s(&fp, fName, "w");

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				fprintf(fp, "%f ", ptrCadInfo->mCameraInfo.RT[i * 4 + j]);
			}
			fprintf(fp, "\n");
		}

		fclose(fp);

	}
#endif
	CadCalculateM(ptrInfo);

	

#if DebugInfo_Switch == Release_Alva //Release_Alva  Debug_Alva
	{
		char fName[200];
		int width, height;
		void *ptrBuffer = ptrCadInfo->bufferList[mGradientBuffer];
		width = ptrCadInfo->bufferSize[mGradientBuffer].width;
		height = ptrCadInfo->bufferSize[mGradientBuffer].height;
		//sprintf_s(fName, "Gradient_w_%d_h_%d_4.bin", width, height);
		//dump_temp(fName, ptrBuffer, width * height * 4 * sizeof(float));
		//convertStretch("GradientX", (float *)ptrBuffer, width, height, 4, 0);
		//convertStretch("GradientY", (float *)ptrBuffer, width, height, 4, 1);
		//convertStretch("Angle", (float *)ptrBuffer, width, height, 4, 2);
		for (int i = 3; i < 4; i++)
		{
			//convertStretch("Gradientdx", (float *)ptrBuffer, width, height, 4, 0);
			convertStretch_dot("Gradientdy", (float *)ptrBuffer, width, height, 4, 3, ptrInfo);
		}

	}
#endif

	// 是否更新
	CadUpDataRT(ptrCadInfo);

	{
		char fName[200];
		sprintf_s(fName, "test/%d_After_RT.txt", ptrCadInfo->Frame);

		FILE * fp;
		fopen_s(&fp, fName, "w");

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				fprintf(fp, "%f ", ptrCadInfo->mCameraInfo.RT[i * 4 + j]);
			}
			fprintf(fp, "\n");
		}

		fprintf(fp, "M= \n");

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				fprintf(fp, "%f ", ptrCadInfo->mAllMartix.M[i * 4 + j]);
			}
			fprintf(fp, "\n");
		}
		fclose(fp);

	}

#if 0
	MARTIX_Alva RT;
	MARTIX_Alva NewRT;
	MARTIX_Alva M;
	RT.cols = 4; RT.rows = 4;
	M.cols = 4; M.rows = 4;
	NewRT.cols = 4; NewRT.rows = 4;

	float temp[16 * 3];
	RT.martix = temp + 0;
	NewRT.martix = temp + 16;
	M.martix = temp + 16 * 2;


	memcpy(RT.martix, ptrCadInfo->mCameraInfo.RT, sizeof(float) * 16);
	memcpy(M.martix, ptrCadInfo->mAllMartix.M, sizeof(float) * 16);

	mul_maritx(RT, M, &NewRT);
	// 更新了 RT
	memcpy(ptrCadInfo->mCameraInfo.RT, NewRT.martix, sizeof(float) * 16);

	CadIsSeeDeal(ptrCadInfo);

	float Angle[3] = { 0.0f };
	float Transform[3] = { 0.0f };

	
	RT2RotateAndTrant(ptrCadInfo->mCameraInfo.RT, Angle, Transform);

#endif
#if 0
	GLInfo * ptr = myEyeDeal(Angle[0], Angle[1], Angle[2],
		Transform[0], Transform[1], Transform[2]);

	for (int i = 0; i < ptr->NumFaceList; i++)
	{
		memcpy(ptrCadInfo->MyFaceListPtr[i].isEyeSee, ptr->MyFaceListPtr[i].isEyeSee, sizeof(int) * 3);
	}
#endif

	//LOGE("findMaxSimilar time = %ld ", diffTime(mTimer));
	ptrCadInfo->Frame++;
	return ret;
}

int CadDetect_Track(void * ptrInfo, unsigned char * data, int format)
{
	CadInfo * ptrCadInfo = (CadInfo *)ptrInfo;
	int ret = 0;
	int StatuesFlag = ptrCadInfo->StatusFlag;
	if (0 == StatuesFlag)
	{
		ret = CadDetect( ptrInfo, data, format);
		if (ret == 1)
		{
			ptrCadInfo->StatusFlag = 1;
		}
	}
	else if (1 == StatuesFlag)
	{
		CadTrack(ptrInfo, data, format);
		ret = 2;
	}

	return ret;
}


int uintCad(void * ptrInfo)
{
	CadInfo * ptrCadInfo = (CadInfo *)ptrInfo;
	int i = 0;
	if (ptrCadInfo != NULL)
	{
		for (i = 0; i < BufferNum; i++)
		{
			if (ptrCadInfo->bufferList[i] != NULL)
			{
				free(ptrCadInfo->bufferList[i]);
				ptrCadInfo->bufferList[i] = 0;
			}
		}
		free(ptrCadInfo);
		ptrCadInfo = 0;
	}
	return 0;
}



