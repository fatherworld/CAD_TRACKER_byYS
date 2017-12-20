#include "opencv2/opencv.hpp"
#include "CadStruct.h"
#include "CadMatrix.h"
#include "CadIsSee.h"
using namespace cv;



int convertStretch(char * fileName, float * src, int width, int height, int channel, int showDim)
{
	int i, j;
	Mat temp;
	if (0 == showDim || 1 == showDim || 2 == showDim || 3 == showDim)
	{
		float tmax, tmin;
		temp = Mat(height, width, CV_8UC1);
		float tempF;

		tmax = src[0 + showDim];
		tmin = src[0 + showDim];
		float scale = 0;
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				tempF = src[(i * width + j) * channel + showDim];
				if (tmax < tempF)
				{
					tmax = tempF;
				}
				if (tmin > tempF)
				{
					tmin = tempF;
				}

			}
		}
		//printf(" %s max= %f min = %f\n", fileName, tmax, tmin);
		scale = 255.0f / (tmax - tmin);
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				temp.data[i * width + j] = (unsigned char)((int)((src[(i * width + j) * channel + showDim] - tmin) * scale));

			}
		}
	}
	else if (-1 == showDim)
	{
		float tmax0, tmax1, tmax2, tmin0, tmin1, tmin2;
		temp = Mat(height, width, CV_8UC3);
		float tempF0, tempF1, tempF2;
		float scale0, scale1, scale2;

		tmax0 = src[0];
		tmin0 = src[0];
		tmax1 = src[1];
		tmin1 = src[1];
		tmax2 = src[2];
		tmin2 = src[2];

		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				tempF0 = src[(i * width + j) * channel + 0];
				tempF1 = src[(i * width + j) * channel + 1];
				tempF2 = src[(i * width + j) * channel + 2];
				if (tmax0 < tempF0)
				{
					tmax0 = tempF0;
				}
				if (tmin0 > tempF0)
				{
					tmin0 = tempF0;
				}

				if (tmax1 < tempF1)
				{
					tmax1 = tempF1;
				}
				if (tmin1 > tempF1)
				{
					tmin1 = tempF1;
				}

				if (tmax2 < tempF2)
				{
					tmax2 = tempF2;
				}
				if (tmin2 > tempF2)
				{
					tmin2 = tempF2;
				}
			}
		}
		printf(" %s max0= %f min0 = %f\n", fileName, tmax0, tmin0);
		scale0 = 255.0f / (tmax0 - tmin0);

		printf(" %s max1= %f min1 = %f\n", fileName, tmax1, tmin1);
		scale1 = 255.0f / (tmax1 - tmin1);

		printf(" %s max2= %f min2 = %f\n", fileName, tmax2, tmin2);
		scale2 = 255.0f / (tmax2 - tmin2);

		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				temp.data[(i * width + j) * 3 + 0] = (unsigned char)((int)((src[(i * width + j) * channel + 0] - tmin0) * scale0));
				temp.data[(i * width + j) * 3 + 1] = (unsigned char)((int)((src[(i * width + j) * channel + 1] - tmin1) * scale1));
				temp.data[(i * width + j) * 3 + 2] = (unsigned char)((int)((src[(i * width + j) * channel + 2] - tmin2) * scale2));

			}
		}
	}
	imshow(fileName, temp);
	waitKey(10);
	return 0;

}




int convertStretch_dot(char * fileName, float * src, int width, int height, int channel, int showDim, void *ptr)
{
	int i, j;
	Mat temp;
	Mat imageBgr = Mat(height, width, CV_8UC3);
	if (0 == showDim || 1 == showDim || 2 == showDim || 3 == showDim)
	{
		float tmax, tmin;
		temp = Mat(height, width, CV_8UC1);
		
		float tempF;

		tmax = src[0 + showDim];
		tmin = src[0 + showDim];
		float scale = 0;
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				tempF = src[(i * width + j) * channel + showDim];
				if (tmax < tempF)
				{
					tmax = tempF;
				}
				if (tmin > tempF)
				{
					tmin = tempF;
				}

			}
		}
		//printf(" %s max= %f min = %f\n", fileName, tmax, tmin);
		scale = 255.0f / (tmax - tmin);
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				temp.data[i * width + j] = (unsigned char)((int)((src[(i * width + j) * channel + showDim] - tmin) * scale));
				imageBgr.data[(i  * width + j) * 3 + 0] = temp.data[i * width + j];
				imageBgr.data[(i  * width + j) * 3 + 1] = temp.data[i * width + j];
				imageBgr.data[(i  * width + j) * 3 + 2] = temp.data[i * width + j];
			}
		}
	}
	CadInfo * ptrCadInfo = (CadInfo *)ptr;

	for (i = 0; i < ptrCadInfo->NumForMartixNow; i++)
	{

			
		Point center;
		Point find;
		int r = 3;

		center.x = ptrCadInfo->mForMartix[i].d2Point.v[0];

		center.y = ptrCadInfo->mForMartix[i].d2Point.v[1];

		find.x = ptrCadInfo->mForMartix[i].d2FindPoint.v[0];

		find.y = ptrCadInfo->mForMartix[i].d2FindPoint.v[1];

		circle(imageBgr, center, r, Scalar(255, 0, 0));

		circle(imageBgr, find, r, Scalar(0, 0, 255));

		line(imageBgr, center, find, Scalar(0, 255, 0));

	}

	imshow(fileName, imageBgr);
	waitKey(10);
	return 0;

}

int DumpFloat(char * filename, char * buf, int mWidth, int mHeight)
{
	FILE* file;
	int w, h;
	float * temp = (float *)buf;
	file = fopen(filename, "w+");

	//fwrite(temp, mWidth * mHeight * sizeof(float), 1, file);

	for (h = 0; h < mHeight; h++)
	{
		for (w = 0; w < mWidth - 1; w++)
		{
			fprintf_s(file, "%f,", temp[h * mWidth + w]);
		}
		fprintf_s(file, "%f\n", temp[h * mWidth + w]);
	}

	fclose(file);
	return 0;
}


int DrawLine(unsigned char * src, int width, int height, int x0, int y0, int x1, int y1, int shiness)
{
	Mat newImage = Mat(height, width, CV_8UC1, src);

	cv::Point p00, p11;

	p00.x = x0;
	p00.y = y0;

	p11.x = x1;
	p11.y = y1;

	cv::line(newImage, p00, p11, cv::Scalar(255, 255, 255), shiness);

	memcpy(src, newImage.data, sizeof(char) * width * height);

	return 0;
}


int drawAllPoint(void * ptr)
{
	CadInfo * ptrCadInfo = (CadInfo *)ptr;
	static float tempAngle[3] = { 0.0f };

	static float tempTran[3] = { 10.0f, 10.0f, -30.0f };
	int width = ptrCadInfo->bufferSize[mGrayBuffer].width;
	int height = ptrCadInfo->bufferSize[mGrayBuffer].height;

	cv::Mat ImageT = cv::Mat::zeros(height, width, CV_8UC3);
	

	RoAndTranToRT(tempAngle[0], tempAngle[1], tempAngle[2],
		tempTran[0], tempTran[1], tempTran[2], ptrCadInfo->mCameraInfo.RT);


	CadIsSeeDeal(ptrCadInfo);

	for (int i = 0; i < ptrCadInfo->NumFaceList; i++)
	{
		MyFaceList * current = &(ptrCadInfo->MyFaceListPtr[i]);

		for (int j = 0; j < 3; j++)
		{

			for (int k = 0; k < current->linePointNum[j]; k++)
			{

				if (current->flagEveryPoint[j][k] == 1)
				{
					float input[3] = { 0.0f };

					float output[2] = { 0.0f };

					input[0] = current->linePoint[j][k].v[0];
					input[1] = current->linePoint[j][k].v[1];
					input[2] = current->linePoint[j][k].v[2];


					ThreeDim2SecDimPoint(input, ptrCadInfo->mCameraInfo.InRef, ptrCadInfo->mCameraInfo.RT, output);
					cv::Point2f p;
					p.x = output[0];
					p.y = output[1];

					circle(ImageT, p, 3, Scalar(0, 0, 255));

				}
			}

		}


	}

	imshow("testZheDang", ImageT);
	waitKey(30);

	tempAngle[0] += 5.0f;
	tempAngle[1] += 5.0f;
	tempAngle[2] += 5.0f;

	return 0;
}
