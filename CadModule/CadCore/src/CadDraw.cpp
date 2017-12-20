#include "Config.h"
#include "Format.h"
#define LOG_TAG "CadDraw"
#include "Log.h"
#include "CadStruct.h"
#include "CadMacro.h"
#include "CadMatrix.h"
#include "CadDraw.h"
#include "CadMacro.h"
#include "CadCutImage.h"
#include "CadNormal.h"
#include "CadDraw.h"
#include "testShow.h"

int LiDrewLine_Source(unsigned char * data, int width, int height, int channel, int x0, int y0, int x1, int y1, int shinner, unsigned char red, unsigned char gree, unsigned char blue)
{
	float a = 0;
	float b = 0;
	int i = 0;
	int j = 0;
	int k = 0;
	int m = 0;
	unsigned char color[4] = { red, gree, blue, 0 };
	int minx = 0;
	int maxx = 0;

	int miny = 0;
	int maxy = 0;

	float yy = 0.0f;
	float xx = 0.0f;

	if (x0 > x1)
	{
		maxx = x0;
		minx = x1;
	}
	else
	{
		maxx = x1;
		minx = x0;
	}

	if (y0 > y1)
	{
		maxy = y0;
		miny = y1;
	}
	else
	{
		maxy = y1;
		miny = y0;
	}


	if (maxy - miny > maxx - minx)
	{
		a = (x1 - x0) * 1.0f / (y1 - y0);

		b = x0 * 1.0f - a * y0;

		for (i = miny; i <= maxy; i++)
		{
			xx = a * i + b;

			m = (int)xx;
			for (j = m - shinner; j <= m + shinner; j++)
			{
				for (k = 0; k < channel; k++)
				{
					data[(CLAMP(i, 0, height - 1) * width + CLAMP(j, 0, width - 1)) * channel + k] = color[k];
				}
			}
		}

	}
	else
	{
		a = (y1 - y0) * 1.0f / (x1 - x0);

		b = y0 * 1.0f - a * x0;

		for (i = minx; i <= maxx; i++)
		{
			yy = a * i + b;

			m = (int)yy;
			for (j = m - shinner; j <= m + shinner; j++)
			{
				for (k = 0; k < channel; k++)
				{
					data[(CLAMP(j, 0, height - 1) * width + CLAMP(i, 0, width - 1)) * channel + k] = color[k];
				}
			}
		}
	}


	return 0;
}

static int drewDot(unsigned char * data, int width, int height, int x, int y, int r)
{
	int i, j;
	int RR = 0;
	for (j = y - r; j <= y + r; j++)
	{
		for (i = x - r; i <= x + r; i++)
		{
			RR = (j - y) * (j - y) + (i - x) * (i - x);
			if (RR <= r * r)
			{
				data[(CLAMP(j, 0, height - 1) * width + CLAMP(i, 0, width - 1))] = 255;
			}
		}
	}
	return 0;

}

int LiDrewLine(unsigned char * data, int width, int height, int x0, int y0, int x1, int y1, int shinner)
{
	float a = 0;
	float b = 0;
	int i = 0;
	int j = 0;
	int k = 0;
	int m = 0;
	
	int minx = 0;
	int maxx = 0;

	int miny = 0;
	int maxy = 0;

	float yy = 0.0f;
	float xx = 0.0f;

	if (x0 > x1)
	{
		maxx = x0;
		minx = x1;
	}
	else
	{
		maxx = x1;
		minx = x0;
	}

	if (y0 > y1)
	{
		maxy = y0;
		miny = y1;
	}
	else
	{
		maxy = y1;
		miny = y0;
	}


	if (maxy - miny > maxx - minx)
	{
		a = (x1 - x0) * 1.0f / (y1 - y0);

		b = x0 * 1.0f - a * y0;

		for (i = miny; i <= maxy; i++)
		{
			xx = a * i + b;

			m = (int)xx;
			for (j = m - shinner; j <= m + shinner; j++)
			{
				data[(CLAMP(i, 0, height - 1) * width + CLAMP(j, 0, width - 1))] = 255;
			}
		}

	}
	else
	{
		a = (y1 - y0) * 1.0f / (x1 - x0);

		b = y0 * 1.0f - a * x0;

		for (i = minx; i <= maxx; i++)
		{
			yy = a * i + b;

			m = (int)yy;
			for (j = m - shinner; j <= m + shinner; j++)
			{

				data[(CLAMP(j, 0, height - 1) * width + CLAMP(i, 0, width - 1))] = 255;

			}
		}
	}

	drewDot(data, width, height, x0, y0, shinner * 2);
	drewDot(data, width, height, x1, y1, shinner * 2);

	return 0;
}


int CadDrawLine(unsigned char * src, int width, int height, int x0, int y0, int x1, int y1, int shickness)
{

	LiDrewLine(src, width, height, x0, y0, x1, y1, shickness);

	return 0;
}

int CadDrawAllLine(void * ptrCadInfo)
{
	CadInfo * ptrInfo = (CadInfo *)ptrCadInfo;
	int i = 0;
	int j = 0;
	float input0[3] = { 0.0f };
	float output0[2] = { 0.0f };

	float input1[3] = { 0.0f };
	float output1[2] = { 0.0f };
	
	float edgemin[2] = { (float)(ptrInfo->bufferSize[mGrayBuffer].width), (float)(ptrInfo->bufferSize[mGrayBuffer].height)};
	float edgemax[2] = { 0.0f, 0.0f };

	int jj = 0;
	memset(ptrInfo->bufferList[mMakeGrayBuffer], 0, ptrInfo->bufferSize[mMakeGrayBuffer].width * ptrInfo->bufferSize[mMakeGrayBuffer].height);
	for (i = 0; i < ptrInfo->NumFaceList; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (ptrInfo->MyFaceListPtr[i].flag[j] == 1 && ptrInfo->MyFaceListPtr[i].isEyeSee[j] == 1)
			{
				jj = j + 1;
				if (jj == 3)
				{
					jj = 0;
				}
				input0[0] = ptrInfo->MyFaceListPtr[i].facePoint[j].v[0];
				input0[1] = ptrInfo->MyFaceListPtr[i].facePoint[j].v[1];
				input0[2] = ptrInfo->MyFaceListPtr[i].facePoint[j].v[2];

				input1[0] = ptrInfo->MyFaceListPtr[i].facePoint[jj].v[0];
				input1[1] = ptrInfo->MyFaceListPtr[i].facePoint[jj].v[1];
				input1[2] = ptrInfo->MyFaceListPtr[i].facePoint[jj].v[2];

				ThreeDim2SecDimPoint(input0, ptrInfo->mCameraInfo.InRef, ptrInfo->mCameraInfo.RT, output0);
				ThreeDim2SecDimPoint(input1, ptrInfo->mCameraInfo.InRef, ptrInfo->mCameraInfo.RT, output1);



				//output0[0] = ptrInfo->bufferSize[mGrayBuffer].width - 1 - output0[0];
				//output1[0] = ptrInfo->bufferSize[mGrayBuffer].width - 1 - output1[0];

				

				

				ptrInfo->MyFaceListPtr[i].face2DimPoint[j].v[0] = output0[0];
				ptrInfo->MyFaceListPtr[i].face2DimPoint[j].v[1] = output0[1];
				
				ptrInfo->MyFaceListPtr[i].face2DimPoint[jj].v[0] = output1[0];
				ptrInfo->MyFaceListPtr[i].face2DimPoint[jj].v[1] = output1[1];

				float minx = output0[0] < output1[0] ? output0[0] : output1[0];
				float miny = output0[1] < output1[1] ? output0[1] : output1[1];

				float maxx = output0[0] > output1[0] ? output0[0] : output1[0];
				float maxy = output0[1] > output1[1] ? output0[1] : output1[1];


				if (minx < edgemin[0])
				{
					edgemin[0] = minx;
				}
				if (miny < edgemin[1])
				{
					edgemin[1] = miny;
				}
				if (maxx > edgemax[0])
				{
					edgemax[0] = maxx;
				}
				if (maxy > edgemax[1])
				{
					edgemax[1] = maxy;
				}

				

				CadDrawLine((unsigned char *)ptrInfo->bufferList[mMakeGrayBuffer], ptrInfo->bufferSize[mMakeGrayBuffer].width, ptrInfo->bufferSize[mMakeGrayBuffer].height,
					(int)output0[0], (int)output0[1], (int)output1[0], (int)output1[1], 1);

				//DrawLine((unsigned char *)ptrInfo->bufferList[mMakeGrayBuffer], ptrInfo->bufferSize[mMakeGrayBuffer].width, ptrInfo->bufferSize[mMakeGrayBuffer].height,
				//	(int)output0[0], (int)output0[1], (int)output1[0], (int)output1[1], 3);

			}
		}
		
	}
	int temp[4] = { (int)(edgemin[0] - 3) , (int)(edgemin[1] - 3), (int)(edgemax[0] + 3), (int)(edgemax[1] + 3) };

	ptrInfo->edgeMin.x = CLAMP(temp[0], 0, ptrInfo->bufferSize[mMakeGrayBuffer].width - 1);
	ptrInfo->edgeMin.y = CLAMP(temp[1], 0, ptrInfo->bufferSize[mMakeGrayBuffer].height - 1);
	ptrInfo->edgeMax.x = CLAMP(temp[2], 0, ptrInfo->bufferSize[mMakeGrayBuffer].width - 1);
	ptrInfo->edgeMax.y = CLAMP(temp[3], 0, ptrInfo->bufferSize[mMakeGrayBuffer].height - 1);


	

	//printf("%f %f %f %f \n", edgemin[0], edgemin[1], edgemax[0], edgemax[1]);

	//drewDot((unsigned char *)ptrInfo->bufferList[mMakeGrayBuffer], ptrInfo->bufferSize[mMakeGrayBuffer].width, ptrInfo->bufferSize[mMakeGrayBuffer].height, 
	//	edgemin[0], edgemin[1], 3);
	//drewDot((unsigned char *)ptrInfo->bufferList[mMakeGrayBuffer], ptrInfo->bufferSize[mMakeGrayBuffer].width, ptrInfo->bufferSize[mMakeGrayBuffer].height, 
	//	edgemax[0], edgemax[1], 3);
	return 0;
}

