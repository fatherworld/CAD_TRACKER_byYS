#include "Config.h"
#include <string.h>
#define LOG_TAG "CadCutImage"
#include "Log.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "CadMacro.h"
#include "Format.h"
#include "CadCutImage.h"


int cutImageResult(unsigned char * src, unsigned char * desData, int srcW, int srcH, int x0, int y0, int x1, int y1, int channel)
{

	int err = 0;

	int destW = x1 - x0 + 1;
	int destH = y1 - y0 + 1;
	int i = 0;
	int destIdy = 0;

	for (i = y0; i <= y1; i++)
	{
		destIdy = i - y0;
		memcpy(&(desData[destIdy * destW * channel]), &(src[(i * srcW + x0) * channel]), sizeof(char) * channel * destW);
	}
	return 0;

}

int cutImageAndLinear(unsigned char * src, unsigned char * desData, int srcW, int srcH, int linearW, int linearH, int x0, int y0, int x1, int y1, int channels)
{
	float timeX = 0.0f;
	float timeY = 0.0f;

	int destW = x1 - x0 + 1;
	int destH = y1 - y0 + 1;

	timeX = destW * 1.0f / linearW;
	timeY = destH * 1.0f / linearH;

	int i = 0;
	int j = 0;
	int k = 0;

	for (i = 0; i < linearH; i++)
	{
		for (j = 0; j < linearW; j++)
		{

			int srcIdX = 0;
			int srcIdY = 0;

			float srcfIdX = 0.0f;
			float srcfIdY = 0.0f;


			float weightX[2] = { 0.0f };
			float weightY[2] = { 0.0f };

			srcfIdX = j * timeX + x0;
			srcfIdY = i * timeY + y0;

			srcIdX = (int)(srcfIdX);
			srcIdY = (int)(srcfIdY);

			weightX[1] = srcfIdX - srcIdX;
			weightX[0] = 1.0f - weightX[1];

			weightY[1] = srcfIdY - srcIdY;
			weightY[0] = 1.0f - weightY[1];

			for (k = 0; k < channels; k++)
			{
				float temp = (src[(srcIdY * srcW + srcIdX) * channels + k] * weightX[0] + src[(srcIdY * srcW + CLAMP(srcIdX + 1, 0, srcW - 1)) * channels + k] * weightX[1]) * weightY[0] +
					(src[(CLAMP(srcIdY + 1, 0, srcH - 1) * srcW + srcIdX) * channels + k] * weightX[0] + src[(CLAMP(srcIdY + 1, 0, srcH - 1) * srcW + CLAMP(srcIdX + 1, 0, srcW - 1)) * channels + k] * weightX[1]) * weightY[1];
				desData[(i * linearW + j) * channels + k] = (unsigned char)temp;
			}

		}
	}
	return 0;
}

int NVLinearRect(unsigned char * srcImage, int srcW, int srcH, int x0, int y0, int x1, int y1, unsigned char *destImage, int lineW, int lineH)
{
	float timeX = 0.0f;
	float timeY = 0.0f;

	int width = x1 - x0 + 1;
	int height = y1 - y0 + 1;

	int i = 0;
	int j = 0;
	int k = 0;

	float temp = 0.0f;
	timeX = width * 1.0f / lineW;
	timeY = height * 1.0f / lineH;



	unsigned char * srcUV = srcImage + srcW * srcH;
	unsigned char * destUV = destImage + lineW * lineH;


	for (i = 0; i < lineH; i++)
	{
		for (j = 0; j < lineW; j++)
		{

			int srcIdX = 0;
			int srcIdY = 0;

			float srcfIdX = 0.0f;
			float srcfIdY = 0.0f;


			float weightX[2] = { 0.0f, 0.0f };
			float weightY[2] = { 0.0f, 0.0f };

			srcfIdX = j * timeX + x0;
			srcfIdY = i * timeY + y0;

			srcIdX = (int)(srcfIdX);
			srcIdY = (int)(srcfIdY);

			weightX[1] = srcfIdX - srcIdX;
			weightX[0] = 1.0f - weightX[1];

			weightY[1] = srcfIdY - srcIdY;
			weightY[0] = 1.0f - weightY[1];

			temp = (srcImage[(srcIdY * srcW + srcIdX)] * weightX[0] + srcImage[(srcIdY * srcW + CLAMP(srcIdX + 1, 0, srcW - 1))] * weightX[1]) * weightY[0] +
				(srcImage[(CLAMP(srcIdY + 1, 0, srcH - 1) * srcW + srcIdX)] * weightX[0] + srcImage[(CLAMP(srcIdY + 1, 0, srcH - 1) * srcW + CLAMP(srcIdX + 1, 0, srcW - 1))] * weightX[1]) * weightY[1];

			destImage[(i * lineW + j)] = (unsigned char)((int)temp);

			if (0 == i % 2 && 0 == j % 2)
			{

				int srcVUW = srcW / 2;
				int srcVUH = srcH / 2;

				int destVUy = i / 2;
				int destVUx = j / 2;


				int UVsrcIdX = 0;
				int UVsrcIdY = 0;

				float UVsrcfIdX = 0.0f;
				float UVsrcfIdY = 0.0f;


				float UVweightX[2] = { 0.0f };
				float UVweightY[2] = { 0.0f };

				UVsrcfIdX = destVUx * timeX + x0 / 2;
				UVsrcfIdY = destVUy * timeY + y0 / 2;


				UVsrcIdX = (int)(UVsrcfIdX);
				UVsrcIdY = (int)(UVsrcfIdY);



				UVweightX[1] = UVsrcfIdX - UVsrcIdX;
				UVweightX[0] = 1.0f - UVweightX[1];

				UVweightY[1] = UVsrcfIdY - UVsrcIdY;
				UVweightY[0] = 1.0f - UVweightY[1];


				temp =
					(srcUV[(UVsrcIdY * srcVUW + UVsrcIdX) * 2 + 0] * UVweightX[0] + srcUV[(UVsrcIdY * srcVUW + CLAMP(UVsrcIdX + 1, 0, srcVUW - 1)) * 2 + 0] * UVweightX[1]) * UVweightY[0] +
					(srcUV[(CLAMP(UVsrcIdY + 1, 0, srcVUH - 1) * srcVUW + UVsrcIdX) * 2 + 0] * UVweightX[0] + srcUV[(CLAMP(UVsrcIdY + 1, 0, srcVUH - 1) * srcVUW + CLAMP(UVsrcIdX + 1, 0, srcVUW - 1)) * 2 + 0] * UVweightX[1]) * UVweightY[1];
				destUV[(i / 2 * (lineW / 2) + j / 2) * 2 + 0] = (unsigned char)((int)temp);

				temp =
					(srcUV[(UVsrcIdY * srcVUW + UVsrcIdX) * 2 + 1] * UVweightX[0] + srcUV[(UVsrcIdY * srcVUW + CLAMP(UVsrcIdX + 1, 0, srcVUW - 1)) * 2 + 1] * UVweightX[1]) * UVweightY[0] +
					(srcUV[(CLAMP(UVsrcIdY + 1, 0, srcVUH - 1) * srcVUW + UVsrcIdX) * 2 + 1] * UVweightX[0] + srcUV[(CLAMP(UVsrcIdY + 1, 0, srcVUH - 1) * srcVUW + CLAMP(UVsrcIdX + 1, 0, srcVUW - 1)) * 2 + 1] * UVweightX[1]) * UVweightY[1];
				destUV[(i / 2 * (lineW / 2) + j / 2) * 2 + 1] = (unsigned char)((int)temp);

			}

		}
	}

	return 0;
}


int NV21ToBGR(unsigned char * srcYVU, unsigned char * destBGR, int width, int height)
{

	unsigned char * srcVU = srcYVU + width * height;

	unsigned char Y, U, V;
	int B, G, R;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{

			Y = srcYVU[i * width + j];
			V = srcVU[(i / 2 * width / 2 + j / 2) * 2 + 0];
			U = srcVU[(i / 2 * width / 2 + j / 2) * 2 + 1];


			R = (int)(1.164*(Y - 16) + 1.596*(V - 128));
			G = (int)(1.164*(Y - 16) - 0.813*(V - 128) - 0.392*(U - 128));
			B = (int)(1.164*(Y - 16) + 2.017*(U - 128));

			destBGR[(i * width + j) * 3 + 0] = CLAMP(B, 0, 255);
			destBGR[(i * width + j) * 3 + 1] = CLAMP(G, 0, 255);
			destBGR[(i * width + j) * 3 + 2] = CLAMP(R, 0, 255);


		}
	}
	return 0;
}

int NV21ToRGB(unsigned char * srcYVU, unsigned char * dest, int width, int height)
{

	unsigned char * srcVU = srcYVU + width * height;

	unsigned char Y, U, V;
	int B, G, R;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{

			Y = srcYVU[i * width + j];
			V = srcVU[(i / 2 * width / 2 + j / 2) * 2 + 0];
			U = srcVU[(i / 2 * width / 2 + j / 2) * 2 + 1];


			R = (int)(1.164*(Y - 16) + 1.596*(V - 128));
			G = (int)(1.164*(Y - 16) - 0.813*(V - 128) - 0.392*(U - 128));
			B = (int)(1.164*(Y - 16) + 2.017*(U - 128));

			dest[(i * width + j) * 3 + 0] = CLAMP(R, 0, 255);
			dest[(i * width + j) * 3 + 1] = CLAMP(G, 0, 255);
			dest[(i * width + j) * 3 + 2] = CLAMP(B, 0, 255);


		}
	}
	return 0;
}

int cutImageResultOffset(unsigned char * src, int srcW, int srcH, unsigned char * dest, int destW, int destH, int destOffX, int destOffY,
	int x0, int y0, int x1, int y1)
{
	int x0_ = CLAMP(x0, 0, srcW - 1);
	int x1_ = CLAMP(x1, 0, srcW - 1);
	int y0_ = CLAMP(y0, 0, srcH - 1);
	int y1_ = CLAMP(y1, 0, srcH - 1);

	memset(dest, 0, sizeof(char) * destW * destH);

	int destW_ = x1_ - x0_ + 1;
	int destH_ = y1_ - y0_ + 1;

	if (destOffX + destW_ > destW || destOffY + destH_ > destH)
	{
		LOGE("Para set error \n");
		return -1;
	}

	int i = 0;
	int j = 0;
	int srcIdx = 0;
	int srcIdy = 0;
	int destIdx = 0;
	int destIdy = 0;
	for (i = 0; i < destH_; i++)
	{
		srcIdy = i + y0_;
		srcIdx = x0_;

		destIdy = i + destOffY;
		destIdx = destOffX;

		memcpy(&dest[destIdy * destW + destIdx], &src[srcIdy * srcW + srcIdx], destW * sizeof(char));
	}
	return 0;
}
int cutToResultNew(unsigned char * data, unsigned char * desData, int srcW, int srcH, int desW, int desH,
	int x0, int y0, int x1, int y1, int channel)
{
	int err = 0;

	int i = 0;
	int j = 0;
	int xStart = 0;
	int yStart = 0;
	int xEnd = 0;
	int yEnd = 0;


	int desXstart = 0;
	int desYstart = 0;


	xStart = x0;
	yStart = y0;
	xEnd = x1;
	yEnd = y1;

	if (xStart < 0)
	{
		desXstart = -xStart;
	}
	else
	{
		desXstart = 0;
	}
	if (yStart < 0)
	{
		desYstart = -yStart;
	}
	else
	{
		desYstart = 0;
	}
	for (i = yStart; i < yStart + desH; i++)
	{
		if (i < 0 || i >= srcH)
		{
			memset(&desData[((i - yStart) * desW + desXstart) * channel], 0, (CLAMP(xEnd, 0, srcW - 1) - CLAMP(xStart, 0, srcW - 1) + 1) * channel);
		}
		else
		{
			memcpy(&desData[((i - yStart) * desW + desXstart) * channel], &data[(CLAMP(i, 0, srcH - 1) * srcW + CLAMP(xStart, 0, srcW - 1)) * channel],
				(CLAMP(xEnd, 0, srcW - 1) - CLAMP(xStart, 0, srcW - 1) + 1) * channel);

		}

	}
	return 0;
}


int convertU1ToF1(unsigned char * src, float * des, int width, int height)
{
	int i, j;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			des[i * width + j] = (float)src[i * width + j];
		}
	}
	return 0;
}

int LinearImage(unsigned char * srcImage, unsigned char * destImage, int srcW, int srcH, int destW, int destH, int channels)
{
	float timeX = 0.0f;
	float timeY = 0.0f;

	timeX = srcW * 1.0f / destW;
	timeY = srcH * 1.0f / destH;

	int i = 0;
	int j = 0;
	int k = 0;


	for (i = 0; i < destH; i++)
	{
		for (j = 0; j < destW; j++)
		{

			int srcIdX = 0;
			int srcIdY = 0;

			float srcfIdX = 0.0f;
			float srcfIdY = 0.0f;


			float weightX[2] = { 0.0f };
			float weightY[2] = { 0.0f };

			srcfIdX = j * timeX;
			srcfIdY = i * timeY;

			srcIdX = (int)(srcfIdX);
			srcIdY = (int)(srcfIdY);

			weightX[1] = srcfIdX - srcIdX;
			weightX[0] = 1.0f - weightX[1];

			weightY[1] = srcfIdY - srcIdY;
			weightY[0] = 1.0f - weightY[1];

			for (k = 0; k < channels; k++)
			{
				float temp  = (srcImage[(srcIdY * srcW + srcIdX) * channels + k] * weightX[0] + srcImage[(srcIdY * srcW + CLAMP(srcIdX + 1, 0, srcW - 1)) * channels + k] * weightX[1]) * weightY[0] +
					(srcImage[(CLAMP(srcIdY + 1, 0, srcH - 1) * srcW + srcIdX) * channels + k] * weightX[0] + srcImage[(CLAMP(srcIdY + 1, 0, srcH - 1) * srcW + CLAMP(srcIdX + 1, 0, srcW - 1)) * channels + k] * weightX[1]) * weightY[1];

				destImage[(i * destW + j) * channels + k] = (unsigned char)((int)temp);
			}

		}
	}

	return 0;

}